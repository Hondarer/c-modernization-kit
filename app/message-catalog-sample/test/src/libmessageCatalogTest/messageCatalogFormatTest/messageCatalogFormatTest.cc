#include <testfw.h>

#include "fake_catalog.h"

#include <message_catalog.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/**
 *  可変長引数を組み立てて、message_catalog_vformat へ中継します。
 *
 *  `va_start` の直前の名前付き引数を持たせるため、メンバー関数ではなく通常の関数とします。
 */
static int call_vformat(char *dest, size_t dest_size, int message_id, ...)
{
    va_list args;
    int ret;

    va_start(args, message_id);
    ret = message_catalog_vformat(dest, dest_size, message_id, args);
    va_end(args);

    return ret;
}

class messageCatalogFormatTest : public Test
{
  protected:
    /** 組み立てたメッセージの格納先です。 */
    char dest[64];

    void SetUp() override
    {
        memset(dest, 0, sizeof(dest));
        fake_catalog_reset();
        message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_JAPANESE);
    }
};

// 引数を取らないメッセージを組み立てられることの確認
TEST_F(messageCatalogFormatTest, no_argument)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(dest, sizeof(dest),
                                        FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 引数を取らないメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("開始しました。", dest);      // [確認_正常系] - 現在の言語の書式がそのまま組み立てられること。
}

// 言語設定の変更が語順だけを変えることの確認
TEST_F(messageCatalogFormatTest, language_changes_order_only)
{
    // Arrange
    int actual_ret_japanese;
    int actual_ret_english;
    char english_dest[64];

    memset(english_dest, 0, sizeof(english_dest));

    // Pre-Assert

    // Act
    actual_ret_japanese = message_catalog_format(dest, sizeof(dest), FAKE_CATALOG_ID_TWO_ARGUMENTS, "config.json",
                                                 INT32_C(2)); // [手順] - 日本語のまま 2 引数のメッセージを組み立てる。

    message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_ENGLISH);
    actual_ret_english =
        message_catalog_format(english_dest, sizeof(english_dest), FAKE_CATALOG_ID_TWO_ARGUMENTS, "config.json",
                               INT32_C(2)); // [手順] - 言語を英語へ変更し、同じ引数で組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK,
              actual_ret_japanese); // [確認_正常系] - 日本語の戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret_english); // [確認_正常系] - 英語の戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("ファイル config.json 番号 2", dest); // [確認_正常系] - 日本語では書式の語順で並ぶこと。
    EXPECT_STREQ("number 2 of config.json",
                 english_dest); // [確認_正常系] - 英語では引数順を変えずに語順だけが変わること。
}

// 言語を設定していないプロセスがニュートラル言語を使用することの確認
TEST_F(messageCatalogFormatTest, neutral_language)
{
    // Arrange
    int actual_ret;
    message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_NEUTRAL);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(dest, sizeof(dest), FAKE_CATALOG_ID_TWO_ARGUMENTS, "config.json",
                                        INT32_C(2)); // [手順] - ニュートラル言語でメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret);       // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("file config.json number 2", dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// va_list を受け取る API でも同じ結果になることの確認
TEST_F(messageCatalogFormatTest, vformat_accepts_argument_list)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = call_vformat(dest, sizeof(dest), FAKE_CATALOG_ID_ONE_ARGUMENT,
                              (size_t)4096U); // [手順] - va_list を渡してメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("上限 4096", dest);           // [確認_正常系] - 可変長引数と同じ結果になること。
}

// 書き込み先が NULL の場合に引数不正となることの確認
TEST_F(messageCatalogFormatTest, null_dest)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(NULL, sizeof(dest),
                                        FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 書き込み先へ NULL を渡す。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_ARGUMENT であること。
}

// 書き込み先の容量が 0 の場合に引数不正となることの確認
TEST_F(messageCatalogFormatTest, zero_dest_size)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(dest, 0U,
                                        FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 書き込み先の容量へ 0 を渡す。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_ARGUMENT であること。
}

// カタログに無いメッセージ ID を指定した場合に未検出となることの確認
TEST_F(messageCatalogFormatTest, unknown_message_id)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret =
        message_catalog_format(dest, sizeof(dest),
                               FAKE_CATALOG_ID_UNKNOWN); // [手順] - カタログに登録していないメッセージ ID を渡す。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_NOT_FOUND,
              actual_ret);  // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_NOT_FOUND であること。
    EXPECT_STREQ("", dest); // [確認_異常系] - 書き込み先が NUL 終端されること。
}

// 引数個数が上限を超える定義が定義エラーになることの確認
TEST_F(messageCatalogFormatTest, argument_count_over_max)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_argument_count(FAKE_CATALOG_INDEX_NO_ARGUMENT, MESSAGE_CATALOG_ARGUMENT_MAX + 1);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(
        dest, sizeof(dest),
        FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 引数個数が上限を超える定義でメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
}

