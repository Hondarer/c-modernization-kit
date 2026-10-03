#include <testfw.h>

#include "sampleFilterTestSupport.h"

#include "gen/sample_worker_trace.h"
#include "sample_filter_share.h"
#include "sample_filter_share_region.h"

#include <cplat/base/result.h>
#include <cplat/crt/path.h>
#include <cplat/runtime/process.h>
#include <cplat/sync/sync.h>
#include <cplat/trace/tracer.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

using namespace sample_filter_test;

class sampleFilterShareTest : public Test
{
  protected:
    void SetUp() override
    {
        char temp_dir[PLATFORM_PATH_MAX];
        const TestInfo *info = UnitTest::GetInstance()->current_test_info();

        ASSERT_EQ(CPLAT_OK, cplat_path_get_temp_dir(temp_dir, sizeof(temp_dir), nullptr));
        path_ = std::string(temp_dir) + "/sampleFilterShareTest_" + std::to_string(cplat_process_get_pid()) + "_" +
                info->name() + ".share";
        (void)std::remove(path_.c_str());

        ASSERT_EQ(CPLAT_OK, sample_filter_share_lock_create(path_.c_str(), &lock_));
        ASSERT_EQ(CPLAT_OK, sample_filter_share_open(path_.c_str(), lock_, kLineCapacity, kLineWidth, &writer_));
        ASSERT_EQ(CPLAT_OK, sample_filter_share_open(path_.c_str(), lock_, kLineCapacity, kLineWidth, &reader_));
        ASSERT_EQ(CPLAT_OK, sample_worker_trace_create_filter(nullptr, kLineCapacity, kLineWidth, &slot_));

        /* 読み取り側の共有メモリを、書き込み側の排他とともにスロットへ結び付け、別プロセスの読み取り側に見立てる */
        source_ = sample_filter_share_get_source(reader_, &source_size_);
        ASSERT_NE(nullptr, source_);
        ASSERT_EQ(CPLAT_OK, sample_filter_share_get_source_lock(reader_, &source_lock_));
        ASSERT_EQ(CPLAT_OK,
                  cplat_string_catalog_filter_slot_attach_source(slot_, source_, source_size_, &source_lock_));
    }

    void TearDown() override
    {
        (void)cplat_string_catalog_filter_slot_attach_source(slot_, nullptr, 0U, nullptr);
        cplat_string_catalog_filter_slot_dispose(&slot_);
        sample_filter_share_close(&reader_);
        sample_filter_share_close(&writer_);
        sample_filter_share_lock_dispose(&lock_);
        (void)std::remove(path_.c_str());
    }

    cplat_string_catalog_filter_state state_of(const int string_key)
    {
        cplat_string_catalog_filter_state state = CPLAT_STRING_CATALOG_FILTER_STATE_NEVER_MATCH;

        (void)cplat_string_catalog_filter_slot_test(slot_, string_key, &state);
        return state;
    }

    /** 判定付きで JOB_FAILED を組み立てます。トレース出力と同じく、組み立ての前に公開内容を取り込みます。 */
    int format_job_failed(int *matched_out)
    {
        char dest[CPLAT_STRING_CATALOG_TEXT_MAX];

        return cplat_string_catalog_filter_slot_format(slot_, dest, sizeof(dest), matched_out,
                                                       SAMPLE_WORKER_TRACE_KEY_JOB_FAILED, (uint64_t)1, (int)5,
                                                       SAMPLE_FILTER_TEST_CONTEXT_ARGS(1));
    }

    uint64_t published_timestamp()
    {
        cplat_string_catalog_filter_source_info info;

        if (cplat_string_catalog_filter_source_get_info(source_, source_size_, &info) != CPLAT_OK)
        {
            return UINT64_MAX;
        }
        return info.published_timestamp;
    }

    uint64_t taken_timestamp()
    {
        cplat_string_catalog_filter_source_status status;

        if (cplat_string_catalog_filter_slot_get_source_status(slot_, &status) != CPLAT_OK)
        {
            return UINT64_MAX;
        }
        return status.taken_timestamp;
    }

