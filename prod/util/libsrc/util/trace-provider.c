#include <trace-util.h>
#include <trace-file-util.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <inttypes.h>

/* ===== Windows: TraceLogging プロバイダ定義 ===== */

#ifdef _WIN32

#include <windows.h>
#include <TraceLoggingProvider.h>

/* trace-util が管理するデフォルトプロバイダ定義 */
TRACELOGGING_DEFINE_PROVIDER(
    s_trace_provider,
    TRACE_DEFAULT_PROVIDER_NAME,
    TRACE_DEFAULT_PROVIDER_GUID);

/** デフォルトプロバイダの参照カウント。 */
static volatile LONG s_trace_ref = 0;

/** デフォルトプロバイダの共有 ETW ハンドル。 */
static etw_provider_t *s_etw_handle = NULL;

#else /* !_WIN32 */
#include <pthread.h>
#endif /* _WIN32 */

/**
 *  @brief  トレースプロバイダハンドル構造体 (内部定義)。
 */
struct trace_provider
{
    /** アプリケーション管理識別番号 (診断用)。0 = 識別番号なし。 */
    int64_t identifier;

#ifdef _WIN32
    /** サービス名 (ETW "Trace" イベントの "Service" フィールド)。 */
    char *service_name;
#else /* !_WIN32 */
    /** syslog プロバイダハンドル (Linux)。 */
    syslog_provider_t *syslog_handle;
#endif /* _WIN32 */

    /** OS トレース (ETW/syslog) のスレッショルドレベル。デフォルト: TRACE_LV_INFO。 */
    enum trace_level os_level;

    /** ファイルトレースのスレッショルドレベル。デフォルト: TRACE_LV_ERROR。 */
    enum trace_level file_level;

    /** ファイルトレースプロバイダハンドル。NULL = ファイルトレース無効。 */
    trace_file_provider_t *file_handle;

    /** 実行状態フラグ (0=停止中, 1=実行中)。 */
    volatile int running;

#ifdef _WIN32
    /** 読み書きロック。write 系は共有ロック、設定変更・stop・dispose は排他ロック。
     *  SRWLOCK は初期化関数のみ必要で破棄関数は不要。 */
    SRWLOCK config_rwlock;
#else /* !_WIN32 */
    /** 読み書きロック。write 系は共有ロック、設定変更・stop・dispose は排他ロック。 */
    pthread_rwlock_t config_rwlock;
    /** config_rwlock が初期化済みかどうかのフラグ。 */
    int              config_rwlock_initialized;
#endif /* _WIN32 */
};

#ifdef _WIN32

/**
 *  @brief  enum trace_level を ETW Level (1-5) に変換する。
 */
static int to_etw_level(enum trace_level lv)
{
    switch (lv)
    {
    case TRACE_LV_CRITICAL: return 1;
    case TRACE_LV_ERROR:    return 2;
    case TRACE_LV_WARNING:  return 3;
    case TRACE_LV_INFO:     return 4;
    default:              return 5;
    }
}

#else /* !_WIN32 */

#include <syslog.h>

/**
 *  @brief  enum trace_level を syslog severity に変換する。
 */
static int to_syslog_level(enum trace_level lv)
{
    switch (lv)
    {
    case TRACE_LV_CRITICAL: return LOG_CRIT;
    case TRACE_LV_ERROR:    return LOG_ERR;
    case TRACE_LV_WARNING:  return LOG_WARNING;
    case TRACE_LV_INFO:     return LOG_INFO;
    case TRACE_LV_VERBOSE:  return LOG_DEBUG;
    default:                return LOG_DEBUG;
    }
}

#endif /* _WIN32 */

/** プロセス名取得失敗時のフォールバック名。 */
#define FALLBACK_NAME "unknown"

#ifdef _WIN32

/**
 *  @brief  自プロセスの実行ファイル名 (ベースネーム) を取得する。
 *  @param[out]  buf       パス格納バッファ。
 *  @param[in]   buf_size  バッファサイズ。
 *  @return      ベースネームへのポインタ (buf 内または FALLBACK_NAME)。
 */
