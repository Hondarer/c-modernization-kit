/**
 *******************************************************************************
 *  @file           trace-provider.c
 *  @brief          トレースプロバイダ実装ファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  trace_init / trace_dispose / trace_start / trace_stop / trace_write など
 *  公開 API の実装と、内部レジストリ管理機能を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */
#include <trace-util.h>
#include <trace-file-util.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <inttypes.h>

#include "trace-provider-internal.h"
#include "file-provider-internal.h"

#ifdef _WIN32
#include "etw-provider-internal.h"
#else /* !_WIN32 */
#include "syslog-provider-internal.h"
#endif /* _WIN32 */

/* ===== Windows: TraceLogging プロバイダ定義 ===== */

#ifdef _WIN32

#include <windows.h>
#include <TraceLoggingProvider.h>

TRACELOGGING_DEFINE_PROVIDER(
    s_trace_provider,
    TRACE_DEFAULT_PROVIDER_NAME,
    TRACE_DEFAULT_PROVIDER_GUID);

static volatile LONG s_trace_ref = 0;
static etw_provider_t *s_etw_handle = NULL;
static SRWLOCK s_registry_lock = SRWLOCK_INIT;

#else /* !_WIN32 */

#include <time.h>
#include <pthread.h>
#include <syslog.h>
#include <unistd.h>

static pthread_mutex_t s_registry_lock = PTHREAD_MUTEX_INITIALIZER;

#endif /* _WIN32 */

/** プロセス名取得失敗時のフォールバック名。 */
#define FALLBACK_NAME "unknown"

/** registry の初期容量。 */
#define TRACE_REGISTRY_INITIAL_CAPACITY 8

enum trace_handle_state
{
    TRACE_HANDLE_ACTIVE = 0,
    TRACE_HANDLE_DISPOSING,
    TRACE_HANDLE_DISPOSED
};

/**
 *  @brief  トレースプロバイダハンドル構造体 (内部定義)。
 */
struct trace_provider
{
    int64_t identifier;

#ifdef _WIN32
    char *service_name;
#else /* !_WIN32 */
    syslog_provider_t *syslog_handle;
#endif /* _WIN32 */

    trace_file_provider_t *file_handle;

#ifdef _WIN32
    SRWLOCK config_rwlock;
#else /* !_WIN32 */
    pthread_rwlock_t config_rwlock;
#endif /* _WIN32 */

    enum trace_level os_level;
    enum trace_level file_level;
    enum trace_level stderr_level;
    volatile int running;
    volatile int lifecycle_state;

#ifndef _WIN32
    int config_rwlock_initialized;
#endif /* !_WIN32 */
};

struct trace_registry
{
    trace_provider_t **items;
    size_t             count;
    size_t             capacity;
    volatile size_t    shutdown_started;
};

static struct trace_registry s_trace_registry = {0};

/**
 *******************************************************************************
 *  @brief          レジストリの排他ロックを取得する。
 *******************************************************************************
 */
static void registry_lock(void)
{
#ifdef _WIN32
    AcquireSRWLockExclusive(&s_registry_lock);
#else /* !_WIN32 */
    pthread_mutex_lock(&s_registry_lock);
#endif /* _WIN32 */
}

/**
 *******************************************************************************
 *  @brief          レジストリの排他ロックを解放する。
 *******************************************************************************
 */
static void registry_unlock(void)
{
#ifdef _WIN32
    ReleaseSRWLockExclusive(&s_registry_lock);
#else /* !_WIN32 */
    pthread_mutex_unlock(&s_registry_lock);
#endif /* _WIN32 */
}

/**
 *******************************************************************************
 *  @brief          レジストリを拡張する (ロック保持中)。
 *  @return         成功時 0、メモリ確保失敗時 -1。
 *******************************************************************************
 */
