// main を組み込み、mock で標準出力の失敗を注入する。
// 先例は app/struct-meta/test/src/structMetaSampleIoTest/structMetaSampleIoTest.cc である。
// libbase は共有ライブラリの読み込みの時 (libbase_lifecycle.c の onLoad) に定義ファイルを読むため、テストの中で一時ディレクトリの libbase_extdef.jsonc を消しても間に合わない。
// 読み込み済みの定義によって base_calc の結果と result: の行の有無が変わるため、計算の結果には依存せず、終了コードと ret: の行だけを確認する。

#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_stdio.h>
#include <mock_cplat.h>
#include <wrapmain.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#if defined(PLATFORM_LINUX)
    #include <stdio_ext.h>
    #include <unistd.h>
#endif

// [サブ手順 名前=override_sampleIoTest.disable_console_init]
static void disable_console_init(Mock_cplat &mock_cplat)
{
    // テスト実行体の終了時にコンソールの設定の復元が登録されないようにする。
    ON_CALL(mock_cplat, cplat_console_init()).WillByDefault(Return()); // [状態] - cplat_console_init() を何もしない既定動作にする。
}
// [サブ手順終了]

// --help の表示と標準出力の検査が成功すれば EXIT_SUCCESS を返すことの確認
TEST(override_sampleIoTest, returns_exit_success_when_help_succeeds)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char arg1[] = "--help";
    char *argv[] = {arg0, arg1}; // [状態] - --help の引数を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]

    // Pre-Assert

    // Act
    testing::internal::CaptureStdout();
    int exit_code = __real_main(2, argv); // [手順] - --help を指定して __real_main() を呼び出す。
    std::string output = testing::internal::GetCapturedStdout();

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, exit_code); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_NE(std::string::npos,
              output.find("--help")); // [確認_正常系] - "--help" を探した output.find() の戻り値が string::npos 以外であること。
}

// --help の使い方の表示が失敗すれば EXIT_FAILURE を返し、標準エラー出力へ診断を出すことの確認
TEST(override_sampleIoTest, returns_exit_failure_when_help_print_usage_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char arg1[] = "--help";
    char *argv[] = {arg0, arg1}; // [状態] - --help の引数を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_ERR_OUT_OF_MEMORY));
    // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_ERR_OUT_OF_MEMORY を返す。

    // Act
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    int exit_code = __real_main(2, argv); // [手順] - 使い方の表示が失敗する状態で --help を指定して __real_main() を呼び出す。
    // 失敗の注入では標準エラー出力を確認し、標準出力の捕捉は後続へ漏らさないためだけに終えるため、戻り値は明示的に破棄する。
    (void)testing::internal::GetCapturedStdout();
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
    EXPECT_NE(std::string::npos, errors.find("error: failed to write stdout"));
    // [確認_異常系] - "error: failed to write stdout" を探した errors.find() の戻り値が string::npos 以外であること。

    // Cleanup
    // 注入した失敗で mock が書き出さなかった分を後のテストへ漏らさない後始末のため、戻り値は明示的に破棄する。
    (void)fflush(stdout);
}

// --help の表示の後に fflush() が失敗すれば EXIT_FAILURE を返し、標準エラー出力へ診断を出すことの確認
TEST(override_sampleIoTest, returns_exit_failure_when_help_fflush_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char arg1[] = "--help";
    char *argv[] = {arg0, arg1}; // [状態] - --help の引数を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_OK を返す。
    EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(EOF));
    // [Pre-Assert確認_異常系] - fflush() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - fflush() から EOF を返す。

    // Act
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    int exit_code = __real_main(2, argv); // [手順] - fflush() が失敗する状態で --help を指定して __real_main() を呼び出す。
    // 失敗の注入では標準エラー出力を確認し、標準出力の捕捉は後続へ漏らさないためだけに終えるため、戻り値は明示的に破棄する。
    (void)testing::internal::GetCapturedStdout();
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
    EXPECT_NE(std::string::npos, errors.find("error: failed to write stdout"));
    // [確認_異常系] - "error: failed to write stdout" を探した errors.find() の戻り値が string::npos 以外であること。

    // Cleanup
    // 注入した失敗で mock が書き出さなかった分を後のテストへ漏らさない後始末のため、戻り値は明示的に破棄する。
    (void)fflush(stdout);
}

