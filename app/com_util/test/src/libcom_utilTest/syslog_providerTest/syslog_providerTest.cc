#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)

#include <syslog.h>
#include <testfw.h>
#include <com_util/trace/trace_syslog.h>

/* ===== テストクラス ===== */

class syslog_providerTest : public Test
{
};

/* ===== プロバイダ単体テスト ===== */

// プロバイダを初期化し、有効なハンドルが返されることの確認
TEST_F(syslog_providerTest, test_init_and_dispose)
{
    // Arrange & Act
    trace_syslog_sink_t *handle = trace_syslog_sink_create("syslog_test", LOG_USER); // [手順] - プロバイダを初期化する。

    // Assert
    EXPECT_NE((trace_syslog_sink_t *)NULL, handle); // [確認_正常系] - ハンドルが NULL でないこと。

    // Cleanup
    trace_syslog_sink_destroy(handle);
}

// INFO レベルでメッセージを書き込み、戻り値が 0 であることの確認
TEST_F(syslog_providerTest, test_write_returns_zero)
{
    // Arrange
    trace_syslog_sink_t *handle = trace_syslog_sink_create("syslog_test", LOG_USER);
    ASSERT_NE((trace_syslog_sink_t *)NULL, handle);

    // Act
    int result = trace_syslog_sink_write(handle, LOG_INFO, "test message"); // [手順] - INFO レベルでメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    trace_syslog_sink_destroy(handle);
}

// 全レベル (CRIT〜DEBUG) で書き込みが成功することの確認
TEST_F(syslog_providerTest, test_write_all_levels)
{
    // Arrange
    trace_syslog_sink_t *handle = trace_syslog_sink_create("syslog_test", LOG_USER);
    ASSERT_NE((trace_syslog_sink_t *)NULL, handle);

    // Act & Assert
    EXPECT_EQ(0, trace_syslog_sink_write(handle, LOG_CRIT,    "critical")); // [確認_正常系] - CRIT レベルで書き込めること。
    EXPECT_EQ(0, trace_syslog_sink_write(handle, LOG_ERR,     "error"));    // [確認_正常系] - ERR レベルで書き込めること。
    EXPECT_EQ(0, trace_syslog_sink_write(handle, LOG_WARNING, "warning"));  // [確認_正常系] - WARNING レベルで書き込めること。
    EXPECT_EQ(0, trace_syslog_sink_write(handle, LOG_INFO,    "info"));     // [確認_正常系] - INFO レベルで書き込めること。
    EXPECT_EQ(0, trace_syslog_sink_write(handle, LOG_DEBUG,   "debug"));    // [確認_正常系] - DEBUG レベルで書き込めること。

    // Cleanup
    trace_syslog_sink_destroy(handle);
}

// ident に NULL を渡した場合に NULL が返されることの確認
TEST_F(syslog_providerTest, test_init_null_ident)
{
    // Act
    trace_syslog_sink_t *handle = trace_syslog_sink_create(NULL, LOG_USER); // [手順] - NULL を渡す。

    // Assert
    EXPECT_EQ((trace_syslog_sink_t *)NULL, handle); // [確認_異常系] - NULL が返されること。
}

// handle に NULL を渡した場合に安全に 0 が返されることの確認
TEST_F(syslog_providerTest, test_write_with_null_handle)
{
    // Act
    int result = trace_syslog_sink_write(NULL, LOG_INFO, "test message"); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// message に NULL を渡した場合に安全に 0 が返されることの確認
TEST_F(syslog_providerTest, test_write_with_null_message)
{
    // Arrange
    trace_syslog_sink_t *handle = trace_syslog_sink_create("syslog_test", LOG_USER);

    // Act
    int result = trace_syslog_sink_write(handle, LOG_INFO, NULL); // [手順] - message に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL メッセージでも安全に 0 が返されること。

    // Cleanup
    trace_syslog_sink_destroy(handle);
}

// NULL ハンドルで dispose を呼んでもクラッシュしないことの確認
TEST_F(syslog_providerTest, test_dispose_with_null_handle)
{
    // Act & Assert - クラッシュしないことを確認
    trace_syslog_sink_destroy(NULL); // [手順] - NULL ハンドルで dispose を呼ぶ。安全に何もしないこと。
}

#elif defined(PLATFORM_WINDOWS)

#include <testfw.h>

// Windows 環境ではテスト対象外
TEST(syslog_providerTest, not_supported)
{
    GTEST_SKIP() << "syslog is not supported on this platform";
}

#endif /* PLATFORM_ */
