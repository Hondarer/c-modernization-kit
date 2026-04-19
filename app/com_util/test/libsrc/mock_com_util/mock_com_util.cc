#include <testfw.h>
#include <mock_com_util.h>
#include <string.h>

#if defined(PLATFORM_LINUX)
    #include <time.h>
#elif defined(PLATFORM_WINDOWS)
    #include <windows.h>
#endif /* PLATFORM_ */

Mock_com_util *_mock_com_util = nullptr;

namespace
{
constexpr uint64_t MSEC_PER_SEC = 1000ULL;
constexpr int64_t NSEC_PER_SEC = 1000000000LL;
constexpr uint64_t NSEC_PER_MSEC = 1000000ULL;

uint64_t default_clock_get_monotonic_ms()
{
#if defined(PLATFORM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * MSEC_PER_SEC + (uint64_t)ts.tv_nsec / NSEC_PER_MSEC;
#elif defined(PLATFORM_WINDOWS)
    return (uint64_t)GetTickCount64();
#endif /* PLATFORM_ */
}

void default_clock_get_realtime_utc(struct tm *utc_tm, int32_t *tv_nsec)
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

void default_clock_get_realtime_deadline_ms(uint64_t timeout_ms, struct timespec *abs_timeout)
{
#if defined(PLATFORM_LINUX)
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += (time_t)(timeout_ms / MSEC_PER_SEC);
    ts.tv_nsec += (long)((timeout_ms % MSEC_PER_SEC) * NSEC_PER_MSEC);

    if (ts.tv_nsec >= NSEC_PER_SEC)
    {
        ts.tv_sec += 1;
        ts.tv_nsec -= (long)NSEC_PER_SEC;
    }

    *abs_timeout = ts;
#elif defined(PLATFORM_WINDOWS)
    FILETIME ft;
    ULARGE_INTEGER uli;
    uint64_t total_100ns;
    uint64_t sec_part;
    uint64_t nsec_part;

    GetSystemTimeAsFileTime(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    total_100ns = uli.QuadPart + timeout_ms * 10000ULL;
    sec_part = total_100ns / 10000000ULL - 11644473600ULL;
    nsec_part = (total_100ns % 10000000ULL) * 100ULL;

    abs_timeout->tv_sec = (time_t)sec_part;
    abs_timeout->tv_nsec = (long)nsec_part;
#endif /* PLATFORM_ */
}
} /* namespace */

Mock_com_util::Mock_com_util()
{
    ON_CALL(*this, com_util_compress(_, _, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, com_util_decompress(_, _, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, com_util_encrypt(_, _, _, _, _, _, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, com_util_decrypt(_, _, _, _, _, _, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, com_util_passphrase_to_key(_, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, trace_logger_create())
        .WillByDefault(Return(nullptr)); // 一般的にはモックの既定の挙動は NOP にしておき、テストプログラムで具体的な挙動を決める
    ON_CALL(*this, trace_logger_destroy(_))
        .WillByDefault(Return());
    ON_CALL(*this, trace_logger_start(_))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_stop(_))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_write(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_write_hex(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_writef(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_write_hexf(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_name(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_os_level(_, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_file_level(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_stderr_level(_, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_get_os_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));
    ON_CALL(*this, trace_logger_get_file_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));
    ON_CALL(*this, trace_logger_get_stderr_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));
    ON_CALL(*this, clock_get_monotonic_ms())
        .WillByDefault(Invoke(default_clock_get_monotonic_ms));
    ON_CALL(*this, clock_get_realtime_utc(_, _))
        .WillByDefault(Invoke(default_clock_get_realtime_utc));
    ON_CALL(*this, clock_get_realtime_deadline_ms(_, _))
        .WillByDefault(Invoke(default_clock_get_realtime_deadline_ms));

    _mock_com_util = this;
}

Mock_com_util::~Mock_com_util()
{
    _mock_com_util = nullptr;
}
