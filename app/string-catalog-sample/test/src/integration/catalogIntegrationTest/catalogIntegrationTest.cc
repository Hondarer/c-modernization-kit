#include <testfw.h>

#include "sample_messages.h"
#include "sample_metrics.h"
#include "sample_trace.h"

#include <cplat/string_catalog/string_catalog.h>
#include <cplat/runtime/process.h>
#include <cplat/trace/tracer.h>
#include <stdint.h>
#include <string.h>

#include <string>

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
    int string_key = SAMPLE_MESSAGES_KEY_STARTUP_COMPLETED;
    cplat_string_catalog_language language = CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = cplat_string_catalog_verify(sample_messages_catalog(), &string_key,
                                             &language); // [手順] - 注入したカタログ全体を確認する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - すべての書式が引数スキーマと整合していること。
}

// 文字列定義の ID、レベル、備考を参照できることの確認
TEST_F(catalogIntegrationTest, metadata)
{
    // Arrange
    const char *actual_id;
    const char *actual_note;
    const char *actual_unknown_id;
    int actual_category;

    // Pre-Assert

    // Act
    actual_id = cplat_string_catalog_get_id(sample_messages_catalog(),
                                            SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED); // [手順] - ID を取得する。
    actual_note = cplat_string_catalog_get_note(sample_messages_catalog(),
                                                SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED); // [手順] - 備考を取得する。
    actual_category =
        cplat_string_catalog_get_category(sample_messages_catalog(),
                                          SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED); // [手順] - 分類値を取得する。
    actual_unknown_id = cplat_string_catalog_get_id(sample_messages_catalog(),
                                                    0); // [手順] - 未登録の文字列キーで ID を取得する。

    // Assert
    ASSERT_NE(nullptr, actual_id);                       // [確認_正常系] - ID を取得できること。
    ASSERT_NE(nullptr, actual_note);                     // [確認_正常系] - 備考を取得できること。
    EXPECT_STREQ("SAMPLE_MESSAGES_ID_0002", actual_id);  // [確認_正常系] - ID が一致すること。
    EXPECT_EQ(CPLAT_TRACE_LEVEL_ERROR, actual_category); // [確認_正常系] - カタログの分類値が一致すること。
    EXPECT_LT(0U, strlen(actual_note));                  // [確認_正常系] - 備考が空でないこと。
    EXPECT_EQ(nullptr, actual_unknown_id);               // [確認_異常系] - 未登録の文字列キーでは NULL を返すこと。
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
        SAMPLE_MESSAGES_KEY_STARTUP_COMPLETED); // [手順] - 引数を取らない文字列を日本語で組み立てる。

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
                                             SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED, "config.json",
                                             2); // [手順] - ニュートラル言語で文字列を組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_STREQ("Failed to open file config.json. Error code=2 (0x00000002)",
                 dest); // [確認_正常系] - ニュートラル言語の書式で組み立てられること。
}

// 引数の型と表現が文字列キー側で決まることの確認
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
        sample_messages_catalog(), dest, sizeof(dest), SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED, "config.json",
        2); // [手順] - ファイル オープン失敗の文字列を日本語で組み立てる。

    cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_ENGLISH);
    actual_ret_english = cplat_string_catalog_format(sample_messages_catalog(), english_dest, sizeof(english_dest),
                                                     SAMPLE_MESSAGES_KEY_FILE_OPEN_FAILED, "config.json",
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
        sample_messages_catalog(), dest, sizeof(dest), SAMPLE_MESSAGES_KEY_RECORD_MISMATCH, UINT32_C(42),
        UINT32_C(0x1234ABCD)); // [手順] - レコード不一致の文字列を日本語で組み立てる。

    cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_ENGLISH);
    actual_ret_english =
        cplat_string_catalog_format(sample_messages_catalog(), english_dest, sizeof(english_dest),
                                    SAMPLE_MESSAGES_KEY_RECORD_MISMATCH, UINT32_C(42),
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
        sample_metrics_catalog(), dest, sizeof(dest), SAMPLE_METRICS_KEY_THROUGHPUT_REPORT, 12.5,
        UINT64_C(4000000000)); // [手順] - 同じ位置指定を 2 回含む文字列を組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_STREQ("処理速度は 12.5 件/秒です。累計 4000000000 件を 12.5 件/秒で処理しました。",
                 dest); // [確認_正常系] - 同じ値が 2 回展開されること。
}

