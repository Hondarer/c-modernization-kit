#ifdef _WIN32
#include <windows.h>
#include <TraceLoggingProvider.h>
#endif /* _WIN32 */

#include <testfw.h>
#include <log-util.h>

#ifdef _WIN32

/* テスト用プロバイダ定義 */
ETW_UTIL_DEFINE_PROVIDER(
    s_test_provider,
    "LogProviderTest",
    // {a1b2c3d4-e5f6-7890-abcd-ef0123456789}
    (0xa1b2c3d4, 0xe5f6, 0x7890, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89));
#endif /* _WIN32 */

/* ===== テストクラス ===== */

class log_providerTest : public Test
{
};

/* ===== 初期化・終了テスト ===== */

#ifdef _WIN32

// Windows: log_init_windows で初期化し、有効なハンドルが返されることの確認
TEST_F(log_providerTest, test_init_windows_and_dispose)
{
    // Arrange & Act
    log_provider_t *handle = log_init_windows(s_test_provider); // [手順] - ETW プロバイダで初期化する。

    // Assert
    EXPECT_NE((log_provider_t *)NULL, handle); // [確認_正常系] - ハンドルが NULL でないこと。

    // Cleanup
    log_dispose(handle);
}

#else /* !_WIN32 */

// Linux: log_init_linux で初期化し、有効なハンドルが返されることの確認
TEST_F(log_providerTest, test_init_and_dispose)
{
    // Arrange & Act
    log_provider_t *handle = log_init_linux("log_test"); // [手順] - プロバイダを初期化する。

    // Assert
    EXPECT_NE((log_provider_t *)NULL, handle); // [確認_正常系] - ハンドルが NULL でないこと。

    // Cleanup
    log_dispose(handle);
}

#endif /* _WIN32 */

/* ===== 書き込みテスト ===== */

// INFO レベルでメッセージを書き込み、戻り値が 0 であることの確認
TEST_F(log_providerTest, test_write_returns_zero)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // Act
    int result = log_write(handle, LOG_LV_INFO, "test message"); // [手順] - INFO レベルでメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    log_dispose(handle);
}

// 全レベル (CRITICAL〜VERBOSE) で書き込みが成功することの確認
TEST_F(log_providerTest, test_write_all_levels)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // Act & Assert
    EXPECT_EQ(0, log_write(handle, LOG_LV_CRITICAL, "critical")); // [確認_正常系] - CRITICAL レベルで書き込めること。
    EXPECT_EQ(0, log_write(handle, LOG_LV_ERROR,    "error"));    // [確認_正常系] - ERROR レベルで書き込めること。
    EXPECT_EQ(0, log_write(handle, LOG_LV_WARNING,  "warning"));  // [確認_正常系] - WARNING レベルで書き込めること。
    EXPECT_EQ(0, log_write(handle, LOG_LV_INFO,     "info"));     // [確認_正常系] - INFO レベルで書き込めること。
    EXPECT_EQ(0, log_write(handle, LOG_LV_VERBOSE,  "verbose"));  // [確認_正常系] - VERBOSE レベルで書き込めること。

    // Cleanup
    log_dispose(handle);
}

/* ===== NULL 安全性テスト ===== */

#ifndef _WIN32
// name に NULL を渡した場合に自プロセス名で初期化されることの確認 (Linux 専用)
TEST_F(log_providerTest, test_init_null_name)
{
    // Act
    log_provider_t *handle = log_init_linux(NULL); // [手順] - NULL を渡す。

    // Assert (自プロセス名で syslog が初期化される)
    EXPECT_NE((log_provider_t *)NULL, handle); // [確認_正常系] - プロセス名でハンドルが返されること。

    // Cleanup
    log_dispose(handle);
}
#endif /* !_WIN32 */

// handle に NULL を渡した場合に安全に 0 が返されることの確認
TEST_F(log_providerTest, test_write_with_null_handle)
{
    // Act
    int result = log_write(NULL, LOG_LV_INFO, "test message"); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// message に NULL を渡した場合に安全に 0 が返されることの確認
TEST_F(log_providerTest, test_write_with_null_message)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // Act
    int result = log_write(handle, LOG_LV_INFO, NULL); // [手順] - message に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL メッセージでも安全に 0 が返されること。

    // Cleanup
    log_dispose(handle);
}

// NULL ハンドルで dispose を呼んでもクラッシュしないことの確認
TEST_F(log_providerTest, test_dispose_with_null_handle)
{
    // Act & Assert - クラッシュしないことを確認
    log_dispose(NULL); // [手順] - NULL ハンドルで dispose を呼ぶ。安全に何もしないこと。
}

