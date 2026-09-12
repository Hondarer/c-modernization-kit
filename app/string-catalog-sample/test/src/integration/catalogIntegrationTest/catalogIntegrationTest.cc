#include <testfw.h>

#include "sample_messages.h"
#include "sample_metrics.h"

#include <cplat/string_catalog/string_catalog.h>
#include <cplat/trace/tracer.h>
#include <stdint.h>
#include <string.h>

class catalogIntegrationTest : public Test
{
  protected:
    /** 組み立てた文字列の格納先です。 */
    char dest[CPLAT_STRING_CATALOG_TEXT_MAX];

    void SetUp() override
    {
        memset(dest, 0, sizeof(dest));
        cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE);
    }
};

// 注入したカタログが引数スキーマと整合していることの確認
TEST_F(catalogIntegrationTest, injected_catalog_is_consistent)
{
    // Arrange
    int string_id = SAMPLE_MESSAGES_ID_STARTUP_COMPLETED;
    cplat_string_catalog_language language = CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = cplat_string_catalog_verify(sample_messages_catalog(), &string_id,
                                             &language); // [手順] - 注入したカタログ全体を確認する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - すべての書式が引数スキーマと整合していること。
}

// 文字列 ID の固定文字列、レベル、備考を参照できることの確認
TEST_F(catalogIntegrationTest, metadata)
{
    // Arrange
    const char *actual_id_text;
    const char *actual_note;
    const char *actual_unknown_id_text;
    int actual_category;

    // Pre-Assert

    // Act
    actual_id_text = cplat_string_catalog_get_id_text(
        sample_messages_catalog(),
        SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED); // [手順] - 文字列 ID の固定文字列を取得する。
    actual_note = cplat_string_catalog_get_note(sample_messages_catalog(),
                                                SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED); // [手順] - 備考を取得する。
    actual_category =
        cplat_string_catalog_get_category(sample_messages_catalog(),
                                          SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED); // [手順] - 分類値を取得する。
    actual_unknown_id_text = cplat_string_catalog_get_id_text(sample_messages_catalog(),
                                                              0); // [手順] - 未登録の文字列 ID で固定文字列を取得する。

    // Assert
    ASSERT_NE(nullptr, actual_id_text);                      // [確認_正常系] - 固定文字列を取得できること。
    ASSERT_NE(nullptr, actual_note);                         // [確認_正常系] - 備考を取得できること。
    EXPECT_STREQ("SAMPLE_MESSAGES_ID_0002", actual_id_text); // [確認_正常系] - 固定文字列が一致すること。
    EXPECT_EQ(CPLAT_TRACE_LEVEL_ERROR, actual_category);     // [確認_正常系] - カタログの分類値が一致すること。
    EXPECT_LT(0U, strlen(actual_note));                      // [確認_正常系] - 備考が空でないこと。
    EXPECT_EQ(nullptr, actual_unknown_id_text);              // [確認_異常系] - 未登録の文字列 ID では NULL を返すこと。
}

// 波括弧のエスケープを含む文字列を組み立てられることの確認
TEST_F(catalogIntegrationTest, escaped_braces)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = cplat_string_catalog_format(
        sample_messages_catalog(), dest, sizeof(dest),
        SAMPLE_MESSAGES_ID_STARTUP_COMPLETED); // [手順] - 引数を取らない文字列を日本語で組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_STREQ("起動が完了しました。既定の設定は { default } です。",
                 dest); // [確認_正常系] - {{ と }} が 1 文字の波括弧になること。
}

// 言語を設定していない状態でニュートラル言語の書式を使用することの確認
TEST_F(catalogIntegrationTest, neutral_language)
{
    // Arrange
    int actual_ret;
    cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL);

    // Pre-Assert

    // Act
    actual_ret = cplat_string_catalog_format(sample_messages_catalog(), dest, sizeof(dest),
                                             SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED, "config.json",
                                             2); // [手順] - ニュートラル言語で文字列を組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// 引数の型と表現が文字列 ID 側で決まることの確認
TEST_F(catalogIntegrationTest, argument_text_is_language_independent)
{
    // Arrange
    char english_dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    int actual_ret_japanese;
    int actual_ret_english;

    memset(english_dest, 0, sizeof(english_dest));

    // Pre-Assert

    // Act
    actual_ret_japanese = cplat_string_catalog_format(
        sample_messages_catalog(), dest, sizeof(dest), SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED, "config.json",
        2); // [手順] - ファイル オープン失敗の文字列を日本語で組み立てる。

    cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_ENGLISH);
    actual_ret_english = cplat_string_catalog_format(sample_messages_catalog(), english_dest, sizeof(english_dest),
                                                     SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED, "config.json",
                                                     2); // [手順] - 言語を英語へ変更し、同じ引数で組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret_japanese);          // [確認_正常系] - 日本語の戻り値が CPLAT_OK であること。
    EXPECT_EQ(CPLAT_OK, actual_ret_english); // [確認_正常系] - 英語の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)",
                 dest); // [確認_正常系] - 日本語の文字列が一致すること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 english_dest); // [確認_正常系] - エラー コードの表現が言語に依らないこと。
}

// 言語別リソースが語順だけを決めることの確認
TEST_F(catalogIntegrationTest, language_changes_order_only)
{
    // Arrange
    char english_dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    int actual_ret_japanese;
    int actual_ret_english;

    memset(english_dest, 0, sizeof(english_dest));

    // Pre-Assert

    // Act
    actual_ret_japanese = cplat_string_catalog_format(
        sample_messages_catalog(), dest, sizeof(dest), SAMPLE_MESSAGES_ID_RECORD_MISMATCH, UINT32_C(42),
        UINT32_C(0x1234ABCD)); // [手順] - レコード不一致の文字列を日本語で組み立てる。

    cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_ENGLISH);
    actual_ret_english = cplat_string_catalog_format(
        sample_messages_catalog(), english_dest, sizeof(english_dest), SAMPLE_MESSAGES_ID_RECORD_MISMATCH, UINT32_C(42),
        UINT32_C(0x1234ABCD)); // [手順] - 言語を英語へ変更し、同じ引数で組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret_japanese);          // [確認_正常系] - 日本語の戻り値が CPLAT_OK であること。
    EXPECT_EQ(CPLAT_OK, actual_ret_english); // [確認_正常系] - 英語の戻り値が CPLAT_OK であること。
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
    actual_ret = cplat_string_catalog_format(
        sample_metrics_catalog(), dest, sizeof(dest), SAMPLE_METRICS_ID_THROUGHPUT_REPORT, 12.5,
        UINT64_C(4000000000)); // [手順] - 同じ位置指定を 2 回含む文字列を組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_STREQ("処理速度は 12.5 件/秒です。累計 4000000000 件を 12.5 件/秒で処理しました。",
                 dest); // [確認_正常系] - 同じ値が 2 回展開されること。
}
