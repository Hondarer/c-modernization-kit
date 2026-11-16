#include <cplat/base/platform.h>
#include <cplat/base/result.h>
#include <cplat/crt/path.h>
#include <cplat/crt/stdio.h>
#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_stdio.h>
#include <mock_cplat.h>
#include <wrapmain.h>
#include <struct_meta/catalog/catalog.h>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#if !defined(PLATFORM_WINDOWS)
    #include <stdio_ext.h>
    #include <unistd.h>
#endif
#include "struct_meta_sample.inject.h"

// 入出力だけを試すため、組み込みカタログの取得は使用しない。
extern "C" const struct_meta_catalog *sample_types_meta_catalog(void)
{
    return nullptr;
}

// 正の部分読み取りとエラーが同時に返る場合に、読み取ったデータと診断を両方出すことの確認
TEST(structMetaSampleIoTest, reports_partial_read_error_after_output)
{
    // Arrange
    NiceMock<Mock_cplat> mock;
    char json_path[PLATFORM_PATH_MAX] = {};
    char binary_path[PLATFORM_PATH_MAX] = {};
    FILE *json = cplat_fopen_temp("sio", "w+b", json_path, sizeof(json_path), nullptr);
    ASSERT_NE(nullptr, json); // [状態確認] - cplat_fopen_temp() が JSON 用の入力先を返すこと。
    FILE *binary = cplat_fopen_temp("sio", "w+b", binary_path, sizeof(binary_path), nullptr);
    if (binary == nullptr)
    {
        (void)fclose(json);
        (void)cplat_remove(json_path, nullptr);
    }
    ASSERT_NE(nullptr, binary); // [状態確認] - cplat_fopen_temp() がバイナリ用の入力先を返すこと。
    auto partial_read = [](void *buffer, size_t, size_t, FILE *, cplat_error *error) -> size_t
    {
        memcpy(buffer, "abc", 3);
        cplat_error_capture_errno(error, EIO);
        return 3;
    }; // [状態] - 3 バイトを読み取った直後の I/O エラーを再現する。

    // Pre-Assert
    EXPECT_CALL(mock, cplat_fopen(_, _, _)).WillOnce(Return(json)).WillOnce(Return(binary));
    // [Pre-Assert確認_正常系] - 各形式の入力を合計 2 回開くこと。
    // [Pre-Assert手順] - JSON とバイナリの入力先を順番に返す。
    EXPECT_CALL(mock, cplat_fread(_, _, _, _, _)).Times(2).WillRepeatedly(Invoke(partial_read));
    // [Pre-Assert確認_異常系] - 各形式で読み取りを 1 回ずつ行うこと。
    // [Pre-Assert手順] - 正の読み取り数とエラー詳細を同時に返す。

    // Act
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    test_sample_catjson("json-input");
    test_sample_catbin("binary-input"); // [手順] - 両形式で部分読み取りを処理する。
    std::string output = testing::internal::GetCapturedStdout();
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_NE(std::string::npos, output.find("abc")); // [確認_異常系] - 読み取ったデータを出力すること。
    EXPECT_NE(std::string::npos, errors.find("catjson の入力ファイルを読み取れません"));
    EXPECT_NE(std::string::npos,
              errors.find("catbin の入力ファイルを読み取れません")); // [確認_異常系] - 両形式でエラーを通知すること。

    // Cleanup
    (void)cplat_remove(json_path, nullptr);
    (void)cplat_remove(binary_path, nullptr);
}

// --help 指定時にヘルプ表示が成功すれば EXIT_SUCCESS を返すことの確認
TEST(structMetaSampleIoTest, returns_exit_success_when_help_succeeds)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "struct-meta-sample";
    char arg1[] = "--help";
    char *argv[] = {arg0, arg1}; // [状態] - --help 引数を組み立てる。

    // Pre-Assert

    // Act
    testing::internal::CaptureStdout();
    int exit_code = __real_main(2, argv); // [手順] - --help オプションで実行する。
    std::string output = testing::internal::GetCapturedStdout();

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, exit_code);                  // [確認_正常系] - 終了コードが EXIT_SUCCESS であること。
    EXPECT_NE(std::string::npos, output.find("--help")); // [確認_正常系] - ヘルプに --help が含まれること。

    // Cleanup
}

