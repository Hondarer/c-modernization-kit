#include <cplat/base/platform.h>
#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_stdio.h>
#include <mock_cplat.h>
#include <wrapmain.h>

#include <cstdlib>
#include <string>

// すべての出力が成功した場合に EXIT_SUCCESS を返すことの確認
TEST(string_catalog_library_sampleTest, returns_exit_success_when_output_succeeds)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "string-catalog-library-sample";
    char *argv[] = {arg0}; // [状態] - 引数なしで起動する。

    // Pre-Assert

    // Act
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    int exit_code = __real_main(1, argv); // [手順] - __real_main() を呼び出す。
    std::string errors = testing::internal::GetCapturedStderr();
    std::string output = testing::internal::GetCapturedStdout();

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, exit_code); // [確認_正常系] - 終了コードが EXIT_SUCCESS であること。
    EXPECT_NE(std::string::npos, output.find("delta")); // [確認_正常系] - 標準出力へ検索の結果を出力すること。
    EXPECT_EQ(std::string::npos, errors.find("エラー:")); // [確認_正常系] - 標準エラー出力へ診断を出さないこと。
}

// 終了の直前の fflush(stdout) または ferror(stdout) が失敗を示した場合に、診断を出して EXIT_FAILURE を返すことの確認
TEST(string_catalog_library_sampleTest, returns_exit_failure_when_stdout_check_fails)
{
    // Arrange
    char arg0[] = "string-catalog-library-sample";
    char *argv[] = {arg0}; // [状態] - 引数なしで起動する。

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 終了の直前の fflush(stdout) が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout))
            .WillOnce(Invoke(delegate_real_fflush))
            .WillOnce(Invoke(delegate_real_fflush))
            .WillOnce(Invoke(delegate_real_fflush))
            .WillOnce(Return(EOF));
        // [Pre-Assert確認_異常系] - fflush(stdout) が途中の 3 回と終了の直前の 1 回、合計 4 回呼び出されること。
        // [Pre-Assert手順] - 途中の 3 回は実関数へ委譲し、終了の直前では EOF を返す。

        // Act
        testing::internal::CaptureStdout();
        testing::internal::CaptureStderr();
        int exit_code = __real_main(1, argv); // [手順] - __real_main() を呼び出す。
        std::string errors = testing::internal::GetCapturedStderr();
        (void)testing::internal::GetCapturedStdout();

        // Assert
        EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - 終了コードが EXIT_FAILURE であること。
        EXPECT_NE(std::string::npos, errors.find("エラー: 標準出力へ書き込めませんでした。"));
        // [確認_異常系] - 標準エラー出力へ診断を出すこと。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - ferror(stdout) がエラーを示す条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, ferror(_, _, _, stdout)).WillOnce(Return(1));
        // [Pre-Assert確認_異常系] - ferror(stdout) が 1 回呼び出されること。
        // [Pre-Assert手順] - ferror() から 1 を返す。

        // Act
        testing::internal::CaptureStdout();
        testing::internal::CaptureStderr();
        int exit_code = __real_main(1, argv); // [手順] - __real_main() を呼び出す。
        std::string errors = testing::internal::GetCapturedStderr();
        (void)testing::internal::GetCapturedStdout();

        // Assert
        EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - 終了コードが EXIT_FAILURE であること。
        EXPECT_NE(std::string::npos, errors.find("エラー: 標準出力へ書き込めませんでした。"));
        // [確認_異常系] - 標準エラー出力へ診断を出すこと。
    }
}

// 利用側からのトレース出力が失敗した場合に、診断を出して EXIT_FAILURE を返すことの確認
TEST(string_catalog_library_sampleTest, returns_exit_failure_when_trace_output_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "string-catalog-library-sample";
    char *argv[] = {arg0}; // [状態] - 引数なしで起動する。

    // Pre-Assert
    /*
     * トレース出力の関数 (samplecatalog_trace_write) はライブラリの中から cplat を呼ぶため、
     * テストから書き込みそのものを失敗させられない。
     * そこで失敗の原因を置き換え、トレーサーを開始しないまま開始の成功を返す。
     * 開始していないトレーサーへの書き込みは CPLAT_ERR_UNKNOWN を返すため、利用側からのトレース出力が失敗する。
     */
    EXPECT_CALL(mock_cplat, cplat_tracer_start(_)).WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - cplat_tracer_start() が 1 回呼び出されること。
    // [Pre-Assert手順] - トレーサーを開始せずに CPLAT_OK を返す。

    // Act
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    int exit_code = __real_main(1, argv); // [手順] - __real_main() を呼び出す。
    std::string errors = testing::internal::GetCapturedStderr();
    (void)testing::internal::GetCapturedStdout();

    // Assert
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - 終了コードが EXIT_FAILURE であること。
    EXPECT_NE(std::string::npos, errors.find("エラー: トレースを出力できませんでした。"));
    // [確認_異常系] - 標準エラー出力へトレース出力の失敗の診断を出すこと。
}