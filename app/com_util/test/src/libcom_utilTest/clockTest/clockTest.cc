#include <testfw.h>
#include <com_util/clock/clock.h>
#include <stdint.h>

/* ===== テストクラス ===== */

class clockTest : public Test
{
};

/* ===== 内部ヘルパー ===== */

static int64_t to_epoch_ns(int64_t tv_sec, int32_t tv_nsec)
{
    return tv_sec * 1000000000LL + (int64_t)tv_nsec;
}

static int64_t timespec_to_epoch_ns(const struct timespec *ts)
{
    return (int64_t)ts->tv_sec * 1000000000LL + (int64_t)ts->tv_nsec;
}

static int tm_fields_equal(const struct tm *lhs, const struct tm *rhs)
{
    return lhs->tm_year == rhs->tm_year
        && lhs->tm_mon == rhs->tm_mon
        && lhs->tm_mday == rhs->tm_mday
        && lhs->tm_hour == rhs->tm_hour
        && lhs->tm_min == rhs->tm_min
        && lhs->tm_sec == rhs->tm_sec;
}

static int capture_matching_realtime_snapshot(struct tm *utc_tm, int32_t *utc_nsec,
                                              int64_t *realtime_sec, int32_t *realtime_nsec)
{
    int attempt;

    for (attempt = 0; attempt < 8; attempt++)
    {
        struct tm utc_candidate;
        struct tm expected_tm;
        int32_t utc_nsec_candidate;
        int64_t realtime_sec_candidate;
        int32_t realtime_nsec_candidate;
        time_t realtime_time;

        clock_get_realtime_utc(&utc_candidate, &utc_nsec_candidate);
        clock_get_realtime(&realtime_sec_candidate, &realtime_nsec_candidate);
        realtime_time = (time_t)realtime_sec_candidate;

#if defined(PLATFORM_WINDOWS)
        if (gmtime_s(&expected_tm, &realtime_time) != 0)
        {
            continue;
        }
#elif defined(PLATFORM_LINUX)
        if (gmtime_r(&realtime_time, &expected_tm) == NULL)
        {
            continue;
        }
#endif /* PLATFORM_ */

        if (tm_fields_equal(&utc_candidate, &expected_tm))
        {
            *utc_tm = utc_candidate;
            *utc_nsec = utc_nsec_candidate;
            *realtime_sec = realtime_sec_candidate;
            *realtime_nsec = realtime_nsec_candidate;
            return 0;
        }
    }

    return -1;
}

/* ===== realtime UTC テスト ===== */

TEST_F(clockTest, test_realtime_utc_returns_valid_range)
{
    struct tm utc_tm;
    int32_t utc_nsec;

    clock_get_realtime_utc(&utc_tm, &utc_nsec);

    EXPECT_GE(utc_tm.tm_year, 120);
    EXPECT_GE(utc_tm.tm_mon, 0);
    EXPECT_LE(utc_tm.tm_mon, 11);
    EXPECT_GE(utc_tm.tm_mday, 1);
    EXPECT_LE(utc_tm.tm_mday, 31);
    EXPECT_GE(utc_tm.tm_hour, 0);
    EXPECT_LE(utc_tm.tm_hour, 23);
    EXPECT_GE(utc_tm.tm_min, 0);
    EXPECT_LE(utc_tm.tm_min, 59);
    EXPECT_GE(utc_tm.tm_sec, 0);
    EXPECT_LE(utc_tm.tm_sec, 60);
    EXPECT_GE(utc_nsec, 0);
    EXPECT_LT(utc_nsec, 1000000000);
}

TEST_F(clockTest, test_realtime_utc_matches_realtime_snapshot)
{
    struct tm utc_tm = {};
    struct tm expected_tm = {};
    int32_t utc_nsec = 0;
    int64_t realtime_sec = 0;
    int32_t realtime_nsec = 0;
    time_t realtime_time;

    ASSERT_EQ(0, capture_matching_realtime_snapshot(&utc_tm, &utc_nsec, &realtime_sec, &realtime_nsec));

    realtime_time = (time_t)realtime_sec;

#if defined(PLATFORM_WINDOWS)
    ASSERT_EQ(0, gmtime_s(&expected_tm, &realtime_time));
#elif defined(PLATFORM_LINUX)
    ASSERT_NE((struct tm *)NULL, gmtime_r(&realtime_time, &expected_tm));
#endif /* PLATFORM_ */

    EXPECT_TRUE(tm_fields_equal(&utc_tm, &expected_tm));
    EXPECT_GE(utc_nsec, 0);
    EXPECT_LT(utc_nsec, 1000000000);
    EXPECT_GE(realtime_nsec, 0);
    EXPECT_LT(realtime_nsec, 1000000000);
}

/* ===== deadline テスト ===== */

TEST_F(clockTest, test_realtime_deadline_ms_advances_requested_timeout)
{
    const uint64_t timeout_ms = 250;
    const int64_t timeout_ns = (int64_t)timeout_ms * 1000000LL;
    struct timespec abs_timeout;
    int64_t before_sec;
    int32_t before_nsec;
    int64_t after_sec;
    int32_t after_nsec;
    int64_t before_ns;
    int64_t after_ns;
    int64_t deadline_ns;

    clock_get_realtime(&before_sec, &before_nsec);
    clock_get_realtime_deadline_ms(timeout_ms, &abs_timeout);
    clock_get_realtime(&after_sec, &after_nsec);

    before_ns = to_epoch_ns(before_sec, before_nsec);
    after_ns = to_epoch_ns(after_sec, after_nsec);
    deadline_ns = timespec_to_epoch_ns(&abs_timeout);

    EXPECT_GE(abs_timeout.tv_nsec, 0L);
    EXPECT_LT(abs_timeout.tv_nsec, 1000000000L);
    EXPECT_GE(deadline_ns, before_ns + timeout_ns);
    EXPECT_LE(deadline_ns, after_ns + timeout_ns);
}
