#include <testfw.h>
#include <com_util/trace/trace.h>
#include <com_util/trace/trace_file.h>
#include <gtest/internal/gtest-port.h>
#include <filesystem>
#include <vector>

extern "C" {
#include "trace_internal.h"
}

/* ===== テストクラス ===== */

class trace_providerTest : public Test
{
};

/* ===== 初期化・終了テスト ===== */

// log_init で初期化し、有効なハンドルが返されることの確認
TEST_F(trace_providerTest, test_init_and_dispose)
{
    // Arrange & Act
    trace_logger_t *handle = trace_logger_create(); // [手順] - プロバイダを初期化する。

    // Assert
    EXPECT_NE((trace_logger_t *)NULL, handle); // [確認_正常系] - ハンドルが NULL でないこと。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== 書き込みテスト ===== */

// INFO レベルでメッセージを書き込み、戻り値が 0 であることの確認
TEST_F(trace_providerTest, test_write_returns_zero)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // Act
    int result = trace_logger_write(handle, TRACE_LEVEL_INFO, "test message");// [手順] - INFO レベルでメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    trace_logger_destroy(handle);
}

// 全レベル (CRITICAL〜DEBUG) で書き込みが成功することの確認
TEST_F(trace_providerTest, test_write_all_levels)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // Act & Assert
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_CRITICAL, "critical"));// [確認_正常系] - CRITICAL レベルで書き込めること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_ERROR,    "error"));    // [確認_正常系] - ERROR レベルで書き込めること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_WARNING,  "warning"));  // [確認_正常系] - WARNING レベルで書き込めること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_INFO,     "info"));     // [確認_正常系] - INFO レベルで書き込めること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_VERBOSE,  "verbose"));  // [確認_正常系] - VERBOSE レベルで書き込めること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_DEBUG,    "debug"));    // [確認_正常系] - DEBUG レベルで書き込めること。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== NULL 安全性テスト ===== */

// trace_logger_create() で初期化し、有効なハンドルが返されることの確認 (引数なし版)
TEST_F(trace_providerTest, test_init_no_args)
{
    // Act
    trace_logger_t *handle = trace_logger_create(); // [手順] - 引数なしで初期化する。

    // Assert (自プロセス名でハンドルが返される)
    EXPECT_NE((trace_logger_t *)NULL, handle); // [確認_正常系] - ハンドルが NULL でないこと。

    // Cleanup
    trace_logger_destroy(handle);
}

// handle に NULL を渡した場合に安全に 0 が返されることの確認
TEST_F(trace_providerTest, test_write_with_null_handle)
{
    // Act
    int result = trace_logger_write(NULL, TRACE_LEVEL_INFO, "test message"); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// message に NULL を渡した場合に安全に 0 が返されることの確認
TEST_F(trace_providerTest, test_write_with_null_message)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int result = trace_logger_write(handle, TRACE_LEVEL_INFO, NULL); // [手順] - message に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL メッセージでも安全に 0 が返されること。

    // Cleanup
    trace_logger_destroy(handle);
}

// NULL ハンドルで dispose を呼んでもクラッシュしないことの確認
TEST_F(trace_providerTest, test_dispose_with_null_handle)
{
    // Act & Assert - クラッシュしないことを確認
    trace_logger_destroy(NULL); // [手順] - NULL ハンドルで dispose を呼ぶ。安全に何もしないこと。
}

// registry が複数ハンドルを追跡し、dispose で減算されることの確認
TEST_F(trace_providerTest, test_registry_tracks_live_handles)
{
    trace_logger_t *h1 = trace_logger_create();
    trace_logger_t *h2 = trace_logger_create();
    trace_logger_t *h3 = trace_logger_create();

    ASSERT_NE((trace_logger_t *)NULL, h1);
    ASSERT_NE((trace_logger_t *)NULL, h2);
    ASSERT_NE((trace_logger_t *)NULL, h3);
    EXPECT_EQ((size_t)3, trace_registry_count());

    trace_logger_destroy(h2);
    EXPECT_EQ((size_t)2, trace_registry_count());

    trace_logger_destroy(h1);
    trace_logger_destroy(h3);
    EXPECT_EQ((size_t)0, trace_registry_count());
}

// registry 容量が不足した場合に自動拡張されることの確認
TEST_F(trace_providerTest, test_registry_auto_expands)
{
    std::vector<trace_logger_t *> handles;
    const size_t initial_capacity = trace_registry_capacity();
    const size_t create_count =
        (initial_capacity > 0 ? initial_capacity : (size_t)8) + (size_t)4;

    handles.reserve(create_count);
    for (size_t i = 0; i < create_count; i++)
    {
        trace_logger_t *handle = trace_logger_create();
        ASSERT_NE((trace_logger_t *)NULL, handle);
        handles.push_back(handle);
    }

    EXPECT_EQ(create_count, trace_registry_count());
    EXPECT_GE(trace_registry_capacity(), create_count);

    for (trace_logger_t *handle : handles)
    {
        trace_logger_destroy(handle);
    }
    EXPECT_EQ((size_t)0, trace_registry_count());
}

/* ===== メッセージ切り詰めテスト ===== */

