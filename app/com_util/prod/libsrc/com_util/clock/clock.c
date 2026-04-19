/**
 *******************************************************************************
 *  @file           clock.c
 *  @brief          プラットフォーム抽象クロック取得ユーティリティー実装。
 *  @author         Tetsuo Honda
 *  @date           2026/04/19
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>
#include <com_util/clock/clock.h>
#include <string.h>

#if defined(PLATFORM_LINUX)
    #include <time.h>
#elif defined(PLATFORM_WINDOWS)
    #include <com_util/base/windows_sdk.h>
#endif /* PLATFORM_ */

/* 変換定数 */
#define MSEC_PER_SEC              (1000ULL)       /* ミリ秒 / 秒 */
#define NSEC_PER_SEC              (1000000000LL)  /* ナノ秒 / 秒 */
#define NSEC_PER_MSEC             (1000000ULL)    /* ナノ秒 / ミリ秒 */
#define FILETIME_UNITS_PER_SEC    (10000000ULL)   /* FILETIME 単位 (100ns) / 秒 */
#define NSEC_PER_FILETIME_UNIT    (100ULL)        /* ナノ秒 / FILETIME 単位 */
#define FILETIME_EPOCH_OFFSET_SEC (11644473600LL) /* 1601-01-01 → 1970-01-01 の差 (秒) */

static void clock_fill_timespec(struct timespec *ts, int64_t tv_sec, int32_t tv_nsec)
{
    ts->tv_sec = (time_t)tv_sec;
    ts->tv_nsec = (long)tv_nsec;
}

/* doxygen コメントはヘッダに記載 */
uint64_t clock_get_monotonic_ms(void)
{
#if defined(PLATFORM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * MSEC_PER_SEC + (uint64_t)ts.tv_nsec / NSEC_PER_MSEC;
#elif defined(PLATFORM_WINDOWS)
    return (uint64_t)GetTickCount64();
#endif /* PLATFORM_ */
}

/* doxygen コメントはヘッダに記載 */
void clock_get_monotonic(int64_t *tv_sec, int32_t *tv_nsec)
{
#if defined(PLATFORM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    *tv_sec = (int64_t)ts.tv_sec;
    *tv_nsec = (int32_t)ts.tv_nsec;
#elif defined(PLATFORM_WINDOWS)
    ULONGLONG ms = GetTickCount64();
    *tv_sec = (int64_t)(ms / MSEC_PER_SEC);
    *tv_nsec = (int32_t)((ms % MSEC_PER_SEC) * NSEC_PER_MSEC);
#endif /* PLATFORM_ */
}

/* doxygen コメントはヘッダに記載 */
void clock_get_realtime(int64_t *tv_sec, int32_t *tv_nsec)
{
#if defined(PLATFORM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    *tv_sec = (int64_t)ts.tv_sec;
    *tv_nsec = (int32_t)ts.tv_nsec;
#elif defined(PLATFORM_WINDOWS)
    FILETIME ft;
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    *tv_sec = (int64_t)(uli.QuadPart / FILETIME_UNITS_PER_SEC) - FILETIME_EPOCH_OFFSET_SEC;
    *tv_nsec = (int32_t)((uli.QuadPart % FILETIME_UNITS_PER_SEC) * NSEC_PER_FILETIME_UNIT);
#endif /* PLATFORM_ */
}

/* doxygen コメントはヘッダに記載 */
void clock_get_realtime_utc(struct tm *utc_tm, int32_t *tv_nsec)
{
    int64_t realtime_sec;

    clock_get_realtime(&realtime_sec, tv_nsec);

#if defined(PLATFORM_LINUX)
    {
        time_t realtime_time = (time_t)realtime_sec;
        if (gmtime_r(&realtime_time, utc_tm) == NULL)
        {
            memset(utc_tm, 0, sizeof(*utc_tm));
        }
    }
#elif defined(PLATFORM_WINDOWS)
    {
        time_t realtime_time = (time_t)realtime_sec;
        if (gmtime_s(utc_tm, &realtime_time) != 0)
        {
            memset(utc_tm, 0, sizeof(*utc_tm));
        }
    }
#endif /* PLATFORM_ */
}

/* doxygen コメントはヘッダに記載 */
void clock_get_realtime_deadline_ms(uint64_t timeout_ms, struct timespec *abs_timeout)
{
    int64_t deadline_sec;
    int32_t deadline_nsec;

    clock_get_realtime(&deadline_sec, &deadline_nsec);

    deadline_sec += (int64_t)(timeout_ms / MSEC_PER_SEC);
    deadline_nsec += (int32_t)((timeout_ms % MSEC_PER_SEC) * NSEC_PER_MSEC);

    if (deadline_nsec >= NSEC_PER_SEC)
    {
        deadline_sec += 1;
        deadline_nsec -= (int32_t)NSEC_PER_SEC;
    }

    clock_fill_timespec(abs_timeout, deadline_sec, deadline_nsec);
}
