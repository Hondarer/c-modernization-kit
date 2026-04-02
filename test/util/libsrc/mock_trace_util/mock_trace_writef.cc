#include <stdarg.h>
#include <stdio.h>
#include <testfw.h>
#include <mock_trace_util.h>

WEAK_ATR int trace_writef(trace_provider_t *handle, enum trace_level level,
                          const char *format, ...)
{
    int rtc = 0;

    char buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    if (_mock_trace_util != nullptr)
    {
        rtc = _mock_trace_util->trace_writef(handle, level, buf);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p, %d, %s", __func__, (void *)handle, (int)level, buf);
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
