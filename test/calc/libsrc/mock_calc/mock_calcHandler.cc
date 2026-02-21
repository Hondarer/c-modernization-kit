#include <testfw.h>
#include <mock_calc.h>

CALC_API WEAK_ATR int WINAPI calcHandler(int kind, int a, int b, int *result)
{
    int rtc = 0;

    if (_mock_calc != nullptr)
    {
        rtc = _mock_calc->calcHandler(kind, a, b, result);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %d, %d, 0x%p", __func__, kind, a, b, (void *)result);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> %d, %d\n", *result, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
