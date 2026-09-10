#include <testfw.h>

#include <message_catalog.h>

class messageCatalogLanguageTest : public Test
{
};

// 設定していないプロセスの既定がニュートラル言語であることの確認
// 言語設定はプロセス グローバルな状態のため、既定値を確認するテストを最初に置く
TEST_F(messageCatalogLanguageTest, default_is_neutral)
{
    // Arrange
    message_catalog_language actual_language;

    // Pre-Assert

    // Act
    actual_language = message_catalog_get_language(); // [手順] - 設定を行わずに現在の言語を取得する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_NEUTRAL, actual_language); // [確認_正常系] - 既定がニュートラル言語であること。
}

// 設定した言語を取得できることの確認
TEST_F(messageCatalogLanguageTest, set_and_get)
{
    // Arrange
    int actual_ret_japanese;
    int actual_ret_english;
    message_catalog_language actual_language_japanese;
    message_catalog_language actual_language_english;

    // Pre-Assert

    // Act
    actual_ret_japanese =
        message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_JAPANESE); // [手順] - 日本語を設定する。
    actual_language_japanese = message_catalog_get_language();           // [手順] - 現在の言語を取得する。
    actual_ret_english = message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_ENGLISH); // [手順] - 英語を設定する。
    actual_language_english = message_catalog_get_language(); // [手順] - 現在の言語を取得する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret_japanese); // [確認_正常系] - 日本語の設定が成功すること。
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_JAPANESE,
              actual_language_japanese);               // [確認_正常系] - 設定した日本語を取得できること。
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret_english); // [確認_正常系] - 英語の設定が成功すること。
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_ENGLISH,
              actual_language_english); // [確認_正常系] - 設定した英語を取得できること。
}

// 範囲外の言語を設定できないことの確認
TEST_F(messageCatalogLanguageTest, invalid_language)
{
    // Arrange
    int actual_ret;
    message_catalog_language actual_language;

    message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_JAPANESE);

    // Pre-Assert

    // Act
    actual_ret = message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_COUNT); // [手順] - 言語ではない値を設定する。
    actual_language = message_catalog_get_language();                          // [手順] - 現在の言語を取得する。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(MESSAGE_CATALOG_LANGUAGE_JAPANESE, actual_language); // [確認_異常系] - 設定が変更されないこと。
}
