#include <testfw.h>

#include "sampleFilterTestSupport.h"

#include "gen/sample_worker_trace.h"
#include "sample_filter_share.h"
#include "sample_filter_share_region.h"

#include <cplat/base/result.h>
#include <cplat/sync/atomic.h>
#include <cplat/crt/path.h>
#include <cplat/runtime/process.h>
#include <cplat/sync/sync.h>
#include <cplat/trace/tracer.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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
        lock_path_ = path_ + ".lock";
        (void)std::remove(path_.c_str());
        (void)std::remove(lock_path_.c_str());

        ASSERT_EQ(SAMPLE_FILTER_SHARE_REGION_OK, sample_filter_share_lock_create(path_.c_str(), &lock_));
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
        (void)std::remove(lock_path_.c_str());
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
    std::string lock_path_;
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

// 既存の領域の大きさが一致しない場合は、作り直さずに開くことを拒否することの確認
TEST_F(sampleFilterShareTest, open_rejects_existing_region_of_different_size)
{
    // Arrange
    sample_filter_share *other = nullptr;

    // Pre-Assert

    // Act
    int actual_ret = sample_filter_share_open(path_.c_str(), lock_, kLineCapacity, kLineWidth + 8U,
                                              &other); // [手順] - 行幅を変えて同じ領域を開く。

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual_ret); // [確認_異常系] - 大きさの不一致を拒否すること。
    EXPECT_EQ(nullptr, other);                           // [確認_異常系] - ハンドルを返さないこと。
}

// 別に作成した排他 (別プロセスに見立てる) が保持している間は、待ち時間のうちに取得できないことの確認
TEST_F(sampleFilterShareTest, lock_held_by_another_handle_times_out)
{
    // Arrange
    sample_filter_share_lock *another = nullptr;
    ASSERT_EQ(SAMPLE_FILTER_SHARE_REGION_OK,
              sample_filter_share_lock_create(path_.c_str(), &another)); // [状態] - 同じパスで別の排他を作成する。
    ASSERT_EQ(
        SAMPLE_FILTER_SHARE_REGION_OK,
        sample_filter_share_lock_acquire(another, SAMPLE_FILTER_SHARE_WAIT_FOREVER)); // [状態] - 別の排他を保持する。

    // Pre-Assert

    // Act
    const sample_filter_share_region_result actual_while_held =
        sample_filter_share_lock_acquire(lock_, 0); // [手順] - 保持されている間に待たずに取得する。
    sample_filter_share_lock_release(another);      // [手順] - 別の排他を解放する。
    const sample_filter_share_region_result actual_after_release =
        sample_filter_share_lock_acquire(lock_, 0); // [手順] - 解放後に待たずに取得する。

    // Assert
    EXPECT_EQ(SAMPLE_FILTER_SHARE_REGION_TIMEOUT,
              actual_while_held); // [確認_異常系] - 保持されている間は取得できないこと。
    EXPECT_EQ(SAMPLE_FILTER_SHARE_REGION_OK, actual_after_release); // [確認_正常系] - 解放後は取得できること。

    sample_filter_share_lock_release(lock_);
    sample_filter_share_lock_dispose(&another);
}

// 別の排他が保持している間の取り込みは待ち時間の上限で諦め、解放後の出力で取り込むことの確認
TEST_F(sampleFilterShareTest, take_retries_after_reader_lock_timeout)
{
    // Arrange
    static unsigned char image[kImageSize];
    sample_filter_share_lock *another = nullptr;
    uint64_t timestamp = 0U;
    int actual_while_held = 1;
    int actual_after_release = 0;
    ASSERT_EQ(CPLAT_OK, compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED",
                                            image)); // [状態] - 条件をコンパイルする。
    ASSERT_EQ(CPLAT_OK, sample_filter_share_publish(writer_, image, kImageSize, &timestamp)); // [状態] - 公開する。
    ASSERT_EQ(SAMPLE_FILTER_SHARE_REGION_OK,
              sample_filter_share_lock_create(path_.c_str(), &another)); // [状態] - 同じパスで別の排他を作成する。
    ASSERT_EQ(
        SAMPLE_FILTER_SHARE_REGION_OK,
        sample_filter_share_lock_acquire(another, SAMPLE_FILTER_SHARE_WAIT_FOREVER)); // [状態] - 別の排他を保持する。

    // Pre-Assert

    // Act
    (void)format_job_failed(&actual_while_held); // [手順] - 保持されている間に組み立てる。
    const cplat_string_catalog_filter_source_status status_while_held = [this]()
    {
        cplat_string_catalog_filter_source_status status;
        (void)cplat_string_catalog_filter_slot_get_source_status(slot_, &status);
        return status;
    }();
    sample_filter_share_lock_release(another);      // [手順] - 別の排他を解放する。
    (void)format_job_failed(&actual_after_release); // [手順] - 解放後に組み立てる。

    // Assert
    EXPECT_EQ(0, actual_while_held);                  // [確認_異常系] - 取り込まず、以前の条件で判定すること。
    EXPECT_EQ(0U, status_while_held.taken_timestamp); // [確認_異常系] - 取り込み済みとしないこと。
    EXPECT_EQ(CPLAT_ERR_TIMEOUT, status_while_held.last_result); // [確認_異常系] - 待ち時間の超過を記録すること。
    EXPECT_NE(0, actual_after_release);                          // [確認_正常系] - 解放後の出力で取り込むこと。
    EXPECT_EQ(timestamp, taken_timestamp());                     // [確認_正常系] - 公開時刻を取り込み済みとすること。

    sample_filter_share_lock_dispose(&another);
}