static int registry_expand_locked(void)
{
    trace_provider_t **new_items;
    size_t             new_capacity;

    new_capacity = (s_trace_registry.capacity == 0)
        ? TRACE_REGISTRY_INITIAL_CAPACITY
        : s_trace_registry.capacity * 2;

    new_items = (trace_provider_t **)realloc(
        s_trace_registry.items,
        new_capacity * sizeof(trace_provider_t *));
    if (new_items == NULL)
    {
        return -1;
    }

    s_trace_registry.items    = new_items;
    s_trace_registry.capacity = new_capacity;
    return 0;
}

/**
 *******************************************************************************
 *  @brief          ハンドルをレジストリに登録する。
 *  @param[in]      handle  登録するトレースプロバイダハンドル。
 *  @return         成功時 0、シャットダウン中またはメモリ不足時 -1。
 *******************************************************************************
 */
static int registry_register_handle(trace_provider_t *handle)
{
    int rc = 0;

    registry_lock();

    if (s_trace_registry.shutdown_started)
    {
        rc = -1;
    }
    else
    {
        if (s_trace_registry.count == s_trace_registry.capacity)
        {
            rc = registry_expand_locked();
        }
        if (rc == 0)
        {
            s_trace_registry.items[s_trace_registry.count++] = handle;
        }
    }

    registry_unlock();
    return rc;
}

/**
 *******************************************************************************
 *  @brief          ハンドルをレジストリから削除する。
 *  @param[in]      handle  削除するトレースプロバイダハンドル。
 *******************************************************************************
 */
static void registry_unregister_handle(trace_provider_t *handle)
{
    size_t i;

    registry_lock();

    for (i = 0; i < s_trace_registry.count; i++)
    {
        if (s_trace_registry.items[i] == handle)
        {
            s_trace_registry.count--;
            s_trace_registry.items[i] = s_trace_registry.items[s_trace_registry.count];
            s_trace_registry.items[s_trace_registry.count] = NULL;
            break;
        }
    }

    registry_unlock();
}

size_t trace_registry_count(void)
{
    size_t count;

    registry_lock();
    count = s_trace_registry.count;
    registry_unlock();
    return count;
}

size_t trace_registry_capacity(void)
{
    size_t capacity;

    registry_lock();
    capacity = s_trace_registry.capacity;
    registry_unlock();
    return capacity;
}

/**
 *******************************************************************************
 *  @brief          ハンドルがアクティブか判定する。
 *  @param[in]      handle  判定対象のトレースプロバイダハンドル。
 *  @return         アクティブの場合 1、それ以外 0。
 *******************************************************************************
 */
static int handle_is_active(const trace_provider_t *handle)
{
    return handle != NULL
        && handle->lifecycle_state == TRACE_HANDLE_ACTIVE
        && !s_trace_registry.shutdown_started;
}

/**
 *******************************************************************************
 *  @brief          解放処理を開始する。
 *  @param[in]      handle  解放対象のトレースプロバイダハンドル。
 *  @return         成功時 0、ハンドルが NULL またはアクティブでない場合 -1。
 *******************************************************************************
 */
static int begin_dispose(trace_provider_t *handle)
{
    if (handle == NULL || handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        return -1;
    }

    handle->lifecycle_state = TRACE_HANDLE_DISPOSING;
    return 0;
}

#ifdef _WIN32

/**
 *******************************************************************************
 *  @brief          トレースレベルを ETW レベルに変換する。
 *  @param[in]      lv  変換元のトレースレベル。
 *  @return         対応する ETW レベル値。
 *******************************************************************************
 */
static int to_etw_level(enum trace_level lv)
{
    switch (lv)
    {
    case TRACE_LV_CRITICAL: return 1;
    case TRACE_LV_ERROR:    return 2;
    case TRACE_LV_WARNING:  return 3;
    case TRACE_LV_INFO:     return 4;
    default:                return 5;
    }
}

#else /* !_WIN32 */

/**
 *******************************************************************************
 *  @brief          トレースレベルを syslog レベルに変換する。
 *  @param[in]      lv  変換元のトレースレベル。
 *  @return         対応する syslog レベル値。
 *******************************************************************************
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
    case TRACE_LV_NONE:
    default:                return LOG_DEBUG;
    }
}

#endif /* _WIN32 */