static const char *get_process_basename(char *buf, size_t buf_size)
{
    DWORD len;
    const char *sep;

    len = GetModuleFileNameA(NULL, buf, (DWORD)buf_size);
    if (len == 0 || len >= (DWORD)buf_size)
    {
        return FALLBACK_NAME;
    }

    sep = strrchr(buf, '\\');
    if (sep == NULL)
    {
        sep = strrchr(buf, '/');
    }
    return sep ? sep + 1 : buf;
}
#else /* !_WIN32 */
#include <unistd.h>

/**
 *  @brief  自プロセスの実行ファイル名 (ベースネーム) を取得する。
 *  @param[out]  buf       パス格納バッファ。
 *  @param[in]   buf_size  バッファサイズ。
 *  @return      ベースネームへのポインタ (buf 内または FALLBACK_NAME)。
 */
static const char *get_process_basename(char *buf, size_t buf_size)
{
    ssize_t len;
    const char *slash;

    len = readlink("/proc/self/exe", buf, buf_size - 1);
    if (len <= 0)
    {
        return FALLBACK_NAME;
    }
    buf[len] = '\0';

    slash = strrchr(buf, '/');
    return slash ? slash + 1 : buf;
}
#endif /* _WIN32 */

/**
 *  @brief  設定・状態変更用の排他ロックを取得する。
 *          trace_start / trace_stop / trace_modify_* / trace_dispose で使用する。
 */
static void config_lock_exclusive(trace_provider_t *handle)
{
#ifdef _WIN32
    AcquireSRWLockExclusive(&handle->config_rwlock);
#else /* !_WIN32 */
    pthread_rwlock_wrlock(&handle->config_rwlock);
#endif /* _WIN32 */
}

/**
 *  @brief  排他ロックを解放する。
 */
static void config_unlock_exclusive(trace_provider_t *handle)
{
#ifdef _WIN32
    ReleaseSRWLockExclusive(&handle->config_rwlock);
#else /* !_WIN32 */
    pthread_rwlock_unlock(&handle->config_rwlock);
#endif /* _WIN32 */
}

/** 共有ロック取得のタイムアウト (ミリ秒)。 */
#define LOCK_TIMEOUT_MS 100

/**
 *  @brief  書き込み系 API 用の共有ロックをタイムアウト付きで取得する。
 *          複数スレッドが同時に取得できる。
 *          排他ロック保持中はブロックするが、LOCK_TIMEOUT_MS 経過で諦める。
 *  @return 取得成功: 0 / タイムアウト: -1。
 */
static int config_lock_shared_timed(trace_provider_t *handle)
{
#ifdef _WIN32
    /* SRWLOCK には timed 取得 API がないため spin + TryAcquireSRWLockShared で代替する */
    DWORD deadline = GetTickCount() + (DWORD)LOCK_TIMEOUT_MS;
    while (!TryAcquireSRWLockShared(&handle->config_rwlock))
    {
        if ((LONG)(GetTickCount() - deadline) >= 0)
        {
            return -1;
        }
        SwitchToThread();
    }
    return 0;
#else /* !_WIN32 */
    struct timespec abs_timeout;
    clock_gettime(CLOCK_REALTIME, &abs_timeout);
    abs_timeout.tv_nsec += (long)LOCK_TIMEOUT_MS * 1000000L;
    if (abs_timeout.tv_nsec >= 1000000000L)
    {
        abs_timeout.tv_sec  += 1;
        abs_timeout.tv_nsec -= 1000000000L;
    }
    return (pthread_rwlock_timedrdlock(&handle->config_rwlock, &abs_timeout) == 0) ? 0 : -1;
#endif /* _WIN32 */
}

/**
 *  @brief  共有ロックを解放する。
 */
static void config_unlock_shared(trace_provider_t *handle)
{
#ifdef _WIN32
    ReleaseSRWLockShared(&handle->config_rwlock);
#else /* !_WIN32 */
    pthread_rwlock_unlock(&handle->config_rwlock);
#endif /* _WIN32 */
}

