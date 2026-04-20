/**
 *******************************************************************************
 *  @file           trace.c
 *  @brief          トレースプロバイダ実装ファイル。
 *  @author         Tetsuo Honda
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  trace_logger_create / trace_logger_destroy / trace_logger_start / trace_logger_stop / trace_logger_write など
 *  公開 API の実装と、内部レジストリ管理機能を提供します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */
#include <com_util/clock/clock.h>
#include <com_util/trace/trace.h>
#include <com_util/trace/trace_file.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <inttypes.h>

#include "trace_internal.h"
#include "backends/file/trace_file_internal.h"

#if defined(PLATFORM_LINUX)
#include "backends/syslog/trace_syslog_internal.h"
#elif defined(PLATFORM_WINDOWS)
#include "backends/etw/trace_etw_internal.h"
#endif /* PLATFORM_ */

/* ===== Windows: TraceLogging プロバイダ定義 ===== */

#if defined(PLATFORM_WINDOWS)

#include <com_util/base/windows_sdk.h>
#include <TraceLoggingProvider.h>
#pragma comment(lib, "Advapi32.lib")

TRACELOGGING_DEFINE_PROVIDER(
    s_trace_provider_ref,
    TRACE_LOGGER_DEFAULT_PROVIDER_NAME,
    TRACE_LOGGER_DEFAULT_PROVIDER_GUID);

static volatile LONG s_trace_ref = 0;
static trace_etw_provider_t *s_etw_handle = NULL;
static SRWLOCK s_registry_lock = SRWLOCK_INIT;

#elif defined(PLATFORM_LINUX)

#include <pthread.h>
#include <syslog.h>
#include <unistd.h>

static pthread_mutex_t s_registry_lock = PTHREAD_MUTEX_INITIALIZER;

#endif /* PLATFORM_ */

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
struct trace_logger
{
    int64_t identifier;

#if defined(PLATFORM_LINUX)
    trace_syslog_sink_t *syslog_handle;
#elif defined(PLATFORM_WINDOWS)
    char *service_name;
#endif /* PLATFORM_ */

    trace_file_sink_t *file_handle;

#if defined(PLATFORM_LINUX)
    pthread_rwlock_t config_rwlock;
#elif defined(PLATFORM_WINDOWS)
    SRWLOCK config_rwlock;
#endif /* PLATFORM_ */

    trace_level_t os_level;
    trace_level_t file_level;
    trace_level_t stderr_level;
    volatile int running;
    volatile int lifecycle_state;

#if defined(PLATFORM_LINUX)
    int config_rwlock_initialized;
#endif /* PLATFORM_LINUX */
};

struct trace_registry
{
    trace_logger_t **items;
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
#if defined(PLATFORM_LINUX)
    pthread_mutex_lock(&s_registry_lock);
#elif defined(PLATFORM_WINDOWS)
    AcquireSRWLockExclusive(&s_registry_lock);
#endif /* PLATFORM_ */
}

/**
 *******************************************************************************
 *  @brief          レジストリの排他ロックを解放する。
 *******************************************************************************
 */
static void registry_unlock(void)
{
#if defined(PLATFORM_LINUX)
    pthread_mutex_unlock(&s_registry_lock);
#elif defined(PLATFORM_WINDOWS)
    ReleaseSRWLockExclusive(&s_registry_lock);
#endif /* PLATFORM_ */
}

/**
 *******************************************************************************
 *  @brief          レジストリを拡張する (ロック保持中)。
 *  @return         成功時 0、メモリ確保失敗時 -1。
 *******************************************************************************
 */
