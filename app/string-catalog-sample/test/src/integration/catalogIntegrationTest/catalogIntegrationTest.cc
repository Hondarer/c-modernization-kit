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

    // [サブ手順 名前=catalogIntegrationTest.SetUp]
    void SetUp() override
    {
        memset(dest, 0, sizeof(dest));
        cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE);
    }
    // [サブ手順終了]
};

// 注入したカタログが引数スキーマと整合していることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret); // [確認_正常系] - cplat_string_catalog_verify() の戻り値が CPLAT_OK であること。(すべての書式が引数スキーマと整合していることの確認)
}

// 文字列定義の ID、レベル、備考を参照できることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    ASSERT_NE(
        nullptr,
        actual_id); // [確認_正常系] - ID を取得したときの cplat_string_catalog_get_id() の戻り値が nullptr 以外であること。
    ASSERT_NE(nullptr,
              actual_note); // [確認_正常系] - cplat_string_catalog_get_note() の戻り値が nullptr 以外であること。
    EXPECT_STREQ(
        "SAMPLE_MESSAGES_ID_0002",
        actual_id); // [確認_正常系] - ID を取得したときの cplat_string_catalog_get_id() の戻り値が "SAMPLE_MESSAGES_ID_0002" であること。
    EXPECT_EQ(
        CPLAT_TRACE_LEVEL_ERROR,
        actual_category); // [確認_正常系] - cplat_string_catalog_get_category() の戻り値が CPLAT_TRACE_LEVEL_ERROR であること。
    EXPECT_LT(
        0U,
        strlen(
            actual_note)); // [確認_正常系] - strlen() の戻り値が 0U より大きいこと。(cplat_string_catalog_get_note() の戻り値が空でないことの確認)
    EXPECT_EQ(
        nullptr,
        actual_unknown_id); // [確認_異常系] - 未登録の文字列キーを渡したときの cplat_string_catalog_get_id() の戻り値が nullptr であること。
}

// 波括弧のエスケープを含む文字列を組み立てられることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - cplat_string_catalog_format() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ(
        "起動が完了しました。既定の設定は { default } です。",
        dest); // [確認_正常系] - dest が "起動が完了しました。既定の設定は { default } です。" であること。({{ と }} が 1 文字の波括弧になったことの確認)
}

// 言語を設定していない状態でニュートラル言語の書式を使用することの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - cplat_string_catalog_format() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ(
        "Failed to open file config.json. Error code=2 (0x00000002)",
        dest); // [確認_正常系] - dest が "Failed to open file config.json. Error code=2 (0x00000002)" であること。(ニュートラル言語の書式で組み立てられたことの確認)
}

// 引数の型と表現が文字列キー側で決定されることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret_japanese); // [確認_正常系] - 日本語での cplat_string_catalog_format() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret_english); // [確認_正常系] - 英語での cplat_string_catalog_format() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ(
        "ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)",
        dest); // [確認_正常系] - dest が "ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)" であること。
    EXPECT_STREQ(
        "Failed to open file config.json. Error code=2 (0x00000002)",
        english_dest); // [確認_正常系] - english_dest が "Failed to open file config.json. Error code=2 (0x00000002)" であること。(エラー コードの表現が言語に依存しないことの確認)
}

// 言語別リソースが語順のみを決定することの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret_japanese); // [確認_正常系] - 日本語での cplat_string_catalog_format() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret_english); // [確認_正常系] - 英語での cplat_string_catalog_format() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ(
        "シグネチャー 0x1234abcd は、レコード 42 の想定と一致しません。",
        dest); // [確認_正常系] - dest が "シグネチャー 0x1234abcd は、レコード 42 の想定と一致しません。" であること。(日本語では位置指定を入れ替えた語順になったことの確認)
    EXPECT_STREQ(
        "Record 42 has an unexpected signature 0x1234abcd.",
        english_dest); // [確認_正常系] - english_dest が "Record 42 has an unexpected signature 0x1234abcd." であること。(呼び出し側の引数順を変更せずに語順のみが変化したことの確認)
}

// 同じ引数を複数回参照できることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - cplat_string_catalog_format() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ(
        "処理速度は 12.5 件/秒です。累計 4000000000 件を 12.5 件/秒で処理しました。",
        dest); // [確認_正常系] - dest が "処理速度は 12.5 件/秒です。累計 4000000000 件を 12.5 件/秒で処理しました。" であること。(同じ値が 2 回展開されたことの確認)
}

// トレース種別のカタログが引数スキーマと整合していることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret); // [確認_正常系] - cplat_string_catalog_verify() の戻り値が CPLAT_OK であること。(文脈引数を含む定義が点検を通過したことの確認)
}

