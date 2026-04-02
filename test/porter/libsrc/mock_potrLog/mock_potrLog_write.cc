#include <stdarg.h>
#include <stdio.h>
#include <testfw.h>
#include <mock_potrLog.h>
#include <infra/potrLog.h>

WEAK_ATR void potr_log_write(PotrLogLevel level, const char *file, int line,
                              const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (_mock_potrLog != nullptr)
    {
        _mock_potrLog->log_write(level, file, line, buf);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %s, %d, %s", __func__, (int)level, file, line, buf);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> (void)\n");
        }
        else
        {
            printf("\n");
        }
    }
}