// トレース種別のカタログが引数スキーマと整合していることの確認
TEST_F(catalogIntegrationTest, trace_catalog_is_consistent)
{
    // Arrange
    int string_key = SAMPLE_TRACE_KEY_SERVICE_STARTED;
    cplat_string_catalog_language language = CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = cplat_string_catalog_verify(sample_trace_catalog(), &string_key,
                                             &language); // [手順] - トレース種別のカタログ全体を確認する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 文脈引数を含む定義が点検を通過すること。
}

// トレース レベルの名前が分類値の整数として保持されていることの確認
TEST_F(catalogIntegrationTest, trace_level_is_stored_as_category)
{
    // Arrange
    int info_category;
    int error_category;
    int verbose_category;

    // Pre-Assert

    // Act
    info_category = cplat_string_catalog_get_category(
        sample_trace_catalog(), SAMPLE_TRACE_KEY_SERVICE_STARTED); // [手順] - INFO の分類値を取得する。
    error_category = cplat_string_catalog_get_category(
        sample_trace_catalog(), SAMPLE_TRACE_KEY_FILE_OPEN_FAILED); // [手順] - ERROR の分類値を取得する。
    verbose_category = cplat_string_catalog_get_category(
        sample_trace_catalog(), SAMPLE_TRACE_KEY_RECORD_PARSED); // [手順] - VERBOSE の分類値を取得する。

    // Assert
    EXPECT_EQ((int)CPLAT_TRACE_LEVEL_INFO, info_category); // [確認_正常系] - INFO が対応する整数になること。
    EXPECT_EQ((int)CPLAT_TRACE_LEVEL_ERROR, error_category);     // [確認_正常系] - ERROR が対応する整数になること。
    EXPECT_EQ((int)CPLAT_TRACE_LEVEL_VERBOSE, verbose_category); // [確認_正常系] - VERBOSE が対応する整数になること。
}

// 文脈引数が引数配列の 40 番から並んでいることの確認
TEST_F(catalogIntegrationTest, context_arguments_follow_user_arguments)
{
    // Arrange
    const cplat_string_catalog_entry *entry;

    // Pre-Assert

    // Act
    entry = cplat_string_catalog_get_entry(sample_trace_catalog(),
                                           SAMPLE_TRACE_KEY_FILE_OPEN_FAILED); // [手順] - 項目を取得する。

    // Assert
    ASSERT_NE(nullptr, entry);           // [確認_正常系] - 項目を取得できること。
    EXPECT_EQ(46, entry->argument_count); // [確認_正常系] - 文脈引数の末尾までを含む要素数であること。
    EXPECT_STREQ("file_path", entry->arguments[0].name); // [確認_正常系] - 利用者の引数が 0 番から並ぶこと。
    EXPECT_EQ(CPLAT_STRING_CATALOG_ARGUMENT_KIND_UNUSED,
              entry->arguments[2].kind); // [確認_正常系] - 利用者の引数と文脈引数の間が未使用であること。
    EXPECT_STREQ("source_file_path", entry->arguments[40].name); // [確認_正常系] - 文脈引数が 40 番から並ぶこと。
    EXPECT_STREQ("thread_id", entry->arguments[45].name);        // [確認_正常系] - 文脈引数の末尾が 45 番であること。
}