    std::string path_;
    sample_filter_share_lock *lock_ = nullptr;
    sample_filter_share *writer_ = nullptr;
    sample_filter_share *reader_ = nullptr;
    cplat_string_catalog_filter_slot *slot_ = nullptr;
    const void *source_ = nullptr;
    size_t source_size_ = 0U;
    cplat_string_catalog_filter_source_lock source_lock_ = {nullptr, nullptr, nullptr};
};

// 未公開の共有メモリからは何も取り込まないことの確認
TEST_F(sampleFilterShareTest, nothing_is_taken_before_first_publish)
{
    // Arrange
    int actual_matched = 1;

    // Pre-Assert

    // Act
    int actual_ret = format_job_failed(&actual_matched); // [手順] - 未公開のまま組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);      // [確認_正常系] - 組み立てが成功すること。
    EXPECT_EQ(0, actual_matched);         // [確認_正常系] - 何も取り込まず、条件に一致しないこと。
    EXPECT_EQ(0U, published_timestamp()); // [確認_正常系] - 共有メモリが未公開であること。
    EXPECT_EQ(0U, taken_timestamp());     // [確認_正常系] - 取り込み済みの公開時刻が 0 であること。
}

// 書き込み側のハンドルが公開した内容を、読み取り側の共有メモリを結び付けたスロットが次の組み立てで反映することの確認
TEST_F(sampleFilterShareTest, published_image_is_taken_on_next_format)
{
    // Arrange
    static unsigned char image[kImageSize];
    uint64_t timestamp = 0U;
    int actual_matched = 0;
    ASSERT_EQ(CPLAT_OK, compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED",
                                            image)); // [状態] - JOB_FAILED に一致する条件式をコンパイルする。

    // Pre-Assert

    // Act
    int actual_publish_ret = sample_filter_share_publish(writer_, image, kImageSize, &timestamp); // [手順] - 公開する。
    cplat_string_catalog_filter_state actual_before = state_of(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED);
    (void)format_job_failed(&actual_matched); // [手順] - 組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_publish_ret);     // [確認_正常系] - 公開が成功すること。
    EXPECT_EQ(timestamp, published_timestamp()); // [確認_正常系] - 公開時刻を読み取り側から読めること。
    EXPECT_EQ(CPLAT_STRING_CATALOG_FILTER_STATE_NEVER_MATCH,
              actual_before);                // [確認_正常系] - 公開だけでは取り込まないこと。
    EXPECT_NE(0, actual_matched);            // [確認_正常系] - 組み立てで取り込んだ条件に一致すること。
    EXPECT_EQ(timestamp, taken_timestamp()); // [確認_正常系] - 公開時刻を取り込み済みとすること。
}

// 書き込み側のハンドルをまたいでも、公開時刻が増加することの確認
TEST_F(sampleFilterShareTest, timestamp_advances_across_writers)
{
    // Arrange
    static unsigned char image[kImageSize];
    sample_filter_share *another_writer = nullptr;
    uint64_t first = 0U;
    uint64_t second = 0U;
    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。
    ASSERT_EQ(CPLAT_OK, sample_filter_share_open(path_.c_str(), lock_, kLineCapacity, kLineWidth,
                                                 &another_writer)); // [状態] - 別の書き込み側を開く。

    // Pre-Assert

    // Act
    int actual_first_ret = sample_filter_share_publish(writer_, image, kImageSize, &first); // [手順] - 公開する。
    int actual_second_ret = sample_filter_share_publish(another_writer, image, kImageSize,
                                                        &second); // [手順] - 別の書き込み側から公開する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_first_ret);    // [確認_正常系] - 1 回目の公開が成功すること。
    EXPECT_EQ(CPLAT_OK, actual_second_ret);   // [確認_正常系] - 2 回目の公開が成功すること。
    EXPECT_GT(second, first);                 // [確認_正常系] - 公開時刻が増加すること。
    EXPECT_EQ(second, published_timestamp()); // [確認_正常系] - 最後の公開時刻が共有メモリに残ること。

    sample_filter_share_close(&another_writer);
}

