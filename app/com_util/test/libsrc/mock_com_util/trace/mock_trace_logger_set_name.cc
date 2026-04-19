#include <inttypes.h>
#include <testfw.h>
#include <mock_com_util.h>

WEAK_ATR int trace_logger_set_name(trace_logger_t *handle, const char *name,
                               int64_t identifier)
{
    int rtc = 0;

    if (_mock_com_util != nullptr)
    {
        rtc = _mock_com_util->trace_logger_set_name(handle, name, identifier);
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