// 引数個数が負の定義が定義エラーになることの確認
TEST_F(messageCatalogFormatTest, argument_count_negative)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_argument_count(FAKE_CATALOG_INDEX_NO_ARGUMENT, -1);

    // Pre-Assert

    // Act
    actual_ret =
        message_catalog_format(dest, sizeof(dest),
                               FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 引数個数が負の定義でメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
}

// 現在の言語のリソースが無い場合にニュートラル言語へ読み替えることの確認
TEST_F(messageCatalogFormatTest, falls_back_to_neutral_text)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_text(FAKE_CATALOG_INDEX_NO_ARGUMENT, MESSAGE_CATALOG_LANGUAGE_JAPANESE, NULL);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(
        dest, sizeof(dest),
        FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 日本語のリソースが無いメッセージを日本語で組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("started", dest);             // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// ニュートラル言語のリソースも無い場合に定義エラーになることの確認
TEST_F(messageCatalogFormatTest, missing_neutral_text)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_text(FAKE_CATALOG_INDEX_NO_ARGUMENT, MESSAGE_CATALOG_LANGUAGE_JAPANESE, NULL);
    fake_catalog_set_text(FAKE_CATALOG_INDEX_NO_ARGUMENT, MESSAGE_CATALOG_LANGUAGE_NEUTRAL, NULL);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(
        dest, sizeof(dest),
        FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - どの言語にもリソースが無いメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
}

// 備考を現在の言語で参照し、無い場合はニュートラル言語へ読み替えることの確認
TEST_F(messageCatalogFormatTest, metadata)
{
    // Arrange
    const char *actual_id_text;
    const char *actual_note_japanese;
    const char *actual_note_fallback;
    const char *actual_unknown_note;
    const char *actual_unknown_id_text;
    int actual_category;
    int actual_unknown_category;

    // Pre-Assert

    // Act
    actual_id_text =
        message_catalog_id_text(FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - メッセージ ID の固定文字列を取得する。
    actual_note_japanese = message_catalog_note(FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 日本語の備考を取得する。

    message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_ENGLISH);
    actual_note_fallback =
        message_catalog_note(FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 英語の備考が無い状態で備考を取得する。
    actual_category = message_catalog_category(FAKE_CATALOG_ID_NO_ARGUMENT); // [手順] - 分類値を取得する。
    actual_unknown_category =
        message_catalog_category(FAKE_CATALOG_ID_UNKNOWN); // [手順] - 未登録のメッセージ ID で分類値を取得する。
    actual_unknown_id_text =
        message_catalog_id_text(FAKE_CATALOG_ID_UNKNOWN); // [手順] - 未登録のメッセージ ID で固定文字列を取得する。
    actual_unknown_note =
        message_catalog_note(FAKE_CATALOG_ID_UNKNOWN); // [手順] - 未登録のメッセージ ID で備考を取得する。

    // Assert
    ASSERT_NE(nullptr, actual_id_text);          // [確認_正常系] - 固定文字列を取得できること。
    EXPECT_STREQ("MSG_ID_0001", actual_id_text); // [確認_正常系] - 固定文字列が一致すること。
    EXPECT_EQ(3, actual_category);               // [確認_正常系] - カタログの分類値をそのまま返すこと。
    EXPECT_EQ(0, actual_unknown_category);       // [確認_異常系] - 未登録のメッセージ ID では 0 を返すこと。
    EXPECT_STREQ("引数を取らないメッセージです。",
                 actual_note_japanese); // [確認_正常系] - 現在の言語の備考を返すこと。
    EXPECT_STREQ("no argument",
                 actual_note_fallback); // [確認_正常系] - 備考が無い言語ではニュートラル言語の備考を返すこと。
    EXPECT_EQ(nullptr,
              actual_unknown_id_text);       // [確認_異常系] - 未登録のメッセージ ID では固定文字列が NULL であること。
    EXPECT_EQ(nullptr, actual_unknown_note); // [確認_異常系] - 未登録のメッセージ ID では NULL を返すこと。
}

// 列挙に無い引数種別を持つ定義が定義エラーになることの確認
TEST_F(messageCatalogFormatTest, unknown_argument_kind)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_argument_kind(FAKE_CATALOG_INDEX_ONE_ARGUMENT, 0, (message_catalog_argument_kind)15);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(dest, sizeof(dest), FAKE_CATALOG_ID_ONE_ARGUMENT,
                                        (size_t)1U); // [手順] - 列挙に無い引数種別を持つ定義でメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
}

// 書き込み先に収まらない場合に切り詰めを報告することの確認
TEST_F(messageCatalogFormatTest, truncated_output)
{
    // Arrange
    char small_dest[8];
    int actual_ret;

    memset(small_dest, 0, sizeof(small_dest));
    message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_ENGLISH);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_format(small_dest, sizeof(small_dest), FAKE_CATALOG_ID_ONE_ARGUMENT,
                                        (size_t)4096U); // [手順] - 結果が収まらない容量でメッセージを組み立てる。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_TRUNCATED,
              actual_ret);               // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_TRUNCATED であること。
    EXPECT_STREQ("limit 4", small_dest); // [確認_異常系] - 容量まで書き込んで NUL 終端すること。
}