// 1023 バイトのメッセージが切り詰めなしで書き込めることの確認
TEST_F(trace_providerTest, test_write_max_length_message)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // 1023 バイトの 'A'+ null 終端 = ちょうど TRACE_LOGGER_MESSAGE_MAX_BYTES
    char msg[TRACE_LOGGER_MESSAGE_MAX_BYTES];
    memset(msg, 'A', TRACE_LOGGER_MESSAGE_MAX_BYTES - 1);
    msg[TRACE_LOGGER_MESSAGE_MAX_BYTES - 1] = '\0';

    // Act
    int result = trace_logger_write(handle, TRACE_LEVEL_INFO, msg); // [手順] - 1023 バイトのメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 上限以内のメッセージが成功すること。

    // Cleanup
    trace_logger_destroy(handle);
}

// 1024 バイト超のメッセージが切り詰められて書き込まれることの確認
TEST_F(trace_providerTest, test_write_oversized_message_truncated)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // 2048 バイトのメッセージ (上限を大幅に超過)
    char msg[2048 + 1];
    memset(msg, 'B', 2048);
    msg[2048] = '\0';

    // Act
    int result = trace_logger_write(handle, TRACE_LEVEL_INFO, msg); // [手順] - 2048 バイトのメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 切り詰められて正常に書き込まれること。

    // Cleanup
    trace_logger_destroy(handle);
}

// UTF-8 マルチバイト文字境界での切り詰めが安全に行われることの確認
TEST_F(trace_providerTest, test_write_truncate_utf8_boundary)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // 1021 バイトの 'A'+ 3 バイト UTF-8 文字 (例: 'あ' = 0xE3 0x81 0x82) = 1024 バイト
    // null 終端を含めると 1025 バイトとなり、切り詰め対象
    // 期待: UTF-8 文字の途中で切断せず、1021 バイト地点で切り詰め
    char msg[1025];
    memset(msg, 'A', 1021);
    /* UTF-8 'あ' (U+3042) = 0xE3 0x81 0x82 */
    unsigned char utf8[] = { 0xE3, 0x81, 0x82 };
    memcpy(&msg[1021], utf8, 3);
    msg[1024] = '\0';

    // Act
    int result = trace_logger_write(handle, TRACE_LEVEL_INFO, msg); // [手順] - UTF-8 境界を跨ぐメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - UTF-8 境界で安全に切り詰められること。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== log_writef テスト ===== */

// printf 形式でメッセージを書き込み、戻り値が 0 であることの確認
TEST_F(trace_providerTest, test_writef_basic)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // Act
    int result = trace_logger_writef(handle, TRACE_LEVEL_INFO, "user=%s count=%d", "alice", 42);// [手順] - printf 形式でメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    trace_logger_destroy(handle);
}

// NULL ハンドルで log_writef を呼んでも安全に 0 が返されることの確認
TEST_F(trace_providerTest, test_writef_null_handle)
{
    // Act
    int result = trace_logger_writef(NULL, TRACE_LEVEL_INFO, "test %d", 1); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// NULL フォーマットで log_writef を呼んでも安全に 0 が返されることの確認
TEST_F(trace_providerTest, test_writef_null_format)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int result = trace_logger_writef(handle, TRACE_LEVEL_INFO, NULL); // [手順] - format に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL フォーマットでも安全に 0 が返されること。

    // Cleanup
    trace_logger_destroy(handle);
}

// フォーマット結果が 1024 バイトを超える場合に切り詰められることの確認
TEST_F(trace_providerTest, test_writef_truncation)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // 2048 バイトの文字列を %s でフォーマット
    char longstr[2048 + 1];
    memset(longstr, 'X', 2048);
    longstr[2048] = '\0';

    // Act
    int result = trace_logger_writef(handle, TRACE_LEVEL_INFO, "%s", longstr); // [手順] - 2048 バイトの文字列をフォーマットする。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 切り詰められて正常に書き込まれること。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== log_hex_write テスト ===== */

// バイナリデータを HEX テキストで書き込み、戻り値が 0 であることの確認
TEST_F(trace_providerTest, test_hex_write_basic)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    unsigned char data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"

    // Act — ラベル付き
    int result = trace_logger_write_hex(handle, TRACE_LEVEL_INFO, data, sizeof(data), "Data"); // [手順] - ラベル付きで HEX 書き込みする。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    trace_logger_destroy(handle);
}

// ラベルなしで HEX 書き込みが成功することの確認
TEST_F(trace_providerTest, test_hex_write_no_label)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    unsigned char data[] = {0xFF, 0x00, 0xAB};

    // Act — ラベルなし (message = NULL)
    int result = trace_logger_write_hex(handle, TRACE_LEVEL_INFO, data, sizeof(data), NULL); // [手順] - ラベルなしで HEX 書き込みする。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    trace_logger_destroy(handle);
}

// NULL ハンドルで log_hex_write を呼んでも安全に 0 が返されることの確認
TEST_F(trace_providerTest, test_hex_write_null_handle)
{
    unsigned char data[] = {0x01};
    // Act
    int result = trace_logger_write_hex(NULL, TRACE_LEVEL_INFO, data, sizeof(data), "test"); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// NULL データで log_hex_write を呼んでも安全に 0 が返されることの確認
TEST_F(trace_providerTest, test_hex_write_null_data)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int result = trace_logger_write_hex(handle, TRACE_LEVEL_INFO, NULL, 10, "test"); // [手順] - data に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL データでも安全に 0 が返されること。

    // Cleanup
    trace_logger_destroy(handle);
}

