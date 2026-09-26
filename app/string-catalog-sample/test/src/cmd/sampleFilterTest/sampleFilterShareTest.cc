#include <testfw.h>

#include "sampleFilterTestSupport.h"

#include "gen/sample_worker_trace.h"
#include "sample_filter_output.h"
#include "sample_filter_share.h"
#include "sample_worker_trace_key_names.h"

#include <cplat/base/result.h>
#include <cplat/crt/path.h>
#include <cplat/mmap/mmap.h>
#include <cplat/runtime/process.h>
#include <cplat/sync/sync.h>

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

        ASSERT_EQ(CPLAT_OK, cplat_get_temp_dir(temp_dir, sizeof(temp_dir), nullptr));
        path_ = std::string(temp_dir) + "/sampleFilterShareTest_" + std::to_string(cplat_process_get_pid()) + "_" +
                info->name() + ".share";
        (void)std::remove(path_.c_str());

        ASSERT_EQ(CPLAT_OK, cplat_local_lock_create(&lock_));
        ASSERT_EQ(CPLAT_OK, sample_filter_share_open(path_.c_str(), lock_, kLineCapacity, kLineWidth, &writer_));
        ASSERT_EQ(CPLAT_OK, sample_filter_share_open(path_.c_str(), lock_, kLineCapacity, kLineWidth, &reader_));
        ASSERT_EQ(CPLAT_OK,
                  sample_filter_slot_create(sample_worker_trace_catalog(), sample_worker_trace_key_names(),
                                            sample_worker_trace_key_name_count(), kLineCapacity, kLineWidth, &slot_));
    }

    void TearDown() override
    {
        sample_filter_slot_dispose(&slot_);
        sample_filter_share_close(&reader_);
        sample_filter_share_close(&writer_);
        if (map_ != nullptr)
        {
            (void)cplat_mmap_detach(map_, nullptr);
            map_ = nullptr;
        }
        cplat_local_lock_dispose(lock_);
        (void)std::remove(path_.c_str());
    }

    /** 共有メモリの配布ヘッダーを、テストから直接読み書きするために対応付けます。 */
    sample_filter_share_header *header()
    {
        if (map_ == nullptr)
        {
            if (cplat_mmap_attach(path_.c_str(), CPLAT_MMAP_ACCESS_READ_WRITE, 1U, &map_, nullptr) != CPLAT_OK)
            {
                return nullptr;
            }
        }
        return static_cast<sample_filter_share_header *>(cplat_mmap_get_address(map_));
    }

    sample_filter_state state_of(const int string_key)
    {
        sample_filter_state state = SAMPLE_FILTER_STATE_NEVER_MATCH;

        (void)sample_filter_slot_test(slot_, string_key, &state);
        return state;
    }

    std::string path_;
    cplat_local_lock *lock_ = nullptr;
    sample_filter_share *writer_ = nullptr;
    sample_filter_share *reader_ = nullptr;
    sample_filter_slot *slot_ = nullptr;
    cplat_mmap *map_ = nullptr;
};

// 未公開の共有メモリからは何も取り込まないことの確認
TEST_F(sampleFilterShareTest, nothing_is_taken_before_first_publish)
{
    // Arrange
    sample_filter_share_status status;
    int is_taken = -1;

    // Pre-Assert

    // Act
    int actual_ret =
        sample_filter_share_refresh(reader_, slot_, &is_taken); // [手順] - 未公開の状態で取り込みを確かめる。
    int actual_status_ret = sample_filter_share_get_status(reader_, &status); // [手順] - 状態を取得する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);        // [確認_正常系] - 取り込みが成功すること。
    EXPECT_EQ(0, is_taken);                 // [確認_正常系] - 何も取り込まないこと。
    EXPECT_EQ(CPLAT_OK, actual_status_ret); // [確認_正常系] - 状態を取得できること。
    EXPECT_EQ((uint64_t)SAMPLE_FILTER_SHARE_GENERATION_NONE,
              status.published_generation); // [確認_正常系] - 未公開であること。
    EXPECT_EQ((uint64_t)SAMPLE_FILTER_SHARE_GENERATION_NONE,
              status.taken_generation); // [確認_正常系] - 未取り込みであること。
}

