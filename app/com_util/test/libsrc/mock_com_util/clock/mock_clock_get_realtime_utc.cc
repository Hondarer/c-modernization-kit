#include <mock_com_util.h>
#include <com_util/clock/clock.h>
#include <com_util/clock/mock_clock.h>

void delegate_real_clock_get_realtime_utc(struct tm *utc_tm, int32_t *tv_nsec)
{
    clock_get_realtime_utc(utc_tm, tv_nsec);
}

void mock_clock_get_realtime_utc(struct tm *utc_tm, int32_t *tv_nsec)
{
    if (_mock_com_util != nullptr)
    {
        _mock_com_util->clock_get_realtime_utc(utc_tm, tv_nsec);
    }
    else
    {
        delegate_real_clock_get_realtime_utc(utc_tm, tv_nsec);
    }
}
