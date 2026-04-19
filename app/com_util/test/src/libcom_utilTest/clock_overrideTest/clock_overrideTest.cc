#include <testfw.h>
#include <mock_com_util.h>
#include <com_util/clock/clock.h>
#include <com_util/clock/mock_clock.h>

class clock_overrideTest : public Test
{
};

static int64_t to_epoch_ns(int64_t tv_sec, int32_t tv_nsec)
{
    return tv_sec * 1000000000LL + (int64_t)tv_nsec;
}

static int64_t timespec_to_epoch_ns(const struct timespec *ts)
{
    return (int64_t)ts->tv_sec * 1000000000LL + (int64_t)ts->tv_nsec;
}

static void expect_valid_utc_snapshot(const struct tm *utc_tm, int32_t tv_nsec)
{
    EXPECT_GE(utc_tm->tm_year, 120);
    EXPECT_GE(utc_tm->tm_mon, 0);
    EXPECT_LE(utc_tm->tm_mon, 11);
    EXPECT_GE(utc_tm->tm_mday, 1);
    EXPECT_LE(utc_tm->tm_mday, 31);
    EXPECT_GE(utc_tm->tm_hour, 0);
    EXPECT_LE(utc_tm->tm_hour, 23);
    EXPECT_GE(utc_tm->tm_min, 0);
    EXPECT_LE(utc_tm->tm_min, 59);
    EXPECT_GE(utc_tm->tm_sec, 0);
    EXPECT_LE(utc_tm->tm_sec, 60);
    EXPECT_GE(tv_nsec, 0);
    EXPECT_LT(tv_nsec, 1000000000);
}

TEST_F(clock_overrideTest, test_without_mock_object_uses_real_delegate)
{
    struct tm utc_tm = {};
    struct timespec abs_timeout;
    int32_t utc_nsec = -1;
    int64_t before_sec = 0;
    int32_t before_nsec = 0;
    int64_t after_sec = 0;
    int32_t after_nsec = 0;
    int64_t before_ns;
    int64_t after_ns;
    int64_t deadline_ns;

    clock_get_realtime(&before_sec, &before_nsec);
    mock_clock_get_realtime_utc(&utc_tm, &utc_nsec);
    mock_clock_get_realtime_deadline_ms(100, &abs_timeout);
    clock_get_realtime(&after_sec, &after_nsec);

    before_ns = to_epoch_ns(before_sec, before_nsec);
    after_ns = to_epoch_ns(after_sec, after_nsec);
    deadline_ns = timespec_to_epoch_ns(&abs_timeout);

    EXPECT_GT(mock_clock_get_monotonic_ms(), 0U);
    expect_valid_utc_snapshot(&utc_tm, utc_nsec);
    EXPECT_GE(abs_timeout.tv_nsec, 0L);
    EXPECT_LT(abs_timeout.tv_nsec, 1000000000L);
    EXPECT_GE(deadline_ns, before_ns + 100000000LL);
    EXPECT_LE(deadline_ns, after_ns + 100000000LL);
}

TEST_F(clock_overrideTest, test_mock_default_action_uses_real_delegate)
{
    Mock_com_util mock_com_util;
    struct tm utc_tm = {};
    struct timespec abs_timeout;
    int32_t utc_nsec = -1;

    EXPECT_CALL(mock_com_util, clock_get_monotonic_ms())
        .Times(1);
    EXPECT_CALL(mock_com_util, clock_get_realtime_utc(_, _))
        .Times(1);
    EXPECT_CALL(mock_com_util, clock_get_realtime_deadline_ms(100, _))
        .Times(1);

    EXPECT_GT(mock_clock_get_monotonic_ms(), 0U);
    mock_clock_get_realtime_utc(&utc_tm, &utc_nsec);
    mock_clock_get_realtime_deadline_ms(100, &abs_timeout);

    expect_valid_utc_snapshot(&utc_tm, utc_nsec);
    EXPECT_GE(abs_timeout.tv_nsec, 0L);
    EXPECT_LT(abs_timeout.tv_nsec, 1000000000L);
}

TEST_F(clock_overrideTest, test_mock_explicit_override_returns_configured_values)
{
    Mock_com_util mock_com_util;
    struct tm utc_tm = {};
    struct timespec abs_timeout = {};
    int32_t utc_nsec = 0;

    EXPECT_CALL(mock_com_util, clock_get_monotonic_ms())
        .WillOnce(Return(1234U));
    EXPECT_CALL(mock_com_util, clock_get_realtime_utc(_, _))
        .WillOnce([](struct tm *utc_tm_arg, int32_t *tv_nsec_arg)
        {
            utc_tm_arg->tm_year = 126;
            utc_tm_arg->tm_mon = 3;
            utc_tm_arg->tm_mday = 20;
            utc_tm_arg->tm_hour = 12;
            utc_tm_arg->tm_min = 34;
            utc_tm_arg->tm_sec = 56;
            *tv_nsec_arg = 789012345;
        });
    EXPECT_CALL(mock_com_util, clock_get_realtime_deadline_ms(250, _))
        .WillOnce([](uint64_t timeout_ms, struct timespec *abs_timeout_arg)
        {
            EXPECT_EQ(250U, timeout_ms);
            abs_timeout_arg->tv_sec = 123;
            abs_timeout_arg->tv_nsec = 456000000L;
        });

    EXPECT_EQ(1234U, mock_clock_get_monotonic_ms());

    mock_clock_get_realtime_utc(&utc_tm, &utc_nsec);
    EXPECT_EQ(126, utc_tm.tm_year);
    EXPECT_EQ(3, utc_tm.tm_mon);
    EXPECT_EQ(20, utc_tm.tm_mday);
    EXPECT_EQ(12, utc_tm.tm_hour);
    EXPECT_EQ(34, utc_tm.tm_min);
    EXPECT_EQ(56, utc_tm.tm_sec);
    EXPECT_EQ(789012345, utc_nsec);

    mock_clock_get_realtime_deadline_ms(250, &abs_timeout);
    EXPECT_EQ(123, abs_timeout.tv_sec);
    EXPECT_EQ(456000000L, abs_timeout.tv_nsec);
}