// --help の表示と fflush() の後に ferror() が失敗を示せば EXIT_FAILURE を返し、標準エラー出力へ診断を出すことの確認
TEST(override_sampleIoTest, returns_exit_failure_when_help_ferror_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char arg1[] = "--help";
    char *argv[] = {arg0, arg1}; // [状態] - --help の引数を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_OK を返す。
    EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - fflush() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - fflush() から 0 を返す。
    EXPECT_CALL(mock_stdio, ferror(_, _, _, stdout)).WillOnce(Return(1));
    // [Pre-Assert確認_異常系] - ferror() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - ferror() から 1 を返す。

    // Act
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    int exit_code = __real_main(2, argv); // [手順] - ferror() が失敗を示す状態で --help を指定して __real_main() を呼び出す。
    // 失敗の注入では標準エラー出力を確認し、標準出力の捕捉は後続へ漏らさないためだけに終えるため、戻り値は明示的に破棄する。
    (void)testing::internal::GetCapturedStdout();
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
    EXPECT_NE(std::string::npos, errors.find("error: failed to write stdout"));
    // [確認_異常系] - "error: failed to write stdout" を探した errors.find() の戻り値が string::npos 以外であること。

    // Cleanup
    // 注入した失敗で mock が書き出さなかった分を後のテストへ漏らさない後始末のため、戻り値は明示的に破棄する。
    (void)fflush(stdout);
}

// 引数なしの通常の経路で標準出力の検査が成功すれば EXIT_SUCCESS を返し、ret: の行を出すことの確認
TEST(override_sampleIoTest, returns_exit_success_on_normal_path)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char *argv[] = {arg0}; // [状態] - 引数なしの通常の経路を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).Times(1);
    // [Pre-Assert確認_正常系] - fflush() が stdout を指定して 1 回呼び出されること。

    // Act
    testing::internal::CaptureStdout();
    int exit_code = __real_main(1, argv); // [手順] - 引数なしで __real_main() を呼び出す。
    std::string output = testing::internal::GetCapturedStdout();

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, exit_code); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_NE(std::string::npos,
              output.find("ret: ")); // [確認_正常系] - "ret: " を探した output.find() の戻り値が string::npos 以外であること。
}

// 通常の経路の終了の直前に fflush() が失敗すれば EXIT_FAILURE を返し、ferror() を呼ばないことの確認
TEST(override_sampleIoTest, returns_exit_failure_when_normal_fflush_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char *argv[] = {arg0}; // [状態] - 引数なしの通常の経路を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(EOF));
    // [Pre-Assert確認_異常系] - fflush() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - fflush() から EOF を返す。
    EXPECT_CALL(mock_stdio, ferror(_, _, _, stdout)).Times(0);
    // [Pre-Assert確認_異常系] - stdout を指定した ferror() の呼び出しがないこと。

    // Act
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    int exit_code = __real_main(1, argv); // [手順] - fflush() が失敗する状態で引数なしの __real_main() を呼び出す。
    // 失敗の注入では標準エラー出力を確認し、標準出力の捕捉は後続へ漏らさないためだけに終えるため、戻り値は明示的に破棄する。
    (void)testing::internal::GetCapturedStdout();
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
    EXPECT_NE(std::string::npos, errors.find("error: failed to write stdout"));
    // [確認_異常系] - "error: failed to write stdout" を探した errors.find() の戻り値が string::npos 以外であること。

    // Cleanup
    // 注入した失敗で mock が書き出さなかった分を後のテストへ漏らさない後始末のため、戻り値は明示的に破棄する。
    (void)fflush(stdout);
}

// 通常の経路の終了の直前に ferror() が失敗を示せば EXIT_FAILURE を返し、標準エラー出力へ診断を出すことの確認
TEST(override_sampleIoTest, returns_exit_failure_when_normal_ferror_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char *argv[] = {arg0}; // [状態] - 引数なしの通常の経路を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - fflush() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - fflush() から 0 を返す。
    EXPECT_CALL(mock_stdio, ferror(_, _, _, stdout)).WillOnce(Return(1));
    // [Pre-Assert確認_異常系] - ferror() が stdout を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - ferror() から 1 を返す。

    // Act
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    int exit_code = __real_main(1, argv); // [手順] - ferror() が失敗を示す状態で引数なしの __real_main() を呼び出す。
    // 失敗の注入では標準エラー出力を確認し、標準出力の捕捉は後続へ漏らさないためだけに終えるため、戻り値は明示的に破棄する。
    (void)testing::internal::GetCapturedStdout();
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
    EXPECT_NE(std::string::npos, errors.find("error: failed to write stdout"));
    // [確認_異常系] - "error: failed to write stdout" を探した errors.find() の戻り値が string::npos 以外であること。

    // Cleanup
    // 注入した失敗で mock が書き出さなかった分を後のテストへ漏らさない後始末のため、戻り値は明示的に破棄する。
    (void)fflush(stdout);
}