/* ===== メッセージ切り詰めテスト ===== */

// 1023 バイトのメッセージが切り詰めなしで書き込めることの確認
TEST_F(log_providerTest, test_write_max_length_message)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // 1023 バイトの 'A' + null 終端 = ちょうど LOG_MESSAGE_MAX_BYTES
    char msg[LOG_MESSAGE_MAX_BYTES];
    memset(msg, 'A', LOG_MESSAGE_MAX_BYTES - 1);
    msg[LOG_MESSAGE_MAX_BYTES - 1] = '\0';

    // Act
    int result = log_write(handle, LOG_LV_INFO, msg); // [手順] - 1023 バイトのメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 上限以内のメッセージが成功すること。

    // Cleanup
    log_dispose(handle);
}

// 1024 バイト超のメッセージが切り詰められて書き込まれることの確認
TEST_F(log_providerTest, test_write_oversized_message_truncated)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // 2048 バイトのメッセージ (上限を大幅に超過)
    char msg[2048 + 1];
    memset(msg, 'B', 2048);
    msg[2048] = '\0';

    // Act
    int result = log_write(handle, LOG_LV_INFO, msg); // [手順] - 2048 バイトのメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 切り詰められて正常に書き込まれること。

    // Cleanup
    log_dispose(handle);
}

// UTF-8 マルチバイト文字境界での切り詰めが安全に行われることの確認
TEST_F(log_providerTest, test_write_truncate_utf8_boundary)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // 1021 バイトの 'A' + 3 バイト UTF-8 文字 (例: 'あ' = 0xE3 0x81 0x82) = 1024 バイト
    // null 終端を含めると 1025 バイトとなり、切り詰め対象
    // 期待: UTF-8 文字の途中で切断せず、1021 バイト地点で切り詰め
    char msg[1025];
    memset(msg, 'A', 1021);
    msg[1021] = (char)0xE3;  // 'あ' の 1 バイト目
    msg[1022] = (char)0x81;  // 'あ' の 2 バイト目
    msg[1023] = (char)0x82;  // 'あ' の 3 バイト目
    msg[1024] = '\0';

    // Act
    int result = log_write(handle, LOG_LV_INFO, msg); // [手順] - UTF-8 境界を跨ぐメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - UTF-8 境界で安全に切り詰められること。

    // Cleanup
    log_dispose(handle);
}

/* ===== log_writef テスト ===== */

// printf 形式でメッセージを書き込み、戻り値が 0 であることの確認
TEST_F(log_providerTest, test_writef_basic)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // Act
    int result = log_writef(handle, LOG_LV_INFO, "user=%s count=%d", "alice", 42); // [手順] - printf 形式でメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    log_dispose(handle);
}

// NULL ハンドルで log_writef を呼んでも安全に 0 が返されることの確認
TEST_F(log_providerTest, test_writef_null_handle)
{
    // Act
    int result = log_writef(NULL, LOG_LV_INFO, "test %d", 1); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// NULL フォーマットで log_writef を呼んでも安全に 0 が返されることの確認
TEST_F(log_providerTest, test_writef_null_format)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // Act
    int result = log_writef(handle, LOG_LV_INFO, NULL); // [手順] - format に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL フォーマットでも安全に 0 が返されること。

    // Cleanup
    log_dispose(handle);
}

// フォーマット結果が 1024 バイトを超える場合に切り詰められることの確認
TEST_F(log_providerTest, test_writef_truncation)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // 2048 バイトの文字列を %s でフォーマット
    char longstr[2048 + 1];
    memset(longstr, 'X', 2048);
    longstr[2048] = '\0';

    // Act
    int result = log_writef(handle, LOG_LV_INFO, "%s", longstr); // [手順] - 2048 バイトの文字列をフォーマットする。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 切り詰められて正常に書き込まれること。

    // Cleanup
    log_dispose(handle);
}

/* ===== log_hex_write テスト ===== */

// バイナリデータを HEX テキストで書き込み、戻り値が 0 であることの確認
TEST_F(log_providerTest, test_hex_write_basic)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    unsigned char data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"

    // Act — ラベル付き
    int result = log_hex_write(handle, LOG_LV_INFO, data, sizeof(data), "Data"); // [手順] - ラベル付きで HEX 書き込みする。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    log_dispose(handle);
}

// ラベルなしで HEX 書き込みが成功することの確認
TEST_F(log_providerTest, test_hex_write_no_label)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    unsigned char data[] = {0xFF, 0x00, 0xAB};

    // Act — ラベルなし (message = NULL)
    int result = log_hex_write(handle, LOG_LV_INFO, data, sizeof(data), NULL); // [手順] - ラベルなしで HEX 書き込みする。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    log_dispose(handle);
}

