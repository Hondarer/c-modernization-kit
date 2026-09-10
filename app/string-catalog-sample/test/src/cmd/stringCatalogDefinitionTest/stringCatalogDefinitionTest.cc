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
        string_catalog_set_catalog(string_catalog_definition_entries(), string_catalog_definition_entry_count(),
                                    string_catalog_definition_id_index(), string_catalog_definition_id_index_count());
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
    actual_ret = string_catalog_verify(&string_id, &language); // [手順] - 注入したカタログ全体を確認する。

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
                  STRING_CATALOG_TRACE_LEVEL_NONE); // [確認_正常系] - 分類値がトレース レベルの範囲内であること。
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
    actual_id_text = string_catalog_id_text(STRING_CATALOG_ID_FILE_OPEN_FAILED);   // [手順] - 固定文字列を取得する。
    actual_category = string_catalog_category(STRING_CATALOG_ID_FILE_OPEN_FAILED); // [手順] - 分類値を取得する。
    actual_ret = string_catalog_format(dest, sizeof(dest), STRING_CATALOG_ID_FILE_OPEN_FAILED, "config.json",
                                        2); // [手順] - ニュートラル言語で文字列を組み立てる。

    // Assert
    ASSERT_NE(nullptr, actual_id_text);                            // [確認_正常系] - 固定文字列を取得できること。
    EXPECT_STREQ("STRING_CATALOG_ID_0002", actual_id_text);                   // [確認_正常系] - 固定文字列が一致すること。
    EXPECT_EQ(STRING_CATALOG_TRACE_LEVEL_ERROR, actual_category); // [確認_正常系] - 分類値が一致すること。
    EXPECT_EQ(STRING_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が STRING_CATALOG_OK であること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}
