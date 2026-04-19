#include <mock_com_util.h>
#include <com_util/clock/clock.h>
#include <com_util/clock/mock_clock.h>

uint64_t delegate_real_clock_get_monotonic_ms(void)
{
    return clock_get_monotonic_ms();
}

uint64_t mock_clock_get_monotonic_ms(void)
{
    uint64_t rtc = 0;

    if (_mock_com_util != nullptr)
    {
        rtc = _mock_com_util->clock_get_monotonic_ms();
    }
    else
    {
        rtc = delegate_real_clock_get_monotonic_ms();
    }

    return rtc;
}
