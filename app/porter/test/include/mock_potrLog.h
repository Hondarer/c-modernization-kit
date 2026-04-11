#ifndef MOCK_POTR_LOG_H
#define MOCK_POTR_LOG_H

#include <testfw.h>
#include <porter_type.h>

class Mock_potrLog
{
public:
    MOCK_METHOD(void, log_write, (PotrLogLevel, const char *, int, const char *));

    Mock_potrLog();
    ~Mock_potrLog();
};

extern Mock_potrLog *_mock_potrLog;

#endif /* MOCK_POTR_LOG_H */