// トレース レベルの名前が分類値の整数として保持されていることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
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
    EXPECT_EQ(
        (int)CPLAT_TRACE_LEVEL_INFO,
        info_category); // [確認_正常系] - SAMPLE_TRACE_KEY_SERVICE_STARTED に対する cplat_string_catalog_get_category() の戻り値が (int)CPLAT_TRACE_LEVEL_INFO であること。
    EXPECT_EQ(
        (int)CPLAT_TRACE_LEVEL_ERROR,
        error_category); // [確認_正常系] - SAMPLE_TRACE_KEY_FILE_OPEN_FAILED に対する cplat_string_catalog_get_category() の戻り値が (int)CPLAT_TRACE_LEVEL_ERROR であること。
    EXPECT_EQ(
        (int)CPLAT_TRACE_LEVEL_VERBOSE,
        verbose_category); // [確認_正常系] - SAMPLE_TRACE_KEY_RECORD_PARSED に対する cplat_string_catalog_get_category() の戻り値が (int)CPLAT_TRACE_LEVEL_VERBOSE であること。
}

// コンテキスト引数が引数配列のインデックス 40 から並んでいることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
TEST_F(catalogIntegrationTest, context_arguments_follow_user_arguments)
{
    // Arrange
    const cplat_string_catalog_entry *entry;

    // Pre-Assert

    // Act
    entry = cplat_string_catalog_get_entry(sample_trace_catalog(),
                                           SAMPLE_TRACE_KEY_FILE_OPEN_FAILED); // [手順] - 項目を取得する。

    // Assert
    ASSERT_NE(nullptr, entry); // [確認_正常系] - cplat_string_catalog_get_entry() の戻り値が nullptr 以外であること。
    EXPECT_EQ(
        CPLAT_STRING_CATALOG_ARGUMENT_MAX,
        entry
            ->argument_count); // [確認_正常系] - entry->argument_count が CPLAT_STRING_CATALOG_ARGUMENT_MAX であること。(文脈引数のために予約した番号空間の全体を確保したことの確認)
    EXPECT_STREQ(
        "file_path",
        entry->arguments[0]
            .name); // [確認_正常系] - entry->arguments[0].name が "file_path" であること。(利用者の引数が 0 番から並ぶことの確認)
    EXPECT_EQ(
        CPLAT_STRING_CATALOG_ARGUMENT_KIND_UNUSED,
        entry->arguments[2]
            .kind); // [確認_正常系] - entry->arguments[2].kind が CPLAT_STRING_CATALOG_ARGUMENT_KIND_UNUSED であること。(利用者の引数と文脈引数の間が未使用であることの確認)
    EXPECT_STREQ(
        "source_file_path",
        entry->arguments[40]
            .name); // [確認_正常系] - entry->arguments[40].name が "source_file_path" であること。(文脈引数が 40 番から並ぶことの確認)
    EXPECT_STREQ(
        "thread_id",
        entry->arguments[45]
            .name); // [確認_正常系] - entry->arguments[45].name が "thread_id" であること。(cplat が定義するコンテキスト引数の末尾が 45 番であることの確認)

    // 本カタログは app が定義するコンテキスト引数を保持しないため、インデックス 46 から 49 は予約された未使用領域となる。
    for (int index = 46; index < CPLAT_STRING_CATALOG_ARGUMENT_MAX; index++)
    {
        EXPECT_EQ(
            CPLAT_STRING_CATALOG_ARGUMENT_KIND_UNUSED,
            entry->arguments[index]
                .kind); // [確認_正常系 回数=4] - entry->arguments[index].kind が CPLAT_STRING_CATALOG_ARGUMENT_KIND_UNUSED であること。(予約した番号が未使用のまま残ることの確認)
    }
}

// 型付きラッパーのマクロが、トレースへ出力することの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
TEST_F(catalogIntegrationTest, trace_macro_writes_to_tracer)
{
    // Arrange
    cplat_tracer *tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    int actual_ret;

    ASSERT_NE(nullptr, tracer); // [状態確認] - cplat_tracer_create() の戻り値が nullptr 以外であること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_set_stderr_level(
                            tracer, CPLAT_TRACE_LEVEL_VERBOSE)); // [状態] - 標準エラー出力の詳細度を VERBOSE とする。
    // [状態確認] - cplat_tracer_set_stderr_level() の戻り値が CPLAT_OK であること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_start(tracer)); // [状態] - トレースを開始する。
    // [状態確認] - cplat_tracer_start() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    sample_trace_set_tracer(tracer); // [状態] - カタログの出力先へトレーサーを設定する。

    // Act
    testing::internal::CaptureStderr();
    actual_ret =
        sample_trace_key_file_open_failed("config.json", 2); // [手順] - 型付きラッパーのマクロでトレースへ出力する。
    std::string captured = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret); // [確認_正常系] - sample_trace_key_file_open_failed() の戻り値が CPLAT_OK であること。
    EXPECT_NE(std::string::npos,
              captured.find("E ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)"));
    // [確認_正常系] - "E ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)" を探した captured.find() の戻り値が std::string::npos 以外であること。(定義したレベルと現在の出力言語の書式で出力されたことの確認)

    // Cleanup
    sample_trace_set_tracer(nullptr);
    cplat_tracer_dispose(&tracer);
}