/**
 *  @brief  name と identifier から有効識別名 (effective name) を生成する。
 *  @details  name == NULL の場合はプロセスのベースネームを使用する。\n
 *            identifier == 0 の場合は name をそのままコピーして返す。\n
 *            identifier > 0 の場合は "<name>-<identifier>" を生成して返す。\n
 *            戻り値はヒープ上に確保されるため、呼び出し元が free する必要がある。
 *  @return  ヒープ上に確保された有効識別名。失敗時 NULL。
 */
static char *build_effective_name(const char *name, int64_t identifier)
{
    char path_buf[256];
    const char *base;
    char *result;

    base = (name != NULL) ? name : get_process_basename(path_buf, sizeof(path_buf));

    if (identifier == 0)
    {
#ifdef _WIN32
        return _strdup(base);
#else
        return strdup(base);
#endif
    }

    {
        int id_len = snprintf(NULL, 0, "%" PRId64, identifier);
        size_t base_len = strlen(base);
        /* "<base>-<identifier>\0" */
        size_t total = base_len + 1 /* '-' */ + (size_t)id_len + 1;

        result = (char *)malloc(total);
        if (result == NULL)
        {
            return NULL;
        }
        snprintf(result, total, "%s-%" PRId64, base, identifier);
        return result;
    }
}

trace_provider_t *TRACE_UTIL_API
    trace_init(void)
{
    trace_provider_t *handle;
    char path_buf[256];
    const char *effective_name;

    effective_name = get_process_basename(path_buf, sizeof(path_buf));

#ifdef _WIN32
    {
        char *svc;

        svc = _strdup(effective_name);
        if (svc == NULL)
        {
            return NULL;
        }

        handle = (trace_provider_t *)malloc(sizeof(trace_provider_t));
        if (handle == NULL)
        {
            free(svc);
            return NULL;
        }

        handle->identifier   = 0;
        handle->service_name = svc;
        handle->os_level     = TRACE_DEFAULT_OS_LEVEL;
        handle->file_level   = TRACE_DEFAULT_FILE_LEVEL;
        handle->file_handle  = NULL;
        handle->running      = 0;

        InitializeSRWLock(&handle->config_rwlock);

        if (InterlockedIncrement(&s_trace_ref) == 1)
        {
            s_etw_handle = etw_provider_init(s_trace_provider);
            if (s_etw_handle == NULL)
            {
                InterlockedDecrement(&s_trace_ref);
                free(handle->service_name);
                free(handle);
                return NULL;
            }
        }
    }
#else /* !_WIN32 */
    {
        syslog_provider_t *sp;

        sp = syslog_provider_init(effective_name, LOG_USER);
        if (sp == NULL)
        {
            return NULL;
        }

        handle = (trace_provider_t *)malloc(sizeof(trace_provider_t));
        if (handle == NULL)
        {
            syslog_provider_dispose(sp);
            return NULL;
        }

        handle->identifier    = 0;
        handle->syslog_handle = sp;
        handle->os_level      = TRACE_DEFAULT_OS_LEVEL;
        handle->file_level    = TRACE_DEFAULT_FILE_LEVEL;
        handle->file_handle   = NULL;
        handle->running       = 0;

        if (pthread_rwlock_init(&handle->config_rwlock, NULL) != 0)
        {
            syslog_provider_dispose(sp);
            free(handle);
            return NULL;
        }
        handle->config_rwlock_initialized = 1;
    }
#endif /* _WIN32 */

    return handle;
}

int TRACE_UTIL_API
    trace_start(trace_provider_t *handle)
{
    if (handle == NULL)
    {
        return -1;
    }

    config_lock_exclusive(handle);

    if (handle->running)
    {
        config_unlock_exclusive(handle);
        return 0;
    }

    handle->running = 1;
    config_unlock_exclusive(handle);
    return 0;
}

int TRACE_UTIL_API
    trace_stop(trace_provider_t *handle)
{
    if (handle == NULL)
    {
        return -1;
    }

    config_lock_exclusive(handle);

    if (!handle->running)
    {
        config_unlock_exclusive(handle);
        return 0;
    }

    handle->running = 0;
    config_unlock_exclusive(handle);
    return 0;
}

