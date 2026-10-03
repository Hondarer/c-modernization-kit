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
    void SetUp() override
    {
        char temp_dir[PLATFORM_PATH_MAX];
        const TestInfo *info = UnitTest::GetInstance()->current_test_info();

        ASSERT_EQ(CPLAT_OK, cplat_path_get_temp_dir(temp_dir, sizeof(temp_dir), nullptr));
        path_ = std::string(temp_dir) + "/sampleFilterFileTest_" + std::to_string(cplat_process_get_pid()) + "_" +
                info->name() + ".txt";
        saved_path_ = path_ + ".saved";
        std::memset(image_, 0x5A, sizeof(image_));
        std::memset(&result_, 0, sizeof(result_));
        std::memset(diagnostics_, 0, sizeof(diagnostics_));
    }

    void TearDown() override
    {
        (void)std::remove(path_.c_str());
        (void)std::remove(saved_path_.c_str());
    }

    /** ファイルへ内容をそのまま書き込みます。 */
    void write_file(const std::string &content)
    {
        FILE *stream = cplat_fopen(path_.c_str(), "wb", nullptr);

        ASSERT_NE(nullptr, stream);
        ASSERT_EQ(content.size(), std::fwrite(content.data(), 1U, content.size(), stream));
        ASSERT_EQ(0, std::fclose(stream));
    }

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
TEST_F(sampleFilterFileTest, load_reports_invalid_lines_by_file_line_number)
{
    // Arrange
    char actual_first[kLineWidth];
    write_file("\xEF\xBB\xBF# comment\r\n"
               "key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED\r\n"
               "\r\n"
               "  # indented comment\n"
               "arg.job_name starts_with \"import\" &&\n"
               "category <= 2\n"); // [状態] - 条件式リストのファイルを書き込む。

    // Pre-Assert

    // Act
    int actual_ret = load(path_); // [手順] - ファイルを読み込む。
    ASSERT_EQ(CPLAT_OK,
              cplat_string_catalog_filter_decompile_line(image_, sizeof(image_), 0U, actual_first,
                                                         sizeof(actual_first))); // [手順] - 1 行目を復元する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);           // [確認_正常系] - 無効な行があっても読み込みに成功すること。
    EXPECT_EQ(6U, result_.line_count);         // [確認_正常系] - ファイルの行数を数えること。
    EXPECT_EQ(2U, result_.condition_count);    // [確認_正常系] - 有効な条件式 2 行を格納すること。
    EXPECT_EQ(1U, result_.invalid_count);      // [確認_正常系] - 無効な行が 1 件であること。
    EXPECT_EQ(4U, diagnostics_[0].line_index); // [確認_正常系] - ファイルの 5 行目 (0 起点で 4) を通知すること。
    EXPECT_EQ(CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_SYNTAX,
              diagnostics_[0].error); // [確認_正常系] - 構文の誤りであること。
    EXPECT_STREQ("key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED",
                 actual_first); // [確認_正常系] - BOM と CRLF を取り除いた条件式を格納すること。
}

// 行幅を超える行は切り詰めずに無効にし、ほかの行は読み込むことの確認
TEST_F(sampleFilterFileTest, line_longer_than_width_is_invalid)
{
    // Arrange
    const std::string long_line = "arg.job_name == \"" + std::string(kLineWidth, 'x') + "\"";
    write_file("category <= 2\n" + long_line +
               "\nkey == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED\n"); // [状態] - 行幅を超える行を含むファイルを書き込む。

    // Pre-Assert

    // Act
    int actual_ret = load(path_); // [手順] - ファイルを読み込む。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);           // [確認_正常系] - 読み込みに成功すること。
    EXPECT_EQ(2U, result_.condition_count);    // [確認_正常系] - ほかの 2 行を格納すること。
    EXPECT_EQ(1U, result_.invalid_count);      // [確認_異常系] - 行幅を超える行を無効にすること。
    EXPECT_EQ(1U, diagnostics_[0].line_index); // [確認_異常系] - ファイルの 2 行目を通知すること。
    EXPECT_EQ(CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_LIMIT_EXCEEDED,
              diagnostics_[0].error); // [確認_異常系] - 原因が上限の超過であること。
}

// 有効な条件式が行数の上限を超える場合は、超えた行を無効にすることの確認
TEST_F(sampleFilterFileTest, conditions_beyond_capacity_are_invalid)
{
    // Arrange
    std::string content;
    for (std::size_t index = 0; index < kLineCapacity + 1U; index++)
    {
        content += "arg.worker_index == " + std::to_string(index) + "\n";
    }
    write_file(content); // [状態] - 行数の上限より 1 行多い条件式を書き込む。

    // Pre-Assert

    // Act
    int actual_ret = load(path_); // [手順] - ファイルを読み込む。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);                   // [確認_正常系] - 読み込みに成功すること。
    EXPECT_EQ(kLineCapacity, result_.condition_count); // [確認_正常系] - 上限まで格納すること。
    EXPECT_EQ(1U, result_.invalid_count);              // [確認_異常系] - 超えた 1 行を無効にすること。
    EXPECT_EQ(CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_LINE_CAPACITY,
              diagnostics_[0].error); // [確認_異常系] - 原因が行数の上限の超過であること。
}

// 読み込めない場合は失敗を返し、格納先を変更しないことの確認
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
    write_file(too_many);                              // [手順] - 行数の上限を超えるファイルを書き込む。
    int actual_too_many_ret = load(path_);             // [手順] - 行数の上限を超えるファイルを読み込む。

    // Assert
    EXPECT_EQ(CPLAT_ERR_UNKNOWN, actual_missing_ret);            // [確認_異常系] - 開けないファイルで失敗すること。
    EXPECT_EQ(CPLAT_ERR_LIMIT_EXCEEDED, actual_too_many_ret);    // [確認_異常系] - 行数の上限の超過で失敗すること。
    EXPECT_EQ(0, std::memcmp(expected, image_, sizeof(image_))); // [確認_異常系] - 格納先を変更しないこと。
}

// 書き出したファイルを読み込むと、元と同じフィルター オブジェクトになることの確認
TEST_F(sampleFilterFileTest, saved_file_loads_to_same_image)
{
    // Arrange
    static unsigned char original[kImageSize];
    const char *lines[] = {"key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED", "!(arg.job_name contains \"a\\\"b\")",
                           "id matches \"000[45]$\""};
    ASSERT_EQ(CPLAT_OK, compile_lines(lines, 3U, original)); // [状態] - 3 行をコンパイルする。

    // Pre-Assert

    // Act
    int actual_save_ret =
        sample_filter_file_save(saved_path_.c_str(), original, sizeof(original)); // [手順] - 書き出す。
    int actual_load_ret = load(saved_path_); // [手順] - 書き出したファイルを読み込む。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_save_ret);                          // [確認_正常系] - 書き出しに成功すること。
    EXPECT_EQ(CPLAT_OK, actual_load_ret);                          // [確認_正常系] - 読み込みに成功すること。
    EXPECT_EQ(0U, result_.invalid_count);                          // [確認_正常系] - 無効な行がないこと。
    EXPECT_EQ(0, std::memcmp(original, image_, sizeof(original))); // [確認_正常系] - 元と同じ内容になること。
}
