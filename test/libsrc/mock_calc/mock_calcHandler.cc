#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_calc.h>

using namespace testing;

int calcHandler(int kind, int a, int b)
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