#ifdef _WIN32

/**
 *******************************************************************************
 *  @brief          プロセスの実行ファイルパスからベース名を取得する。
 *  @param[in,out]  buf       パス文字列を格納するバッファ。
 *  @param[in]      buf_size  バッファのバイト数。
 *  @return         ベース名へのポインタ。失敗時は FALLBACK_NAME。
 *******************************************************************************
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

/**
 *******************************************************************************
 *  @brief          プロセスの実行ファイルパスからベース名を取得する。
 *  @param[in,out]  buf       パス文字列を格納するバッファ。
 *  @param[in]      buf_size  バッファのバイト数。
 *  @return         ベース名へのポインタ。失敗時は FALLBACK_NAME。
 *******************************************************************************
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
 *******************************************************************************
 *  @brief          設定の排他ロック (書き込みロック) を取得する。
 *  @param[in]      handle  ロック対象のトレースプロバイダハンドル。
 *******************************************************************************
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
 *******************************************************************************
 *  @brief          設定の排他ロック (書き込みロック) を解放する。
 *  @param[in]      handle  ロック解放対象のトレースプロバイダハンドル。
 *******************************************************************************
 */
static void config_unlock_exclusive(trace_provider_t *handle)
{
#ifdef _WIN32
    ReleaseSRWLockExclusive(&handle->config_rwlock);
#else /* !_WIN32 */
    pthread_rwlock_unlock(&handle->config_rwlock);
#endif /* _WIN32 */
}

#define LOCK_TIMEOUT_MS 100

/**
 *******************************************************************************
 *  @brief          タイムアウト付きで設定の共有ロック (読み取りロック) を取得する。
 *  @param[in]      handle  ロック対象のトレースプロバイダハンドル。
 *  @return         成功時 0、タイムアウト時 -1。
 *******************************************************************************
 */
static int config_lock_shared_timed(trace_provider_t *handle)
{
#ifdef _WIN32
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
        abs_timeout.tv_sec++;
        abs_timeout.tv_nsec -= 1000000000L;
    }
    return (pthread_rwlock_timedrdlock(&handle->config_rwlock, &abs_timeout) == 0) ? 0 : -1;
#endif /* _WIN32 */
}

/**
 *******************************************************************************
 *  @brief          設定の共有ロック (読み取りロック) を解放する。
 *  @param[in]      handle  ロック解放対象のトレースプロバイダハンドル。
 *******************************************************************************
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
 *******************************************************************************
 *  @brief          識別子を付加した有効名の文字列を構築する。
 *  @param[in]      name        サービス名 (NULL の場合はプロセスベース名を使用)。
 *  @param[in]      identifier  識別子 (0 の場合はサフィックスなし)。
 *  @return         ヒープ確保された有効名文字列。呼び出し元が free すること。
 *                  失敗時は NULL。
 *******************************************************************************
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
        size_t total = base_len + 1 + (size_t)id_len + 1;

        result = (char *)malloc(total);
        if (result == NULL)
        {
            return NULL;
        }
        snprintf(result, total, "%s-%" PRId64, base, identifier);
        return result;
    }
}

/**
 *******************************************************************************
 *  @brief          クリーンアップのためハンドルのトレース出力を停止する。
 *  @param[in]      handle  停止対象のトレースプロバイダハンドル。
 *  @return         常に 0。
 *******************************************************************************
 */