// 公開した内容を、別のハンドル (別プロセスに見立てた読み取り側) が次の取り込みで反映することの確認
TEST_F(sampleFilterShareTest, published_image_is_taken_on_next_refresh)
{
    // Arrange
    static unsigned char image[kImageSize];
    uint64_t generation = 0U;
    int is_taken_first = -1;
    int is_taken_second = -1;

    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。

    // Pre-Assert

    // Act
    int actual_publish_ret =
        sample_filter_share_publish(writer_, image, kImageSize, &generation); // [手順] - 公開する。
    sample_filter_state actual_state_before =
        state_of(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED); // [手順] - 取り込み前の状態を取得する。
    int actual_first_ret = sample_filter_share_refresh(reader_, slot_, &is_taken_first); // [手順] - 取り込む。
    sample_filter_state actual_state_after =
        state_of(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED); // [手順] - 取り込み後の状態を取得する。
    int actual_second_ret =
        sample_filter_share_refresh(reader_, slot_, &is_taken_second); // [手順] - 変化のない状態で再び確かめる。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_publish_ret); // [確認_正常系] - 公開が成功すること。
    EXPECT_EQ(1U, generation);               // [確認_正常系] - 最初の公開は世代 1 であること。
    EXPECT_EQ(SAMPLE_FILTER_STATE_NEVER_MATCH,
              actual_state_before);        // [確認_正常系] - 公開だけでは読み取り側が変わらないこと。
    EXPECT_EQ(CPLAT_OK, actual_first_ret); // [確認_正常系] - 取り込みが成功すること。
    EXPECT_EQ(1, is_taken_first);          // [確認_正常系] - 取り込んだことが報告されること。
    EXPECT_EQ(SAMPLE_FILTER_STATE_ALWAYS_MATCH, actual_state_after); // [確認_正常系] - 公開内容が判定に反映されること。
    EXPECT_EQ(CPLAT_OK, actual_second_ret);                          // [確認_正常系] - 再確認が成功すること。
    EXPECT_EQ(0, is_taken_second);                                   // [確認_正常系] - 変化がなければ取り込まないこと。
}

// 公開のたびに世代が進み、書き込み側のハンドルをまたいで続き番号になることの確認
TEST_F(sampleFilterShareTest, generation_advances_across_writers)
{
    // Arrange
    static unsigned char image[kImageSize];
    uint64_t first = 0U;
    uint64_t second = 0U;

    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。

    // Pre-Assert

    // Act
    int actual_first_ret =
        sample_filter_share_publish(writer_, image, kImageSize, &first); // [手順] - 書き込み側で公開する。
    int actual_second_ret =
        sample_filter_share_publish(reader_, image, kImageSize, &second); // [手順] - 別のハンドルから公開する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_first_ret);  // [確認_正常系] - 1 回目の公開が成功すること。
    EXPECT_EQ(CPLAT_OK, actual_second_ret); // [確認_正常系] - 2 回目の公開が成功すること。
    EXPECT_EQ(first + 1U, second);          // [確認_正常系] - 世代が続き番号になること。
}

// 検証に失敗するイメージは公開せず、共有メモリを変えないことの確認
TEST_F(sampleFilterShareTest, invalid_image_is_not_published)
{
    // Arrange
    static unsigned char image[kImageSize];
    sample_filter_share_status status;

    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。
    image[kImageSize - 1U] ^= 0x01U;                                  // [状態] - 行レコードの領域を 1 バイト改変する。
    image[0] ^= 0x01U;                                                // [状態] - 署名を壊す。

    // Pre-Assert

    // Act
    int actual_ret =
        sample_filter_share_publish(writer_, image, kImageSize, nullptr); // [手順] - 壊れたイメージを公開する。
    (void)sample_filter_share_get_status(reader_, &status);

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual_ret); // [確認_異常系] - 公開が拒否されること。
    EXPECT_EQ((uint64_t)SAMPLE_FILTER_SHARE_GENERATION_NONE,
              status.published_generation); // [確認_異常系] - 世代が進まないこと。
}

