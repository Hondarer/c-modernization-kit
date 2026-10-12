#include <testfw.h>

#include "sampleFilterTestSupport.h"

#include "sample_filter_file.h"

#include <cplat/base/result.h>
#include <cplat/crt/path.h>
#include <cplat/crt/stdio.h>
#include <cplat/runtime/process.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

using namespace sample_filter_test;

class sampleFilterFileTest : public Test
{
  protected:
    // [サブ手順 名前=sampleFilterFileTest.SetUp]
    void SetUp() override
    {
        char temp_dir[PLATFORM_PATH_MAX];
        const TestInfo *info = UnitTest::GetInstance()->current_test_info();

        ASSERT_EQ(CPLAT_OK, cplat_path_get_temp_dir(temp_dir, sizeof(temp_dir), nullptr));
        // [状態確認] - cplat_path_get_temp_dir() の戻り値が CPLAT_OK であること。
        path_ = std::string(temp_dir) + "/sampleFilterFileTest_" + std::to_string(cplat_process_get_pid()) + "_" +
                info->name() + ".txt";
        saved_path_ = path_ + ".saved";
        std::memset(image_, 0x5A, sizeof(image_));
        std::memset(&result_, 0, sizeof(result_));
        std::memset(diagnostics_, 0, sizeof(diagnostics_));
    }
    // [サブ手順終了]

    // [サブ手順 名前=sampleFilterFileTest.TearDown]
    void TearDown() override
    {
        (void)std::remove(path_.c_str());
        (void)std::remove(saved_path_.c_str());
    }
    // [サブ手順終了]

    /** ファイルへ内容をそのまま書き込みます。 */
    // [サブ手順 名前=sampleFilterFileTest.write_file]
    void write_file(const std::string &content)
    {
        FILE *stream = cplat_fopen(path_.c_str(), "wb", nullptr);

        ASSERT_NE(nullptr, stream);
        // [状態確認] - cplat_fopen() の戻り値が nullptr 以外であること。
        ASSERT_EQ(content.size(), std::fwrite(content.data(), 1U, content.size(), stream));
        // [状態確認] - std::fwrite() の戻り値が content.size() の戻り値と一致すること。
        ASSERT_EQ(0, std::fclose(stream));
        // [状態確認] - std::fclose() の戻り値が 0 であること。
    }
    // [サブ手順終了]

    int load(const std::string &path)
    {
        return sample_filter_file_load(path.c_str(), kLineCapacity, kLineWidth, image_, sizeof(image_), diagnostics_,
                                       4U, &result_);
    }

    std::string path_;
    std::string saved_path_;
    unsigned char image_[kImageSize];
    cplat_string_catalog_filter_diagnostic diagnostics_[4];
    sample_filter_file_result result_;
};

// コメント行、空行、CRLF、先頭の BOM を含むファイルを読み込み、無効な行をファイルの行番号で通知することの確認
// [サブ手順参照 名前=sampleFilterFileTest.SetUp]
TEST_F(sampleFilterFileTest, load_reports_invalid_lines_by_file_line_number)
{
    // Arrange
    char actual_first[kLineWidth];
    // [サブ手順参照 名前=sampleFilterFileTest.write_file]
    write_file("\xEF\xBB\xBF# comment\r\n"
               "key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED\r\n"
               "\r\n"
               "  # indented comment\n"
               "arg.job_name starts_with \"import\" &&\n"
               "category <= 2\n"); // [状態] - 条件式リストのファイルを書き込む。

    // Pre-Assert

    // Act
    int actual_ret = load(path_); // [手順] - ファイルを読み込む。
    int actual_ret_decompile = cplat_string_catalog_filter_decompile_line(
        image_, sizeof(image_), 0U, actual_first, sizeof(actual_first)); // [手順] - 1 行目を復元する。
    ASSERT_EQ(CPLAT_OK, actual_ret_decompile);
    // [状態確認] - cplat_string_catalog_filter_decompile_line() の戻り値が CPLAT_OK であること。

    // Assert
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret); // [確認_正常系] - load() の戻り値が CPLAT_OK であること。(無効な行があっても読み込みに成功したことの確認)
    EXPECT_EQ(
        6U,
        result_.line_count); // [確認_正常系] - result_.line_count が 6 であること。(ファイルの行数を数えたことの確認)
    EXPECT_EQ(
        2U,
        result_
            .condition_count); // [確認_正常系] - result_.condition_count が 2 であること。(有効な条件式 2 行を格納したことの確認)
    EXPECT_EQ(1U, result_.invalid_count); // [確認_正常系] - result_.invalid_count が 1 であること。
    EXPECT_EQ(
        4U,
        diagnostics_[0]
            .line_index); // [確認_正常系] - diagnostics_[0].line_index が 4 であること。(ファイルの 5 行目を 0 起点で通知したことの確認)
    EXPECT_EQ(
        CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_SYNTAX,
        diagnostics_[0]
            .error); // [確認_正常系] - diagnostics_[0].error が CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_SYNTAX であること。
    EXPECT_STREQ(
        "key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED",
        actual_first); // [確認_正常系] - actual_first が "key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED" であること。(BOM と CRLF を取り除いた条件式を格納したことの確認)
}
// [サブ手順参照 名前=sampleFilterFileTest.TearDown]