// 書式が参照するコンテキスト引数が、呼び出し位置と実行コンテキストの値へ展開されることの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
TEST_F(catalogIntegrationTest, trace_format_expands_context_arguments)
{
    // Arrange
    cplat_tracer *tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    int actual_ret;

    ASSERT_NE(nullptr, tracer); // [状態確認] - cplat_tracer_create() の戻り値が nullptr 以外であること。
    ASSERT_EQ(CPLAT_OK,
              cplat_tracer_set_stderr_level(tracer,
                                            CPLAT_TRACE_LEVEL_DEBUG)); // [状態] - 標準エラー出力を DEBUG とする。
    // [状態確認] - cplat_tracer_set_stderr_level() の戻り値が CPLAT_OK であること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_start(tracer)); // [状態] - トレースを開始する。
    // [状態確認] - cplat_tracer_start() の戻り値が CPLAT_OK であること。
    sample_trace_set_tracer(tracer);                 // [状態] - カタログの出力先へトレーサーを設定する。

    // Pre-Assert

    // Act
    testing::internal::CaptureStderr();
    actual_ret = sample_trace_key_state_dump(UINT32_C(7)); // [手順] - コンテキスト引数を参照する書式で出力する。
    std::string captured = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - sample_trace_key_state_dump() の戻り値が CPLAT_OK であること。
    EXPECT_NE(
        std::string::npos,
        captured.find(
            "キューの長さは 7 です。")); // [確認_正常系] - "キューの長さは 7 です。" を探した captured.find() の戻り値が std::string::npos 以外であること。(利用者の引数が展開されたことの確認)
    EXPECT_NE(
        std::string::npos,
        captured.find(
            "catalogIntegrationTest.cc:")); // [確認_正常系] - "catalogIntegrationTest.cc:" を探した captured.find() の戻り値が std::string::npos 以外であること。(41 番が呼び出し元のファイル名になったことの確認)
    EXPECT_NE(
        std::string::npos,
        captured.find(
            "TestBody")); // [確認_正常系] - "TestBody" を探した captured.find() の戻り値が std::string::npos 以外であること。(43 番が呼び出し元の関数名になったことの確認)
    EXPECT_NE(
        std::string::npos,
        captured.find(
            "プロセス=" +
            std::to_string(
                cplat_process_get_pid()))); // [確認_正常系] - "プロセス=" と cplat_process_get_pid() の戻り値の文字列を連結した文字列を探した captured.find() の戻り値が std::string::npos 以外であること。(44 番がプロセス ID になったことの確認)
    EXPECT_NE(
        std::string::npos,
        captured.find(
            "スレッド=" +
            std::to_string(
                cplat_process_get_tid()))); // [確認_正常系] - "スレッド=" と cplat_process_get_tid() の戻り値の文字列を連結した文字列を探した captured.find() の戻り値が std::string::npos 以外であること。(45 番がスレッド ID になったことの確認)

    // Cleanup
    sample_trace_set_tracer(nullptr);
    cplat_tracer_dispose(&tracer);
}

// 出力先が未設定のときに、何も出力せず失敗を返すことの確認
// [サブ手順参照 名前=catalogIntegrationTest.SetUp]
TEST_F(catalogIntegrationTest, trace_macro_fails_without_tracer)
{
    // Arrange
    int actual_ret;

    sample_trace_set_tracer(nullptr); // [状態] - カタログの出力先を未設定にする。
    ASSERT_EQ(nullptr,
              sample_trace_get_tracer()); // [状態確認] - sample_trace_get_tracer() の戻り値が nullptr であること。

    // Pre-Assert

    // Act
    testing::internal::CaptureStderr();
    actual_ret = sample_trace_key_file_open_failed("config.json",
                                                   2); // [手順] - 出力先が未設定のまま出力を要求する。
    std::string captured = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        actual_ret); // [確認_異常系] - sample_trace_key_file_open_failed() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ("", captured); // [確認_異常系] - captured が "" であること。(何も出力しなかったことの確認)
}