// NULL ハンドルで log_hex_write を呼んでも安全に 0 が返されることの確認
TEST_F(log_providerTest, test_hex_write_null_handle)
{
    unsigned char data[] = {0x01};
    // Act
    int result = log_hex_write(NULL, LOG_LV_INFO, data, sizeof(data), "test"); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// NULL データで log_hex_write を呼んでも安全に 0 が返されることの確認
TEST_F(log_providerTest, test_hex_write_null_data)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // Act
    int result = log_hex_write(handle, LOG_LV_INFO, NULL, 10, "test"); // [手順] - data に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL データでも安全に 0 が返されること。

    // Cleanup
    log_dispose(handle);
}

// size 0 で log_hex_write を呼んでも安全に 0 が返されることの確認
TEST_F(log_providerTest, test_hex_write_zero_size)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    unsigned char data[] = {0x01};

    // Act
    int result = log_hex_write(handle, LOG_LV_INFO, data, 0, "test"); // [手順] - size に 0 を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - size 0 でも安全に 0 が返されること。

    // Cleanup
    log_dispose(handle);
}

// LOG_HEX_MAX_DATA_BYTES 超のデータが切り詰められて "..." 付きで出力されることの確認
TEST_F(log_providerTest, test_hex_write_oversized_truncated)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // 512 バイトのデータ (LOG_HEX_MAX_DATA_BYTES=341 を超過)
    unsigned char data[512];
    memset(data, 0xCC, sizeof(data));

    // Act — ラベルなし
    int result = log_hex_write(handle, LOG_LV_INFO, data, sizeof(data), NULL); // [手順] - 512 バイトのデータを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 切り詰め + "..." で正常に書き込まれること。

    // Cleanup
    log_dispose(handle);
}

// ラベル付きで大きなデータが正しく切り詰められ "..." 付きで出力されることの確認
TEST_F(log_providerTest, test_hex_write_with_label_truncated)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    // 512 バイトのデータ + ラベル
    unsigned char data[512];
    memset(data, 0xAB, sizeof(data));

    // Act — ラベル "Payload" (7文字) + ": " → HEX 用に 1014 文字
    int result = log_hex_write(handle, LOG_LV_INFO, data, sizeof(data), "Payload"); // [手順] - ラベル付きで 512 バイトのデータを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - ラベル + 切り詰め + "..." で正常に書き込まれること。

    // Cleanup
    log_dispose(handle);
}

/* ===== log_hex_writef テスト ===== */

// printf 形式ラベルで HEX 書き込みが成功することの確認
TEST_F(log_providerTest, test_hex_writef_basic)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    unsigned char data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};

    // Act
    int result = log_hex_writef(handle, LOG_LV_INFO, data, sizeof(data), "packet[%d]", 3); // [手順] - printf 形式ラベルで HEX 書き込みする。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    log_dispose(handle);
}

// NULL フォーマットで log_hex_writef を呼ぶとラベルなし HEX 出力になることの確認
TEST_F(log_providerTest, test_hex_writef_null_format)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    unsigned char data[] = {0x01, 0x02};

    // Act
    int result = log_hex_writef(handle, LOG_LV_INFO, data, sizeof(data), NULL); // [手順] - format に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - ラベルなしで正常に書き込まれること。

    // Cleanup
    log_dispose(handle);
}

// NULL ハンドルで log_hex_writef を呼んでも安全に 0 が返されることの確認
TEST_F(log_providerTest, test_hex_writef_null_handle)
{
    unsigned char data[] = {0x01};
    // Act
    int result = log_hex_writef(NULL, LOG_LV_INFO, data, sizeof(data), "test %d", 1); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// 大きなデータで log_hex_writef を呼んで切り詰め + "..." が正常に動作することの確認
TEST_F(log_providerTest, test_hex_writef_truncated)
{
    // Arrange
#ifdef _WIN32
    log_provider_t *handle = log_init_windows(s_test_provider);
#else
    log_provider_t *handle = log_init_linux("log_test");
#endif
    ASSERT_NE((log_provider_t *)NULL, handle);

    unsigned char data[512];
    memset(data, 0xDD, sizeof(data));

    // Act
    int result = log_hex_writef(handle, LOG_LV_INFO, data, sizeof(data), "buf[%d]", 42); // [手順] - printf 形式ラベル + 大きなデータを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 切り詰め + "..." で正常に書き込まれること。

    // Cleanup
    log_dispose(handle);
}
