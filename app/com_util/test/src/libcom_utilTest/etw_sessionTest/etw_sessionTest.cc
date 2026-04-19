#include <com_util/base/platform.h>

#if defined(PLATFORM_WINDOWS)

#include <com_util/base/windows_sdk.h>
#include <TraceLoggingProvider.h>
#include <testfw.h>
#include <com_util/trace/trace_etw.h>

#include <mutex>
#include <vector>
#include <string>
#include <utility>
#include <cstdio>

/* テスト用プロバイダ定義 */
TRACE_ETW_DEFINE_PROVIDER(
    s_test_provider,
    "EtwSessionTest",
    // {0dfe6031-5678-4688-aee8-611340997caa}
    (0x0dfe6031, 0x5678, 0x4688, 0xae, 0xe8, 0x61, 0x13, 0x40, 0x99, 0x7c, 0xaa));

#define TEST_PROVIDER_GUID "0dfe6031-5678-4688-aee8-611340997caa"

/* ===== イベント収集ヘルパー ===== */

struct EventCollector
{
    std::mutex mtx;
    std::vector<std::pair<int, std::string>> events;
};

static void collect_callback(int level, const char *message, void *context)
{
    EventCollector *c = static_cast<EventCollector *>(context);
    std::lock_guard<std::mutex> lock(c->mtx);
    c->events.emplace_back(level, message ? message : "");
}

static int s_session_counter = 0;

static std::string make_session_name()
{
    char buf[128];
    snprintf(buf, sizeof(buf), "EtwSessionTest_%lu_%d",
             (unsigned long)GetCurrentProcessId(), ++s_session_counter);
    return std::string(buf);
}

/* ===== セッション API テスト ===== */

class etw_sessionTest : public Test
{
};

// NULL セッションで stop を呼んでもクラッシュしないことの確認
TEST_F(etw_sessionTest, test_session_stop_with_null)
{
    // Act & Assert - クラッシュしないことを確認
    trace_etw_session_stop(NULL); // [手順] - NULL セッションで stop を呼ぶ。安全に何もしないこと。
}

// 必須パラメータに NULL を渡した場合に NULL が返され ERR_PARAM が設定されることの確認
TEST_F(etw_sessionTest, test_session_start_null_params)
{
    int status = TRACE_ETW_SESSION_OK;

    // [確認_異常系] - 必須パラメータが NULL の場合 NULL が返されること。
    EXPECT_EQ((trace_etw_session_t *)NULL,
        trace_etw_session_start(NULL, TEST_PROVIDER_GUID, collect_callback, NULL, &status));
    EXPECT_EQ(TRACE_ETW_SESSION_ERR_PARAM, status); // [確認_異常系] - エラーコードが PARAM であること。

    EXPECT_EQ((trace_etw_session_t *)NULL,
        trace_etw_session_start("test", NULL, collect_callback, NULL, &status));
    EXPECT_EQ(TRACE_ETW_SESSION_ERR_PARAM, status);

    EXPECT_EQ((trace_etw_session_t *)NULL,
        trace_etw_session_start("test", TEST_PROVIDER_GUID, NULL, NULL, &status));
    EXPECT_EQ(TRACE_ETW_SESSION_ERR_PARAM, status);
}

// 不正な GUID 文字列を渡した場合に NULL が返され ERR_PARAM が設定されることの確認
TEST_F(etw_sessionTest, test_session_start_invalid_guid)
{
    int status = TRACE_ETW_SESSION_OK;

    // [確認_異常系] - 不正な GUID 文字列で NULL が返されること。
    EXPECT_EQ((trace_etw_session_t *)NULL,
        trace_etw_session_start("test", "not-a-guid", collect_callback, NULL, &status));
    EXPECT_EQ(TRACE_ETW_SESSION_ERR_PARAM, status); // [確認_異常系] - エラーコードが PARAM であること。
}

/* ===== 購読 (round-trip) テスト ===== */

/**
 *  購読テスト用フィクスチャ。
 *  SetUp で ETW セッション権限を検査し、権限不足の場合はテストを失敗させる。
 */