static int registry_expand_locked(void)
{
    trace_logger_t **new_items;
    size_t             new_capacity;

    new_capacity = (s_trace_registry.capacity == 0)
        ? TRACE_REGISTRY_INITIAL_CAPACITY
        : s_trace_registry.capacity * 2;

    new_items = (trace_logger_t **)realloc(
        s_trace_registry.items,
        new_capacity * sizeof(trace_logger_t *));
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
static int registry_register_handle(trace_logger_t *handle)
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
static void registry_unregister_handle(trace_logger_t *handle)
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
static int handle_is_active(const trace_logger_t *handle)
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
static int begin_dispose(trace_logger_t *handle)
{
    if (handle == NULL || handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        return -1;
    }

    handle->lifecycle_state = TRACE_HANDLE_DISPOSING;
    return 0;
}

#if defined(PLATFORM_LINUX)

/**
 *******************************************************************************
 *  @brief          トレースレベルを syslog レベルに変換する。
 *  @param[in]      lv  変換元のトレースレベル。
 *  @return         対応する syslog レベル値。
 *******************************************************************************
 */
static int to_syslog_level(trace_level_t lv)
{
    switch (lv)
    {
    case TRACE_LEVEL_CRITICAL: return LOG_CRIT;
    case TRACE_LEVEL_ERROR:    return LOG_ERR;
    case TRACE_LEVEL_WARNING:  return LOG_WARNING;
    case TRACE_LEVEL_INFO:     return LOG_INFO;
    case TRACE_LEVEL_VERBOSE:  return LOG_DEBUG;
    case TRACE_LEVEL_DEBUG:    return LOG_DEBUG;
    case TRACE_LEVEL_NONE:
    default:                return LOG_DEBUG;
    }
}

#elif defined(PLATFORM_WINDOWS)

/**
 *******************************************************************************
 *  @brief          トレースレベルを ETW レベルに変換する。
 *  @param[in]      lv  変換元のトレースレベル。
 *  @return         対応する ETW レベル値。
 *******************************************************************************
 */
static int to_etw_level(trace_level_t lv)
{
    switch (lv)
    {
    case TRACE_LEVEL_CRITICAL: return 1;
    case TRACE_LEVEL_ERROR:    return 2;
    case TRACE_LEVEL_WARNING:  return 3;
    case TRACE_LEVEL_INFO:     return 4;
    case TRACE_LEVEL_VERBOSE:  return 5;
    case TRACE_LEVEL_DEBUG:    return 5;
    default:                return 5;
    }
}

#endif /* PLATFORM_ */

#if defined(PLATFORM_LINUX)

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

#elif defined(PLATFORM_WINDOWS)

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

#endif /* PLATFORM_ */

/**
 *******************************************************************************
 *  @brief          設定の排他ロック (書き込みロック) を取得する。
 *  @param[in]      handle  ロック対象のトレースプロバイダハンドル。
 *******************************************************************************
 */
static void config_lock_exclusive(trace_logger_t *handle)
{
#if defined(PLATFORM_LINUX)
    pthread_rwlock_wrlock(&handle->config_rwlock);
#elif defined(PLATFORM_WINDOWS)
    AcquireSRWLockExclusive(&handle->config_rwlock);
#endif /* PLATFORM_ */
}

/**
 *******************************************************************************
 *  @brief          設定の排他ロック (書き込みロック) を解放する。
 *  @param[in]      handle  ロック解放対象のトレースプロバイダハンドル。
 *******************************************************************************
 */
static void config_unlock_exclusive(trace_logger_t *handle)
{
#if defined(PLATFORM_LINUX)
    pthread_rwlock_unlock(&handle->config_rwlock);
#elif defined(PLATFORM_WINDOWS)
    ReleaseSRWLockExclusive(&handle->config_rwlock);
#endif /* PLATFORM_ */
}

#define LOCK_TIMEOUT_MS 100

/**
 *******************************************************************************
 *  @brief          タイムアウト付きで設定の共有ロック (読み取りロック) を取得する。
 *  @param[in]      handle  ロック対象のトレースプロバイダハンドル。
 *  @return         成功時 0、タイムアウト時 -1。
 *******************************************************************************
 */
static int config_lock_shared_timed(trace_logger_t *handle)
{
#if defined(PLATFORM_LINUX)
    struct timespec abs_timeout;
    clock_get_realtime_deadline_ms(LOCK_TIMEOUT_MS, &abs_timeout);
    return (pthread_rwlock_timedrdlock(&handle->config_rwlock, &abs_timeout) == 0) ? 0 : -1;
#elif defined(PLATFORM_WINDOWS)
    uint64_t deadline = clock_get_monotonic_ms() + (uint64_t)LOCK_TIMEOUT_MS;
    while (!TryAcquireSRWLockShared(&handle->config_rwlock))
    {
        if (clock_get_monotonic_ms() >= deadline)
        {
            return -1;
        }
        SwitchToThread();
    }
    return 0;
#endif /* PLATFORM_ */
}

/**
 *******************************************************************************
 *  @brief          設定の共有ロック (読み取りロック) を解放する。
 *  @param[in]      handle  ロック解放対象のトレースプロバイダハンドル。
 *******************************************************************************
 */
static void config_unlock_shared(trace_logger_t *handle)
{
#if defined(PLATFORM_LINUX)
    pthread_rwlock_unlock(&handle->config_rwlock);
#elif defined(PLATFORM_WINDOWS)
    ReleaseSRWLockShared(&handle->config_rwlock);
#endif /* PLATFORM_ */
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
#if defined(PLATFORM_LINUX)
        return strdup(base);
#elif defined(PLATFORM_WINDOWS)
        return _strdup(base);
#endif /* PLATFORM_ */
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
static int stop_handle_for_cleanup(trace_logger_t *handle)
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
static void trace_handle_release_normal(trace_logger_t *handle)
{
    if (handle->file_handle != NULL)
    {
        trace_file_sink_destroy(handle->file_handle);
        handle->file_handle = NULL;
    }

#if defined(PLATFORM_LINUX)
    trace_syslog_sink_destroy(handle->syslog_handle);
    if (handle->config_rwlock_initialized)
    {
        pthread_rwlock_destroy(&handle->config_rwlock);
    }
#elif defined(PLATFORM_WINDOWS)
    if (InterlockedDecrement(&s_trace_ref) == 0)
    {
        trace_etw_provider_destroy(s_etw_handle);
        s_etw_handle = NULL;
    }
    free(handle->service_name);
#endif /* PLATFORM_ */

    handle->lifecycle_state = TRACE_HANDLE_DISPOSED;
    free(handle);
}

/**
 *******************************************************************************
 *  @brief          アンロード時のハンドル解放処理を行う。
 *  @param[in]      handle  解放対象のトレースプロバイダハンドル。
 *******************************************************************************
 */
static void trace_handle_release_on_unload(trace_logger_t *handle)
{
    if (handle->file_handle != NULL)
    {
        trace_file_sink_destroy_on_unload(handle->file_handle);
        handle->file_handle = NULL;
    }

#if defined(PLATFORM_LINUX)
    trace_syslog_sink_destroy_on_unload(handle->syslog_handle);
#elif defined(PLATFORM_WINDOWS)
    free(handle->service_name);
#endif /* PLATFORM_ */

    handle->lifecycle_state = TRACE_HANDLE_DISPOSED;
    free(handle);
}

/* doxygen コメントは、ヘッダに記載 */
    COM_UTIL_EXPORT trace_logger_t *COM_UTIL_API
    trace_logger_create(void)
{
    trace_logger_t *handle;
    char path_buf[256];
    const char *effective_name;

    if (s_trace_registry.shutdown_started)
    {
        return NULL;
    }

    effective_name = get_process_basename(path_buf, sizeof(path_buf));

#if defined(PLATFORM_LINUX)
    {
        trace_syslog_sink_t *sp;

        sp = trace_syslog_sink_create(effective_name, LOG_USER);
        if (sp == NULL)
        {
            return NULL;
        }

        handle = (trace_logger_t *)malloc(sizeof(trace_logger_t));
        if (handle == NULL)
        {
            trace_syslog_sink_destroy(sp);
            return NULL;
        }

        handle->identifier                = 0;
        handle->syslog_handle             = sp;
        handle->os_level                  = TRACE_LOGGER_DEFAULT_OS_LEVEL;
        handle->file_level                = TRACE_LOGGER_DEFAULT_FILE_LEVEL;
        handle->file_handle               = NULL;
        handle->stderr_level              = TRACE_LOGGER_DEFAULT_STDERR_LEVEL;
        handle->running                   = 0;
        handle->lifecycle_state           = TRACE_HANDLE_ACTIVE;
        handle->config_rwlock_initialized = 0;

        if (pthread_rwlock_init(&handle->config_rwlock, NULL) != 0)
        {
            trace_syslog_sink_destroy(sp);
            free(handle);
            return NULL;
        }
        handle->config_rwlock_initialized = 1;
    }
#elif defined(PLATFORM_WINDOWS)
    {
        char *svc;

        svc = _strdup(effective_name);
        if (svc == NULL)
        {
            return NULL;
        }

        handle = (trace_logger_t *)malloc(sizeof(trace_logger_t));
        if (handle == NULL)
        {
            free(svc);
            return NULL;
        }

        handle->identifier      = 0;
        handle->service_name    = svc;
        handle->os_level        = TRACE_LOGGER_DEFAULT_OS_LEVEL;
        handle->file_level      = TRACE_LOGGER_DEFAULT_FILE_LEVEL;
        handle->file_handle     = NULL;
        handle->stderr_level    = TRACE_LOGGER_DEFAULT_STDERR_LEVEL;
        handle->running         = 0;
        handle->lifecycle_state = TRACE_HANDLE_ACTIVE;

        InitializeSRWLock(&handle->config_rwlock);

        if (InterlockedIncrement(&s_trace_ref) == 1)
        {
            s_etw_handle = trace_etw_provider_create(s_trace_provider_ref);
            if (s_etw_handle == NULL)
            {
                InterlockedDecrement(&s_trace_ref);
                free(handle->service_name);
                free(handle);
                return NULL;
            }
        }
    }
#endif /* PLATFORM_ */

    if (registry_register_handle(handle) != 0)
    {
#if defined(PLATFORM_LINUX)
        trace_syslog_sink_destroy(handle->syslog_handle);
        pthread_rwlock_destroy(&handle->config_rwlock);
#elif defined(PLATFORM_WINDOWS)
        if (InterlockedDecrement(&s_trace_ref) == 0)
        {
            trace_etw_provider_destroy(s_etw_handle);
            s_etw_handle = NULL;
        }
        free(handle->service_name);
#endif /* PLATFORM_ */
        free(handle);
        return NULL;
    }

    return handle;
}

/* doxygen コメントは、ヘッダに記載 */
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_start(trace_logger_t *handle)
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
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_stop(trace_logger_t *handle)
{
    if (!handle_is_active(handle))
    {
        return -1;
    }

    return stop_handle_for_cleanup(handle);
}

#define MAX_BODY (TRACE_LOGGER_MESSAGE_MAX_BYTES - 1)

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
static int write_to_provider(trace_logger_t *handle, trace_level_t level, const char *msg)
{
#if defined(PLATFORM_LINUX)
    return trace_syslog_sink_write(handle->syslog_handle, to_syslog_level(level), msg);
#elif defined(PLATFORM_WINDOWS)
    return trace_etw_provider_write(s_etw_handle, to_etw_level(level),
                              handle->service_name, msg);
#endif /* PLATFORM_ */
}

/**
 *******************************************************************************
 *  @brief          メッセージを出力すべきか判定する。
 *  @param[in]      msg_level   出力するメッセージのトレースレベル。
 *  @param[in]      threshold   出力閾値となるトレースレベル。
 *  @return         出力すべき場合 1、出力不要の場合 0。
 *******************************************************************************
 */
static int should_output(trace_level_t msg_level, trace_level_t threshold)
{
    if (threshold == TRACE_LEVEL_NONE)
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
static void write_stderr_entry(trace_level_t level, const char *msg)
{
    char ts[STDERR_TS_BUF_SIZE];
    static const char lc_table[] = {'C', 'E', 'W', 'I', 'V', 'D'};
    char lc;
    struct tm utc_tm;
    int32_t tv_nsec;

    clock_get_realtime_utc(&utc_tm, &tv_nsec);

#if defined(COMPILER_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif /* COMPILER_GCC */
    snprintf(ts, sizeof(ts),
             "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             utc_tm.tm_year + 1900, utc_tm.tm_mon + 1, utc_tm.tm_mday,
             utc_tm.tm_hour, utc_tm.tm_min, utc_tm.tm_sec,
             (int)(tv_nsec / 1000000));
#if defined(COMPILER_GCC)
#pragma GCC diagnostic pop
#endif /* COMPILER_GCC */

    lc = ((int)level >= 0 && (int)level < (int)TRACE_LEVEL_NONE) ? lc_table[(int)level] : 'D';
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
static int write_dual(trace_logger_t *handle, trace_level_t level, const char *msg)
{
    int os_result   = 0;
    int file_result = 0;

    if (should_output(level, handle->os_level))
    {
        os_result = write_to_provider(handle, level, msg);
    }

    if (handle->file_handle != NULL && should_output(level, handle->file_level))
    {
        file_result = trace_file_sink_write(handle->file_handle, (int)level, msg);
    }

    if (should_output(level, handle->stderr_level))
    {
        write_stderr_entry(level, msg);
    }

    return (os_result != 0 || file_result != 0) ? -1 : 0;
}

/* doxygen コメントは、ヘッダに記載 */
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_write(trace_logger_t *handle, trace_level_t level, const char *message)
{
    const char *msg;
    char buf[TRACE_LOGGER_MESSAGE_MAX_BYTES];
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
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_writef(trace_logger_t *handle, trace_level_t level, const char *format, ...)
{
    va_list args;
    char buf[TRACE_LOGGER_MESSAGE_MAX_BYTES];
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
static int hex_write_impl(trace_logger_t *handle, trace_level_t level,
                          const void *data, size_t size, const char *label)
{
    char buf[TRACE_LOGGER_MESSAGE_MAX_BYTES];
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
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_write_hex(trace_logger_t *handle, trace_level_t level,
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
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_write_hexf(trace_logger_t *handle, trace_level_t level,
                     const void *data, size_t size, const char *format, ...)
{
    char label[TRACE_LOGGER_MESSAGE_MAX_BYTES];
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
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_set_name(trace_logger_t *handle, const char *name, int64_t identifier)
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

#if defined(PLATFORM_LINUX)
    {
        int rc = trace_syslog_sink_rename(handle->syslog_handle, effective);
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
#elif defined(PLATFORM_WINDOWS)
    free(handle->service_name);
    handle->service_name = effective;
    handle->identifier   = identifier;
    config_unlock_exclusive(handle);
    return 0;
#endif /* PLATFORM_ */
}

/* doxygen コメントは、ヘッダに記載 */
    COM_UTIL_EXPORT trace_level_t COM_UTIL_API
    trace_logger_get_os_level(trace_logger_t *handle)
{
    trace_level_t lv;

    if (!handle_is_active(handle))
    {
        return TRACE_LEVEL_NONE;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return TRACE_LEVEL_NONE;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        config_unlock_shared(handle);
        return TRACE_LEVEL_NONE;
    }
    lv = handle->os_level;
    config_unlock_shared(handle);
    return lv;
}

/* doxygen コメントは、ヘッダに記載 */
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_set_os_level(trace_logger_t *handle, trace_level_t level)
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
    COM_UTIL_EXPORT trace_level_t COM_UTIL_API
    trace_logger_get_file_level(trace_logger_t *handle)
{
    trace_level_t lv;

    if (!handle_is_active(handle))
    {
        return TRACE_LEVEL_NONE;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return TRACE_LEVEL_NONE;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        config_unlock_shared(handle);
        return TRACE_LEVEL_NONE;
    }
    lv = handle->file_level;
    config_unlock_shared(handle);
    return lv;
}

/* doxygen コメントは、ヘッダに記載 */
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_set_file_level(trace_logger_t *handle, const char *path,
                         trace_level_t level, size_t max_bytes, int generations)
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
        trace_file_sink_destroy(handle->file_handle);
        handle->file_handle = NULL;
    }

    handle->file_level = level;
    if (path != NULL)
    {
        handle->file_handle = trace_file_sink_create(path, max_bytes, generations);
        if (handle->file_handle == NULL)
        {
            result = -1;
        }
    }

    config_unlock_exclusive(handle);
    return result;
}

/* doxygen コメントは、ヘッダに記載 */
    COM_UTIL_EXPORT trace_level_t COM_UTIL_API
    trace_logger_get_stderr_level(trace_logger_t *handle)
{
    trace_level_t lv;

    if (!handle_is_active(handle))
    {
        return TRACE_LEVEL_NONE;
    }
    if (config_lock_shared_timed(handle) != 0)
    {
        return TRACE_LEVEL_NONE;
    }
    if (handle->lifecycle_state != TRACE_HANDLE_ACTIVE)
    {
        config_unlock_shared(handle);
        return TRACE_LEVEL_NONE;
    }
    lv = handle->stderr_level;
    config_unlock_shared(handle);
    return lv;
}

/* doxygen コメントは、ヘッダに記載 */
    COM_UTIL_EXPORT int COM_UTIL_API
    trace_logger_set_stderr_level(trace_logger_t *handle, trace_level_t level)
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
    COM_UTIL_EXPORT void COM_UTIL_API
    trace_logger_destroy(trace_logger_t *handle)
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

void trace_registry_dispose_all_on_unload(int process_terminating)
{
    trace_logger_t **items;
    size_t count;
    size_t i;

#if defined(PLATFORM_LINUX)
    (void)process_terminating;
#endif /* PLATFORM_LINUX */

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
        trace_logger_t *handle = items[i];

        if (handle == NULL || handle->lifecycle_state == TRACE_HANDLE_DISPOSED)
        {
            continue;
        }

        handle->lifecycle_state = TRACE_HANDLE_DISPOSING;
        handle->running = 0;
        trace_handle_release_on_unload(handle);
    }

#if defined(PLATFORM_WINDOWS)
    if (s_etw_handle != NULL)
    {
        trace_etw_provider_destroy_on_unload(s_etw_handle, process_terminating);
        s_etw_handle = NULL;
    }
    s_trace_ref = 0;
#endif /* PLATFORM_WINDOWS */

    free(items);
}