#if !defined(PLATFORM_WINDOWS)
// 実際の /dev/full へ --help を出力して失敗した場合に EXIT_FAILURE を返すことの確認
TEST(structMetaSampleIoTest, returns_exit_failure_when_help_output_fails_on_dev_full)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    char arg0[] = "struct-meta-sample";
    char arg1[] = "--help";
    char *argv[] = {arg0, arg1};
    ASSERT_EQ(0, fflush(stdout));
    int saved_stdout = dup(STDOUT_FILENO);
    ASSERT_NE(-1, saved_stdout); // [状態確認] - 元の標準出力ディスクリプターを複製できること。
    FILE *full_stream = freopen("/dev/full", "w", stdout); // [状態] - 標準出力を書き込み不能な /dev/full へ切り替える。
    if (full_stream == nullptr)
    {
        (void)dup2(saved_stdout, STDOUT_FILENO);
        (void)close(saved_stdout);
        clearerr(stdout);
    }
    ASSERT_NE(nullptr, full_stream); // [状態確認] - /dev/full を標準出力として開けること。

    // Pre-Assert

    // Act
    int exit_code = __real_main(2, argv); // [手順] - 実 API と実ファイルで --help を実行する。
    __fpurge(stdout);
    clearerr(stdout);
    int restored = dup2(saved_stdout, STDOUT_FILENO);
    (void)close(saved_stdout);
    clearerr(stdout);

    // Assert
    ASSERT_NE(-1, restored);            // [状態確認] - 標準出力を元へ戻せること。
    EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - /dev/full への書き込み失敗で EXIT_FAILURE を返すこと。

    // Cleanup
}
#endif

// --help 表示で API 戻り値失敗・fflush 失敗・ferror 検出のいずれでも EXIT_FAILURE を返すことの確認
TEST(structMetaSampleIoTest, returns_exit_failure_when_help_print_or_stream_fails)
{
    // Arrange
    char arg0[] = "struct-meta-sample";
    char arg1[] = "--help";
    char *argv[] = {arg0, arg1};

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - print_usage API がエラーを返す条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_ERR_OUT_OF_MEMORY));
        // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が 1 回呼ばれること。
        // [Pre-Assert手順] - CPLAT_ERR_OUT_OF_MEMORY を返す。

        // Act
        int exit_code = __real_main(2, argv); // [手順] - API 失敗時の --help 経路を実行する。

        // Assert
        EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - API 失敗で EXIT_FAILURE を返すこと。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - fflush(stdout) が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_OK));
        // [Pre-Assert確認_異常系] - print_usage() が CPLAT_OK を返してもフラッシュ検査へ進むこと。
        // [Pre-Assert手順] - CPLAT_OK を返す。
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(EOF));
        // [Pre-Assert確認_異常系] - fflush(stdout) が呼ばれること。
        // [Pre-Assert手順] - EOF を返す。

        // Act
        int exit_code = __real_main(2, argv); // [手順] - フラッシュ失敗時の --help 経路を実行する。

        // Assert
        EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - fflush(stdout) 失敗で EXIT_FAILURE を返すこと。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - ferror(stdout) が非 0 になる条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_OK));
        // [Pre-Assert確認_異常系] - print_usage() の成功後にストリーム検査へ進むこと。
        // [Pre-Assert手順] - CPLAT_OK を返す。
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - fflush(stdout) が 0 を返しても ferror(stdout) 検査へ進むこと。
        // [Pre-Assert手順] - 0 を返す。
        EXPECT_CALL(mock_stdio, ferror(_, _, _, stdout)).WillOnce(Return(1));
        // [Pre-Assert確認_異常系] - ferror(stdout) が呼ばれること。
        // [Pre-Assert手順] - ストリーム エラーを示す 1 を返す。

        // Act
        int exit_code = __real_main(2, argv); // [手順] - ストリーム エラー検出時の --help 経路を実行する。

        // Assert
        EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - ferror(stdout) 検出で EXIT_FAILURE を返すこと。
    }

    // Cleanup
}