static int stop_handle_for_cleanup(trace_provider_t *handle)
{
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

/**
 *******************************************************************************
 *  @brief          通常のハンドル解放処理を行う。
 *  @param[in]      handle  解放対象のトレースプロバイダハンドル。
 *******************************************************************************
 */
static void trace_handle_release_normal(trace_provider_t *handle)
{
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
#else /* !_WIN32 */
    syslog_provider_dispose(handle->syslog_handle);
    if (handle->config_rwlock_initialized)
    {
        pthread_rwlock_destroy(&handle->config_rwlock);
    }
#endif /* _WIN32 */

    handle->lifecycle_state = TRACE_HANDLE_DISPOSED;
    free(handle);
}

/**
 *******************************************************************************
 *  @brief          アンロード時のハンドル解放処理を行う。
 *  @param[in]      handle  解放対象のトレースプロバイダハンドル。
 *******************************************************************************
 */
static void trace_handle_release_on_unload(trace_provider_t *handle)
{
    if (handle->file_handle != NULL)
    {
        trace_file_provider_dispose_on_unload(handle->file_handle);
        handle->file_handle = NULL;
    }

#ifdef _WIN32
    free(handle->service_name);
#else /* !_WIN32 */
    syslog_provider_dispose_on_unload(handle->syslog_handle);
#endif /* _WIN32 */

    handle->lifecycle_state = TRACE_HANDLE_DISPOSED;
    free(handle);
}

/* doxygen コメントは、ヘッダに記載 */
trace_provider_t *TRACE_UTIL_API
    trace_init(void)
{
    trace_provider_t *handle;
    char path_buf[256];
    const char *effective_name;

    if (s_trace_registry.shutdown_started)
    {
        return NULL;
    }

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

        handle->identifier      = 0;
        handle->service_name    = svc;
        handle->os_level        = TRACE_DEFAULT_OS_LEVEL;
        handle->file_level      = TRACE_DEFAULT_FILE_LEVEL;
        handle->file_handle     = NULL;
        handle->stderr_level    = TRACE_DEFAULT_STDERR_LEVEL;
        handle->running         = 0;
        handle->lifecycle_state = TRACE_HANDLE_ACTIVE;

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

        handle->identifier                = 0;
        handle->syslog_handle             = sp;
        handle->os_level                  = TRACE_DEFAULT_OS_LEVEL;
        handle->file_level                = TRACE_DEFAULT_FILE_LEVEL;
        handle->file_handle               = NULL;
        handle->stderr_level              = TRACE_DEFAULT_STDERR_LEVEL;
        handle->running                   = 0;
        handle->lifecycle_state           = TRACE_HANDLE_ACTIVE;
        handle->config_rwlock_initialized = 0;

        if (pthread_rwlock_init(&handle->config_rwlock, NULL) != 0)
        {
            syslog_provider_dispose(sp);
            free(handle);
            return NULL;
        }
        handle->config_rwlock_initialized = 1;
    }
#endif /* _WIN32 */

    if (registry_register_handle(handle) != 0)
    {
#ifdef _WIN32
        if (InterlockedDecrement(&s_trace_ref) == 0)
        {
            etw_provider_dispose(s_etw_handle);
            s_etw_handle = NULL;
        }
        free(handle->service_name);
#else /* !_WIN32 */
        syslog_provider_dispose(handle->syslog_handle);
        pthread_rwlock_destroy(&handle->config_rwlock);
#endif /* _WIN32 */
        free(handle);
        return NULL;
    }

    return handle;
}

/* doxygen コメントは、ヘッダに記載 */
int TRACE_UTIL_API
    trace_start(trace_provider_t *handle)
{
    if (!handle_is_active(handle))
    {
        return -1;
    }

    config_lock_exclusive(handle);
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        config_unlock_exclusive(handle);
        return -1;
    }
    if (handle->running)
    {
        config_unlock_exclusive(handle);
        return 0;
    }

    handle->running = 1;
    config_unlock_exclusive(handle);
    return 0;
}

/* doxygen コメントは、ヘッダに記載 */
int TRACE_UTIL_API
    trace_stop(trace_provider_t *handle)
{
    if (!handle_is_active(handle))
    {
        return -1;
    }

    return stop_handle_for_cleanup(handle);
}

#define MAX_BODY (TRACE_MESSAGE_MAX_BYTES - 1)

/**
 *******************************************************************************
 *  @brief          UTF-8 文字境界を考慮して文字列を切り詰める位置を返す。
 *  @param[in]      s    切り詰め対象の UTF-8 文字列。
 *  @param[in]      pos  切り詰め開始位置 (バイト単位)。
 *  @return         文字境界に合わせた切り詰め位置。
 *******************************************************************************
 */
