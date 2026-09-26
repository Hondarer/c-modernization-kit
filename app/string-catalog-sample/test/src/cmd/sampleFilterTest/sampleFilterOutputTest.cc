#include <testfw.h>

#include "sampleFilterTestSupport.h"

#include "gen/sample_worker_trace.h"
#include "sample_filter_output.h"
#include "sample_worker_trace_key_names.h"

#include <cplat/base/result.h>
#include <cplat/trace/tracer.h>

#include <cstdint>
#include <cstring>

using namespace sample_filter_test;

namespace
{
    /** トレース フックが受け取った内容を保存します。 */
    struct hook_capture
    {
        cplat_trace_level level;
        char message[CPLAT_STRING_CATALOG_TEXT_MAX];
        int call_count;
    };

    void capture_hook(cplat_tracer_hook_entry *prev, cplat_tracer *handle, cplat_trace_level level,
                      const cplat_timespec *timestamp, const char *message, void *context)
    {
        hook_capture *capture = static_cast<hook_capture *>(context);

        (void)prev;
        (void)handle;
        (void)timestamp;
        capture->level = level;
        std::strncpy(capture->message, message, sizeof(capture->message) - 1U);
        capture->message[sizeof(capture->message) - 1U] = '\0';
        capture->call_count++;
    }
} // namespace

class sampleFilterOutputTest : public Test
{
  protected:
    void SetUp() override
    {
        ASSERT_EQ(CPLAT_OK,
                 sample_filter_slot_create(sample_worker_trace_catalog(), sample_worker_trace_key_names(),
                                           sample_worker_trace_key_name_count(), kLineCapacity, kLineWidth, &slot_));

        std::memset(&capture_, 0, sizeof(capture_));

        tracer_ = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
        ASSERT_NE(nullptr, tracer_);
        hook_entry_ = cplat_tracer_set_hook(tracer_, capture_hook, &capture_); /* stopped 状態で登録する */
        ASSERT_NE(nullptr, hook_entry_);
        ASSERT_EQ(CPLAT_OK, cplat_tracer_start(tracer_));

        ASSERT_EQ(CPLAT_OK, sample_filter_output_configure(sample_worker_trace_catalog(), slot_, tracer_));
    }

    void TearDown() override
    {
        (void)sample_filter_output_configure(nullptr, nullptr, nullptr);
        (void)cplat_tracer_stop(tracer_);
        cplat_tracer_remove_hook(tracer_, hook_entry_);
        cplat_tracer_dispose(&tracer_);
        sample_filter_slot_dispose(&slot_);
    }

    sample_filter_slot *slot_ = nullptr;
    cplat_tracer *tracer_ = nullptr;
    cplat_tracer_hook_entry *hook_entry_ = nullptr;
    hook_capture capture_{};
};

// 条件式に一致しない場合、分類値をそのままトレース レベルとして使うことの確認 (JOB_FAILED は WARNING)
TEST_F(sampleFilterOutputTest, unmatched_trace_uses_category_level_directly)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = sample_filter_output_write(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED, (uint64_t)1, (int)2,
                                            SAMPLE_FILTER_TEST_CONTEXT_ARGS(
                                                7)); // [手順] - 作成直後 (何も一致しない) のスロットで JOB_FAILED を出力する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);                      // [確認_正常系] - 戻り値が CPLAT_OK であること。
    ASSERT_EQ(1, capture_.call_count);                    // [確認_正常系] - フックが 1 回呼び出されること。
    EXPECT_EQ(CPLAT_TRACE_LEVEL_WARNING, capture_.level); // [確認_正常系] - JOB_FAILED の分類値 (WARNING) がそのまま使われること。
    EXPECT_EQ('#', capture_.message[0]); // [確認_正常系] - 組み立てた文字列の先頭がラウンド トリップ ID の接頭辞 "#" であること。
}

// 条件式に一致する場合、強制出力のレベルへ引き上げることの確認
TEST_F(sampleFilterOutputTest, matched_trace_uses_forced_level)
{
    // Arrange
    static unsigned char image[kImageSize];
    int actual_ret;

    ASSERT_EQ(CPLAT_OK,
             compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED", image)); // [状態] - JOB_FAILED に一致する条件式をコンパイルする。
    ASSERT_EQ(CPLAT_OK, sample_filter_slot_apply(slot_, image, kImageSize, nullptr, 0U, nullptr)); // [状態] - スロットへ適用する。

    // Pre-Assert

    // Act
    actual_ret = sample_filter_output_write(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED, (uint64_t)1, (int)2,
                                            SAMPLE_FILTER_TEST_CONTEXT_ARGS(7)); // [手順] - 一致する状態で JOB_FAILED を出力する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);   // [確認_正常系] - 戻り値が CPLAT_OK であること。
    ASSERT_EQ(1, capture_.call_count); // [確認_正常系] - フックが 1 回呼び出されること。
    EXPECT_EQ(CPLAT_TRACE_LEVEL_TO_FORCE(CPLAT_TRACE_LEVEL_WARNING),
             capture_.level); // [確認_正常系] - 分類値 (WARNING) を強制出力のレベルへ変換した値が使われること。
    EXPECT_EQ('#', capture_.message[0]); // [確認_正常系] - 組み立てた文字列の先頭がラウンド トリップ ID の接頭辞 "#" であること。
}

// 出力先を設定していない場合に CPLAT_ERR_INVALID_ARGUMENT を返し、組み立ても出力も行わないことの確認
TEST_F(sampleFilterOutputTest, write_without_configuration_returns_invalid_argument)
{
    // Arrange
    int actual_ret;

    ASSERT_EQ(CPLAT_OK, sample_filter_output_configure(nullptr, nullptr, nullptr)); // [状態] - 出力の設定を解除する。

    // Pre-Assert

    // Act
    actual_ret = sample_filter_output_write(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED, (uint64_t)1, (int)2,
                                            SAMPLE_FILTER_TEST_CONTEXT_ARGS(7)); // [手順] - 未設定の状態で出力を試みる。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, actual_ret); // [確認_異常系] - CPLAT_ERR_INVALID_ARGUMENT を返すこと。
    EXPECT_EQ(0, capture_.call_count);                 // [確認_異常系] - フックが呼び出されない (組み立ても出力も行わない) こと。
}
