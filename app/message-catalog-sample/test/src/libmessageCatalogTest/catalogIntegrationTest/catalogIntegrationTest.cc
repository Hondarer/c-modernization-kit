#include <testfw.h>

#include "message_catalog_definition.h"

#include <message_catalog.h>
#include <stdint.h>
#include <string.h>

class catalogIntegrationTest : public Test
{
  protected:
    /** 組み立てたメッセージの格納先です。 */
    char dest[MESSAGE_CATALOG_TEXT_MAX];

    void SetUp() override
    {
        memset(dest, 0, sizeof(dest));
        message_catalog_set_catalog(message_catalog_definition_entries(), message_catalog_definition_entry_count(),
                                    message_catalog_definition_id_index(), message_catalog_definition_id_index_count());
        message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_JAPANESE);
    }
};

// 注入したカタログが引数スキーマと整合していることの確認
TEST_F(catalogIntegrationTest, injected_catalog_is_consistent)
{
    // Arrange
    int message_id = MESSAGE_CATALOG_ID_STARTUP_COMPLETED;
    message_catalog_language language = MESSAGE_CATALOG_LANGUAGE_NEUTRAL;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = message_catalog_verify(&message_id, &language); // [手順] - 注入したカタログ全体を確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - すべての書式が引数スキーマと整合していること。
}

// メッセージ ID の固定文字列、レベル、備考を参照できることの確認
TEST_F(catalogIntegrationTest, metadata)
{
    // Arrange
    const char *actual_id_text;
    const char *actual_note;
    const char *actual_unknown_id_text;
    message_catalog_trace_level actual_level;

    // Pre-Assert

    // Act
    actual_id_text =
        message_catalog_id_text(MESSAGE_CATALOG_ID_FILE_OPEN_FAILED); // [手順] - メッセージ ID の固定文字列を取得する。
    actual_note = message_catalog_note(MESSAGE_CATALOG_ID_FILE_OPEN_FAILED);   // [手順] - 備考を取得する。
    actual_level = message_catalog_level(MESSAGE_CATALOG_ID_FILE_OPEN_FAILED); // [手順] - レベルを取得する。
    actual_unknown_id_text = message_catalog_id_text(0); // [手順] - 未登録のメッセージ ID で固定文字列を取得する。

    // Assert
    ASSERT_NE(nullptr, actual_id_text);                         // [確認_正常系] - 固定文字列を取得できること。
    ASSERT_NE(nullptr, actual_note);                            // [確認_正常系] - 備考を取得できること。
    EXPECT_STREQ("MSG_ID_0002", actual_id_text);                // [確認_正常系] - 固定文字列が一致すること。
    EXPECT_EQ(MESSAGE_CATALOG_TRACE_LEVEL_ERROR, actual_level); // [確認_正常系] - カタログのレベルが一致すること。
    EXPECT_LT(0U, strlen(actual_note));                         // [確認_正常系] - 備考が空でないこと。
    EXPECT_EQ(nullptr, actual_unknown_id_text); // [確認_異常系] - 未登録のメッセージ ID では NULL を返すこと。
}

// 波括弧のエスケープを含むメッセージを組み立てられることの確認
TEST_F(catalogIntegrationTest, escaped_braces)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(
        dest, sizeof(dest),
        MESSAGE_CATALOG_ID_STARTUP_COMPLETED); // [手順] - 引数を取らないメッセージを日本語で組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("起動が完了しました。既定の設定は { default } です。",
                 dest); // [確認_正常系] - {{ と }} が 1 文字の波括弧になること。
}

// 言語を設定していない状態でニュートラル言語の書式を使用することの確認
TEST_F(catalogIntegrationTest, neutral_language)
{
    // Arrange
    int actual_ret;
    message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_NEUTRAL);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(dest, sizeof(dest), MESSAGE_CATALOG_ID_FILE_OPEN_FAILED, "config.json",
                                        2); // [手順] - ニュートラル言語でメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// 引数の型と表現がメッセージ ID 側で決まることの確認
TEST_F(catalogIntegrationTest, argument_text_is_language_independent)
{
    // Arrange
    char english_dest[MESSAGE_CATALOG_TEXT_MAX];
    int actual_ret_japanese;
    int actual_ret_english;

    memset(english_dest, 0, sizeof(english_dest));

    // Pre-Assert

    // Act
    actual_ret_japanese = message_catalog_format(dest, sizeof(dest), MESSAGE_CATALOG_ID_FILE_OPEN_FAILED, "config.json",
                                                 2); // [手順] - ファイル オープン失敗のメッセージを日本語で組み立てる。

    message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_ENGLISH);
    actual_ret_english =
        message_catalog_format(english_dest, sizeof(english_dest), MESSAGE_CATALOG_ID_FILE_OPEN_FAILED, "config.json",
                               2); // [手順] - 言語を英語へ変更し、同じ引数で組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK,
              actual_ret_japanese); // [確認_正常系] - 日本語の戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret_english); // [確認_正常系] - 英語の戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)",
                 dest); // [確認_正常系] - 日本語のメッセージが一致すること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 english_dest); // [確認_正常系] - エラー コードの表現が言語に依らないこと。
}

// 言語別リソースが語順だけを決めることの確認
TEST_F(catalogIntegrationTest, language_changes_order_only)
{
    // Arrange
    char english_dest[MESSAGE_CATALOG_TEXT_MAX];
    int actual_ret_japanese;
    int actual_ret_english;

    memset(english_dest, 0, sizeof(english_dest));

    // Pre-Assert

    // Act
    actual_ret_japanese =
        message_catalog_format(dest, sizeof(dest), MESSAGE_CATALOG_ID_RECORD_MISMATCH, UINT32_C(42),
                               UINT32_C(0x1234ABCD)); // [手順] - レコード不一致のメッセージを日本語で組み立てる。

    message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_ENGLISH);
    actual_ret_english =
        message_catalog_format(english_dest, sizeof(english_dest), MESSAGE_CATALOG_ID_RECORD_MISMATCH, UINT32_C(42),
                               UINT32_C(0x1234ABCD)); // [手順] - 言語を英語へ変更し、同じ引数で組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK,
              actual_ret_japanese); // [確認_正常系] - 日本語の戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret_english); // [確認_正常系] - 英語の戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("シグネチャー 0x1234abcd は、レコード 42 の想定と一致しません。",
                 dest); // [確認_正常系] - 日本語では位置指定を入れ替えた語順になること。
    EXPECT_STREQ("Record 42 has an unexpected signature 0x1234abcd.",
                 english_dest); // [確認_正常系] - 呼び出し側の引数順を変えずに語順だけが変わること。
}

// 同じ引数を複数回参照できることの確認
TEST_F(catalogIntegrationTest, repeated_placeholder)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret =
        message_catalog_format(dest, sizeof(dest), MESSAGE_CATALOG_ID_THROUGHPUT_REPORT, 12.5,
                               UINT64_C(4000000000)); // [手順] - 同じ位置指定を 2 回含むメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("処理速度は 12.5 件/秒です。累計 4000000000 件を 12.5 件/秒で処理しました。",
                 dest); // [確認_正常系] - 同じ値が 2 回展開されること。
}
