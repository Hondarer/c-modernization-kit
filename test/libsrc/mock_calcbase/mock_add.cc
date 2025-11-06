#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_calcbase.h>

using namespace testing;

int add(int a, int b)
{
    int rtc = 0;

    if (_mock_calcbase != nullptr)
    {
        rtc = _mock_calcbase->add(a, b);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %d", __func__, a, b);
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
