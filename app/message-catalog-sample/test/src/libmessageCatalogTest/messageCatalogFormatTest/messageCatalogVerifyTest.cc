#include <testfw.h>

#include "fake_catalog.h"

#include <message_catalog.h>

class messageCatalogVerifyTest : public Test
{
  protected:
    /** 不正を検出したメッセージ ID の受け取り先です。 */
    int message_id;

    /** 不正を検出した言語の受け取り先です。 */
    message_catalog_language language;

    void SetUp() override
    {
        message_id = FAKE_CATALOG_ID_UNKNOWN;
        language = MESSAGE_CATALOG_LANGUAGE_ENGLISH;
        fake_catalog_reset();
    }
};

// 整合したカタログが受理されることの確認
TEST_F(messageCatalogVerifyTest, valid_catalog)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = message_catalog_verify(&message_id, &language); // [手順] - 既定のカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
}

// 書式の構文が不正なカタログが拒否されることの確認
TEST_F(messageCatalogVerifyTest, invalid_format)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_text(FAKE_CATALOG_INDEX_ONE_ARGUMENT, MESSAGE_CATALOG_LANGUAGE_ENGLISH, "limit {2}");

    // Pre-Assert

    // Act
    actual_ret =
        message_catalog_verify(&message_id, &language); // [手順] - 引数個数を超える位置指定を持つカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
    EXPECT_EQ(FAKE_CATALOG_ID_ONE_ARGUMENT,
              message_id); // [確認_異常系] - 不正を検出したメッセージ ID を報告すること。
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_ENGLISH, language); // [確認_異常系] - 不正を検出した言語を報告すること。
}

// ニュートラル言語以外のリソースが欠けていても受理されることの確認
TEST_F(messageCatalogVerifyTest, missing_localized_text_is_allowed)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_text(FAKE_CATALOG_INDEX_TWO_ARGUMENTS, MESSAGE_CATALOG_LANGUAGE_JAPANESE, NULL);

    // Pre-Assert

    // Act
    actual_ret =
        message_catalog_verify(&message_id, &language); // [手順] - 日本語のリソースが欠けたカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK,
              actual_ret); // [確認_正常系] - ニュートラル言語へ読み替えられるため、不正としないこと。
}

// ニュートラル言語の書式が欠けたカタログが拒否されることの確認
TEST_F(messageCatalogVerifyTest, missing_neutral_text)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_text(FAKE_CATALOG_INDEX_TWO_ARGUMENTS, MESSAGE_CATALOG_LANGUAGE_NEUTRAL, NULL);

    // Pre-Assert

    // Act
    actual_ret =
        message_catalog_verify(&message_id, &language); // [手順] - ニュートラル言語の書式が欠けたカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
    EXPECT_EQ(FAKE_CATALOG_ID_TWO_ARGUMENTS,
              message_id); // [確認_異常系] - 不正を検出したメッセージ ID を報告すること。
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_NEUTRAL, language); // [確認_異常系] - 不正を検出した言語を報告すること。
}

// ニュートラル言語の備考が欠けたカタログが拒否されることの確認
TEST_F(messageCatalogVerifyTest, missing_neutral_note)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_note(FAKE_CATALOG_INDEX_TWO_ARGUMENTS, MESSAGE_CATALOG_LANGUAGE_NEUTRAL, NULL);

    // Pre-Assert

    // Act
    actual_ret =
        message_catalog_verify(&message_id, &language); // [手順] - ニュートラル言語の備考が欠けたカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
    EXPECT_EQ(FAKE_CATALOG_ID_TWO_ARGUMENTS,
              message_id); // [確認_異常系] - 不正を検出したメッセージ ID を報告すること。
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_NEUTRAL, language); // [確認_異常系] - 不正を検出した言語を報告すること。
}

// 引数個数が上限を超えたカタログが拒否されることの確認
TEST_F(messageCatalogVerifyTest, argument_count_over_max)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_argument_count(FAKE_CATALOG_INDEX_TWO_ARGUMENTS, MESSAGE_CATALOG_ARGUMENT_MAX + 1);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_verify(&message_id, &language); // [手順] - 引数個数が上限を超えるカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
    EXPECT_EQ(FAKE_CATALOG_ID_TWO_ARGUMENTS,
              message_id); // [確認_異常系] - 不正を検出したメッセージ ID を報告すること。
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_COUNT,
              language); // [確認_異常系] - 言語に依らない不正として、言語ではない値を報告すること。
}

// メッセージ ID が重複したカタログが拒否されることの確認
TEST_F(messageCatalogVerifyTest, duplicated_message_id)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_id(FAKE_CATALOG_INDEX_TWO_ARGUMENTS, FAKE_CATALOG_ID_NO_ARGUMENT);

    // Pre-Assert

    // Act
    actual_ret =
        message_catalog_verify(&message_id, &language); // [手順] - メッセージ ID が重複したカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 検索が自分自身へ到達しないため、定義エラーを返すこと。
    EXPECT_EQ(FAKE_CATALOG_ID_NO_ARGUMENT, message_id); // [確認_異常系] - 不正を検出したメッセージ ID を報告すること。
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_COUNT,
              language); // [確認_異常系] - 言語に依らない不正として、言語ではない値を報告すること。
}

// 引数個数が負のカタログが拒否されることの確認
TEST_F(messageCatalogVerifyTest, argument_count_negative)
{
    // Arrange
    int actual_ret;
    fake_catalog_set_argument_count(FAKE_CATALOG_INDEX_TWO_ARGUMENTS, -1);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_verify(&message_id, &language); // [手順] - 引数個数が負のカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
}

// 出力引数を省略しても結果コードを返すことの確認
TEST_F(messageCatalogVerifyTest, omitted_output_arguments)
{
    // Arrange
    int actual_ret_count;
    int actual_ret_text;

    // Pre-Assert

    // Act
    fake_catalog_set_argument_count(FAKE_CATALOG_INDEX_TWO_ARGUMENTS, MESSAGE_CATALOG_ARGUMENT_MAX + 1);
    actual_ret_count =
        message_catalog_verify(NULL, NULL); // [手順] - 出力引数を省略して、引数個数が不正なカタログを確認する。

    fake_catalog_reset();
    fake_catalog_set_text(FAKE_CATALOG_INDEX_TWO_ARGUMENTS, MESSAGE_CATALOG_LANGUAGE_NEUTRAL, NULL);
    actual_ret_text = message_catalog_verify(
        NULL, NULL); // [手順] - 出力引数を省略して、ニュートラル言語の書式が欠けたカタログを確認する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret_count); // [確認_異常系] - 引数個数の不正を報告すること。
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret_text); // [確認_異常系] - 書式の欠落を報告すること。
}
