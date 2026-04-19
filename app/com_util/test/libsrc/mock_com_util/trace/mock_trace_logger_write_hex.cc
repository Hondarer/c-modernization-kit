#include <testfw.h>
#include <mock_com_util.h>

WEAK_ATR int trace_logger_write_hex(trace_logger_t *handle, trace_level_t level,
                             const void *data, size_t size, const char *message)
{
    int rtc = 0;

    if (_mock_com_util != nullptr)
    {
        rtc = _mock_com_util->trace_logger_write_hex(handle, level, data, size, message);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p, %d, 0x%p, %zu, %s", __func__,
               (void *)handle, (int)level, data, size, message);
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
