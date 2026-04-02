#include <testfw.h>
#include <mock_trace_util.h>

WEAK_ATR int trace_modify_filetrc(trace_provider_t *handle, const char *path,
                                  enum trace_level level, size_t max_bytes,
                                  int generations)
{
    int rtc = 0;

    if (_mock_trace_util != nullptr)
    {
        rtc = _mock_trace_util->trace_modify_filetrc(handle, path, level, max_bytes,
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