/** 切り詰め後の本文最大バイト数 (null 終端を除く)。 */
#define MAX_BODY (TRACE_MESSAGE_MAX_BYTES - 1)

/**
 *  @brief  UTF-8 安全な切り詰め位置を返す。
 *  @details  pos バイト目以前で、UTF-8 マルチバイトシーケンスの途中を
 *            避けた安全な切断位置を返す。
 */
static size_t utf8_safe_truncate(const char *s, size_t pos)
{
    /* pos が文字列長以下なら調整不要 */
    while (pos > 0 && ((unsigned char)s[pos] & 0xC0) == 0x80)
    {
        /* 継続バイト (10xxxxxx) を跨がないよう先頭バイトまで戻る */
        pos--;
    }
    return pos;
}

/**
 *  @brief  下層プロバイダに文字列を書き込む (内部ヘルパー)。
 */
static int write_to_provider(trace_provider_t *handle, enum trace_level level, const char *msg)
{
#ifdef _WIN32
    return etw_provider_write(s_etw_handle, to_etw_level(level),
                              handle->service_name, msg);
#else /* !_WIN32 */
    return syslog_provider_write(handle->syslog_handle, to_syslog_level(level), msg);
#endif /* _WIN32 */
}

/**
 *  @brief  メッセージレベルがスレッショルド以内かを判定する。
 *  @return 出力すべき場合は 1、そうでなければ 0。
 */
static int should_output(enum trace_level msg_level, enum trace_level threshold)
{
    if (threshold == TRACE_LV_NONE)
    {
        return 0;
    }
    return (int)msg_level <= (int)threshold;
}

/**
 *  @brief  OS プロバイダとファイルプロバイダの両方へメッセージを書き込む。
 *  @details レベルフィルタリングを行い、各出力先の条件に合致する場合のみ書き込む。
 *           両方とも出力不要の場合は何もしない。
 *  @return  成功 0 / いずれかの書き込み失敗 -1。
 */
static int write_dual(trace_provider_t *handle, enum trace_level level, const char *msg)
{
    int os_result   = 0;
    int file_result = 0;

    /* OS トレース出力 */
    if (should_output(level, handle->os_level))
    {
        os_result = write_to_provider(handle, level, msg);
    }

    /* ファイルトレース出力 */
    if (handle->file_handle != NULL && should_output(level, handle->file_level))
    {
        file_result = trace_file_provider_write(handle->file_handle, (int)level, msg);
    }

    return (os_result != 0 || file_result != 0) ? -1 : 0;
}

int TRACE_UTIL_API
    trace_write(trace_provider_t *handle, enum trace_level level, const char *message)
{
    const char *msg;
    char buf[TRACE_MESSAGE_MAX_BYTES];
    size_t len;
    int ret;

    if (handle == NULL || message == NULL)
    {
        return 0;
    }

    if (config_lock_shared_timed(handle) != 0)
    {
        return -1;
    }

    if (!handle->running)
    {
        config_unlock_shared(handle);
        return -1;
    }

    msg = message;
    len = strlen(message);

    if (len > MAX_BODY)
    {
        size_t safe_len = utf8_safe_truncate(message, MAX_BODY);
        memcpy(buf, message, safe_len);
        buf[safe_len] = '\0';
        msg = buf;
    }

    ret = write_dual(handle, level, msg);
    config_unlock_shared(handle);
    return ret;
}

int TRACE_UTIL_API
    trace_writef(trace_provider_t *handle, enum trace_level level, const char *format, ...)
{
    va_list args;
    char buf[TRACE_MESSAGE_MAX_BYTES];
    int ret;

    if (handle == NULL || format == NULL)
    {
        return 0;
    }

    if (config_lock_shared_timed(handle) != 0)
    {
        return -1;
    }

    if (!handle->running)
    {
        config_unlock_shared(handle);
        return -1;
    }

    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    ret = write_dual(handle, level, buf);
    config_unlock_shared(handle);
    return ret;
}

/** HEX 変換用テーブル。 */
static const char hex_chars[] = "0123456789ABCDEF";

/** "..." サフィックスの長さ。 */
#define ELLIPSIS_LEN 3

