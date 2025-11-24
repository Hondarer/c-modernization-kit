#include <testfw.h>
#include <mock_calcbase.h>

WEAK_ATR int add(int a, int b, int *result)
{
    int rtc = 0;

    if (_mock_calcbase != nullptr)
    {
        rtc = _mock_calcbase->add(a, b, result);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %d, %p", __func__, a, b, (void *)result);
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