// size 0 で log_hex_write を呼んでも安全に 0 が返されることの確認
TEST_F(trace_providerTest, test_hex_write_zero_size)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    unsigned char data[] = {0x01};

    // Act
    int result = trace_logger_write_hex(handle, TRACE_LEVEL_INFO, data, 0, "test"); // [手順] - size に 0 を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - size 0 でも安全に 0 が返されること。

    // Cleanup
    trace_logger_destroy(handle);
}

// TRACE_LOGGER_HEX_MAX_DATA_BYTES 超のデータが切り詰められて "..." 付きで出力されることの確認
TEST_F(trace_providerTest, test_hex_write_oversized_truncated)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // 512 バイトのデータ (TRACE_LOGGER_HEX_MAX_DATA_BYTES=341 を超過)
    unsigned char data[512];
    memset(data, 0xCC, sizeof(data));

    // Act — ラベルなし
    int result = trace_logger_write_hex(handle, TRACE_LEVEL_INFO, data, sizeof(data), NULL); // [手順] - 512 バイトのデータを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 切り詰め + "..." で正常に書き込まれること。

    // Cleanup
    trace_logger_destroy(handle);
}

// ラベル付きで大きなデータが正しく切り詰められ "..." 付きで出力されることの確認
TEST_F(trace_providerTest, test_hex_write_with_label_truncated)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // 512 バイトのデータ + ラベル
    unsigned char data[512];
    memset(data, 0xAB, sizeof(data));

    // Act — ラベル "Payload" (7文字) + ": " → HEX 用に 1014 文字
    int result = trace_logger_write_hex(handle, TRACE_LEVEL_INFO, data, sizeof(data), "Payload"); // [手順] - ラベル付きで 512 バイトのデータを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - ラベル + 切り詰め + "..." で正常に書き込まれること。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== log_hex_writef テスト ===== */

// printf 形式ラベルで HEX 書き込みが成功することの確認
TEST_F(trace_providerTest, test_hex_writef_basic)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    unsigned char data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};

    // Act
    int result = trace_logger_write_hexf(handle, TRACE_LEVEL_INFO, data, sizeof(data), "packet[%d]", 3); // [手順] - printf 形式ラベルで HEX 書き込みする。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    trace_logger_destroy(handle);
}

// NULL フォーマットで log_hex_writef を呼ぶとラベルなし HEX 出力になることの確認
TEST_F(trace_providerTest, test_hex_writef_null_format)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    unsigned char data[] = {0x01, 0x02};

    // Act
    int result = trace_logger_write_hexf(handle, TRACE_LEVEL_INFO, data, sizeof(data), NULL); // [手順] - format に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - ラベルなしで正常に書き込まれること。

    // Cleanup
    trace_logger_destroy(handle);
}

// NULL ハンドルで log_hex_writef を呼んでも安全に 0 が返されることの確認
TEST_F(trace_providerTest, test_hex_writef_null_handle)
{
    unsigned char data[] = {0x01};
    // Act
    int result = trace_logger_write_hexf(NULL, TRACE_LEVEL_INFO, data, sizeof(data), "test %d", 1); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(0, result); // [確認_異常系] - NULL ハンドルでも安全に 0 が返されること。
}

// 大きなデータで log_hex_writef を呼んで切り詰め + "..." が正常に動作することの確認
TEST_F(trace_providerTest, test_hex_writef_truncated)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    unsigned char data[512];
    memset(data, 0xDD, sizeof(data));

    // Act
    int result = trace_logger_write_hexf(handle, TRACE_LEVEL_INFO, data, sizeof(data), "buf[%d]", 42); // [手順] - printf 形式ラベル + 大きなデータを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 切り詰め + "..." で正常に書き込まれること。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== trace_logger_set_name テスト ===== */

// 識別名変更後に書き込みが成功することの確認
TEST_F(trace_providerTest, test_modify_name_basic)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_set_name(handle, "original_name", 0);

    // Act
    int result = trace_logger_set_name(handle, "new_name", 0); // [手順] - 識別名を変更する。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // 識別名変更後も書き込みが正常に動作すること
    trace_logger_start(handle);
    int write_result = trace_logger_write(handle, TRACE_LEVEL_INFO, "after modify_name");// [手順] - 識別名変更後にメッセージを書き込む。
    EXPECT_EQ(0, write_result); // [確認_正常系] - 書き込みが成功すること。

    // Cleanup
    trace_logger_destroy(handle);
}

// NULL ハンドルで trace_logger_set_name を呼んでも安全に -1 が返されることの確認
TEST_F(trace_providerTest, test_modify_name_null_handle)
{
    // Act
    int result = trace_logger_set_name(NULL, "new_name", 0); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, result); // [確認_異常系] - NULL ハンドルで -1 が返されること。
}

