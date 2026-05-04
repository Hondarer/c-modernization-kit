#include <testfw.h>
#include <mock_calcbase.h>

MOCK_WEAK_IMPL(int, subtract, int a, int b, int *result)
{
    int rtc = 0;

    if (_mock_calcbase != nullptr)
    {
        rtc = _mock_calcbase->subtract(a, b, result);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %d, 0x%p", __func__, a, b, (void *)result);
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
