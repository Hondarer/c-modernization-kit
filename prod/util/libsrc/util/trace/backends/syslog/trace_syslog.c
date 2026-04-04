/**
 *******************************************************************************
 *  @file           trace_syslog.c
 *  @brief          syslog プロバイダ実装ファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  Linux syslog ベースのトレースプロバイダを提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _WIN32

#include <syslog.h>
#include <util/trace/trace_syslog.h>
#include <stdlib.h>
#include <string.h>

#include <trace_syslog_internal.h>

/**
 *  @brief  syslog プロバイダハンドル構造体 (内部定義)。
 */
struct trace_syslog_sink
{
    /** openlog に渡した識別子文字列 (複製を保持)。 */
    char *ident;
};

/* doxygen コメントは、ヘッダに記載 */
trace_syslog_sink_t *TRACE_SYSLOG_API
    trace_syslog_sink_create(const char *ident, int facility)
{
    trace_syslog_sink_t *handle;
    size_t len;

    if (ident == NULL)
    {
        return NULL;
    }

    handle = (trace_syslog_sink_t *)malloc(sizeof(trace_syslog_sink_t));
    if (handle == NULL)
    {
        return NULL;
    }

    /* ident 文字列を複製して保持 (openlog はポインタを保持するため) */
    len = strlen(ident) + 1;
    handle->ident = (char *)malloc(len);
    if (handle->ident == NULL)
    {
        free(handle);
        return NULL;
    }
    memcpy(handle->ident, ident, len);

    openlog(handle->ident, LOG_NDELAY | LOG_PID, facility);

    return handle;
}

/* doxygen コメントは、ヘッダに記載 */
int TRACE_SYSLOG_API
    trace_syslog_sink_write(trace_syslog_sink_t *handle, int level, const char *message)
{
    if (handle == NULL || message == NULL)
    {
        return 0;
    }

    syslog(level, "%s", message);

    return 0;
}

/* doxygen コメントは、ヘッダに記載 */
void TRACE_SYSLOG_API
    trace_syslog_sink_destroy(trace_syslog_sink_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

    closelog();
    free(handle->ident);
    free(handle);
}

/* doxygen コメントは、ヘッダに記載 */
int TRACE_SYSLOG_API
    trace_syslog_sink_rename(trace_syslog_sink_t *handle, const char *new_ident)
{
    char *dup;
    size_t len;

    if (handle == NULL || new_ident == NULL)
    {
        return -1;
    }

    len = strlen(new_ident) + 1;
    dup = (char *)malloc(len);
    if (dup == NULL)
    {
        return -1;
    }
    memcpy(dup, new_ident, len);

    closelog();
    free(handle->ident);
    handle->ident = dup;
    openlog(handle->ident, LOG_NDELAY | LOG_PID, LOG_USER);

    return 0;
}

/* doxygen コメントは、ヘッダに記載 */
void trace_syslog_sink_destroy_on_unload(trace_syslog_sink_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

    closelog();
    free(handle->ident);
    free(handle);
}

#else /* _WIN32 */
    #pragma warning(disable : 4206)
#endif /* _WIN32 */
