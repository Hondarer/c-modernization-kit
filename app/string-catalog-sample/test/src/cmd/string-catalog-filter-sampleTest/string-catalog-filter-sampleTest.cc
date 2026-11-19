#include <cplat/base/platform.h>
#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_stdio.h>
#include <mock_stdlib.h>
#include <mock_string.h>
#include <mock_cplat.h>

#include "gen/sample_worker_trace.h"
#include "string-catalog-filter-sample.inject.h"

#include <cplat/base/result.h>
#include <cplat/string_catalog/filter.h>
#include <cplat/trace/tracer.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace
{

/** トレース フックが受け取った 1 件分の内容です。 */
struct captured_trace
{
    std::string message;
    cplat_trace_level level;
    unsigned char pad[4] = {}; // 明示的アラインメントのためのパディング。
};

void capture_hook(cplat_tracer_hook_entry *prev, cplat_tracer *handle, cplat_trace_level level,
                  const cplat_timespec *timestamp, const char *message, void *context)
{
    std::vector<captured_trace> *traces = static_cast<std::vector<captured_trace> *>(context);

    (void)prev;
    (void)handle;
    (void)timestamp;
    traces->push_back({message, level});
}

/** 表示の関数へ渡された文面を 1 件分記録します。 */
struct captured_print
{
    std::string text;
    cplat_pinned_prompt_channel channel;
    unsigned char pad[4] = {}; // 明示的アラインメントのためのパディング。
};

/** 条件式 1 行だけのフィルター オブジェクトの行幅です。 */
constexpr std::size_t kLineWidth = 160U;

/** 条件式 1 行だけのフィルター オブジェクトの行数の上限です。 */
constexpr std::size_t kLineCapacity = 4U;

/** フィルター オブジェクトのバイト数です。 */
constexpr std::size_t kImageSize = CPLAT_STRING_CATALOG_FILTER_IMAGE_SIZE(kLineCapacity, kLineWidth);

} // namespace

// ワーカーの待機に失敗した場合に、ERROR のトレースで結果コードを出して周期の処理を止めることの確認
TEST(string_catalog_filter_sampleTest, worker_stops_when_wait_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    std::vector<captured_trace> traces;
    cplat_tracer *tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    ASSERT_NE(nullptr, tracer); // [状態確認] - トレーサーを生成できること。
    cplat_tracer_hook_entry *hook_entry = cplat_tracer_set_hook(tracer, capture_hook, &traces);
    ASSERT_NE(nullptr, hook_entry); // [状態確認] - フックを登録できること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_start(tracer)); // [状態確認] - トレーサーを開始できること。
    sample_worker_trace_set_tracer(tracer);         // [状態] - ワーカーのトレースの出力先を設定する。
    ASSERT_EQ(CPLAT_OK, test_filter_sample_prepare_worker_sync()); // [状態確認] - ワーカーの同期オブジェクトを用意できること。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_condvar_timed_wait(_, _, 500)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
    // [Pre-Assert確認_異常系] - cplat_condvar_timed_wait() が間隔 500 を指定して 1 回だけ呼び出されること。(失敗の後に待機を繰り返さないことの確認)
    // [Pre-Assert手順] - cplat_condvar_timed_wait() から CPLAT_ERR_UNKNOWN を返す。

    // Act
    test_filter_sample_run_worker(3U, 500U); // [手順] - ワーカー 3 の本体を実行する。

    // Assert
    std::size_t error_count = 0U;
    std::string error_message;
    for (const captured_trace &trace : traces)
    {
        if (trace.level == CPLAT_TRACE_LEVEL_ERROR)
        {
            error_count++;
            error_message = trace.message;
        }
    }
    EXPECT_EQ(1U, error_count); // [確認_異常系] - ERROR のトレースが 1 件であること。
    EXPECT_NE(std::string::npos, error_message.find("3"));
    EXPECT_NE(std::string::npos, error_message.find("=" + std::to_string(CPLAT_ERR_UNKNOWN)));
    // [確認_異常系] - ERROR のトレースに、ワーカーの番号と結果コードが含まれること。
    ASSERT_FALSE(traces.empty());
    EXPECT_EQ(CPLAT_TRACE_LEVEL_INFO, traces.back().level);
    // [確認_異常系] - 最後のトレースが INFO であること。(待機の失敗の後もワーカーの停止のトレースを出すことの確認)

    // Cleanup
    test_filter_sample_dispose_worker_sync();
    sample_worker_trace_set_tracer(nullptr);
    (void)cplat_tracer_stop(tracer);
    cplat_tracer_remove_hook(tracer, hook_entry);
    cplat_tracer_dispose(&tracer);
}