static size_t utf8_safe_truncate(const char *s, size_t pos)
{
    while (pos > 0 && ((unsigned char)s[pos] & 0xC0) == 0x80)
    {
        pos--;
    }
    return pos;
}

/**
 *******************************************************************************
 *  @brief          OS トレースプロバイダ (ETW または syslog) にメッセージを書き込む。
 *  @param[in]      handle  書き込み先のトレースプロバイダハンドル。
 *  @param[in]      level   トレースレベル。
 *  @param[in]      msg     書き込むメッセージ文字列。
 *  @return         成功時 0、失敗時 -1。
 *******************************************************************************
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
 *******************************************************************************
 *  @brief          メッセージを出力すべきか判定する。
 *  @param[in]      msg_level   出力するメッセージのトレースレベル。
 *  @param[in]      threshold   出力閾値となるトレースレベル。
 *  @return         出力すべき場合 1、出力不要の場合 0。
 *******************************************************************************
 */
static int should_output(enum trace_level msg_level, enum trace_level threshold)
{
    if (threshold == TRACE_LV_NONE)
    {
        return 0;
    }
    return (int)msg_level <= (int)threshold;
}

#define STDERR_TS_BUF_SIZE 24

/**
 *******************************************************************************
 *  @brief          タイムスタンプとトレースレベルを付加して stderr にエントリを書き込む。
 *  @param[in]      level  トレースレベル。
 *  @param[in]      msg    書き込むメッセージ文字列。
 *******************************************************************************
 */
static void write_stderr_entry(enum trace_level level, const char *msg)
{
    char ts[STDERR_TS_BUF_SIZE];
    static const char lc_table[] = {'C', 'E', 'W', 'I', 'V'};
    char lc;

#ifdef _WIN32
    SYSTEMTIME st;
    GetSystemTime(&st);
    snprintf(ts, sizeof(ts),
             "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             (int)st.wYear, (int)st.wMonth, (int)st.wDay,
             (int)st.wHour, (int)st.wMinute, (int)st.wSecond,
             (int)st.wMilliseconds);
#else /* !_WIN32 */
    struct timespec tsp;
    struct tm tm_val;
    clock_gettime(CLOCK_REALTIME, &tsp);
    gmtime_r(&tsp.tv_sec, &tm_val);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
    snprintf(ts, sizeof(ts),
             "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             tm_val.tm_year + 1900, tm_val.tm_mon + 1, tm_val.tm_mday,
             tm_val.tm_hour, tm_val.tm_min, tm_val.tm_sec,
             (int)(tsp.tv_nsec / 1000000));
#pragma GCC diagnostic pop
#endif /* _WIN32 */

    lc = ((int)level < (int)TRACE_LV_NONE) ? lc_table[(int)level] : 'V';
    fprintf(stderr, "%s %c %s\n", ts, lc, msg);
}

/**
 *******************************************************************************
 *  @brief          OS プロバイダ・ファイル・stderr の各出力先にメッセージを書き込む。
 *  @param[in]      handle  書き込み先のトレースプロバイダハンドル。
 *  @param[in]      level   トレースレベル。
 *  @param[in]      msg     書き込むメッセージ文字列。
 *  @return         全出力先で成功時 0、いずれかで失敗時 -1。
 *******************************************************************************
 */
static int write_dual(trace_provider_t *handle, enum trace_level level, const char *msg)
{
    int os_result   = 0;
    int file_result = 0;

    if (should_output(level, handle->os_level))
    {
        os_result = write_to_provider(handle, level, msg);
    }

    if (handle->file_handle != NULL && should_output(level, handle->file_level))
    {
        file_result = trace_file_provider_write(handle->file_handle, (int)level, msg);
    }

    if (should_output(level, handle->stderr_level))
    {
        write_stderr_entry(level, msg);
    }

    return (os_result != 0 || file_result != 0) ? -1 : 0;
}