// 行幅を超える行は切り詰めずに無効にし、ほかの行は読み込むことの確認
// [サブ手順参照 名前=sampleFilterFileTest.SetUp]
TEST_F(sampleFilterFileTest, line_longer_than_width_is_invalid)
{
    // Arrange
    const std::string long_line = "arg.job_name == \"" + std::string(kLineWidth, 'x') + "\"";
    // [サブ手順参照 名前=sampleFilterFileTest.write_file]
    write_file("category <= 2\n" + long_line +
               "\nkey == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED\n"); // [状態] - 行幅を超える行を含むファイルを書き込む。

    // Pre-Assert

    // Act
    int actual_ret = load(path_); // [手順] - ファイルを読み込む。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - load() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        2U,
        result_
            .condition_count); // [確認_正常系] - result_.condition_count が 2 であること。(ほかの 2 行を格納したことの確認)
    EXPECT_EQ(
        1U,
        result_
            .invalid_count); // [確認_異常系] - result_.invalid_count が 1 であること。(行幅を超える行を無効にしたことの確認)
    EXPECT_EQ(
        1U,
        diagnostics_[0]
            .line_index); // [確認_異常系] - diagnostics_[0].line_index が 1 であること。(ファイルの 2 行目を 0 起点で通知したことの確認)
    EXPECT_EQ(
        CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_LIMIT_EXCEEDED,
        diagnostics_[0]
            .error); // [確認_異常系] - diagnostics_[0].error が CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_LIMIT_EXCEEDED であること。
}
// [サブ手順参照 名前=sampleFilterFileTest.TearDown]

// 有効な条件式が行数の上限を超える場合は、超えた行を無効にすることの確認
// [サブ手順参照 名前=sampleFilterFileTest.SetUp]
TEST_F(sampleFilterFileTest, conditions_beyond_capacity_are_invalid)
{
    // Arrange
    std::string content;
    for (std::size_t index = 0; index < kLineCapacity + 1U; index++)
    {
        content += "arg.worker_index == " + std::to_string(index) + "\n";
    }
    // [サブ手順参照 名前=sampleFilterFileTest.write_file]
    write_file(content); // [状態] - 行数の上限より 1 行多い条件式を書き込む。

    // Pre-Assert

    // Act
    int actual_ret = load(path_); // [手順] - ファイルを読み込む。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - load() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        kLineCapacity,
        result_
            .condition_count); // [確認_正常系] - result_.condition_count が kLineCapacity であること。(上限まで格納したことの確認)
    EXPECT_EQ(
        1U,
        result_
            .invalid_count); // [確認_異常系] - result_.invalid_count が 1 であること。(超えた 1 行を無効にしたことの確認)
    EXPECT_EQ(
        CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_LINE_CAPACITY,
        diagnostics_[0]
            .error); // [確認_異常系] - diagnostics_[0].error が CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_LINE_CAPACITY であること。
}
// [サブ手順参照 名前=sampleFilterFileTest.TearDown]

// 読み込めない場合は失敗を返し、格納先を変更しないことの確認
// [サブ手順参照 名前=sampleFilterFileTest.SetUp]
TEST_F(sampleFilterFileTest, failed_load_keeps_image)
{
    // Arrange
    unsigned char expected[kImageSize];
    std::string too_many;
    std::memcpy(expected, image_, sizeof(image_));
    for (std::size_t index = 0; index < SAMPLE_FILTER_FILE_LINE_MAX + 1U; index++)
    {
        too_many += "#\n";
    }

    // Pre-Assert

    // Act
    int actual_missing_ret = load(path_ + ".missing"); // [手順] - 存在しないファイルを読み込む。
    // [サブ手順参照 名前=sampleFilterFileTest.write_file]
    write_file(too_many);                              // [手順] - 行数の上限を超えるファイルを書き込む。
    int actual_too_many_ret = load(path_);             // [手順] - 行数の上限を超えるファイルを読み込む。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_UNKNOWN,
        actual_missing_ret); // [確認_異常系] - 存在しないファイルを読み込んだときの load() の戻り値が CPLAT_ERR_UNKNOWN であること。
    EXPECT_EQ(
        CPLAT_ERR_LIMIT_EXCEEDED,
        actual_too_many_ret); // [確認_異常系] - 行数の上限を超えるファイルを読み込んだときの load() の戻り値が CPLAT_ERR_LIMIT_EXCEEDED であること。
    EXPECT_EQ(
        0,
        std::memcmp(
            expected, image_,
            sizeof(
                image_))); // [確認_異常系] - std::memcmp() の戻り値が 0 であること。(読み込みに失敗しても格納先を変更しなかったことの確認)
}
// [サブ手順参照 名前=sampleFilterFileTest.TearDown]

// 書き出したファイルを読み込むと、元と同じフィルター オブジェクトになることの確認
// [サブ手順参照 名前=sampleFilterFileTest.SetUp]
TEST_F(sampleFilterFileTest, saved_file_loads_to_same_image)
{
    // Arrange
    static unsigned char original[kImageSize];
    const char *lines[] = {"key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED", "!(arg.job_name contains \"a\\\"b\")",
                           "id matches \"000[45]$\""};
    ASSERT_EQ(CPLAT_OK, compile_lines(lines, 3U, original)); // [状態] - 3 行をコンパイルする。
    // [状態確認] - compile_lines() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    // Act
    int actual_save_ret =
        sample_filter_file_save(saved_path_.c_str(), original, sizeof(original)); // [手順] - 書き出す。
    int actual_load_ret = load(saved_path_); // [手順] - 書き出したファイルを読み込む。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_save_ret); // [確認_正常系] - sample_filter_file_save() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(CPLAT_OK, actual_load_ret); // [確認_正常系] - load() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        0U, result_.invalid_count); // [確認_正常系] - result_.invalid_count が 0 であること。(無効な行がないことの確認)
    EXPECT_EQ(
        0,
        std::memcmp(
            original, image_,
            sizeof(
                original))); // [確認_正常系] - std::memcmp() の戻り値が 0 であること。(読み込んだ内容が元と同じになったことの確認)
}
// [サブ手順参照 名前=sampleFilterFileTest.TearDown]