#if defined(PLATFORM_LINUX)
// /dev/full への --help の書き込みが失敗すれば EXIT_FAILURE を返すことの確認
TEST(override_sampleIoTest, returns_exit_failure_when_help_output_fails_on_dev_full)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char arg1[] = "--help";
    char *argv[] = {arg0, arg1}; // [状態] - --help の引数を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]
    ASSERT_EQ(0, fflush(stdout)); // [状態] - 切り替える前に標準出力をフラッシュする。
                                  // [状態確認] - fflush() の戻り値が 0 であること。
    int saved_stdout = dup(STDOUT_FILENO); // [状態] - 元の標準出力を複製する。
    ASSERT_NE(-1, saved_stdout); // [状態確認] - dup() の戻り値が -1 以外であること。
    FILE *full_stream = freopen("/dev/full", "w", stdout); // [状態] - 標準出力を /dev/full へ切り替える。
    if (full_stream == nullptr)
    {
        // /dev/full を開けなかった後始末のため、この 2 つの関数の戻り値は明示的に破棄する。
        (void)dup2(saved_stdout, STDOUT_FILENO);
        (void)close(saved_stdout);
        clearerr(stdout);
    }
    ASSERT_NE(nullptr, full_stream); // [状態確認] - freopen() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int exit_code = __real_main(2, argv); // [手順] - 標準出力を /dev/full にした状態で --help を指定して __real_main() を呼び出す。
    // 失敗した書き込みを捨ててから、標準出力を元へ戻す。
    __fpurge(stdout);
    clearerr(stdout);
    int restored = dup2(saved_stdout, STDOUT_FILENO);
    // 複製を戻したあとの後始末のため、戻り値は明示的に破棄する。
    (void)close(saved_stdout);
    clearerr(stdout);
    ASSERT_NE(-1, restored); // [状態確認] - dup2() の戻り値が -1 以外であること。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
}

// /dev/full への通常の経路の書き込みが失敗すれば EXIT_FAILURE を返すことの確認
TEST(override_sampleIoTest, returns_exit_failure_when_normal_output_fails_on_dev_full)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "override-sample";
    char *argv[] = {arg0}; // [状態] - 引数なしの通常の経路を組み立てる。
    disable_console_init(mock_cplat); // [サブ手順参照 名前=override_sampleIoTest.disable_console_init]
    ASSERT_EQ(0, fflush(stdout)); // [状態] - 切り替える前に標準出力をフラッシュする。
                                  // [状態確認] - fflush() の戻り値が 0 であること。
    int saved_stdout = dup(STDOUT_FILENO); // [状態] - 元の標準出力を複製する。
    ASSERT_NE(-1, saved_stdout); // [状態確認] - dup() の戻り値が -1 以外であること。
    FILE *full_stream = freopen("/dev/full", "w", stdout); // [状態] - 標準出力を /dev/full へ切り替える。
    if (full_stream == nullptr)
    {
        // /dev/full を開けなかった後始末のため、この 2 つの関数の戻り値は明示的に破棄する。
        (void)dup2(saved_stdout, STDOUT_FILENO);
        (void)close(saved_stdout);
        clearerr(stdout);
    }
    ASSERT_NE(nullptr, full_stream); // [状態確認] - freopen() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int exit_code = __real_main(1, argv); // [手順] - 標準出力を /dev/full にした状態で引数なしの __real_main() を呼び出す。
    // 失敗した書き込みを捨ててから、標準出力を元へ戻す。
    __fpurge(stdout);
    clearerr(stdout);
    int restored = dup2(saved_stdout, STDOUT_FILENO);
    // 複製を戻したあとの後始末のため、戻り値は明示的に破棄する。
    (void)close(saved_stdout);
    clearerr(stdout);
    ASSERT_NE(-1, restored); // [状態確認] - dup2() の戻り値が -1 以外であること。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
}
#endif