/* doxygen コメントは、ヘッダに記載 */
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
    if (!handle_is_active(handle))
    {
        return -1;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return -1;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE || !handle->running)
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

/* doxygen コメントは、ヘッダに記載 */
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
    if (!handle_is_active(handle))
    {
        return -1;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return -1;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE || !handle->running)
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

static const char hex_chars[] = "0123456789ABCDEF";
#define ELLIPSIS_LEN 3

/**
 *******************************************************************************
 *  @brief          16 進ダンプメッセージを構築して出力先に書き込む内部実装。
 *  @param[in]      handle  書き込み先のトレースプロバイダハンドル。
 *  @param[in]      level   トレースレベル。
 *  @param[in]      data    ダンプ対象のバイト列。
 *  @param[in]      size    バイト列のサイズ。
 *  @param[in]      label   メッセージに付加するラベル (NULL 可)。
 *  @return         成功時 0、失敗時 -1。
 *******************************************************************************
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

    if (label != NULL && label[0] != '\0')
    {
        size_t lbl_len = strlen(label);
        if (lbl_len + 2 >= MAX_BODY)
        {
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

    remaining = MAX_BODY - pos;
    max_data_bytes = (remaining + 1) / 3;

    if (size > max_data_bytes)
    {
        truncated = 1;
        if (remaining < ELLIPSIS_LEN)
        {
            buf[pos] = '\0';
            return write_dual(handle, level, buf);
        }
        max_data_bytes = (remaining - ELLIPSIS_LEN) / 3;
        if (max_data_bytes == 0)
        {
            memcpy(buf + pos, "...", ELLIPSIS_LEN);
            pos += ELLIPSIS_LEN;
            buf[pos] = '\0';
            return write_dual(handle, level, buf);
        }
        size = max_data_bytes;
    }

    for (i = 0; i < size; i++)
    {
        if (i > 0)
        {
            buf[pos++] = ' ';
        }
        buf[pos++] = hex_chars[(bytes[i] >> 4) & 0x0F];
        buf[pos++] = hex_chars[bytes[i] & 0x0F];
    }

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

/* doxygen コメントは、ヘッダに記載 */
int TRACE_UTIL_API
    trace_hex_write(trace_provider_t *handle, enum trace_level level,
                    const void *data, size_t size, const char *message)
{
    int ret;

    if (handle == NULL || data == NULL || size == 0)
    {
        return 0;
    }
    if (!handle_is_active(handle))
    {
        return -1;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return -1;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE || !handle->running)
    {
        config_unlock_shared(handle);
        return -1;
    }

    ret = hex_write_impl(handle, level, data, size, message);
    config_unlock_shared(handle);
    return ret;
}

/* doxygen コメントは、ヘッダに記載 */
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
    if (!handle_is_active(handle))
    {
        return -1;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return -1;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE || !handle->running)
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

/* doxygen コメントは、ヘッダに記載 */
void TRACE_UTIL_API
    trace_dispose(trace_provider_t *handle)
{
    if (handle == NULL)
    {
        return;
    }
    if (begin_dispose(handle) != 0)
    {
        return;
    }

    registry_unregister_handle(handle);
    stop_handle_for_cleanup(handle);
    trace_handle_release_normal(handle);
}

/* doxygen コメントは、ヘッダに記載 */
int TRACE_UTIL_API
    trace_modify_name(trace_provider_t *handle, const char *name, int64_t identifier)
{
    char *effective;

    if (!handle_is_active(handle))
    {
        return -1;
    }
    if (identifier < 0)
    {
        return -1;
    }

    config_lock_exclusive(handle);
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE || handle->running)
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

/* doxygen コメントは、ヘッダに記載 */
int TRACE_UTIL_API
    trace_modify_ostrc(trace_provider_t *handle, enum trace_level level)
{
    if (!handle_is_active(handle))
    {
        return -1;
    }

    config_lock_exclusive(handle);
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE || handle->running)
    {
        config_unlock_exclusive(handle);
        return -1;
    }

    handle->os_level = level;
    config_unlock_exclusive(handle);
    return 0;
}

