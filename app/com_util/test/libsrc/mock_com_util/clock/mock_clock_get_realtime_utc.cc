#include <testfw.h>
#include <mock_com_util.h>

#if defined(PLATFORM_LINUX)
    #include <string.h>
    #include <time.h>
#elif defined(PLATFORM_WINDOWS)
    #include <string.h>
    #include <windows.h>
#endif /* PLATFORM_ */

WEAK_ATR void clock_get_realtime_utc(struct tm *utc_tm, int32_t *tv_nsec)
{
    if (_mock_com_util != nullptr)
    {
        _mock_com_util->clock_get_realtime_utc(utc_tm, tv_nsec);
    }
    else
    {
#if defined(PLATFORM_LINUX)
        struct timespec ts;
        time_t realtime_time;

        clock_gettime(CLOCK_REALTIME, &ts);
        realtime_time = ts.tv_sec;
        *tv_nsec = (int32_t)ts.tv_nsec;

        if (gmtime_r(&realtime_time, utc_tm) == NULL)
        {
            memset(utc_tm, 0, sizeof(*utc_tm));
        }
#elif defined(PLATFORM_WINDOWS)
        FILETIME ft;
        ULARGE_INTEGER uli;
        time_t realtime_time;
        uint64_t tick_100ns;

        GetSystemTimeAsFileTime(&ft);
        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        tick_100ns = uli.QuadPart;
        realtime_time = (time_t)(tick_100ns / 10000000ULL - 11644473600ULL);
        *tv_nsec = (int32_t)((tick_100ns % 10000000ULL) * 100ULL);

        if (gmtime_s(utc_tm, &realtime_time) != 0)
        {
            memset(utc_tm, 0, sizeof(*utc_tm));
        }
#endif /* PLATFORM_ */
    }
}
