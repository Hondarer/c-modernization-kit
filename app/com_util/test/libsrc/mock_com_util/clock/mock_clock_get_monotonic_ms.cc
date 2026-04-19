#include <testfw.h>
#include <mock_com_util.h>

#if defined(PLATFORM_LINUX)
    #include <time.h>
#elif defined(PLATFORM_WINDOWS)
    #include <windows.h>
#endif /* PLATFORM_ */

WEAK_ATR uint64_t clock_get_monotonic_ms(void)
{
    uint64_t rtc = 0;

    if (_mock_com_util != nullptr)
    {
        rtc = _mock_com_util->clock_get_monotonic_ms();
    }
    else
    {
#if defined(PLATFORM_LINUX)
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        rtc = (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
#elif defined(PLATFORM_WINDOWS)
        rtc = (uint64_t)GetTickCount64();
#endif /* PLATFORM_ */
    }

    return rtc;
}