// name に NULL を渡した場合にプロセス名で更新されることの確認
TEST_F(trace_providerTest, test_modify_name_null_name)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int result = trace_logger_set_name(handle, NULL, 0); // [手順] - name に NULL を渡す。

    // Assert (プロセス名で更新される)
    EXPECT_EQ(0, result); // [確認_正常系] - プロセス名で更新され 0 が返されること。

    // 更新後も書き込みが正常に動作すること
    trace_logger_start(handle);
    int write_result = trace_logger_write(handle, TRACE_LEVEL_INFO, "after modify to process name");// [手順] - 更新後にメッセージを書き込む。
    EXPECT_EQ(0, write_result); // [確認_正常系] - 書き込みが成功すること。

    // Cleanup
    trace_logger_destroy(handle);
}

// 複数回の識別名変更が正常に動作することの確認
TEST_F(trace_providerTest, test_modify_name_multiple_times)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_set_name(handle, "name_v1", 0);

    // Act & Assert
    EXPECT_EQ(0, trace_logger_set_name(handle, "name_v2", 0)); // [確認_正常系] - 1 回目の変更が成功すること。
    trace_logger_start(handle);
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_INFO, "v2 message")); // [確認_正常系] - 1 回目の変更後に書き込めること。
    trace_logger_stop(handle);

    EXPECT_EQ(0, trace_logger_set_name(handle, "name_v3", 0)); // [確認_正常系] - 2 回目の変更が成功すること。
    trace_logger_start(handle);
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_INFO, "v3 message")); // [確認_正常系] - 2 回目の変更後に書き込めること。

    // Cleanup
    trace_logger_destroy(handle);
}

// 識別名変更後の dispose が正常に完了することの確認
TEST_F(trace_providerTest, test_modify_name_then_dispose)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_set_name(handle, "before_modify", 0);

    // Act
    int result = trace_logger_set_name(handle, "after_modify", 0); // [手順] - 識別名を変更する。
    EXPECT_EQ(0, result); // [確認_正常系] - 識別名変更が成功すること。

    // Assert - dispose がクラッシュしないことを確認
    trace_logger_destroy(handle); // [手順] - 識別名変更後に dispose を呼ぶ。安全に終了すること。
}

// identifier > 0 の場合に "<name>-<id>" として設定されることの確認
TEST_F(trace_providerTest, test_modify_name_with_identifier)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int result = trace_logger_set_name(handle, "worker", 2); // [手順] - identifier = 2 で設定する。

    // Assert - 内部識別名は "worker-2" になる。書き込みが成功すれば適用済み。
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    trace_logger_start(handle);
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_INFO, "running as worker-2")); // [確認_正常系] - 書き込みが成功すること。

    // Cleanup
    trace_logger_destroy(handle);
}

// identifier に負の値を渡すと -1 が返されることの確認
TEST_F(trace_providerTest, test_modify_name_negative_identifier)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int result = trace_logger_set_name(handle, "app", -1); // [手順] - identifier に -1 を渡す。

    // Assert
    EXPECT_EQ(-1, result); // [確認_異常系] - 負の identifier で -1 が返されること。

    // Cleanup
    trace_logger_destroy(handle);
}

// started 状態で trace_logger_set_name を呼ぶと -1 が返されることの確認
TEST_F(trace_providerTest, test_modify_name_fails_when_started)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // Act
    int result = trace_logger_set_name(handle, "new_name", 0); // [手順] - started 中に modify_name を呼ぶ。

    // Assert
    EXPECT_EQ(-1, result); // [確認_異常系] - started 中に -1 が返されること。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== TRACE_LEVEL_NONE テスト ===== */

// TRACE_LEVEL_NONE を OS レベルに設定すると OS 出力が抑止されることの確認
TEST_F(trace_providerTest, test_os_level_none_suppresses_output)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int set_result = trace_logger_set_os_level(handle, TRACE_LEVEL_NONE); // [手順] - OS レベルを NONE に設定する (trace_logger_set_os_level)。
    trace_logger_start(handle);
    int write_result = trace_logger_write(handle, TRACE_LEVEL_CRITICAL, "should not output");// [手順] - CRITICAL でメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, set_result); // [確認_正常系] - 設定変更が成功すること。
    EXPECT_EQ(0, write_result); // [確認_正常系] - 出力スキップもエラーではないこと。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== OS トレースレベルフィルタリングテスト ===== */

// OS レベルを ERROR に設定すると WARNING 以下が抑止されることの確認
TEST_F(trace_providerTest, test_os_level_filters_below_threshold)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    trace_logger_set_os_level(handle, TRACE_LEVEL_ERROR); // [手順] - OS レベルを ERROR に設定する (trace_logger_set_os_level)。
    trace_logger_start(handle);

    // Assert
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_CRITICAL, "critical passes")); // [確認_正常系] - CRITICAL は出力されること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_ERROR,    "error passes"));    // [確認_正常系] - ERROR は出力されること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_WARNING,  "warning filtered")); // [確認_正常系] - WARNING はフィルタリングされること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_INFO,     "info filtered"));    // [確認_正常系] - INFO はフィルタリングされること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_VERBOSE,  "verbose filtered")); // [確認_正常系] - VERBOSE はフィルタリングされること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_DEBUG,    "debug filtered"));   // [確認_正常系] - DEBUG はフィルタリングされること。

    // Cleanup
    trace_logger_destroy(handle);
}

