#include <com_util/base/platform.h>

#if defined(PLATFORM_WINDOWS)

#include <com_util/base/windows_sdk.h>
#include <TraceLoggingProvider.h>
#include <testfw.h>
#include <com_util/trace/trace_etw.h>

/* テスト用プロバイダ定義 */
TRACE_ETW_DEFINE_PROVIDER(
    s_test_provider,
    "EtwProviderTest",
    // {62ab1ccc-5fc6-4e1e-8260-9ea2772afe5e}
    (0x62ab1ccc, 0x5fc6, 0x4e1e, 0x82, 0x60, 0x9e, 0xa2, 0x77, 0x2a, 0xfe, 0x5e));

/* ===== テストクラス ===== */

class etw_providerTest : public Test
{
};

/* ===== プロバイダ単体テスト ===== */

// プロバイダを登録し、有効なハンドルが返されることの確認
TEST_F(etw_providerTest, test_init_and_fini)
{
    // Arrange & Act
    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider); // [手順] - プロバイダを登録する。

    // Assert
    EXPECT_NE((trace_etw_provider_t *)NULL, handle); // [確認_正常系] - ハンドルが NULL でないこと。

    // Cleanup
    trace_etw_provider_destroy(handle);
}

// INFO レベルでメッセージを書き込み、戻り値が 0 であることの確認
TEST_F(etw_providerTest, test_write_returns_zero)
{
    // Arrange
    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider);
    ASSERT_NE((trace_etw_provider_t *)NULL, handle);

    // Act
    int result = trace_etw_provider_write(handle, 4, NULL, "test message"); // [手順] - INFO レベルでメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    trace_etw_provider_destroy(handle);
}

// 全レベル (CRITICAL〜VERBOSE) で書き込みが成功することの確認
TEST_F(etw_providerTest, test_write_all_levels)
{
    // Arrange
    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider);
    ASSERT_NE((trace_etw_provider_t *)NULL, handle);

    // Act & Assert
    EXPECT_EQ(0, trace_etw_provider_write(handle, 1, NULL, "critical")); // [確認_正常系] - CRITICAL レベルで書き込めること。
    EXPECT_EQ(0, trace_etw_provider_write(handle, 2, NULL, "error"));    // [確認_正常系] - ERROR レベルで書き込めること。
    EXPECT_EQ(0, trace_etw_provider_write(handle, 3, NULL, "warning"));  // [確認_正常系] - WARNING レベルで書き込めること。
    EXPECT_EQ(0, trace_etw_provider_write(handle, 4, NULL, "info"));     // [確認_正常系] - INFO レベルで書き込めること。
    EXPECT_EQ(0, trace_etw_provider_write(handle, 5, NULL, "verbose"));  // [確認_正常系] - VERBOSE レベルで書き込めること。

    // Cleanup
    trace_etw_provider_destroy(handle);
}

// provider_ref に NULL を渡した場合に NULL が返されることの確認
TEST_F(etw_providerTest, test_init_null_provider_ref)
{
    // Act
    trace_etw_provider_t *handle = trace_etw_provider_create(NULL); // [手順] - NULL を渡す。

    // Assert
    EXPECT_EQ((trace_etw_provider_t *)NULL, handle); // [確認_異常系] - NULL が返されること。
}

// handle に NULL を渡した場合に安全に 0 が返されることの確認
TEST_F(etw_providerTest, test_write_with_null_handle)
{
    // Act
    int result = trace_etw_provider_write(NULL, 4, NULL, "test message"); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// message に NULL を渡した場合に安全に 0 が返されることの確認
TEST_F(etw_providerTest, test_write_with_null_message)
{
    // Arrange
    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider);

    // Act
    int result = trace_etw_provider_write(handle, 4, NULL, NULL); // [手順] - message に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL メッセージでも安全に 0 が返されること。

    // Cleanup
    trace_etw_provider_destroy(handle);
}

// NULL ハンドルで dispose を呼んでもクラッシュしないことの確認
TEST_F(etw_providerTest, test_dispose_with_null_handle)
{
    // Act & Assert - クラッシュしないことを確認
    trace_etw_provider_destroy(NULL); // [手順] - NULL ハンドルで dispose を呼ぶ。安全に何もしないこと。
}

#elif defined(PLATFORM_LINUX)

#include <testfw.h>

// Linux 環境ではテスト対象外
TEST(etw_providerTest, not_supported)
{
    GTEST_SKIP() << "ETW is not supported on this platform";
}

#endif /* PLATFORM_ */
