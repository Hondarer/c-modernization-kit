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
TEST(string_catalog_command_sampleTest, returns_exit_success_when_output_succeeds)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "string-catalog-command-sample";
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
    EXPECT_NE(std::string::npos, output.find("[English]")); // [確認_正常系] - 標準出力へ結果を出力すること。
    EXPECT_EQ(std::string::npos, errors.find("標準出力へ書き込めませんでした"));
    // [確認_正常系] - 標準出力の書き込みの失敗を診断しないこと。
}

// 終了の直前の fflush(stdout) または ferror(stdout) が失敗を示した場合に、診断を出して EXIT_FAILURE を返すことの確認
TEST(string_catalog_command_sampleTest, returns_exit_failure_when_stdout_check_fails)
{
    // Arrange
    char arg0[] = "string-catalog-command-sample";
    char *argv[] = {arg0}; // [状態] - 引数なしで起動する。

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - fflush(stdout) が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(EOF));
        // [Pre-Assert確認_異常系] - fflush(stdout) が 1 回呼び出されること。
        // [Pre-Assert手順] - fflush() から EOF を返す。

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
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - fflush(stdout) が 1 回呼び出されること。
        // [Pre-Assert手順] - fflush() から 0 を返す。
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