// 行数の上限と行幅が異なるハンドルからの公開を拒否することの確認
TEST_F(sampleFilterShareTest, publish_with_different_geometry_is_rejected)
{
    // Arrange
    static unsigned char image[kImageSize];
    static unsigned char narrow_image[SAMPLE_FILTER_IMAGE_SIZE(kLineCapacity, 64U)];
    const char line[] = "category <= 2";
    sample_filter_share *narrow = nullptr;

    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。
    ASSERT_EQ(CPLAT_OK,
              sample_filter_share_publish(writer_, image, kImageSize, nullptr)); // [状態] - 配布ヘッダーを初期化する。
    ASSERT_EQ(CPLAT_OK, sample_filter_share_open(path_.c_str(), lock_, kLineCapacity, 64U,
                                                 &narrow)); // [状態] - 行幅の異なるハンドルを開く。
    ASSERT_EQ(CPLAT_OK, sample_filter_compile(line, 1U, sizeof(line), kLineCapacity, narrow_image, sizeof(narrow_image),
                                              nullptr, 0U, nullptr)); // [状態] - 行幅の異なるイメージを作る。

    // Pre-Assert

    // Act
    int actual_ret =
        sample_filter_share_publish(narrow, narrow_image, sizeof(narrow_image), nullptr); // [手順] - 公開する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual_ret); // [確認_異常系] - 形式の異なる公開が拒否されること。

    // Cleanup
    sample_filter_share_close(&narrow);
}

// 世代が一巡しても変化を検知し、未公開を表す 0 を飛ばすことの確認
TEST_F(sampleFilterShareTest, generation_wraps_around_skipping_zero)
{
    // Arrange
    static unsigned char image[kImageSize];
    uint64_t generation = 0U;
    int is_taken_max = -1;
    int is_taken_wrapped = -1;
    sample_filter_share_status status;

    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。
    ASSERT_EQ(CPLAT_OK, sample_filter_share_publish(writer_, image, kImageSize, nullptr)); // [状態] - 公開する。
    ASSERT_NE(nullptr, header());      // [状態確認] - 配布ヘッダーを対応付けられること。
    cplat_atomic_store_u64(&header()->generation, UINT64_MAX,
                           CPLAT_MEMORY_ORDER_RELAXED); // [状態] - 世代を上限の値へ書き換える。

    // Pre-Assert

    // Act
    int actual_max_ret = sample_filter_share_refresh(reader_, slot_, &is_taken_max); // [手順] - 上限の世代を取り込む。
    int actual_publish_ret =
        sample_filter_share_publish(writer_, image, kImageSize, &generation); // [手順] - 一巡する公開を行う。
    int actual_wrapped_ret =
        sample_filter_share_refresh(reader_, slot_, &is_taken_wrapped); // [手順] - 一巡した世代を取り込む。
    (void)sample_filter_share_get_status(reader_, &status);

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_max_ret);     // [確認_正常系] - 上限の世代を取り込めること。
    EXPECT_EQ(1, is_taken_max);              // [確認_正常系] - 上限の世代への変化を検知すること。
    EXPECT_EQ(CPLAT_OK, actual_publish_ret); // [確認_正常系] - 一巡する公開が成功すること。
    EXPECT_EQ(1U, generation);               // [確認_正常系] - 0 を飛ばして世代 1 になること。
    EXPECT_EQ(CPLAT_OK, actual_wrapped_ret); // [確認_正常系] - 一巡した世代を取り込めること。
    EXPECT_EQ(1, is_taken_wrapped);          // [確認_正常系] - 大小ではなく不一致で変化を検知すること。
    EXPECT_EQ(1U, status.taken_generation);  // [確認_正常系] - 取り込み済みの世代が 1 であること。
}