// 検証に失敗するイメージは公開せず、共有メモリを変えないことの確認
TEST_F(sampleFilterShareTest, invalid_image_is_not_published)
{
    // Arrange
    static unsigned char image[kImageSize];
    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。
    filter_test_record_address(image, kLineWidth, 0U)[0] ^= 0xFFU;    // [状態] - 行レコードを壊す。

    // Pre-Assert

    // Act
    int actual_ret =
        sample_filter_share_publish(writer_, image, kImageSize, nullptr); // [手順] - 壊れたイメージを公開する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual_ret); // [確認_異常系] - 公開が拒否されること。
    EXPECT_EQ(0U, published_timestamp());                // [確認_異常系] - 共有メモリが未公開のままであること。
}

// 行数の上限と行幅が異なるイメージの公開を拒否することの確認
TEST_F(sampleFilterShareTest, publish_with_different_geometry_is_rejected)
{
    // Arrange
    static unsigned char narrow_image[CPLAT_STRING_CATALOG_FILTER_IMAGE_SIZE(kLineCapacity, 64U)];
    char line[64] = "category <= 2";
    size_t invalid_count = 0U;
    ASSERT_EQ(CPLAT_OK, cplat_string_catalog_filter_compile(
                            line, 1U, sizeof(line), kLineCapacity, narrow_image, sizeof(narrow_image), nullptr, 0U,
                            &invalid_count)); // [状態] - 行幅の異なるイメージをコンパイルする。

    // Pre-Assert

    // Act
    int actual_ret = sample_filter_share_publish(writer_, narrow_image, sizeof(narrow_image),
                                                 nullptr); // [手順] - 行幅の異なるイメージを公開する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual_ret); // [確認_異常系] - 形式の異なる公開が拒否されること。
    EXPECT_EQ(0U, published_timestamp());                // [確認_異常系] - 共有メモリが未公開のままであること。
}

// 生成物のトレース出力が、出力の前に公開内容を取り込み、一致したトレースを強制出力にすることの確認
TEST_F(sampleFilterShareTest, trace_output_takes_published_image_before_output)
{
    // Arrange
    static unsigned char image[kImageSize];
    cplat_tracer *tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    uint64_t timestamp = 0U;

    ASSERT_NE(nullptr, tracer);                                       // [状態確認] - トレーサーを作成できること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_start(tracer));                  // [状態確認] - トレーサーを開始できること。
    sample_worker_trace_set_tracer(tracer);                           // [状態] - 出力先を設定する。
    ASSERT_EQ(CPLAT_OK, sample_worker_trace_set_filter(slot_));       // [状態] - スロットを出力へ接続する。
    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。
    ASSERT_EQ(CPLAT_OK, sample_filter_share_publish(writer_, image, kImageSize, &timestamp)); // [状態] - 公開する。

    // Pre-Assert

    // Act
    int actual_ret = sample_worker_trace_key_job_failed((uint64_t)1, 5); // [手順] - トレースを出力する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);         // [確認_正常系] - 出力が成功すること。
    EXPECT_EQ(timestamp, taken_timestamp()); // [確認_正常系] - 出力の前に公開内容を取り込むこと。
    EXPECT_EQ(CPLAT_STRING_CATALOG_FILTER_STATE_ALWAYS_MATCH,
              state_of(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED)); // [確認_正常系] - 取り込んだ条件が判定に使われること。

    // Cleanup
    (void)sample_worker_trace_set_filter(nullptr);
    sample_worker_trace_set_tracer(nullptr);
    (void)cplat_tracer_stop(tracer);
    cplat_tracer_dispose(&tracer);
}