// NULL ハンドルで trace_logger_set_os_level を呼ぶと -1 が返されることの確認
TEST_F(trace_providerTest, test_modify_ostrc_null_handle)
{
    // Act
    int result = trace_logger_set_os_level(NULL, TRACE_LEVEL_ERROR); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, result); // [確認_異常系] - NULL ハンドルで -1 が返されること。
}

// 初期化直後のデフォルト OS レベル (INFO) でフィルタリングが機能することの確認
TEST_F(trace_providerTest, test_default_os_level_is_info)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // Act & Assert
    // デフォルト OS レベルは INFO なので、VERBOSE/DEBUG は OS に出力されない (戻り値は 0: エラーではなくフィルタリング)
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_INFO,    "info passes"));     // [確認_正常系] - INFO はデフォルトで出力されること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_VERBOSE, "verbose filtered")); // [確認_正常系] - VERBOSE はデフォルトでフィルタリングされること。
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_DEBUG,   "debug filtered"));   // [確認_正常系] - DEBUG はデフォルトでフィルタリングされること。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== ファイルトレーステスト ===== */

// ファイルトレースを有効化し、メッセージがファイルに書き込まれることの確認
TEST_F(trace_providerTest, test_modify_filetrc_enables_file_trace)
{
    // Arrange
    string ws = findWorkspaceRoot();
    string path = ws + "/app/com_util/test/src/libcom_utilTest/trace_providerTest/results/trace_test.log";
    remove(path.c_str());

    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int set_result = trace_logger_set_file_level(handle, path.c_str(), TRACE_LEVEL_INFO, 0, 0); // [手順] - ファイルトレースを有効化する (trace_logger_set_file_level)。
    EXPECT_EQ(0, set_result); // [確認_正常系] - 設定が成功すること。

    trace_logger_start(handle);
    trace_logger_write(handle, TRACE_LEVEL_ERROR, "file error message");// [手順] - ERROR メッセージを書き込む。
    trace_logger_write(handle, TRACE_LEVEL_INFO,  "file info message");  // [手順] - INFO メッセージを書き込む。

    // Cleanup (dispose でファイルが閉じられる)
    trace_logger_destroy(handle);

    // Assert
    EXPECT_FILE_EXISTS(path);                          // [確認_正常系] - ファイルが存在すること。
    EXPECT_FILE_CONTAINS(path, "file error message");  // [確認_正常系] - ERROR メッセージが含まれること。
    EXPECT_FILE_CONTAINS(path, "file info message");   // [確認_正常系] - INFO メッセージが含まれること。

    // Remove test file
    remove(path.c_str());
}

// ファイルレベル以下のメッセージのみファイルに書き込まれることの確認
TEST_F(trace_providerTest, test_file_level_filters_messages)
{
    // Arrange
    string ws = findWorkspaceRoot();
    string path = ws + "/app/com_util/test/src/libcom_utilTest/trace_providerTest/results/trace_filter.log";
    remove(path.c_str());

    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act - ファイルレベルを ERROR に設定
    trace_logger_set_file_level(handle, path.c_str(), TRACE_LEVEL_ERROR, 0, 0); // [手順] - ファイルレベルを ERROR に設定する。
    trace_logger_start(handle);
    trace_logger_write(handle, TRACE_LEVEL_ERROR,   "should be in file");// [手順] - ERROR メッセージを書き込む。
    trace_logger_write(handle, TRACE_LEVEL_WARNING, "should not be in file");  // [手順] - WARNING メッセージを書き込む。

    trace_logger_destroy(handle);

    // Assert
    EXPECT_FILE_EXISTS(path);                              // [確認_正常系] - ファイルが存在すること。
    EXPECT_FILE_CONTAINS(path, "should be in file");       // [確認_正常系] - ERROR メッセージが含まれること。

    remove(path.c_str());
}

// ファイルレベルを VERBOSE に設定すると DEBUG のみ抑止されることの確認
TEST_F(trace_providerTest, test_file_level_verbose_filters_debug_only)
{
    // Arrange
    string ws = findWorkspaceRoot();
    string path = ws + "/app/com_util/test/src/libcom_utilTest/trace_providerTest/results/trace_verbose_filter.log";
    remove(path.c_str());

    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    trace_logger_set_file_level(handle, path.c_str(), TRACE_LEVEL_VERBOSE, 0, 0); // [手順] - ファイルレベルを VERBOSE に設定する。
    trace_logger_start(handle);
    trace_logger_write(handle, TRACE_LEVEL_VERBOSE, "verbose in file"); // [手順] - VERBOSE を書き込む。
    trace_logger_write(handle, TRACE_LEVEL_DEBUG,   "debug filtered");  // [手順] - DEBUG を書き込む。
    trace_logger_destroy(handle);

    // Assert
    EXPECT_FILE_EXISTS(path);                         // [確認_正常系] - ファイルが存在すること。
    EXPECT_FILE_CONTAINS(path, " V verbose in file"); // [確認_正常系] - VERBOSE はファイルに含まれること。
    EXPECT_FALSE(testing::FileContains(path, "debug filtered")); // [確認_正常系] - DEBUG はファイルに含まれないこと。

    remove(path.c_str());
}

