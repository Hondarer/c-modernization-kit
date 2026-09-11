#include <testfw.h>

#include "string_catalog_definition.h"

#include <string_catalog.h>
#include <stddef.h>
#include <string.h>

class stringCatalogDefinitionTest : public Test
{
  protected:
    void SetUp() override
    {
        string_catalog_set_language(STRING_CATALOG_LANGUAGE_NEUTRAL);
    }
};

// カタログを取得できることの確認
TEST_F(stringCatalogDefinitionTest, entries)
{
    // Arrange
    const string_catalog_entry *actual_entries;
    int actual_count;

    // Pre-Assert

    // Act
    actual_entries = string_catalog_definition_entries();   // [手順] - カタログの先頭を取得する。
    actual_count = string_catalog_definition_entry_count(); // [手順] - カタログの件数を取得する。

    // Assert
    EXPECT_NE(nullptr, actual_entries); // [確認_正常系] - カタログの先頭を取得できること。
    EXPECT_GT(actual_count, 0);         // [確認_正常系] - 件数が 1 件以上であること。
}

// 注入したカタログが引数スキーマと整合していることの確認
TEST_F(stringCatalogDefinitionTest, catalog_is_consistent)
{
    // Arrange
    int string_id = 0;
    string_catalog_language language = STRING_CATALOG_LANGUAGE_NEUTRAL;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = string_catalog_verify(string_catalog_definition_catalog(), &string_id,
                                       &language); // [手順] - 注入したカタログ全体を確認する。

    // Assert
    EXPECT_EQ(STRING_CATALOG_OK, actual_ret); // [確認_正常系] - すべての書式が引数スキーマと整合していること。
}

// 各文字列が固定文字列とニュートラル言語のリソースを持つことの確認
TEST_F(stringCatalogDefinitionTest, every_entry_has_neutral_resource)
{
    // Arrange
    const string_catalog_entry *entries = string_catalog_definition_entries();
    const int count = string_catalog_definition_entry_count();
    int index;

    // Pre-Assert

    // Act / Assert
    for (index = 0; index < count; index++) // [手順] - すべてのカタログを走査する。
    {
        EXPECT_NE(nullptr, entries[index].id_text); // [確認_正常系] - 文字列 ID の固定文字列を持つこと。
        EXPECT_NE(nullptr,
                  entries[index]
                      .texts[STRING_CATALOG_LANGUAGE_NEUTRAL]); // [確認_正常系] - ニュートラル言語の書式を持つこと。
        EXPECT_NE(nullptr,
                  entries[index]
                      .notes[STRING_CATALOG_LANGUAGE_NEUTRAL]); // [確認_正常系] - ニュートラル言語の備考を持つこと。
        EXPECT_LE(entries[index].category,
                  STRING_CATALOG_TRACE_LEVEL_NONE);  // [確認_正常系] - 分類値がトレース レベルの範囲内であること。
        EXPECT_GE(entries[index].argument_count, 0); // [確認_正常系] - 引数個数が 0 以上であること。
        EXPECT_LE(entries[index].argument_count,
                  STRING_CATALOG_ARGUMENT_MAX); // [確認_正常系] - 引数個数が上限以下であること。
    }
}