class etw_subscribeTest : public Test
{
protected:
    void SetUp() override
    {
        int status = trace_etw_session_check_access();
        ASSERT_NE(TRACE_ETW_SESSION_ERR_ACCESS, status)
            << "ETW session requires 'Performance Log Users' group membership.\n"
               "Run: net localgroup \"Performance Log Users\" %USERNAME% /add";
        ASSERT_EQ(TRACE_ETW_SESSION_OK, status)
            << "trace_etw_session_check_access failed (status=" << status << ")";
    }
};

// ASCII メッセージの書き込みと購読による round-trip 受信の確認
TEST_F(etw_subscribeTest, test_subscribe_ascii)
{
    // Arrange
    std::string session_name = make_session_name();
    EventCollector collector;

    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider);
    ASSERT_NE((trace_etw_provider_t *)NULL, handle);

    trace_etw_session_t *session = trace_etw_session_start(
        session_name.c_str(), TEST_PROVIDER_GUID,
        collect_callback, &collector, NULL);
    ASSERT_NE((trace_etw_session_t *)NULL, session);

    // Act
    Sleep(200);
    trace_etw_provider_write(handle, 4, NULL, "hello world"); // [手順] - ASCII メッセージを書き込む。

    // Stop (flushes all events)
    trace_etw_session_stop(session);

    // Assert
    bool found = false;
    for (const auto &evt : collector.events)
    {
        if (evt.second == "hello world")
        {
            EXPECT_EQ(4, evt.first); // [確認_正常系] - レベルが INFO (4) であること。
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found) << "Expected event 'hello world' not found"; // [確認_正常系]

    // Cleanup
    trace_etw_provider_destroy(handle);
}

// 日本語 UTF-8 マルチバイト文字列の round-trip 受信の確認
TEST_F(etw_subscribeTest, test_subscribe_utf8_japanese)
{
    // Arrange
    std::string session_name = make_session_name();
    EventCollector collector;
    const char *msg = "\xe8\xa8\x88\xe7\xae\x97\xe7\xb5\x90\xe6\x9e\x9c: "
                      "\xe6\x88\x90\xe5\x8a\x9f"; /* 計算結果: 成功 */

    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider);
    ASSERT_NE((trace_etw_provider_t *)NULL, handle);

    trace_etw_session_t *session = trace_etw_session_start(
        session_name.c_str(), TEST_PROVIDER_GUID,
        collect_callback, &collector, NULL);
    ASSERT_NE((trace_etw_session_t *)NULL, session);

    // Act
    Sleep(200);
    trace_etw_provider_write(handle, 3, NULL, msg); // [手順] - 日本語 UTF-8 メッセージを書き込む。

    // Stop
    trace_etw_session_stop(session);

    // Assert
    bool found = false;
    for (const auto &evt : collector.events)
    {
        if (evt.second == msg)
        {
            EXPECT_EQ(3, evt.first); // [確認_正常系] - レベルが WARNING (3) であること。
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found) << "Expected UTF-8 Japanese event not found"; // [確認_正常系]

    // Cleanup
    trace_etw_provider_destroy(handle);
}

// ASCII・日本語・絵文字混在 UTF-8 文字列の round-trip 受信の確認
TEST_F(etw_subscribeTest, test_subscribe_utf8_mixed)
{
    // Arrange
    std::string session_name = make_session_name();
    EventCollector collector;
    const char *msg = "Hello "
                      "\xe4\xb8\x96\xe7\x95\x8c"  /* 世界 */
                      " "
                      "\xf0\x9f\x8c\x8d"           /* 🌍 */
                      " World";

    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider);
    ASSERT_NE((trace_etw_provider_t *)NULL, handle);

    trace_etw_session_t *session = trace_etw_session_start(
        session_name.c_str(), TEST_PROVIDER_GUID,
        collect_callback, &collector, NULL);
    ASSERT_NE((trace_etw_session_t *)NULL, session);

    // Act
    Sleep(200);
    trace_etw_provider_write(handle, 2, NULL, msg); // [手順] - 混在 UTF-8 メッセージを書き込む。

    // Stop
    trace_etw_session_stop(session);

    // Assert
    bool found = false;
    for (const auto &evt : collector.events)
    {
        if (evt.second == msg)
        {
            EXPECT_EQ(2, evt.first); // [確認_正常系] - レベルが ERROR (2) であること。
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found) << "Expected UTF-8 mixed event not found"; // [確認_正常系]

    // Cleanup
    trace_etw_provider_destroy(handle);
}