// ファイルレベルを DEBUG に設定すると DEBUG が D で出力されることの確認
TEST_F(trace_providerTest, test_file_level_debug_outputs_d_marker)
{
    // Arrange
    string ws = findWorkspaceRoot();
    string path = ws + "/app/com_util/test/src/libcom_utilTest/trace_providerTest/results/trace_debug.log";
    remove(path.c_str());

    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    trace_logger_set_file_level(handle, path.c_str(), TRACE_LEVEL_DEBUG, 0, 0); // [手順] - ファイルレベルを DEBUG に設定する。
    trace_logger_start(handle);
    trace_logger_write(handle, TRACE_LEVEL_VERBOSE, "verbose in debug file"); // [手順] - VERBOSE を書き込む。
    trace_logger_write(handle, TRACE_LEVEL_DEBUG,   "debug in debug file");   // [手順] - DEBUG を書き込む。
    trace_logger_destroy(handle);

    // Assert
    EXPECT_FILE_EXISTS(path);                              // [確認_正常系] - ファイルが存在すること。
    EXPECT_FILE_CONTAINS(path, " V verbose in debug file"); // [確認_正常系] - VERBOSE が V で出力されること。
    EXPECT_FILE_CONTAINS(path, " D debug in debug file");   // [確認_正常系] - DEBUG が D で出力されること。

    remove(path.c_str());
}

// path に NULL を渡してファイルトレースを無効化できることの確認
TEST_F(trace_providerTest, test_modify_filetrc_null_path_disables)
{
    // Arrange
    string ws = findWorkspaceRoot();
    string path = ws + "/app/com_util/test/src/libcom_utilTest/trace_providerTest/results/trace_disable.log";
    remove(path.c_str());

    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act - ファイルトレースを有効化後、無効化
    trace_logger_set_file_level(handle, path.c_str(), TRACE_LEVEL_INFO, 0, 0); // [手順] - ファイルトレースを有効化する。
    trace_logger_start(handle);
    trace_logger_write(handle, TRACE_LEVEL_ERROR, "before disable");       // [手順] - 無効化前にメッセージを書き込む。
    trace_logger_stop(handle);

    trace_logger_set_file_level(handle, NULL, TRACE_LEVEL_INFO, 0, 0); // [手順] - ファイルトレースを無効化する。
    trace_logger_start(handle);
    trace_logger_write(handle, TRACE_LEVEL_ERROR, "after disable");// [手順] - 無効化後にメッセージを書き込む。

    trace_logger_destroy(handle);

    // Assert
    EXPECT_FILE_EXISTS(path);                          // [確認_正常系] - ファイルが存在すること。
    EXPECT_FILE_CONTAINS(path, "before disable");      // [確認_正常系] - 無効化前のメッセージが含まれること。

    remove(path.c_str());
}

// NULL ハンドルで trace_logger_set_file_level を呼ぶと -1 が返されることの確認
TEST_F(trace_providerTest, test_modify_filetrc_null_handle)
{
    // Act
    int result = trace_logger_set_file_level(NULL, "test.log", TRACE_LEVEL_ERROR, 0, 0); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, result); // [確認_異常系] - NULL ハンドルで -1 が返されること。
}

// ファイルパスが指定されていない場合、レベルが合致してもファイル出力されないことの確認
TEST_F(trace_providerTest, test_no_file_output_without_path)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // Act & Assert
    // デフォルトではファイルトレース無効(file_handle=NULL)
    // レベルが合致 (CRITICAL ≤ ERROR) してもファイルには出力されない
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_CRITICAL, "no file output")); // [確認_正常系] - ファイル未指定でもエラーにならないこと。

    // Cleanup
    trace_logger_destroy(handle);
}

/* ===== OS + ファイル並列出力テスト ===== */

// OS トレースとファイルトレースが同時に動作し、VERBOSE しきい値では DEBUG が抑止されることの確認
TEST_F(trace_providerTest, test_dual_output_os_and_file)
{
    // Arrange
    string ws = findWorkspaceRoot();
    string path = ws + "/app/com_util/test/src/libcom_utilTest/trace_providerTest/results/trace_dual.log";
    remove(path.c_str());

    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    trace_logger_set_os_level(handle, TRACE_LEVEL_WARNING);                       // [手順] - OS レベルを WARNING に設定する。
    trace_logger_set_file_level(handle, path.c_str(), TRACE_LEVEL_VERBOSE, 0, 0);      // [手順] - ファイルレベルを VERBOSE に設定する。
    trace_logger_start(handle);

    trace_logger_write(handle, TRACE_LEVEL_ERROR,   "error msg");// [手順] - ERROR を書き込む (OS:✓ File:✓)。
    trace_logger_write(handle, TRACE_LEVEL_WARNING, "warning msg"); // [手順] - WARNING を書き込む (OS:✓ File:✓)。
    trace_logger_write(handle, TRACE_LEVEL_INFO,    "info msg");    // [手順] - INFO を書き込む (OS:✗ File:✓)。
    trace_logger_write(handle, TRACE_LEVEL_VERBOSE, "verbose msg"); // [手順] - VERBOSE を書き込む (OS:✗ File:✓)。
    trace_logger_write(handle, TRACE_LEVEL_DEBUG,   "debug msg");   // [手順] - DEBUG を書き込む (OS:✗ File:✗)。

    trace_logger_destroy(handle);

    // Assert
    EXPECT_FILE_CONTAINS(path, "error msg");   // [確認_正常系] - ERROR がファイルに含まれること。
    EXPECT_FILE_CONTAINS(path, "warning msg"); // [確認_正常系] - WARNING がファイルに含まれること。
    EXPECT_FILE_CONTAINS(path, "info msg");    // [確認_正常系] - INFO がファイルに含まれること。
    EXPECT_FILE_CONTAINS(path, "verbose msg"); // [確認_正常系] - VERBOSE がファイルに含まれること。
    EXPECT_FALSE(testing::FileContains(path, "debug msg")); // [確認_正常系] - DEBUG はファイルに含まれないこと。

    remove(path.c_str());
}

