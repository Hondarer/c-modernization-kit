#include <testfw.h>
#include <mock_trace_util.h>

WEAK_ATR enum trace_level trace_get_filetrc(trace_provider_t *handle)
{
    enum trace_level rtc = TRACE_LV_NONE;

    if (_mock_trace_util != nullptr)
    {
        rtc = _mock_trace_util->trace_get_filetrc(handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p", __func__, (void *)handle);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> %d\n", (int)rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
