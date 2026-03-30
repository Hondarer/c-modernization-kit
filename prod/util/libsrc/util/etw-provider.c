#ifdef _WIN32

#include <windows.h>
#include <TraceLoggingProvider.h>
#include <trace-etw-util.h>
#include <stdlib.h>

/**
 *  @brief  ETW プロバイダハンドル構造体 (内部定義)。
 */
struct etw_provider
{
    /** TraceLogging プロバイダ参照。 */
    etw_provider_ref_t provider_ref;
};

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
 *  @brief  "Log" イベントを書き込む。
 */
static void write_trace_event(etw_provider_ref_t ref, int level, const char *message)
{
    switch (level)
    {
    case 1:
        TraceLoggingWrite(ref, "Log",
            TraceLoggingLevel(1), TraceLoggingString(message, "Message"));
        break;
    case 2:
        TraceLoggingWrite(ref, "Log",
            TraceLoggingLevel(2), TraceLoggingString(message, "Message"));
        break;
    case 3:
        TraceLoggingWrite(ref, "Log",
            TraceLoggingLevel(3), TraceLoggingString(message, "Message"));
        break;
    case 4:
        TraceLoggingWrite(ref, "Log",
            TraceLoggingLevel(4), TraceLoggingString(message, "Message"));
        break;
    default:
        TraceLoggingWrite(ref, "Log",
            TraceLoggingLevel(5), TraceLoggingString(message, "Message"));
        break;
    }
}

int TRACE_ETW_UTIL_API
    etw_provider_write(etw_provider_t *handle, int level, const char *message)
{
    if (handle == NULL || message == NULL)
    {
        return 0;
    }

    write_trace_event(handle->provider_ref, level, message);

    return 0;
}

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

#endif /* _WIN32 */
