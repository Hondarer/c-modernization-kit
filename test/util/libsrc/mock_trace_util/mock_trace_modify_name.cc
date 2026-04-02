#include <inttypes.h>
#include <testfw.h>
#include <mock_trace_util.h>

WEAK_ATR int trace_modify_name(trace_provider_t *handle, const char *name,
                               int64_t identifier)
{
    int rtc = 0;

    if (_mock_trace_util != nullptr)
    {
        rtc = _mock_trace_util->trace_modify_name(handle, name, identifier);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p, %s, %" PRId64, __func__, (void *)handle, name, identifier);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> %d\n", rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