namespace
{
/** 判定付きの組み立てを繰り返すスレッドへ渡す引数です。 */
struct format_args
{
    cplat_string_catalog_filter_slot *slot;
    std::atomic<int> *stop_flag;
    int is_ok;
    int pad; /**< 明示的アラインメントです。 */
};

void format_worker(void *raw_arg)
{
    format_args *args = static_cast<format_args *>(raw_arg);
    char dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    int matched;

    args->is_ok = 1;
    while (args->stop_flag->load() == 0)
    {
        if (cplat_string_catalog_filter_slot_format(args->slot, dest, sizeof(dest), &matched,
                                                    SAMPLE_WORKER_TRACE_KEY_JOB_FAILED, (uint64_t)1, (int)5,
                                                    SAMPLE_FILTER_TEST_CONTEXT_ARGS(1)) != CPLAT_OK)
        {
            args->is_ok = 0;
        }
    }
}
} // namespace

// 公開を繰り返す間に複数スレッドが取り込みと判定を行っても、失敗せず最後の公開内容に収束することの確認
TEST_F(sampleFilterShareTest, concurrent_publish_and_format_converge)
{
    // Arrange
    static unsigned char image_failed[kImageSize];
    static unsigned char image_started[kImageSize];
    std::atomic<int> stop_flag(0);
    format_args args[4];
    cplat_thread *threads[4];
    cplat_string_catalog_filter_source_status status;
    uint64_t last_timestamp = 0U;
    int matched = 0;

    ASSERT_EQ(CPLAT_OK, compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED",
                                            image_failed)); // [状態] - 1 つ目の条件をコンパイルする。
    ASSERT_EQ(CPLAT_OK, compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_WORKER_STARTED",
                                            image_started)); // [状態] - 2 つ目の条件をコンパイルする。
    for (int index = 0; index < 4; index++)
    {
        args[index].slot = slot_;
        args[index].stop_flag = &stop_flag;
        args[index].is_ok = 0;
        args[index].pad = 0;
        ASSERT_EQ(CPLAT_OK, cplat_thread_create(&threads[index], format_worker,
                                                &args[index])); // [状態] - 組み立てを繰り返すスレッドを起動する。
    }

    // Pre-Assert

    // Act
    for (int iteration = 0; iteration < 200; iteration++)
    {
        const unsigned char *image = image_failed;

        if ((iteration % 2) == 1)
        {
            image = image_started;
        }
        ASSERT_EQ(CPLAT_OK, sample_filter_share_publish(writer_, image, kImageSize,
                                                        &last_timestamp)); // [手順] - 2 種類の条件を交互に公開する。
    }
    stop_flag = 1; // [手順] - スレッドへ終了を通知する。
    for (int index = 0; index < 4; index++)
    {
        (void)cplat_thread_join(threads[index], CPLAT_SYNC_WAIT_FOREVER);
    }
    int actual_final_ret = format_job_failed(&matched); // [手順] - 最後にもう一度組み立てる。
    (void)cplat_string_catalog_filter_slot_get_source_status(slot_, &status);

    // Assert
    for (int index = 0; index < 4; index++)
    {
        EXPECT_EQ(1, args[index].is_ok); // [確認_正常系] - 各スレッドの組み立てが常に成功すること。
    }
    EXPECT_EQ(CPLAT_OK, actual_final_ret);             // [確認_正常系] - 最後の組み立てが成功すること。
    EXPECT_EQ(last_timestamp, status.taken_timestamp); // [確認_正常系] - 最後の公開内容を取り込んでいること。
    EXPECT_EQ(CPLAT_OK, status.last_result);           // [確認_正常系] - 最後の取り込みが成功していること。
    EXPECT_EQ(
        CPLAT_STRING_CATALOG_FILTER_STATE_ALWAYS_MATCH,
        state_of(SAMPLE_WORKER_TRACE_KEY_WORKER_STARTED)); // [確認_正常系] - 最後に公開した条件が判定に使われること。
}
