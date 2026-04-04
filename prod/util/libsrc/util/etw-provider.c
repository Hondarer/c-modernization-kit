/**
 *******************************************************************************
 *  @file           etw-provider.c
 *  @brief          ETW プロバイダ実装ファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.0.0
 *
 *  Windows TraceLogging ベースの ETW プロバイダを提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifdef _WIN32

#include <windows.h>
#include <TraceLoggingProvider.h>
#pragma comment(lib, "Advapi32.lib")
#include <trace-etw-util.h>
#include <stdlib.h>

#include "etw-provider-internal.h"

/**
 *  @brief  ETW プロバイダハンドル構造体 (内部定義)。
 */
struct etw_provider
{
    /** TraceLogging プロバイダ参照。 */
    etw_provider_ref_t provider_ref;
};

/* doxygen コメントは、ヘッダに記載 */
etw_provider_t *TRACE_ETW_UTIL_API
    etw_provider_init(etw_provider_ref_t provider_ref)
{
    etw_provider_t *handle;
    TLG_STATUS status;

    if (provider_ref == NULL)
    {
        return NULL;
    }

    handle = (etw_provider_t *)malloc(sizeof(etw_provider_t));
    if (handle == NULL)
    {
        return NULL;
    }

    handle->provider_ref = provider_ref;

    status = TraceLoggingRegister(provider_ref);
    if (status != S_OK)
    {
        free(handle);
        return NULL;
    }

    return handle;
}

/**
 *******************************************************************************
 *  @brief          ETW イベントを書き込む内部関数。
 *  @details        service が NULL の場合は Service フィールドを含めない。
 *  @param[in]      ref     TraceLogging プロバイダ参照。
 *  @param[in]      level   トレースレベル (1=Critical 〜 5=Verbose)。
 *  @param[in]      service サービス名 (NULL 可)。NULL の場合 Service フィールドを省略。
 *  @param[in]      message メッセージ文字列。
 *******************************************************************************
 */
static void write_trace_event(etw_provider_ref_t ref, int level,
                               const char *service, const char *message)
{
    if (service != NULL)
    {
        switch (level)
        {
        case 1:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(1),
                TraceLoggingString(service, "Service"),
                TraceLoggingString(message, "Message"));
            break;
        case 2:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(2),
                TraceLoggingString(service, "Service"),
                TraceLoggingString(message, "Message"));
            break;
        case 3:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(3),
                TraceLoggingString(service, "Service"),
                TraceLoggingString(message, "Message"));
            break;
        case 4:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(4),
                TraceLoggingString(service, "Service"),
                TraceLoggingString(message, "Message"));
            break;
        default:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(5),
                TraceLoggingString(service, "Service"),
                TraceLoggingString(message, "Message"));
            break;
        }
    }
    else
    {
        switch (level)
        {
        case 1:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(1),
                TraceLoggingString(message, "Message"));
            break;
        case 2:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(2),
                TraceLoggingString(message, "Message"));
            break;
        case 3:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(3),
                TraceLoggingString(message, "Message"));
            break;
        case 4:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(4),
                TraceLoggingString(message, "Message"));
            break;
        default:
            TraceLoggingWrite(ref, "Trace",
                TraceLoggingLevel(5),
                TraceLoggingString(message, "Message"));
            break;
        }
    }
}

/* doxygen コメントは、ヘッダに記載 */
int TRACE_ETW_UTIL_API
    etw_provider_write(etw_provider_t *handle, int level,
                       const char *service, const char *message)
{
    if (handle == NULL || message == NULL)
    {
        return 0;
    }

    write_trace_event(handle->provider_ref, level, service, message);

    return 0;
}

/* doxygen コメントは、ヘッダに記載 */
void TRACE_ETW_UTIL_API
    etw_provider_dispose(etw_provider_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

    TraceLoggingUnregister(handle->provider_ref);
    free(handle);
}

/* doxygen コメントは、ヘッダに記載 */
void etw_provider_dispose_on_unload(etw_provider_t *handle, int process_terminating)
{
    if (handle == NULL)
    {
        return;
    }

    if (!process_terminating)
    {
        TraceLoggingUnregister(handle->provider_ref);
    }
    free(handle);
}

#endif /* _WIN32 */