namespace
{
/**
 *  @brief          ソース領域のヘッダーにおける公開時刻のオフセットです。
 *
 *  cplat のソース領域の配置 (署名と形式版、行数の上限と行幅、フィルター オブジェクトのバイト数に続く 0x18)
 *  に合わせます。公開時刻を奇数にして、書き込みの途中で止まった状態を作るために使います。
 */
constexpr std::size_t kPublishedTimestampOffset = 0x18U;

/**
 *  @brief          別のプロセスの書き込み側として、書き込みの途中で異常終了します。
 *
 *  同じパスの排他を新しく作成して取得し、公開時刻を奇数にして、フィルター オブジェクトの一部を書き換えた
 *  ところで、排他を解放せずに異常終了します。cplat の公開が書き込みの途中で止まった状態と同じです。
 */
void crash_while_writing(const char *path, void *source)
{
    sample_filter_share_lock *lock = nullptr;
    cplat_atomic_u64 *timestamp =
        reinterpret_cast<cplat_atomic_u64 *>(static_cast<unsigned char *>(source) + kPublishedTimestampOffset);

    if ((sample_filter_share_lock_create(path, &lock) != SAMPLE_FILTER_SHARE_REGION_OK) ||
        (sample_filter_share_lock_acquire(lock, SAMPLE_FILTER_SHARE_WAIT_FOREVER) != SAMPLE_FILTER_SHARE_REGION_OK))
    {
        std::_Exit(2);
    }
    cplat_atomic_store_u64(timestamp, cplat_atomic_load_u64(timestamp, CPLAT_MEMORY_ORDER_RELAXED) | 1U,
                           CPLAT_MEMORY_ORDER_RELEASE);
    std::memset(static_cast<unsigned char *>(source) + CPLAT_STRING_CATALOG_FILTER_SOURCE_HEADER_SIZE, 0xAA, 64U);
    std::abort();
}
} // namespace

// 別のプロセスの書き込み側が書き込みの途中で異常終了しても、取り込み済みの条件を保ち、
// OS が排他を解放し、次の公開で回復することの確認
TEST_F(sampleFilterShareTest, writer_crash_while_writing_keeps_conditions_and_recovers)
{
#if defined(_WIN32)
    GTEST_SKIP() << "子プロセスを fork で起動するデス テストを使うため、Linux 環境専用のテストです";
#else
    // Arrange
    static unsigned char image_failed[kImageSize];
    static unsigned char image_started[kImageSize];
    uint64_t first_timestamp = 0U;
    uint64_t recovered_timestamp = 0U;
    int actual_before_crash = 0;
    int actual_after_crash = 0;
    int actual_after_recovery = 1;
    void *source = const_cast<void *>(source_);

    ASSERT_EQ(CPLAT_OK, compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED",
                                            image_failed)); // [状態] - 1 つ目の条件をコンパイルする。
    ASSERT_EQ(CPLAT_OK, compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_WORKER_STARTED",
                                            image_started)); // [状態] - 2 つ目の条件をコンパイルする。
    ASSERT_EQ(CPLAT_OK, sample_filter_share_publish(writer_, image_failed, kImageSize,
                                                    &first_timestamp)); // [状態] - 1 つ目の条件を公開する。
    (void)format_job_failed(&actual_before_crash);                      // [状態] - 1 つ目の条件を取り込む。
    ASSERT_NE(0, actual_before_crash);                                  // [状態確認] - 1 つ目の条件で一致すること。
    GTEST_FLAG_SET(death_test_style, "fast");

    // Pre-Assert

    // Act
    EXPECT_DEATH(crash_while_writing(path_.c_str(), source),
                 ""); // [手順] - 別のプロセスの書き込み側を、書き込みの途中で異常終了させる。
    cplat_string_catalog_filter_source_info info_after_crash;
    const int actual_info_after_crash = cplat_string_catalog_filter_source_get_info(
        source_, source_size_, &info_after_crash); // [手順] - 異常終了の後の領域の状態を読む。
    (void)format_job_failed(&actual_after_crash);  // [手順] - 異常終了の後に組み立てる。
    const uint64_t taken_after_crash = taken_timestamp();
    const sample_filter_share_region_result actual_lock_after_crash =
        sample_filter_share_lock_acquire(lock_, 0); // [手順] - 異常終了したプロセスの排他が解放されたかを確かめる。
    if (actual_lock_after_crash == SAMPLE_FILTER_SHARE_REGION_OK)
    {
        sample_filter_share_lock_release(lock_);
    }
    ASSERT_EQ(CPLAT_OK, sample_filter_share_publish(writer_, image_started, kImageSize,
                                                    &recovered_timestamp)); // [手順] - 2 つ目の条件を公開し直す。
    (void)format_job_failed(&actual_after_recovery);                        // [手順] - 公開し直した後に組み立てる。

    // Assert
    EXPECT_EQ(CPLAT_ERR_BUSY, actual_info_after_crash); // [確認_異常系] - 領域が書き込み中のまま残っていること。
    EXPECT_NE(0, actual_after_crash);              // [確認_異常系] - 異常終了の後も取り込み済みの条件で判定すること。
    EXPECT_EQ(first_timestamp, taken_after_crash); // [確認_異常系] - 書き込み途中の内容を取り込まないこと。
    EXPECT_EQ(SAMPLE_FILTER_SHARE_REGION_OK, actual_lock_after_crash); // [確認_異常系] - OS が排他を解放していること。
    EXPECT_GT(recovered_timestamp, first_timestamp);   // [確認_正常系] - 公開し直した公開時刻が増加すること。
    EXPECT_EQ(0, actual_after_recovery);               // [確認_正常系] - 公開し直した条件で判定すること。
    EXPECT_EQ(recovered_timestamp, taken_timestamp()); // [確認_正常系] - 公開し直した内容を取り込むこと。
#endif
}