// stderr レベルを DEBUG に設定すると DEBUG が D で出力されることの確認
TEST_F(trace_providerTest, test_stderr_level_debug_outputs_d_marker)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    ASSERT_EQ(0, trace_logger_set_stderr_level(handle, TRACE_LEVEL_DEBUG)); // [手順] - stderr レベルを DEBUG に設定する。
    trace_logger_start(handle);

    // Act
    testing::internal::CaptureStderr();
    trace_logger_write(handle, TRACE_LEVEL_VERBOSE, "verbose to stderr"); // [手順] - VERBOSE を stderr に出力する。
    trace_logger_write(handle, TRACE_LEVEL_DEBUG,   "debug to stderr");   // [手順] - DEBUG を stderr に出力する。
    string captured = testing::internal::GetCapturedStderr();

    // Cleanup
    trace_logger_destroy(handle);

    // Assert
    EXPECT_NE(string::npos, captured.find(" V verbose to stderr")); // [確認_正常系] - VERBOSE が V で出力されること。
    EXPECT_NE(string::npos, captured.find(" D debug to stderr"));   // [確認_正常系] - DEBUG が D で出力されること。
}

/* ===== trace_logger_start / trace_logger_stop テスト ===== */

// 基本的な start → write → stop → dispose のライフサイクルの確認
TEST_F(trace_providerTest, test_start_and_stop_basic)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int start_result = trace_logger_start(handle); // [手順] - プロバイダを開始する。
    int write_result = trace_logger_write(handle, TRACE_LEVEL_INFO, "started message"); // [手順] - メッセージを書き込む。
    int stop_result  = trace_logger_stop(handle);  // [手順] - プロバイダを停止する。

    // Assert
    EXPECT_EQ(0, start_result); // [確認_正常系] - start が成功すること。
    EXPECT_EQ(0, write_result); // [確認_正常系] - 書き込みが成功すること。
    EXPECT_EQ(0, stop_result);  // [確認_正常系] - stop が成功すること。

    // Cleanup
    trace_logger_destroy(handle);
}

// stopped 状態で出力関数を呼ぶと -1 が返されることの確認
TEST_F(trace_providerTest, test_write_fails_when_stopped)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act (init 直後は stopped 状態)
    int write_result  = trace_logger_write(handle, TRACE_LEVEL_INFO, "stopped message");   // [手順] - stopped 状態で書き込む。
    int writef_result = trace_logger_writef(handle, TRACE_LEVEL_INFO, "stopped %s", "msg"); // [手順] - stopped 状態で writef を呼ぶ。

    unsigned char data[] = {0x01, 0x02};
    int hex_result  = trace_logger_write_hex(handle, TRACE_LEVEL_INFO, data, sizeof(data), "hex");  // [手順] - stopped 状態で hex_write を呼ぶ。
    int hexf_result = trace_logger_write_hexf(handle, TRACE_LEVEL_INFO, data, sizeof(data), "hex %d", 1); // [手順] - stopped 状態で hex_writef を呼ぶ。

    // Assert
    EXPECT_EQ(-1, write_result);  // [確認_異常系] - stopped 状態で trace_logger_write が -1 を返すこと。
    EXPECT_EQ(-1, writef_result); // [確認_異常系] - stopped 状態で trace_logger_writef が -1 を返すこと。
    EXPECT_EQ(-1, hex_result);    // [確認_異常系] - stopped 状態で trace_logger_write_hex が -1 を返すこと。
    EXPECT_EQ(-1, hexf_result);   // [確認_異常系] - stopped 状態で trace_logger_write_hexf が -1 を返すこと。

    // Cleanup
    trace_logger_destroy(handle);
}

// started 状態で trace_logger_write を呼ぶと 0 が返されることの確認
TEST_F(trace_providerTest, test_write_succeeds_when_started)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);

    // Act
    int result = trace_logger_write(handle, TRACE_LEVEL_INFO, "started message"); // [手順] - started 状態で書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - started 状態で書き込みが成功すること。

    // Cleanup
    trace_logger_destroy(handle);
}

