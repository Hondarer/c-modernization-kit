#include <testfw.h>
#include <mock_potrLog.h>
#include <infra/potrLog.h>

#include <stdarg.h>
#include <stdio.h>

Mock_potrLog *_mock_potrLog = nullptr;

Mock_potrLog::Mock_potrLog()
{
    ON_CALL(*this, log_write(_, _, _, _))
        .WillByDefault(Return()); /* デフォルト NOP */

    _mock_potrLog = this;
}

Mock_potrLog::~Mock_potrLog()
{
    _mock_potrLog = nullptr;
}

WEAK_ATR void potr_log_write(PotrLogLevel level, const char *file, int line,
                              const char *fmt, ...)
{
    if (_mock_potrLog == nullptr)
    {
        return;
    }

    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    _mock_potrLog->log_write(level, file, line, buf);
}