// 全レベル (CRITICAL〜VERBOSE) のイベントがそれぞれ正しいレベルで受信されることの確認
TEST_F(etw_subscribeTest, test_subscribe_multiple_levels)
{
    // Arrange
    std::string session_name = make_session_name();
    EventCollector collector;

    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider);
    ASSERT_NE((trace_etw_provider_t *)NULL, handle);

    trace_etw_session_t *session = trace_etw_session_start(
        session_name.c_str(), TEST_PROVIDER_GUID,
        collect_callback, &collector, NULL);
    ASSERT_NE((trace_etw_session_t *)NULL, session);

    // Act - 全レベルでイベントを投入
    Sleep(200);
    trace_etw_provider_write(handle, 1, NULL, "critical_msg"); // [手順] - CRITICAL
    trace_etw_provider_write(handle, 2, NULL, "error_msg");    // [手順] - ERROR
    trace_etw_provider_write(handle, 3, NULL, "warning_msg");  // [手順] - WARNING
    trace_etw_provider_write(handle, 4, NULL, "info_msg");     // [手順] - INFO
    trace_etw_provider_write(handle, 5, NULL, "verbose_msg");  // [手順] - VERBOSE

    // Stop
    trace_etw_session_stop(session);

    // Assert - 各レベルのイベントが受信されていること
    int found_count = 0;
    const char *expected_msgs[] = {
        "critical_msg", "error_msg", "warning_msg", "info_msg", "verbose_msg"
    };
    int expected_levels[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++)
    {
        for (const auto &evt : collector.events)
        {
            if (evt.second == expected_msgs[i])
            {
                EXPECT_EQ(expected_levels[i], evt.first); // [確認_正常系] - レベルが一致すること。
                found_count++;
                break;
            }
        }
    }
    EXPECT_EQ(5, found_count) << "Not all level events were received"; // [確認_正常系]

    // Cleanup
    trace_etw_provider_destroy(handle);
}

// 空文字列の書き込みと購読による round-trip 受信の確認
TEST_F(etw_subscribeTest, test_subscribe_empty_string)
{
    // Arrange
    std::string session_name = make_session_name();
    EventCollector collector;

    trace_etw_provider_t *handle = trace_etw_provider_create(s_test_provider);
    ASSERT_NE((trace_etw_provider_t *)NULL, handle);

    trace_etw_session_t *session = trace_etw_session_start(
        session_name.c_str(), TEST_PROVIDER_GUID,
        collect_callback, &collector, NULL);
    ASSERT_NE((trace_etw_session_t *)NULL, session);

    // Act
    Sleep(200);
    trace_etw_provider_write(handle, 5, NULL, ""); // [手順] - 空文字列を書き込む。

    // Stop
    trace_etw_session_stop(session);

    // Assert
    bool found = false;
    for (const auto &evt : collector.events)
    {
        if (evt.second.empty())
        {
            EXPECT_EQ(5, evt.first); // [確認_正常系] - レベルが VERBOSE (5) であること。
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found) << "Expected empty string event not found"; // [確認_正常系]

    // Cleanup
    trace_etw_provider_destroy(handle);
}

#elif defined(PLATFORM_LINUX)

#include <testfw.h>

// Linux 環境ではテスト対象外
TEST(etw_sessionTest, not_supported)
{
    GTEST_SKIP() << "ETW is not supported on this platform";
}

#endif /* PLATFORM_ */