// 型付きラッパーのマクロが、トレースへ出力することの確認
TEST_F(catalogIntegrationTest, trace_macro_writes_to_tracer)
{
    // Arrange
    cplat_tracer *tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    int actual_ret;

    ASSERT_NE(nullptr, tracer); // [状態確認] - トレーサーを生成できること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_set_stderr_level(
                            tracer, CPLAT_TRACE_LEVEL_VERBOSE)); // [状態] - 標準エラー出力の詳細度を VERBOSE とする。
    // [状態確認] - cplat_tracer_set_stderr_level の戻り値が CPLAT_OK であること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_start(tracer)); // [状態] - トレースを開始する。
                                                     // [状態確認] - cplat_tracer_start の戻り値が CPLAT_OK であること。

    // Pre-Assert

    sample_trace_set_tracer(tracer); // [状態] - カタログの出力先へトレーサーを設定する。

    // Act
    testing::internal::CaptureStderr();
    actual_ret =
        sample_trace_key_file_open_failed("config.json", 2); // [手順] - 型付きラッパーのマクロでトレースへ出力する。
    std::string captured = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_NE(std::string::npos,
              captured.find("E ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)"));
    // [確認_正常系] - 定義したレベルと、現在の出力言語の書式で出力されること。

    // Cleanup
    sample_trace_set_tracer(nullptr);
    cplat_tracer_dispose(&tracer);
}

// 書式が参照する文脈引数が、呼び出し位置と実行文脈の値へ展開されることの確認
TEST_F(catalogIntegrationTest, trace_format_expands_context_arguments)
{
    // Arrange
    cplat_tracer *tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    int actual_ret;

    ASSERT_NE(nullptr, tracer); // [状態確認] - トレーサーを生成できること。
    ASSERT_EQ(CPLAT_OK,
              cplat_tracer_set_stderr_level(tracer,
                                            CPLAT_TRACE_LEVEL_DEBUG)); // [状態] - 標準エラー出力を DEBUG とする。
    // [状態確認] - cplat_tracer_set_stderr_level の戻り値が CPLAT_OK であること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_start(tracer)); // [状態] - トレースを開始する。
                                                     // [状態確認] - cplat_tracer_start の戻り値が CPLAT_OK であること。
    sample_trace_set_tracer(tracer);                 // [状態] - カタログの出力先へトレーサーを設定する。

    // Pre-Assert

    // Act
    testing::internal::CaptureStderr();
    actual_ret = sample_trace_key_state_dump(UINT32_C(7)); // [手順] - 文脈引数を参照する書式で出力する。
    std::string captured = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 戻り値が CPLAT_OK であること。
    EXPECT_NE(std::string::npos,
              captured.find("待ち行列の長さは 7 です。")); // [確認_正常系] - 利用者の引数が展開されること。
    EXPECT_NE(std::string::npos,
              captured.find("catalogIntegrationTest.cc:")); // [確認_正常系] - 41 番が呼び出し元のファイル名になること。
    EXPECT_NE(std::string::npos,
              captured.find("TestBody")); // [確認_正常系] - 43 番が呼び出し元の関数名になること。
    EXPECT_NE(std::string::npos,
              captured.find("プロセス=" + std::to_string(cplat_process_get_pid()))); // [確認_正常系] -
                                                                                     // 44 番が実際のプロセス ID
                                                                                     // になること。
    EXPECT_NE(std::string::npos,
              captured.find("スレッド=" + std::to_string(cplat_process_get_tid()))); // [確認_正常系] -
                                                                                     // 45 番が実際のスレッド ID
                                                                                     // になること。

    // Cleanup
    sample_trace_set_tracer(nullptr);
    cplat_tracer_dispose(&tracer);
}

// 出力先が未設定のときに、何も出力せず失敗を返すことの確認
TEST_F(catalogIntegrationTest, trace_macro_fails_without_tracer)
{
    // Arrange
    int actual_ret;

    sample_trace_set_tracer(nullptr); // [状態] - カタログの出力先を未設定にする。

    // Pre-Assert
    ASSERT_EQ(nullptr, sample_trace_get_tracer()); // [Pre-Assert確認_異常系] - 出力先が未設定であること。

    // Act
    testing::internal::CaptureStderr();
    actual_ret = sample_trace_key_file_open_failed("config.json",
                                                   2); // [手順] - 出力先が未設定のまま出力を要求する。
    std::string captured = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - 戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ("", captured); // [確認_異常系] - 何も出力しないこと。
}
