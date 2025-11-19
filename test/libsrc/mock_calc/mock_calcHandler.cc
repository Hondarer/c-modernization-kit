#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

#include <test_com.h>
#include <mock_calc.h>

using namespace testing;

CALC_API int WINAPI calcHandler(int kind, int a, int b)
{
    int rtc = 0;

    if (_mock_calc != nullptr)
    {
        rtc = _mock_calc->calcHandler(kind, a, b);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %d, %d", __func__, kind, a, b);
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
