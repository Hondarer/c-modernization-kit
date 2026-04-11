#include <testfw.h>
#include <mock_com_util.h>

WEAK_ATR trace_level_t trace_logger_get_file_level(trace_logger_t *handle)
{
    trace_level_t rtc = TRACE_LEVEL_NONE;

    if (_mock_com_util != nullptr)
    {
        rtc = _mock_com_util->trace_logger_get_file_level(handle);
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
