#include <testfw.h>

#include "sample_messages.h"
#include "sample_metrics.h"

#include <cplat/string_catalog/string_catalog.h>
#include <cplat/trace/tracer.h>
#include <stddef.h>
#include <string.h>

class sampleMessagesTest : public Test
{
  protected:
    void SetUp() override
    {
        cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL);
    }
};

// カタログを取得できることの確認
TEST_F(sampleMessagesTest, entries)
{
    // Arrange
    const cplat_string_catalog_entry *actual_entries;
    int actual_count;

    // Pre-Assert

    // Act
    actual_entries = sample_messages_entries();   // [手順] - カタログの先頭を取得する。
    actual_count = sample_messages_entry_count(); // [手順] - カタログの件数を取得する。

    // Assert
    EXPECT_NE(nullptr, actual_entries); // [確認_正常系] - カタログの先頭を取得できること。
    EXPECT_GT(actual_count, 0);         // [確認_正常系] - 件数が 1 件以上であること。
}

// 注入したカタログが引数スキーマと整合していることの確認
TEST_F(sampleMessagesTest, catalog_is_consistent)
{
    // Arrange
    int string_key = 0;
    cplat_string_catalog_language language = CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = cplat_string_catalog_verify(sample_messages_catalog(), &string_key,
                                             &language); // [手順] - 注入したカタログ全体を確認する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - すべての書式が引数スキーマと整合していること。
}

// 同じ翻訳単位から 2 つ目のカタログも利用できることの確認
TEST_F(sampleMessagesTest, second_catalog_is_consistent)
{
    // Arrange
    int string_key = 0;
    cplat_string_catalog_language language = CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = cplat_string_catalog_verify(sample_metrics_catalog(), &string_key,
                                             &language); // [手順] - 2 つ目のカタログ全体を確認する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 2 つ目のカタログも書式と引数スキーマが整合していること。
}

// 各文字列が ID とニュートラル言語のリソースを持つことの確認
TEST_F(sampleMessagesTest, every_entry_has_neutral_resource)
{
    // Arrange
    const cplat_string_catalog_entry *entries = sample_messages_entries();
    const int count = sample_messages_entry_count();
    int index;

    // Pre-Assert

    // Act / Assert
    for (index = 0; index < count; index++) // [手順] - すべてのカタログを走査する。
    {
        EXPECT_NE(nullptr, entries[index].id);      // [確認_正常系] - サンプルの定義はすべて ID を持つこと。
        EXPECT_NE(nullptr, entries[index].brief);   // [確認_正常系] - 短い説明を持つこと。
        EXPECT_NE(nullptr, entries[index].details); // [確認_正常系] - 詳細説明を持つこと。
        EXPECT_NE(
            nullptr,
            entries[index]
                .texts[CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL]); // [確認_正常系] - ニュートラル言語の書式を持つこと。
        EXPECT_NE(
            nullptr,
            entries[index]
                .notes[CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL]); // [確認_正常系] - ニュートラル言語の備考を持つこと。
        EXPECT_LE(entries[index].category,
                  CPLAT_TRACE_LEVEL_NONE);           // [確認_正常系] - 分類値がトレース レベルの範囲内であること。
        EXPECT_GE(entries[index].argument_count, 0); // [確認_正常系] - 引数個数が 0 以上であること。
        EXPECT_LE(entries[index].argument_count,
                  CPLAT_STRING_CATALOG_ARGUMENT_MAX); // [確認_正常系] - 引数個数が上限以下であること。
        if (entries[index].argument_count > 0)
        {
            EXPECT_NE(nullptr, entries[index].arguments); // [確認_正常系] - 引数定義を持つこと。
        }
    }
}