/* doxygen コメントは、ヘッダに記載 */
int TRACE_UTIL_API
    trace_modify_filetrc(trace_provider_t *handle, const char *path,
                         enum trace_level level, size_t max_bytes, int generations)
{
    int result = 0;

    if (!handle_is_active(handle))
    {
        return -1;
    }

    config_lock_exclusive(handle);
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE || handle->running)
    {
        config_unlock_exclusive(handle);
        return -1;
    }

    if (handle->file_handle != NULL)
    {
        trace_file_provider_dispose(handle->file_handle);
        handle->file_handle = NULL;
    }

    handle->file_level = level;
    if (path != NULL)
    {
        handle->file_handle = trace_file_provider_init(path, max_bytes, generations);
        if (handle->file_handle == NULL)
        {
            result = -1;
        }
    }

    config_unlock_exclusive(handle);
    return result;
}

/* doxygen コメントは、ヘッダに記載 */
int TRACE_UTIL_API
    trace_modify_stderrtrc(trace_provider_t *handle, enum trace_level level)
{
    if (!handle_is_active(handle))
    {
        return -1;
    }

    config_lock_exclusive(handle);
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE || handle->running)
    {
        config_unlock_exclusive(handle);
        return -1;
    }

    handle->stderr_level = level;
    config_unlock_exclusive(handle);
    return 0;
}

/* doxygen コメントは、ヘッダに記載 */
enum trace_level TRACE_UTIL_API
    trace_get_ostrc(trace_provider_t *handle)
{
    enum trace_level lv;

    if (!handle_is_active(handle))
    {
        return TRACE_LV_NONE;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return TRACE_LV_NONE;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        config_unlock_shared(handle);
        return TRACE_LV_NONE;
    }
    lv = handle->os_level;
    config_unlock_shared(handle);
    return lv;
}

/* doxygen コメントは、ヘッダに記載 */
enum trace_level TRACE_UTIL_API
    trace_get_filetrc(trace_provider_t *handle)
{
    enum trace_level lv;

    if (!handle_is_active(handle))
    {
        return TRACE_LV_NONE;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return TRACE_LV_NONE;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        config_unlock_shared(handle);
        return TRACE_LV_NONE;
    }
    lv = handle->file_level;
    config_unlock_shared(handle);
    return lv;
}

/* doxygen コメントは、ヘッダに記載 */
enum trace_level TRACE_UTIL_API
    trace_get_stderrtrc(trace_provider_t *handle)
{
    enum trace_level lv;

    if (!handle_is_active(handle))
    {
        return TRACE_LV_NONE;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return TRACE_LV_NONE;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        config_unlock_shared(handle);
        return TRACE_LV_NONE;
    }
    lv = handle->stderr_level;
    config_unlock_shared(handle);
    return lv;
}

void trace_registry_dispose_all_on_unload(int process_terminating)
{
    trace_provider_t **items;
    size_t count;
    size_t i;

#ifndef _WIN32
    (void)process_terminating;
#endif /* !_WIN32 */

    if (s_trace_registry.shutdown_started)
    {
        return;
    }

    s_trace_registry.shutdown_started = 1;
    items = s_trace_registry.items;
    count = s_trace_registry.count;

    s_trace_registry.items = NULL;
    s_trace_registry.count = 0;
    s_trace_registry.capacity = 0;

    for (i = 0; i < count; i++)
    {
        trace_provider_t *handle = items[i];

        if (handle == NULL || handle->lifecycle_state == TRACE_HANDLE_DISPOSED)
        {
            continue;
        }

        handle->lifecycle_state = TRACE_HANDLE_DISPOSING;
        handle->running = 0;
        trace_handle_release_on_unload(handle);
    }

#ifdef _WIN32
    if (s_etw_handle != NULL)
    {
        etw_provider_dispose_on_unload(s_etw_handle, process_terminating);
        s_etw_handle = NULL;
    }
    s_trace_ref = 0;
#endif /* _WIN32 */

    free(items);
}
