#include <testfw.h>
#include <mock_com_util.h>

#if defined(PLATFORM_LINUX)
    #include <time.h>
#elif defined(PLATFORM_WINDOWS)
    #include <windows.h>
#endif /* PLATFORM_ */

WEAK_ATR void clock_get_realtime_deadline_ms(uint64_t timeout_ms, struct timespec *abs_timeout)
{
    if (_mock_com_util != nullptr)
    {
        _mock_com_util->clock_get_realtime_deadline_ms(timeout_ms, abs_timeout);
    }
    else
    {
#if defined(PLATFORM_LINUX)
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += (time_t)(timeout_ms / 1000ULL);
        ts.tv_nsec += (long)((timeout_ms % 1000ULL) * 1000000ULL);

        if (ts.tv_nsec >= 1000000000L)
        {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000L;
        }

        *abs_timeout = ts;
#elif defined(PLATFORM_WINDOWS)
        FILETIME ft;
        ULARGE_INTEGER uli;
        uint64_t total_100ns;

        GetSystemTimeAsFileTime(&ft);
        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        total_100ns = uli.QuadPart + timeout_ms * 10000ULL;

        abs_timeout->tv_sec = (time_t)(total_100ns / 10000000ULL - 11644473600ULL);
        abs_timeout->tv_nsec = (long)((total_100ns % 10000000ULL) * 100ULL);
#endif /* PLATFORM_ */
    }
}