// 引数登録エラーおよび解析失敗時に既失敗経路として EXIT_FAILURE を返すことの確認
TEST(structMetaSampleIoTest, returns_exit_failure_on_argparser_register_and_parse_errors)
{
    // Arrange
    char arg0[] = "struct-meta-sample";
    char unknown_opt[] = "--unknown-option";
    char *unknown_argv[] = {arg0, unknown_opt};
    char *plain_argv[] = {arg0};

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 未定義オプションを渡す条件を用意する。

        // Pre-Assert

        // Act
        testing::internal::CaptureStderr();
        int exit_code = __real_main(2, unknown_argv); // [手順] - 不正オプションで起動する。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - 解析失敗時に EXIT_FAILURE を返すこと。
        EXPECT_FALSE(errors.empty());       // [確認_異常系] - 診断メッセージが出力されること。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - オプション登録エラー件数が 1 の条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat, cplat_argparser_get_register_error_count()).WillOnce(Return(1U));
        // [Pre-Assert確認_異常系] - 登録エラー件数を確認すること。
        // [Pre-Assert手順] - 登録エラー件数 1 を返す。
        EXPECT_CALL(mock_cplat, cplat_argparser_print_register_error_messages(stderr))
            .WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - 登録エラー出力が失敗しても終了コードを維持すること。
        // [Pre-Assert手順] - CPLAT_ERR_UNKNOWN を返す。

        // Act
        int exit_code = __real_main(1, plain_argv); // [手順] - 登録エラー経路を実行する。

        // Assert
        EXPECT_EQ(EXIT_FAILURE, exit_code); // [確認_異常系] - 登録エラー時は常に EXIT_FAILURE を返すこと。
    }

    // Cleanup
}

// dumpjson が JSON を末尾改行付きで標準出力し、出力失敗を診断することの確認
TEST(structMetaSampleIoTest, dumps_json_with_trailing_newline_and_reports_write_failure)
{
    // Arrange
    struct SampleValue
    {
        int32_t count;
    } sample = {42};
    const struct_meta_field fields[] = {
        {"count", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(SampleValue, count), sizeof(int32_t), 1, 0, nullptr,
         nullptr, nullptr, 0},
    };
    const struct_meta_descriptor descriptor = {"SampleValue", sizeof(SampleValue), fields, 1, nullptr, nullptr, 0};

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 正常な記述子と値を用意する。

        // Pre-Assert

        // Act
        testing::internal::CaptureStdout();
        testing::internal::CaptureStderr();
        test_sample_dumpjson(&descriptor, &sample); // [手順] - dumpjson で JSON を標準出力する。
        std::string output = testing::internal::GetCapturedStdout();
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_NE(std::string::npos, output.find("\"count\"")); // [確認_正常系] - フィールド名が JSON に含まれること。
        EXPECT_NE(std::string::npos, output.find("42"));        // [確認_正常系] - 値が JSON に含まれること。
        EXPECT_FALSE(output.empty());
        EXPECT_EQ('\n', output.back()); // [確認_正常系] - 末尾が改行で終わること。
        EXPECT_TRUE(errors.empty());    // [確認_正常系] - 正常時は標準エラーへ出力しないこと。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - dumpjson の標準出力書き込みが失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat, cplat_fwrite(_, _, _, stdout, _))
            .WillOnce(Invoke(
                [](const void *, size_t, size_t, FILE *, cplat_error *error) -> size_t
                {
                    cplat_error_capture_errno(error, EIO);
                    return 0U;
                }));
        // [Pre-Assert確認_異常系] - cplat_fwrite() で標準出力失敗が発生すること。
        // [Pre-Assert手順] - EIO を記録して書き込み数 0 を返す。

        // Act
        testing::internal::CaptureStderr();
        test_sample_dumpjson(&descriptor, &sample); // [手順] - 書き込み失敗時の dumpjson を実行する。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_NE(std::string::npos, errors.find("dumpjson の標準出力へ書き込めません"));
        // [確認_異常系] - 標準出力への書き込み失敗を診断出力すること。
    }

    // Cleanup
}
