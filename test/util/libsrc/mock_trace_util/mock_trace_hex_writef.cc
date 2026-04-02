#include <stdarg.h>
#include <stdio.h>
#include <testfw.h>
#include <mock_trace_util.h>

WEAK_ATR int trace_hex_writef(trace_provider_t *handle, enum trace_level level,
                              const void *data, size_t size,
                              const char *format, ...)
{
    int rtc = 0;

    char label[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(label, sizeof(label), format, args);
    va_end(args);

    if (_mock_trace_util != nullptr)
    {
        rtc = _mock_trace_util->trace_hex_writef(
            handle, level, data, size, label);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p, %d, 0x%p, %zu, %s", __func__,
               (void *)handle, (int)level, data, size, label);
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