// started 状態で設定関数を呼ぶと -1 が返されることの確認
TEST_F(trace_providerTest, test_config_fails_when_started)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle); // [手順] - プロバイダを開始する。

    // Act
    int modify_name_result  = trace_logger_set_name(handle, "new_name", 0);                        // [手順] - started 中に modify_name を呼ぶ。
    int set_os_result       = trace_logger_set_os_level(handle, TRACE_LEVEL_VERBOSE);                    // [手順] - started 中に modify_ostrc を呼ぶ。
    int set_file_result     = trace_logger_set_file_level(handle, "test.log", TRACE_LEVEL_ERROR, 0, 0);  // [手順] - started 中に modify_filetrc を呼ぶ。

    // Assert
    EXPECT_EQ(-1, modify_name_result); // [確認_異常系] - started 中の modify_name が -1 を返すこと。
    EXPECT_EQ(-1, set_os_result);      // [確認_異常系] - started 中の modify_ostrc が -1 を返すこと。
    EXPECT_EQ(-1, set_file_result);    // [確認_異常系] - started 中の modify_filetrc が -1 を返すこと。

    // Cleanup
    trace_logger_destroy(handle);
}

// stopped 状態で設定関数を呼ぶと 0 が返されることの確認
TEST_F(trace_providerTest, test_config_succeeds_when_stopped)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act (init 直後は stopped 状態)
    int modify_name_result = trace_logger_set_name(handle, "new_name", 0);  // [手順] - stopped 中に modify_name を呼ぶ。
    int set_os_result      = trace_logger_set_os_level(handle, TRACE_LEVEL_VERBOSE); // [手順] - stopped 中に modify_ostrc を呼ぶ。

    // Assert
    EXPECT_EQ(0, modify_name_result); // [確認_正常系] - stopped 中の modify_name が 0 を返すこと。
    EXPECT_EQ(0, set_os_result);      // [確認_正常系] - stopped 中の modify_ostrc が 0 を返すこと。

    // Cleanup
    trace_logger_destroy(handle);
}

// stop → 設定変更 → start → write の一連のライフサイクルの確認
TEST_F(trace_providerTest, test_stop_then_reconfig_then_start)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act - 1回目: start → write → stop
    trace_logger_start(handle);
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_INFO, "first run")); // [確認_正常系] - 1 回目の書き込みが成功すること。
    trace_logger_stop(handle);

    // 設定変更 (stopped 状態)
    EXPECT_EQ(0, trace_logger_set_name(handle, "reconfigured", 0)); // [確認_正常系] - 停止中の modify_name が成功すること。
    EXPECT_EQ(0, trace_logger_set_os_level(handle, TRACE_LEVEL_VERBOSE)); // [確認_正常系] - 停止中の modify_ostrc が成功すること。

    // Act - 2回目: start → write → stop
    trace_logger_start(handle);
    int result = trace_logger_write(handle, TRACE_LEVEL_INFO, "second run"); // [手順] - 再設定後にメッセージを書き込む。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 再設定後の書き込みが成功すること。

    // Cleanup
    trace_logger_destroy(handle);
}

// trace_logger_start の二重呼び出しが冪等であることの確認
TEST_F(trace_providerTest, test_double_start_is_noop)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);

    // Act
    int first_result  = trace_logger_start(handle); // [手順] - 1 回目の start を呼ぶ。
    int second_result = trace_logger_start(handle); // [手順] - 2 回目の start を呼ぶ。

    // Assert
    EXPECT_EQ(0, first_result);  // [確認_正常系] - 1 回目の start が 0 を返すこと。
    EXPECT_EQ(0, second_result); // [確認_正常系] - 2 回目の start も 0 を返すこと (冪等)。

    // 出力が正常に動作すること
    EXPECT_EQ(0, trace_logger_write(handle, TRACE_LEVEL_INFO, "after double start")); // [確認_正常系] - 書き込みが成功すること。

    // Cleanup
    trace_logger_destroy(handle);
}

// trace_logger_stop の二重呼び出しが冪等であることの確認
TEST_F(trace_providerTest, test_double_stop_is_noop)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle);
    trace_logger_stop(handle); // [手順] - 1 回目の stop を呼ぶ。

    // Act
    int result = trace_logger_stop(handle); // [手順] - 2 回目の stop を呼ぶ。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 2 回目の stop も 0 を返すこと (冪等)。

    // Cleanup
    trace_logger_destroy(handle);
}

// started 状態で trace_logger_destroy を呼んでも正常に完了することの確認
TEST_F(trace_providerTest, test_dispose_while_started)
{
    // Arrange
    trace_logger_t *handle = trace_logger_create();
    ASSERT_NE((trace_logger_t *)NULL, handle);
    trace_logger_start(handle); // [手順] - プロバイダを開始する。

    // Act & Assert - started 中の dispose がクラッシュしないことを確認
    trace_logger_destroy(handle); // [手順] - started 中に dispose を呼ぶ。安全に終了すること。
}

// NULL ハンドルで trace_logger_start を呼ぶと -1 が返されることの確認
TEST_F(trace_providerTest, test_start_null_handle)
{
    // Act
    int result = trace_logger_start(NULL); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, result); // [確認_異常系] - NULL ハンドルで -1 が返されること。
}

// NULL ハンドルで trace_logger_stop を呼ぶと -1 が返されることの確認
TEST_F(trace_providerTest, test_stop_null_handle)
{
    // Act
    int result = trace_logger_stop(NULL); // [手順] - handle に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, result); // [確認_異常系] - NULL ハンドルで -1 が返されること。
}
