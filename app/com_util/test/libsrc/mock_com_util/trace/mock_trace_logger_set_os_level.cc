#include <testfw.h>
#include <mock_com_util.h>

WEAK_ATR int trace_logger_set_os_level(trace_logger_t *handle, trace_level_t level)
{
    int rtc = 0;

    if (_mock_com_util != nullptr)
    {
        rtc = _mock_com_util->trace_logger_set_os_level(handle, level);
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
