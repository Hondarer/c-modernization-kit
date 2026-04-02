#include <testfw.h>
#include <mock_trace_util.h>

WEAK_ATR int trace_modify_stderrtrc(trace_provider_t *handle, enum trace_level level)
{
    int rtc = 0;

    if (_mock_trace_util != nullptr)
    {
        rtc = _mock_trace_util->trace_modify_stderrtrc(handle, level);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p, %d", __func__, (void *)handle, (int)level);
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