// start コマンドで INT_MAX を超える間隔を指定した場合に、STDERR のチャネルへ出してワーカーを起動しないことの確認
TEST(string_catalog_filter_sampleTest, start_rejects_interval_over_int_max)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    std::vector<captured_print> prints;
    char arg[] = "1 3000000000"; // [状態] - ワーカー 1 個、間隔 3000000000 ミリ秒を指定する。
    ON_CALL(mock_cplat, cplat_pinned_prompt_printf(_, _, _))
        .WillByDefault(Invoke(
            [&prints](cplat_pinned_prompt *, cplat_pinned_prompt_channel channel, const char *text) -> int
            {
                prints.push_back({text, channel});
                return 0;
            })); // [状態] - 表示の関数へ渡された文面を記録する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_thread_create(_, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_thread_create() が呼び出されないこと。

    // Act
    test_filter_sample_command_start(nullptr, arg); // [手順] - start コマンドの処理を呼び出す。

    // Assert
    ASSERT_EQ(1U, prints.size()); // [確認_異常系] - 表示が 1 件であること。
    EXPECT_EQ(CPLAT_PINNED_PROMPT_CHANNEL_STDERR, prints[0].channel); // [確認_異常系] - STDERR のチャネルへ出すこと。
    EXPECT_NE(std::string::npos, prints[0].text.find("間隔ミリ秒は 1 以上"));
    // [確認_異常系] - 間隔の範囲を示す文面であること。
}

// 行の状態を取得できない場合に、代替の表示を STDOUT のチャネルへ出して説明の表示を飛ばすことの確認
TEST(string_catalog_filter_sampleTest, print_filter_lines_reports_line_error_failure)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    std::vector<captured_print> prints;
    static unsigned char image[kImageSize];
    char rows[1][kLineWidth] = {};
    const char expression[] = "key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED";
    std::size_t invalid_count = 0U;
    std::memcpy(rows[0], expression, sizeof(expression));
    ASSERT_EQ(CPLAT_OK, cplat_string_catalog_filter_compile(&rows[0][0], 1U, kLineWidth, kLineCapacity, image,
                                                            kImageSize, nullptr, 0U, &invalid_count));
    // [状態確認] - 条件式 1 行のフィルター オブジェクトをコンパイルできること。
    cplat_string_catalog_filter_slot *slot = nullptr;
    ASSERT_EQ(CPLAT_OK, sample_worker_trace_create_filter(nullptr, kLineCapacity, kLineWidth, &slot));
    // [状態確認] - 説明の取得に使うスロットを作成できること。
    ON_CALL(mock_cplat, cplat_pinned_prompt_printf(_, _, _))
        .WillByDefault(Invoke(
            [&prints](cplat_pinned_prompt *, cplat_pinned_prompt_channel channel, const char *text) -> int
            {
                prints.push_back({text, channel});
                return 0;
            })); // [状態] - 表示の関数へ渡された文面を記録する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_string_catalog_filter_slot_get_line_error(slot, 0U, _))
        .WillOnce(Return(CPLAT_ERR_INVALID_ARGUMENT));
    // [Pre-Assert確認_異常系] - cplat_string_catalog_filter_slot_get_line_error() が 1 行目を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_string_catalog_filter_slot_get_line_error() から CPLAT_ERR_INVALID_ARGUMENT を返す。
    EXPECT_CALL(mock_cplat, cplat_string_catalog_filter_slot_describe_line(_, _, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - 説明の取得を呼び出さないこと。

    // Act
    test_filter_sample_print_filter_lines(nullptr, image, kImageSize, slot); // [手順] - 条件式の表示を呼び出す。

    // Assert
    ASSERT_EQ(1U, prints.size()); // [確認_異常系] - 表示が 1 件であること。
    EXPECT_EQ(CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, prints[0].channel); // [確認_異常系] - STDOUT のチャネルへ出すこと。
    EXPECT_NE(std::string::npos, prints[0].text.find("<状態を取得できません (結果コード=" +
                                                     std::to_string(CPLAT_ERR_INVALID_ARGUMENT) + ")>"));
    EXPECT_NE(std::string::npos, prints[0].text.find(expression));
    // [確認_異常系] - 代替の表示に結果コードと条件式が含まれること。

    // Cleanup
    cplat_string_catalog_filter_slot_dispose(&slot);
}