// 文字列キーから定義の内容を参照できることの確認
TEST_F(sampleMessagesTest, file_open_failed_entry)
{
    // Arrange
    const cplat_string_catalog_entry *actual_entry;
    const char *actual_id;
    int actual_category;
    char dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    int actual_ret;

    memset(dest, 0, sizeof(dest));

    // Pre-Assert

    // Act
    actual_entry =
        sample_messages_entry(SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED); // [手順] - 文字列キーの項目メタデータを取得する。
    actual_id = cplat_string_catalog_get_id(sample_messages_catalog(),
                                            SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED); // [手順] - ID を取得する。
    actual_category =
        cplat_string_catalog_get_category(sample_messages_catalog(),
                                          SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED); // [手順] - 分類値を取得する。
    actual_ret = cplat_string_catalog_format(sample_messages_catalog(), dest, sizeof(dest),
                                             SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED, "config.json",
                                             2); // [手順] - ニュートラル言語で文字列を組み立てる。

    // Assert
    ASSERT_NE(nullptr, actual_entry);                              // [確認_正常系] - 項目メタデータを取得できること。
    ASSERT_NE(nullptr, actual_entry->arguments);                   // [確認_正常系] - 引数定義を取得できること。
    EXPECT_STREQ("ファイルのオープン失敗。", actual_entry->brief); // [確認_正常系] - 短い説明を保持すること。
    EXPECT_STREQ("ファイルを開けなかったことを通知する文字列を組み立てます。",
                 actual_entry->details); // [確認_正常系] - 詳細説明を保持すること。
    EXPECT_EQ(CPLAT_STRING_CATALOG_ARGUMENT_KIND_STRING,
              actual_entry->arguments[0].kind);                 // [確認_正常系] - 1 番目の引数種別を保持すること。
    EXPECT_STREQ("file_path", actual_entry->arguments[0].name); // [確認_正常系] - 引数名を保持すること。
    EXPECT_STREQ("開けなかったファイルのパス。呼び出し側の指定をそのまま出力します。",
                 actual_entry->arguments[0].description); // [確認_正常系] - 1 番目の引数説明を保持すること。
    EXPECT_EQ(CPLAT_STRING_CATALOG_ARGUMENT_KIND_ERROR_CODE,
              actual_entry->arguments[1].kind);                  // [確認_正常系] - 2 番目の引数種別を保持すること。
    EXPECT_STREQ("error_code", actual_entry->arguments[1].name); // [確認_正常系] - 2 番目の引数名を保持すること。
    EXPECT_STREQ("errno または Win32 のエラー コード。10 進数と 16 進数を併記します。",
                 actual_entry->arguments[1].description); // [確認_正常系] - 2 番目の引数説明を保持すること。
    ASSERT_NE(nullptr, actual_id);                        // [確認_正常系] - ID を取得できること。
    EXPECT_STREQ("SAMPLE_MESSAGES_ID_0002", actual_id);   // [確認_正常系] - ID が一致すること。
    EXPECT_EQ(CPLAT_TRACE_LEVEL_ERROR, actual_category);  // [確認_正常系] - 分類値が一致すること。
    EXPECT_EQ(CPLAT_OK, actual_ret);                      // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// 補足説明がカタログ項目へ保持されることの確認
TEST_F(sampleMessagesTest, entry_remarks)
{
    // Arrange
    const cplat_string_catalog_entry *actual_entry;

    // Pre-Assert

    // Act
    actual_entry =
        sample_messages_entry(SAMPLE_MESSAGES_KEY_MEMORY_SIGNATURE); // [手順] - 補足説明を持つ項目を取得する。

    // Assert
    ASSERT_NE(nullptr, actual_entry); // [確認_正常系] - 項目メタデータを取得できること。
    EXPECT_STREQ("障害解析で領域の同一性を確認する目的で使用します。",
                 actual_entry->remarks); // [確認_正常系] - 補足説明を保持すること。
}

// 文字列キーごとの型付きラッパーが、カタログを指定した呼び出しと同じ結果を出すことの確認
TEST_F(sampleMessagesTest, typed_wrappers_match_generic_call)
{
    // Arrange
    char actual_dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    char expected_dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    int actual_ret_no_argument;
    int actual_ret_two_arguments;
    int expected_ret_two_arguments;

    memset(actual_dest, 0, sizeof(actual_dest));
    memset(expected_dest, 0, sizeof(expected_dest));

    // Pre-Assert

    // Act
    actual_ret_no_argument = sample_messages_key_startup_completed(
        actual_dest, sizeof(actual_dest)); // [手順] - 引数を取らないラッパーで組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret_no_argument); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_STREQ("Startup completed. The default setting is { default }.",
                 actual_dest); // [確認_正常系] - 引数なしの書式で組み立てられること。

    // Act
    actual_ret_two_arguments = sample_messages_key_file_open_failed(actual_dest, sizeof(actual_dest), "config.json",
                                                                    2); // [手順] - 型付きラッパーで組み立てる。
    expected_ret_two_arguments =
        cplat_string_catalog_format(sample_messages_catalog(), expected_dest, sizeof(expected_dest),
                                    SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED, "config.json",
                                    2); // [手順] - カタログを指定した呼び出しで同じ文字列を組み立てる。

    // Assert
    EXPECT_EQ(expected_ret_two_arguments,
              actual_ret_two_arguments);      // [確認_正常系] - 戻り値が一致すること。
    EXPECT_STREQ(expected_dest, actual_dest); // [確認_正常系] - 組み立てた文字列が一致すること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 actual_dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// 型付きラッパーでも、引数の順序が言語に依らないことの確認
TEST_F(sampleMessagesTest, typed_wrapper_argument_order_is_language_independent)
{
    // Arrange
    char neutral_dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    char japanese_dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    int actual_ret_neutral;
    int actual_ret_japanese;

    memset(neutral_dest, 0, sizeof(neutral_dest));
    memset(japanese_dest, 0, sizeof(japanese_dest));

    // Pre-Assert

    // Act
    actual_ret_neutral = sample_messages_key_record_mismatch(
        neutral_dest, sizeof(neutral_dest), 12U,
        0x1234ABCDU); // [手順] - ニュートラル言語で、レコード番号とシグネチャーの順に渡す。

    cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE);
    actual_ret_japanese = sample_messages_key_record_mismatch(japanese_dest, sizeof(japanese_dest), 12U,
                                                              0x1234ABCDU); // [手順] - 日本語で、同じ順序の引数を渡す。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret_neutral);  // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_EQ(CPLAT_OK, actual_ret_japanese); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_STREQ("Record 12 has an unexpected signature 0x1234abcd.",
                 neutral_dest); // [確認_正常系] - ニュートラル言語の語順で組み立てられること。
    EXPECT_STREQ("シグネチャー 0x1234abcd は、レコード 12 の想定と一致しません。",
                 japanese_dest); // [確認_正常系] - 引数順序を変えずに、日本語の語順で組み立てられること。
}
