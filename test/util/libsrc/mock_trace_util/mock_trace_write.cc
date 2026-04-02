#include <testfw.h>
#include <mock_trace_util.h>

WEAK_ATR int trace_write(trace_provider_t *handle, enum trace_level level,
                         const char *message)
{
    int rtc = 0;

    if (_mock_trace_util != nullptr)
    {
        rtc = _mock_trace_util->trace_write(handle, level, message);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p, %d, %s", __func__, (void *)handle, (int)level, message);
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