/**
 *  @brief  HEX ダンプ出力の内部実装。
 *  @details  ラベルは呼び出し元で事前にフォーマット済みの文字列を受け取る。
 *            データが収まらない場合は切り詰めて "..." を付与する。
 *            呼び出し元が共有ロックを保持した状態で呼ぶこと。
 */
static int hex_write_impl(trace_provider_t *handle, enum trace_level level,
                           const void *data, size_t size, const char *label)
{
    char buf[TRACE_MESSAGE_MAX_BYTES];
    const unsigned char *bytes = (const unsigned char *)data;
    size_t pos = 0;
    size_t remaining;
    size_t max_data_bytes;
    int truncated = 0;
    size_t i;

    if (handle == NULL || data == NULL || size == 0)
    {
        return 0;
    }

    /* ラベル付与 */
    if (label != NULL && label[0] != '\0')
    {
        size_t lbl_len = strlen(label);
        /* ラベル + ": " がバッファに収まるか検査 */
        if (lbl_len + 2 >= MAX_BODY)
        {
            /* ラベルだけで上限超過 — ラベルのみ出力 */
            size_t copy_len = lbl_len < MAX_BODY ? lbl_len : MAX_BODY;
            memcpy(buf, label, copy_len);
            buf[copy_len] = '\0';
            return write_dual(handle, level, buf);
        }
        memcpy(buf, label, lbl_len);
        buf[lbl_len] = ':';
        buf[lbl_len + 1] = ' ';
        pos = lbl_len + 2;
    }

    /* HEX データ出力可能な残りバイト数 */
    remaining = MAX_BODY - pos;

    /* 全データが収まるか判定:
       N バイト → 3*N - 1 文字 (最終バイトはスペースなし) */
    max_data_bytes = (remaining + 1) / 3;

    if (size > max_data_bytes)
    {
        /* 切り詰めが必要: "..." (3文字) の分を確保
           各バイト "XX " (3文字) + "..." (3文字) → M*3 + 3 ≤ remaining
           M ≤ (remaining - 3) / 3 */
        truncated = 1;
        if (remaining < ELLIPSIS_LEN)
        {
            /* "..." すら入らない */
            buf[pos] = '\0';
            return write_dual(handle, level, buf);
        }
        max_data_bytes = (remaining - ELLIPSIS_LEN) / 3;
        if (max_data_bytes == 0)
        {
            /* HEX 1 バイトも入らない — "..." のみ */
            memcpy(buf + pos, "...", ELLIPSIS_LEN);
            pos += ELLIPSIS_LEN;
            buf[pos] = '\0';
            return write_dual(handle, level, buf);
        }
        size = max_data_bytes;
    }

    /* HEX 文字列を構築 */
    for (i = 0; i < size; i++)
    {
        if (i > 0)
        {
            buf[pos++] = ' ';
        }
        buf[pos++] = hex_chars[(bytes[i] >> 4) & 0x0F];
        buf[pos++] = hex_chars[bytes[i] & 0x0F];
    }

    /* 切り詰め時は "..." を付与 */
    if (truncated)
    {
        buf[pos++] = ' ';
        buf[pos++] = '.';
        buf[pos++] = '.';
        buf[pos++] = '.';
    }
    buf[pos] = '\0';

    return write_dual(handle, level, buf);
}

int TRACE_UTIL_API
    trace_hex_write(trace_provider_t *handle, enum trace_level level,
                  const void *data, size_t size, const char *message)
{
    int ret;

    if (handle == NULL || data == NULL || size == 0)
    {
        return 0;
    }

    if (config_lock_shared_timed(handle) != 0)
    {
        return -1;
    }

    if (!handle->running)
    {
        config_unlock_shared(handle);
        return -1;
    }

    ret = hex_write_impl(handle, level, data, size, message);
    config_unlock_shared(handle);
    return ret;
}

