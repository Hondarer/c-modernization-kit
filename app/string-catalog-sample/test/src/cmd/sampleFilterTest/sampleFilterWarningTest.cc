#include <testfw.h>

#include "sample_filter_warning.h"

#include "gen/sample_worker_trace.h"

#include <cplat/base/result.h>

#include <cstring>

class sampleFilterWarningTest : public Test
{
  protected:
    // [サブ手順 名前=sampleFilterWarningTest.SetUp]
    void SetUp() override
    {
        std::memset(&warning_, 0, sizeof(warning_));
        warning_.line_index = 1U;
        warning_.predicate_index = 0U;
        warning_.string_key = SAMPLE_WORKER_TRACE_KEY_JOB_RECEIVED;
        warning_.argument_index = 2;
        warning_.other_string_key = SAMPLE_WORKER_TRACE_KEY_JOB_RECEIVED;
        warning_.other_argument_index = -1;
    }
    // [サブ手順終了]

    cplat_string_catalog_filter_warning warning_;
    int pad_ = 0; /**< 明示的アラインメントです。 */
    char text_[512] = {0};
};

// 型の不一致は、行と比較要素の番号、項目の ID、引数の名前と種別で表すことの確認
// [サブ手順参照 名前=sampleFilterWarningTest.SetUp]
TEST_F(sampleFilterWarningTest, type_mismatch_is_formatted_with_entry_and_argument)
{
    // Arrange
    int actual_ret;

    warning_.kind = CPLAT_STRING_CATALOG_FILTER_WARNING_TYPE_MISMATCH; // [状態] - job_name (STRING) の型の不一致とする。

    // Pre-Assert

    // Act
    actual_ret = sample_filter_warning_format(sample_worker_trace_catalog(), &warning_, text_,
                                              sizeof(text_)); // [手順] - 警告を文へ整える。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - CPLAT_OK を返すこと。
    EXPECT_STREQ("2 行目の 1 番目の比較: SAMPLE_WORKER_TRACE_ID_0002 の引数 job_name は STRING のため、"
                 "この項目では比較が偽になります",
                 text_); // [確認_正常系] - 番号は 1 起点で、項目の ID と引数の名前、種別を含むこと。
}

// 型区分の混在は、文字列の項目と文字列以外の項目の両方を示すことの確認
// [サブ手順参照 名前=sampleFilterWarningTest.SetUp]
TEST_F(sampleFilterWarningTest, mixed_argument_types_is_formatted_with_both_entries)
{
    // Arrange
    int actual_ret;

    warning_.kind = CPLAT_STRING_CATALOG_FILTER_WARNING_MIXED_ARGUMENT_TYPES; // [状態] - 混在の警告とする。
    warning_.other_string_key = SAMPLE_WORKER_TRACE_KEY_JOB_PROGRESS;
    warning_.other_argument_index = 1; // [状態] - もう一方を JOB_PROGRESS の ratio (DOUBLE) とする。

    // Pre-Assert

    // Act
    actual_ret = sample_filter_warning_format(sample_worker_trace_catalog(), &warning_, text_,
                                              sizeof(text_)); // [手順] - 警告を文へ整える。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - CPLAT_OK を返すこと。
    EXPECT_STREQ("2 行目: 引数 job_name は、SAMPLE_WORKER_TRACE_ID_0002 では文字列、SAMPLE_WORKER_TRACE_ID_0003 では"
                 "数値 (DOUBLE) です。型の合わない項目では比較が偽になり、意図した判定結果にならない可能性があります",
                 text_); // [確認_正常系] - 両方の項目と、文字列以外の側の種別を含むこと。
}

// カタログにない項目や引数の位置、種別のない警告を拒否することの確認
// [サブ手順参照 名前=sampleFilterWarningTest.SetUp]
TEST_F(sampleFilterWarningTest, unknown_entry_or_kind_is_rejected)
{
    // Arrange
    int actual_unknown_argument_ret;
    int actual_none_ret;

    warning_.kind = CPLAT_STRING_CATALOG_FILTER_WARNING_TYPE_MISMATCH;
    warning_.argument_index = 99; // [状態] - 項目にない引数の位置とする。

    // Pre-Assert

    // Act
    actual_unknown_argument_ret = sample_filter_warning_format(sample_worker_trace_catalog(), &warning_, text_,
                                                               sizeof(text_)); // [手順] - 引数の位置が不正な警告を整える。
    warning_.argument_index = 2;
    warning_.kind = CPLAT_STRING_CATALOG_FILTER_WARNING_NONE;
    actual_none_ret = sample_filter_warning_format(sample_worker_trace_catalog(), &warning_, text_,
                                                   sizeof(text_)); // [手順] - 種別のない警告を整える。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, actual_unknown_argument_ret); // [確認_異常系] - 不正な引数の位置を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, actual_none_ret);             // [確認_異常系] - 種別のない警告を拒否すること。
    EXPECT_STREQ("", text_);                                            // [確認_異常系] - 空文字列を残すこと。
}

// 格納先に収まらない場合は CPLAT_ERR_BUFFER_TOO_SMALL を返すことの確認
// [サブ手順参照 名前=sampleFilterWarningTest.SetUp]
TEST_F(sampleFilterWarningTest, small_destination_is_reported)
{
    // Arrange
    char small[8];
    int actual_ret;

    warning_.kind = CPLAT_STRING_CATALOG_FILTER_WARNING_TYPE_MISMATCH;

    // Pre-Assert

    // Act
    actual_ret = sample_filter_warning_format(sample_worker_trace_catalog(), &warning_, small,
                                              sizeof(small)); // [手順] - 小さな格納先へ整える。

    // Assert
    EXPECT_EQ(CPLAT_ERR_BUFFER_TOO_SMALL, actual_ret); // [確認_異常系] - 収まらないことを返すこと。
    EXPECT_STREQ("", small);                           // [確認_異常系] - 空文字列にすること。
}
