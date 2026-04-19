#include <testfw.h>
#include <mock_com_util.h>

WEAK_ATR int trace_logger_set_file_level(trace_logger_t *handle, const char *path,
                                  trace_level_t level, size_t max_bytes,
                                  int generations)
{
    int rtc = 0;

    if (_mock_com_util != nullptr)
    {
        rtc = _mock_com_util->trace_logger_set_file_level(handle, path, level, max_bytes,
                                                     generations);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p, %s, %d, %zu, %d", __func__,
               (void *)handle, path, (int)level, max_bytes, generations);
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