// 共有メモリ上で壊れた内容は適用せず、同じ世代の取り込みを繰り返さないことの確認
TEST_F(sampleFilterShareTest, corrupted_shared_image_is_not_retried_for_same_generation)
{
    // Arrange
    static unsigned char image[kImageSize];
    int is_taken_first = -1;
    int is_taken_second = -1;
    sample_filter_share_status status;

    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。
    ASSERT_EQ(CPLAT_OK, sample_filter_share_publish(writer_, image, kImageSize, nullptr)); // [状態] - 公開する。
    ASSERT_NE(nullptr, header()); // [状態確認] - 配布ヘッダーを対応付けられること。
    reinterpret_cast<unsigned char *>(header())[SAMPLE_FILTER_SHARE_HEADER_SIZE + SAMPLE_FILTER_HEADER_SIZE] ^=
        0x01U; // [状態] - 共有メモリ上の行レコードを 1 バイト改変する。

    // Pre-Assert

    // Act
    int actual_first_ret = sample_filter_share_refresh(reader_, slot_, &is_taken_first);   // [手順] - 取り込む。
    int actual_second_ret = sample_filter_share_refresh(reader_, slot_, &is_taken_second); // [手順] - 再び確かめる。
    (void)sample_filter_share_get_status(reader_, &status);

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual_first_ret); // [確認_異常系] - 壊れた内容を適用しないこと。
    EXPECT_EQ(1, is_taken_first);                              // [確認_異常系] - 取り込みを試みたことが報告されること。
    EXPECT_EQ(CPLAT_OK, actual_second_ret);                    // [確認_異常系] - 再確認は何もせず成功すること。
    EXPECT_EQ(0, is_taken_second);                             // [確認_異常系] - 同じ世代を繰り返し取り込まないこと。
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, status.last_take_result); // [確認_異常系] - 状態に適用の失敗が残ること。
    EXPECT_EQ(SAMPLE_FILTER_STATE_NEVER_MATCH,
              state_of(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED)); // [確認_異常系] - スロットは以前の条件のままであること。
}

// トレース出力の入口が、出力の前に公開内容を取り込むことの確認
TEST_F(sampleFilterShareTest, trace_output_takes_published_image_before_output)
{
    // Arrange
    static unsigned char image[kImageSize];
    cplat_tracer *tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    sample_filter_share_status status;

    ASSERT_NE(nullptr, tracer);                      // [状態確認] - トレーサーを作成できること。
    ASSERT_EQ(CPLAT_OK, cplat_tracer_start(tracer)); // [状態確認] - トレーサーを開始できること。
    ASSERT_EQ(CPLAT_OK, sample_filter_output_configure(sample_worker_trace_catalog(), slot_,
                                                       tracer));      // [状態] - 出力を設定する。
    sample_filter_output_set_share(reader_);                          // [状態] - 取り込みに使う配布ハンドルを設定する。
    ASSERT_EQ(CPLAT_OK, compile_single_line("category <= 2", image)); // [状態] - 条件式をコンパイルする。
    ASSERT_EQ(CPLAT_OK, sample_filter_share_publish(writer_, image, kImageSize, nullptr)); // [状態] - 公開する。

    // Pre-Assert

    // Act
    int actual_ret = sample_filter_output_write(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED, (uint64_t)1, (int)5,
                                                SAMPLE_FILTER_TEST_CONTEXT_ARGS(1)); // [手順] - トレースを出力する。
    (void)sample_filter_share_get_status(reader_, &status);

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);        // [確認_正常系] - 出力が成功すること。
    EXPECT_EQ(1U, status.taken_generation); // [確認_正常系] - 出力の前に公開内容を取り込むこと。
    EXPECT_EQ(SAMPLE_FILTER_STATE_ALWAYS_MATCH,
              state_of(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED)); // [確認_正常系] - 取り込んだ条件が判定に使われること。

    // Cleanup
    sample_filter_output_set_share(nullptr);
    (void)sample_filter_output_configure(nullptr, nullptr, nullptr);
    (void)cplat_tracer_stop(tracer);
    cplat_tracer_dispose(&tracer);
}