int TRACE_UTIL_API
    trace_hex_writef(trace_provider_t *handle, enum trace_level level,
                   const void *data, size_t size, const char *format, ...)
{
    char label[TRACE_MESSAGE_MAX_BYTES];
    int ret;

    if (handle == NULL || data == NULL || size == 0)
    {
        return 0;
    }

    if (config_lock_shared_timed(handle) != 0)
    {
        return -1;
    }

    if (!handle->running)
    {
        config_unlock_shared(handle);
        return -1;
    }

    if (format != NULL)
    {
        va_list args;
        va_start(args, format);
        vsnprintf(label, sizeof(label), format, args);
        va_end(args);
        ret = hex_write_impl(handle, level, data, size, label);
    }
    else
    {
        ret = hex_write_impl(handle, level, data, size, NULL);
    }

    config_unlock_shared(handle);
    return ret;
}

void TRACE_UTIL_API
    trace_dispose(trace_provider_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

    /* 排他ロックで running=0 をセットし、進行中の write 完了を待つ。
     * trace_stop 返却後は新規 write は -1 を返し、write 中の処理は完了している。 */
    trace_stop(handle);

    /* ファイルトレースプロバイダの解放 */
    if (handle->file_handle != NULL)
    {
        trace_file_provider_dispose(handle->file_handle);
        handle->file_handle = NULL;
    }

#ifdef _WIN32
    if (InterlockedDecrement(&s_trace_ref) == 0)
    {
        etw_provider_dispose(s_etw_handle);
        s_etw_handle = NULL;
    }
    free(handle->service_name);
    /* SRWLOCK は破棄関数不要 */
#else /* !_WIN32 */
    syslog_provider_dispose(handle->syslog_handle);
    if (handle->config_rwlock_initialized)
    {
        pthread_rwlock_destroy(&handle->config_rwlock);
    }
#endif /* _WIN32 */

    free(handle);
}

int TRACE_UTIL_API
    trace_modify_name(trace_provider_t *handle, const char *name, int64_t identifier)
{
    char *effective;

    if (handle == NULL)
    {
        return -1;
    }

    if (identifier < 0)
    {
        return -1;
    }

    config_lock_exclusive(handle);

    if (handle->running)
    {
        config_unlock_exclusive(handle);
        return -1;
    }

    effective = build_effective_name(name, identifier);
    if (effective == NULL)
    {
        config_unlock_exclusive(handle);
        return -1;
    }

#ifdef _WIN32
    free(handle->service_name);
    handle->service_name = effective;
    handle->identifier   = identifier;
    config_unlock_exclusive(handle);
    return 0;
#else /* !_WIN32 */
    {
        int rc = syslog_provider_rename(handle->syslog_handle, effective);
        free(effective);
        if (rc != 0)
        {
            config_unlock_exclusive(handle);
            return -1;
        }
        handle->identifier = identifier;
        config_unlock_exclusive(handle);
        return 0;
    }
#endif /* _WIN32 */
}

int TRACE_UTIL_API
    trace_modify_ostrc(trace_provider_t *handle, enum trace_level level)
{
    if (handle == NULL)
    {
        return -1;
    }

    config_lock_exclusive(handle);

    if (handle->running)
    {
        config_unlock_exclusive(handle);
        return -1;
    }

    handle->os_level = level;
    config_unlock_exclusive(handle);
    return 0;
}

int TRACE_UTIL_API
    trace_modify_filetrc(trace_provider_t *handle, const char *path,
                         enum trace_level level, size_t max_bytes, int generations)
{
    int result = 0;

    if (handle == NULL)
    {
        return -1;
    }

    config_lock_exclusive(handle);

    if (handle->running)
    {
        config_unlock_exclusive(handle);
        return -1;
    }

    /* 既存のファイルプロバイダを解放する */
    if (handle->file_handle != NULL)
    {
        trace_file_provider_dispose(handle->file_handle);
        handle->file_handle = NULL;
    }

    handle->file_level = level;

    /* path が NULL の場合はファイルトレースを無効化して終了 */
    if (path != NULL)
    {
        /* 新しいファイルプロバイダを初期化する */
        handle->file_handle = trace_file_provider_init(path, max_bytes, generations);
        if (handle->file_handle == NULL)
        {
            result = -1;
        }
    }

    config_unlock_exclusive(handle);
    return result;
}