// 文字列 ID から定義の内容を参照できることの確認
TEST_F(stringCatalogDefinitionTest, file_open_failed_entry)
{
    // Arrange
    const char *actual_id_text;
    int actual_category;
    char dest[STRING_CATALOG_TEXT_MAX];
    int actual_ret;

    memset(dest, 0, sizeof(dest));

    // Pre-Assert

    // Act
    actual_id_text = string_catalog_id_text(string_catalog_definition_catalog(),
                                            STRING_CATALOG_ID_FILE_OPEN_FAILED); // [手順] - 固定文字列を取得する。
    actual_category = string_catalog_category(string_catalog_definition_catalog(),
                                              STRING_CATALOG_ID_FILE_OPEN_FAILED); // [手順] - 分類値を取得する。
    actual_ret = string_catalog_format(string_catalog_definition_catalog(), dest, sizeof(dest),
                                       STRING_CATALOG_ID_FILE_OPEN_FAILED, "config.json",
                                       2); // [手順] - ニュートラル言語で文字列を組み立てる。

    // Assert
    ASSERT_NE(nullptr, actual_id_text);                           // [確認_正常系] - 固定文字列を取得できること。
    EXPECT_STREQ("STRING_CATALOG_ID_0002", actual_id_text);       // [確認_正常系] - 固定文字列が一致すること。
    EXPECT_EQ(STRING_CATALOG_TRACE_LEVEL_ERROR, actual_category); // [確認_正常系] - 分類値が一致すること。
    EXPECT_EQ(STRING_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が STRING_CATALOG_OK であること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// 文字列 ID ごとの型付きラッパーが、カタログを指定した呼び出しと同じ結果を出すことの確認
TEST_F(stringCatalogDefinitionTest, typed_wrappers_match_generic_call)
{
    // Arrange
    char actual_dest[STRING_CATALOG_TEXT_MAX];
    char expected_dest[STRING_CATALOG_TEXT_MAX];
    int actual_ret_no_argument;
    int actual_ret_two_arguments;
    int expected_ret_two_arguments;

    memset(actual_dest, 0, sizeof(actual_dest));
    memset(expected_dest, 0, sizeof(expected_dest));

    // Pre-Assert

    // Act
    actual_ret_no_argument = string_catalog_definition_format_id_startup_completed(
        actual_dest, sizeof(actual_dest)); // [手順] - 引数を取らないラッパーで組み立てる。

    // Assert
    EXPECT_EQ(STRING_CATALOG_OK, actual_ret_no_argument); // [確認_正常系] - 戻り値が STRING_CATALOG_OK であること。
    EXPECT_STREQ("Startup completed. The default setting is { default }.",
                 actual_dest); // [確認_正常系] - 引数なしの書式で組み立てられること。

    // Act
    actual_ret_two_arguments = string_catalog_definition_format_id_file_open_failed(
        actual_dest, sizeof(actual_dest), "config.json", 2); // [手順] - 型付きラッパーで組み立てる。
    expected_ret_two_arguments =
        string_catalog_format(string_catalog_definition_catalog(), expected_dest, sizeof(expected_dest),
                              STRING_CATALOG_ID_FILE_OPEN_FAILED, "config.json",
                              2); // [手順] - カタログを指定した呼び出しで同じ文字列を組み立てる。

    // Assert
    EXPECT_EQ(expected_ret_two_arguments,
              actual_ret_two_arguments);      // [確認_正常系] - 戻り値が一致すること。
    EXPECT_STREQ(expected_dest, actual_dest); // [確認_正常系] - 組み立てた文字列が一致すること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 actual_dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// 型付きラッパーでも、引数の順序が言語に依らないことの確認
TEST_F(stringCatalogDefinitionTest, typed_wrapper_argument_order_is_language_independent)
{
    // Arrange
    char neutral_dest[STRING_CATALOG_TEXT_MAX];
    char japanese_dest[STRING_CATALOG_TEXT_MAX];
    int actual_ret_neutral;
    int actual_ret_japanese;

    memset(neutral_dest, 0, sizeof(neutral_dest));
    memset(japanese_dest, 0, sizeof(japanese_dest));

    // Pre-Assert

    // Act
    actual_ret_neutral = string_catalog_definition_format_id_record_mismatch(
        neutral_dest, sizeof(neutral_dest), 12U,
        0x1234ABCDU); // [手順] - ニュートラル言語で、レコード番号とシグネチャーの順に渡す。

    string_catalog_set_language(STRING_CATALOG_LANGUAGE_JAPANESE);
    actual_ret_japanese =
        string_catalog_definition_format_id_record_mismatch(japanese_dest, sizeof(japanese_dest), 12U,
                                                            0x1234ABCDU); // [手順] - 日本語で、同じ順序の引数を渡す。

    // Assert
    EXPECT_EQ(STRING_CATALOG_OK, actual_ret_neutral);  // [確認_正常系] - 戻り値が STRING_CATALOG_OK であること。
    EXPECT_EQ(STRING_CATALOG_OK, actual_ret_japanese); // [確認_正常系] - 戻り値が STRING_CATALOG_OK であること。
    EXPECT_STREQ("Record 12 has an unexpected signature 0x1234abcd.",
                 neutral_dest); // [確認_正常系] - ニュートラル言語の語順で組み立てられること。
    EXPECT_STREQ("シグネチャー 0x1234abcd は、レコード 12 の想定と一致しません。",
                 japanese_dest); // [確認_正常系] - 引数順序を変えずに、日本語の語順で組み立てられること。
}