namespace
{
/** 取り込みを繰り返すスレッドへ渡す引数です。 */
struct refresh_args
{
    sample_filter_share *share;
    sample_filter_slot *slot;
    std::atomic<int> *stop_flag;
    int is_ok;
    int pad; /**< 明示的アラインメントです。 */
};

void refresh_worker(void *raw_arg)
{
    refresh_args *args = static_cast<refresh_args *>(raw_arg);
    char dest[CPLAT_STRING_CATALOG_TEXT_MAX];
    int matched;

    args->is_ok = 1;
    while (args->stop_flag->load() == 0)
    {
        if ((sample_filter_share_refresh(args->share, args->slot, nullptr) != CPLAT_OK) ||
            (sample_filter_slot_format(args->slot, dest, sizeof(dest), &matched, SAMPLE_WORKER_TRACE_KEY_JOB_FAILED,
                                       (uint64_t)1, (int)5, SAMPLE_FILTER_TEST_CONTEXT_ARGS(1)) != CPLAT_OK))
        {
            args->is_ok = 0;
        }
    }
}
} // namespace

// 公開を繰り返す間に複数スレッドが取り込みと判定を行っても、失敗せず最後の公開内容に収束することの確認
TEST_F(sampleFilterShareTest, concurrent_publish_and_refresh_converge)
{
    // Arrange
    static unsigned char image_failed[kImageSize];
    static unsigned char image_started[kImageSize];
    std::atomic<int> stop_flag(0);
    refresh_args args[4];
    cplat_thread *threads[4];
    sample_filter_share_status status;

    ASSERT_EQ(CPLAT_OK, compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED",
                                            image_failed)); // [状態] - 1 つ目の条件をコンパイルする。
    ASSERT_EQ(CPLAT_OK, compile_single_line("key == SAMPLE_WORKER_TRACE_KEY_WORKER_STARTED",
                                            image_started)); // [状態] - 2 つ目の条件をコンパイルする。
    for (int index = 0; index < 4; index++)
    {
        args[index].share = reader_;
        args[index].slot = slot_;
        args[index].stop_flag = &stop_flag;
        args[index].is_ok = 0;
        args[index].pad = 0;
        ASSERT_EQ(CPLAT_OK, cplat_thread_create(&threads[index], refresh_worker,
                                                &args[index])); // [状態] - 取り込みを繰り返すスレッドを起動する。
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
                                                        nullptr)); // [手順] - 2 種類の条件を交互に公開する。
    }
    stop_flag = 1; // [手順] - スレッドへ終了を通知する。
    for (int index = 0; index < 4; index++)
    {
        (void)cplat_thread_join(threads[index], CPLAT_SYNC_WAIT_FOREVER);
    }
    int actual_final_ret = sample_filter_share_refresh(reader_, slot_, nullptr); // [手順] - 最後にもう一度取り込む。
    (void)sample_filter_share_get_status(reader_, &status);

    // Assert
    for (int index = 0; index < 4; index++)
    {
        EXPECT_EQ(1, args[index].is_ok); // [確認_正常系] - 各スレッドの取り込みと判定が常に成功すること。
    }
    EXPECT_EQ(CPLAT_OK, actual_final_ret);        // [確認_正常系] - 最後の取り込みが成功すること。
    EXPECT_EQ(200U, status.published_generation); // [確認_正常系] - 200 回の公開で世代が 200 になること。
    EXPECT_EQ(status.published_generation,
              status.taken_generation); // [確認_正常系] - 最後の公開内容を取り込んでいること。
    EXPECT_EQ(
        SAMPLE_FILTER_STATE_ALWAYS_MATCH,
        state_of(SAMPLE_WORKER_TRACE_KEY_WORKER_STARTED)); // [確認_正常系] - 最後に公開した条件が判定に使われること。
}
