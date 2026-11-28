#include <testfw.h>
#include <mock_cplat.h>
#include <mock_stdio.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <cplat/service/service.h>
#include "service-sample.h"

using testing::_;
using testing::DoAll;
using testing::HasSubstr;
using testing::InSequence;
using testing::Invoke;
using testing::NiceMock;
using testing::NotNull;
using testing::Return;
using testing::SetArgPointee;
using testing::StrEq;

extern "C" int __real_main(int argc, char *argv[]);

namespace
{

int run_main(const std::vector<std::string> &args)
{
    std::vector<std::vector<char>> buffers;
    std::vector<char *> argv;
    buffers.reserve(args.size());
    argv.reserve(args.size() + 1);
    for (const auto &arg : args)
    {
        buffers.emplace_back(arg.begin(), arg.end());
        buffers.back().push_back('\0');
        argv.push_back(buffers.back().data());
    }
    argv.push_back(nullptr);
    return __real_main(static_cast<int>(args.size()), argv.data());
}

void setup_tracer_defaults(NiceMock<Mock_cplat> &mock_cplat, cplat_tracer *tracer_handle)
{
    ON_CALL(mock_cplat, cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED))
        .WillByDefault(Return(tracer_handle));
    ON_CALL(mock_cplat, cplat_tracer_set_name(_, _, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_set_os_level(_, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_set_file_level(_, _, _, _, _, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_set_stderr_level(_, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_start(_)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_stop(_)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_dispose(_))
        .WillByDefault(Invoke(
            [](cplat_tracer **handle)
            {
                if (handle != nullptr)
                {
                    *handle = nullptr;
                }
            }));
    ON_CALL(mock_cplat, cplat_tracer_write_at(_, _, _, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_writef_at(_, _, _, _)).WillByDefault(Return(CPLAT_OK));
}

} // namespace

// attach_output_pipes が失敗した場合は即座に EXIT_FAILURE を返すことの確認
TEST(serviceSampleTest, main_exits_immediately_when_attach_output_pipes_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillOnce(Return(
            CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - cplat_elevated_process_attach_output_pipes() が 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_elevated_process_attach_output_pipes() から CPLAT_ERR_UNKNOWN を返す。
    EXPECT_CALL(mock_cplat, cplat_console_init())
        .Times(0); // [Pre-Assert確認_異常系] - cplat_console_init() が呼び出されないこと。

    // Act
    int actual_rc = run_main({"service-sample", "install"}); // [手順] - "install" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_rc); // [確認_異常系] - run_main() の戻り値が EXIT_FAILURE であること。
}

// コマンド未指定時に usage を表示して失敗終了することの確認
TEST(serviceSampleTest, usage_without_args)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_service_install(_, _))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_service_install() が呼び出されないこと。
    EXPECT_CALL(mock_cplat, cplat_service_uninstall(_))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_service_uninstall() が呼び出されないこと。
    EXPECT_CALL(mock_cplat, cplat_service_run(_, _))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_service_run() が呼び出されないこと。
    EXPECT_CALL(mock_cplat, cplat_service_run_console(_, _))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_service_run_console() が呼び出されないこと。

    // Act
    int actual_rc = run_main({"service-sample"}); // [手順] - コマンドを指定せずに run_main() を呼び出す。

    // Assert
    EXPECT_NE(EXIT_SUCCESS, actual_rc); // [確認_異常系] - run_main() の戻り値が EXIT_SUCCESS 以外であること。
}

// --help 指定時にヘルプを表示して EXIT_SUCCESS を返すことの確認
TEST(serviceSampleTest, main_help_returns_success)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert

    // Act
    int actual_rc = run_main({"service-sample", "--help"}); // [手順] - "--help" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_rc); // [確認_正常系] - run_main() の戻り値が EXIT_SUCCESS であること。
}

// --help で使い方の表示、fflush、ferror のどれかが失敗した場合に診断を出して EXIT_FAILURE を返すことの確認
TEST(serviceSampleTest, help_returns_exit_failure_when_print_or_stream_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    {
        // Arrange
        NiceMock<Mock_stdio> mock_stdio; // [状態] - cplat_argparser_print_usage() が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout))
            .WillOnce(Return(
                CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
                                     // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_ERR_UNKNOWN を返す。

        // Act
        testing::internal::CaptureStderr();
        int actual_rc = run_main({"service-sample", "--help"}); // [手順] - "--help" を指定して run_main() を呼び出す。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(EXIT_FAILURE, actual_rc); // [確認_異常系] - run_main() の戻り値が EXIT_FAILURE であること。
        EXPECT_NE(
            std::string::npos,
            errors.find(
                "使い方を標準出力へ書き込めませんでした")); // [確認_異常系] - errors に診断メッセージが含まれること。
    }

    {
        // Arrange_2
        NiceMock<Mock_stdio> mock_stdio; // [状態] - fflush(stdout) が失敗する条件を用意する。

        // Pre-Assert_2
        EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout))
            .WillOnce(Return(
                CPLAT_OK)); // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
                            // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_OK を返す。
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout))
            .WillOnce(Return(EOF)); // [Pre-Assert確認_異常系] - fflush() が stdout を指定して 1 回呼び出されること。
                                    // [Pre-Assert手順] - fflush() から EOF を返す。

        // Act_2
        testing::internal::CaptureStderr();
        int actual_rc = run_main({"service-sample", "--help"}); // [手順] - "--help" を指定して run_main() を呼び出す。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert_2
        EXPECT_EQ(EXIT_FAILURE, actual_rc); // [確認_異常系] - run_main() の戻り値が EXIT_FAILURE であること。
        EXPECT_NE(
            std::string::npos,
            errors.find(
                "使い方を標準出力へ書き込めませんでした")); // [確認_異常系] - errors に診断メッセージが含まれること。
    }

    {
        // Arrange_3
        NiceMock<Mock_stdio> mock_stdio; // [状態] - ferror(stdout) がエラーを示す条件を用意する。

        // Pre-Assert_3
        EXPECT_CALL(mock_cplat, cplat_argparser_print_usage(stdout))
            .WillOnce(Return(
                CPLAT_OK)); // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
                            // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_OK を返す。
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout))
            .WillOnce(Return(0)); // [Pre-Assert確認_異常系] - fflush() が stdout を指定して 1 回呼び出されること。
                                  // [Pre-Assert手順] - fflush() から 0 を返す。
        EXPECT_CALL(mock_stdio, ferror(_, _, _, stdout))
            .WillOnce(Return(1)); // [Pre-Assert確認_異常系] - ferror() が stdout を指定して 1 回呼び出されること。
                                  // [Pre-Assert手順] - ferror() から 1 を返す。

        // Act_3
        testing::internal::CaptureStderr();
        int actual_rc = run_main({"service-sample", "--help"}); // [手順] - "--help" を指定して run_main() を呼び出す。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert_3
        EXPECT_EQ(EXIT_FAILURE, actual_rc); // [確認_異常系] - run_main() の戻り値が EXIT_FAILURE であること。
        EXPECT_NE(
            std::string::npos,
            errors.find(
                "使い方を標準出力へ書き込めませんでした")); // [確認_異常系] - errors に診断メッセージが含まれること。
    }
}

// 不明なコマンドで EXIT_FAILURE を返すことの確認
TEST(serviceSampleTest, main_unknown_command_returns_failure)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert

    // Act
    int actual_rc =
        run_main({"service-sample", "unknown"}); // [手順] - 不明なコマンド "unknown" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_rc); // [確認_異常系] - run_main() の戻り値が EXIT_FAILURE であること。
}

// install コマンドで run_piped 昇格再実行済みなら cplat_service_install を呼ばず子終了コードを返すことの確認
TEST(serviceSampleTest, main_install_returns_elevated_exit_code_when_handled)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_elevated_process_run_piped(StrEq("install"), nullptr, nullptr, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>(0), SetArgPointee<4>(1),
            Return(
                CPLAT_OK))); // [Pre-Assert確認_正常系] - cplat_elevated_process_run_piped() が "install" を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_elevated_process_run_piped() から終了コード 0・handled=1 を設定して CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_service_install(_, _))
        .Times(
            0); // [Pre-Assert確認_正常系] - cplat_service_install() が呼び出されないこと。(昇格済みのため再呼び出ししないことの確認)

    // Act
    int actual_rc = run_main({"service-sample", "install"}); // [手順] - "install" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_rc); // [確認_正常系] - run_main() の戻り値が EXIT_SUCCESS であること。
}

// install コマンドで現プロセス継続時に cplat_service_install へ委譲することの確認
TEST(serviceSampleTest, main_install_delegates_to_cplat_service_install)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_elevated_process_run_piped(StrEq("install"), nullptr, nullptr, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>(0), SetArgPointee<4>(0),
            Return(
                CPLAT_OK))); // [Pre-Assert確認_正常系] - cplat_elevated_process_run_piped() が "install" を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_elevated_process_run_piped() から handled=0 を設定して CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_service_install(NotNull(), nullptr))
        .WillOnce(Return(
            CPLAT_OK)); // [Pre-Assert確認_正常系] - cplat_service_install() が nullptr 以外、nullptr を指定して 1 回呼び出されること。
                        // [Pre-Assert手順] - cplat_service_install() から CPLAT_OK を返す。

    // Act
    int actual_rc = run_main({"service-sample", "install"}); // [手順] - "install" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_rc); // [確認_正常系] - run_main() の戻り値が EXIT_SUCCESS であること。
}

// tracer が既定のファイル パスと共有モードを使うことの確認
TEST(serviceSampleTest, tracer_uses_default_file_path_with_shared_mode)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。
    ON_CALL(mock_cplat, cplat_elevated_process_run_piped(_, _, _, _, _))
        .WillByDefault(DoAll(SetArgPointee<3>(0), SetArgPointee<4>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_service_install(_, _)).WillByDefault(Return(CPLAT_OK));

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_tracer_set_file_level(tracer_handle, nullptr, CPLAT_TRACE_LEVEL_VERBOSE, 0U, 0,
                                                        CPLAT_TRACE_FILE_SINK_SHARED))
        .WillOnce(Return(
            CPLAT_OK)); // [Pre-Assert確認_正常系] - cplat_tracer_set_file_level() が tracer_handle、nullptr、CPLAT_TRACE_LEVEL_VERBOSE、0U、0、CPLAT_TRACE_FILE_SINK_SHARED を指定して 1 回呼び出されること。
                        // [Pre-Assert手順] - cplat_tracer_set_file_level() から CPLAT_OK を返す。

    // Act
    int actual_rc = run_main({"service-sample", "install"}); // [手順] - "install" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_rc); // [確認_正常系] - run_main() の戻り値が EXIT_SUCCESS であること。
}

// uninstall コマンドで現プロセス継続時に cplat_service_uninstall へ委譲することの確認
TEST(serviceSampleTest, main_uninstall_delegates_to_cplat_service_uninstall)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_elevated_process_run_piped(StrEq("uninstall"), nullptr, nullptr, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>(0), SetArgPointee<4>(0),
            Return(
                CPLAT_OK))); // [Pre-Assert確認_正常系] - cplat_elevated_process_run_piped() が "uninstall" を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_elevated_process_run_piped() から handled=0 を設定して CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_service_uninstall(NotNull()))
        .WillOnce(Return(
            CPLAT_OK)); // [Pre-Assert確認_正常系] - cplat_service_uninstall() が nullptr 以外を指定して 1 回呼び出されること。
                        // [Pre-Assert手順] - cplat_service_uninstall() から CPLAT_OK を返す。

    // Act
    int actual_rc =
        run_main({"service-sample", "uninstall"}); // [手順] - "uninstall" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_rc); // [確認_正常系] - run_main() の戻り値が EXIT_SUCCESS であること。
}

// run コマンドで prepare_run -> cplat_tracer_create -> cplat_service_run の順に呼ばれ exit_code が返ることの確認
TEST(serviceSampleTest, main_run_calls_prepare_run_and_cplat_service_run)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert
    {
        InSequence sequence;
        EXPECT_CALL(mock_cplat, cplat_service_prepare_run())
            .WillOnce(Return(
                CPLAT_OK)); // [Pre-Assert確認_正常系] - cplat_service_prepare_run() が最初に 1 回呼び出されること。
                            // [Pre-Assert手順] - cplat_service_prepare_run() から CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED))
            .WillOnce(Return(
                tracer_handle)); // [Pre-Assert確認_正常系] - cplat_tracer_create() が prepare_run の後に 1 回呼び出されること。
                                 // [Pre-Assert手順] - cplat_tracer_create() から tracer_handle を返す。
        EXPECT_CALL(mock_cplat, cplat_service_run(NotNull(), NotNull()))
            .WillOnce(DoAll(
                SetArgPointee<1>(0),
                Return(
                    CPLAT_OK))); // [Pre-Assert確認_正常系] - cplat_service_run() が tracer 生成後に 1 回呼び出されること。
                                 // [Pre-Assert手順] - cplat_service_run() から終了コード 0 を設定して CPLAT_OK を返す。
    }

    // Act
    int actual_rc = run_main({"service-sample", "run"}); // [手順] - "run" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_rc); // [確認_正常系] - run_main() の戻り値が EXIT_SUCCESS であること。
}

// run の準備が失敗した場合に、トレーサーを生成せず EXIT_FAILURE を返すことの確認
TEST(serviceSampleTest, run_fails_when_prepare_run_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_service_prepare_run())
        .WillOnce(Return(
            CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - cplat_service_prepare_run() が 1 回呼び出されること。
                                 // [Pre-Assert手順] - cplat_service_prepare_run() から CPLAT_ERR_UNKNOWN を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_create(_))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_tracer_create() が呼び出されないこと。
    EXPECT_CALL(mock_cplat, cplat_service_run(_, _))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_service_run() が呼び出されないこと。

    // Act
    testing::internal::CaptureStderr();
    int actual_rc = run_main({"service-sample", "run"}); // [手順] - "run" を指定して run_main() を呼び出す。
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_rc); // [確認_異常系] - run_main() の戻り値が EXIT_FAILURE であること。
    EXPECT_NE(
        std::string::npos,
        errors.find(
            "エラー: run コマンドの実行の準備に失敗しました。")); // [確認_異常系] - errors に準備失敗の診断メッセージが含まれること。
}

// install、uninstall、console では cplat_service_prepare_run を呼ばないことの確認
TEST(serviceSampleTest, prepare_run_is_not_called_for_install_uninstall_console)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_elevated_process_run_piped(_, _, _, _, _))
        .WillByDefault(DoAll(SetArgPointee<3>(0), SetArgPointee<4>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_service_install(_, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_service_uninstall(_)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_service_run_console(_, _)).WillByDefault(DoAll(SetArgPointee<1>(0), Return(CPLAT_OK)));

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_service_prepare_run())
        .Times(0); // [Pre-Assert確認_正常系] - cplat_service_prepare_run() が呼び出されないこと。

    // Act
    int install_rc = run_main({"service-sample", "install"}); // [手順] - "install" を指定して run_main() を呼び出す。
    int uninstall_rc =
        run_main({"service-sample", "uninstall"});            // [手順] - "uninstall" を指定して run_main() を呼び出す。
    int console_rc = run_main({"service-sample", "console"}); // [手順] - "console" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, install_rc); // [確認_正常系] - install の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(EXIT_SUCCESS,
              uninstall_rc);             // [確認_正常系] - uninstall の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(EXIT_SUCCESS, console_rc); // [確認_正常系] - console の run_main() の戻り値が EXIT_SUCCESS であること。
}

// console コマンドで cplat_service_run_console へ委譲することの確認
TEST(serviceSampleTest, main_console_delegates_to_cplat_service_run_console)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(
            SetArgPointee<2>(0),
            Return(
                CPLAT_OK))); // [状態] - cplat_elevated_process_attach_output_pipes() から CPLAT_OK を返すように既定動作を設定する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_service_run_console(NotNull(), NotNull()))
        .WillOnce(
            DoAll(SetArgPointee<1>(5),
                  Return(CPLAT_OK))); // [Pre-Assert確認_正常系] - cplat_service_run_console() が 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_service_run_console() から終了コード 5 を設定して CPLAT_OK を返す。

    // Act
    int actual_rc = run_main({"service-sample", "console"}); // [手順] - "console" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(5, actual_rc); // [確認_正常系] - run_main() の戻り値が 5 であること。
}

// ファイルへのトレースを設定できない場合に、診断を出して処理を続けることの確認
TEST(serviceSampleTest, console_continues_when_file_trace_setting_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(CPLAT_OK)));

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_tracer_set_file_level(_, _, _, _, _, _))
        .WillOnce(Return(
            CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - cplat_tracer_set_file_level() が 1 回呼び出されること。
                                 // [Pre-Assert手順] - cplat_tracer_set_file_level() から CPLAT_ERR_UNKNOWN を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_start(tracer_handle))
        .WillOnce(Return(CPLAT_OK)); // [Pre-Assert確認_異常系] - cplat_tracer_start() が 1 回呼び出されること。
                                     // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_service_run_console(NotNull(), NotNull()))
        .WillOnce(
            DoAll(SetArgPointee<1>(0),
                  Return(CPLAT_OK))); // [Pre-Assert確認_異常系] - cplat_service_run_console() が 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_service_run_console() から終了コード 0 を設定して CPLAT_OK を返す。

    // Act
    testing::internal::CaptureStderr();
    int actual_rc = run_main({"service-sample", "console"}); // [手順] - "console" を指定して run_main() を呼び出す。
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_NE(
        std::string::npos,
        errors.find(
            "ファイルへのトレースを設定できませんでした")); // [確認_異常系] - errors にファイル トレース設定失敗の診断が含まれること。
    EXPECT_EQ(EXIT_SUCCESS, actual_rc); // [確認_異常系] - run_main() の戻り値が EXIT_SUCCESS であること。
}

// トレーサー開始に失敗しても状態が STARTED のとき、トレーサーを保持して処理を続けることの確認
TEST(serviceSampleTest, tracer_start_failure_keeps_started_tracer)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    const std::string start_diagnostic =
        "トレースを開始できませんでした (結果コード: " + std::to_string(CPLAT_ERR_UNKNOWN) + ")。";
    cplat_tracer *observed_console_tracer = nullptr;
    cplat_tracer *observed_install_tracer = nullptr;
    cplat_tracer *observed_run_tracer = nullptr;
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_elevated_process_run_piped(_, _, _, _, _))
        .WillByDefault(DoAll(SetArgPointee<3>(0), SetArgPointee<4>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_service_prepare_run()).WillByDefault(Return(CPLAT_OK));

    // Pre-Assert
    {
        InSequence sequence;
        EXPECT_CALL(mock_cplat, cplat_tracer_start(tracer_handle))
            .WillOnce(Return(
                CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - console で cplat_tracer_start() が 1 回呼び出されること。
                                     // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(
                delegate_real_fprintf)); // [Pre-Assert確認_異常系] - console で診断が stderr へ 1 回出力されること。
                                         // [Pre-Assert手順] - fprintf() は libc の処理へ委譲する。
        EXPECT_CALL(mock_cplat, cplat_tracer_get_state(tracer_handle))
            .WillOnce(Return(
                CPLAT_TRACER_STATE_STARTED)); // [Pre-Assert確認_異常系] - console で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STARTED を返す。
        EXPECT_CALL(mock_cplat, cplat_service_run_console(NotNull(), NotNull()))
            .WillOnce(Invoke(
                [&observed_console_tracer](const cplat_service_definition *def, int *exit_code)
                {
                    observed_console_tracer = def->tracer;
                    *exit_code = 7;
                    return CPLAT_OK;
                })); // [Pre-Assert確認_異常系] - cplat_service_run_console() が 1 回呼び出されること。
                     // [Pre-Assert手順] - cplat_service_run_console() から終了コード 7 を設定して CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_stop(tracer_handle))
            .WillOnce(Return(
                CPLAT_OK)); // [Pre-Assert確認_異常系] - console 終了時に cplat_tracer_stop() が 1 回呼び出されること。
                            // [Pre-Assert手順] - cplat_tracer_stop() から CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_dispose(_))
            .Times(1); // [Pre-Assert確認_異常系] - console 終了時に cplat_tracer_dispose() が 1 回呼び出されること。

        EXPECT_CALL(mock_cplat, cplat_tracer_start(tracer_handle))
            .WillOnce(Return(
                CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - install で cplat_tracer_start() が 1 回呼び出されること。
                                     // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(
                delegate_real_fprintf)); // [Pre-Assert確認_異常系] - install で診断が stderr へ 1 回出力されること。
                                         // [Pre-Assert手順] - fprintf() は libc の処理へ委譲する。
        EXPECT_CALL(mock_cplat, cplat_tracer_get_state(tracer_handle))
            .WillOnce(Return(
                CPLAT_TRACER_STATE_STARTED)); // [Pre-Assert確認_異常系] - install で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STARTED を返す。
        EXPECT_CALL(mock_cplat, cplat_service_install(NotNull(), nullptr))
            .WillOnce(Invoke(
                [&observed_install_tracer](const cplat_service_definition *def, const cplat_service_install_options *)
                {
                    observed_install_tracer = def->tracer;
                    return CPLAT_OK;
                })); // [Pre-Assert確認_異常系] - cplat_service_install() が 1 回呼び出されること。
                     // [Pre-Assert手順] - cplat_service_install() から CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_stop(tracer_handle))
            .WillOnce(Return(
                CPLAT_OK)); // [Pre-Assert確認_異常系] - install 終了時に cplat_tracer_stop() が 1 回呼び出されること。
                            // [Pre-Assert手順] - cplat_tracer_stop() から CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_dispose(_))
            .Times(1); // [Pre-Assert確認_異常系] - install 終了時に cplat_tracer_dispose() が 1 回呼び出されること。

        EXPECT_CALL(mock_cplat, cplat_tracer_start(tracer_handle))
            .WillOnce(Return(
                CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - run で cplat_tracer_start() が 1 回呼び出されること。
                                     // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(
                Invoke(delegate_real_fprintf)); // [Pre-Assert確認_異常系] - run で診断が stderr へ 1 回出力されること。
                                                // [Pre-Assert手順] - fprintf() は libc の処理へ委譲する。
        EXPECT_CALL(mock_cplat, cplat_tracer_get_state(tracer_handle))
            .WillOnce(Return(
                CPLAT_TRACER_STATE_STARTED)); // [Pre-Assert確認_異常系] - run で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STARTED を返す。
        EXPECT_CALL(mock_cplat, cplat_service_run(NotNull(), NotNull()))
            .WillOnce(Invoke(
                [&observed_run_tracer](const cplat_service_definition *def, int *exit_code)
                {
                    observed_run_tracer = def->tracer;
                    *exit_code = 0;
                    return CPLAT_OK;
                })); // [Pre-Assert確認_異常系] - cplat_service_run() が 1 回呼び出されること。
                     // [Pre-Assert手順] - cplat_service_run() から終了コード 0 を設定して CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_stop(tracer_handle))
            .WillOnce(Return(
                CPLAT_OK)); // [Pre-Assert確認_異常系] - run 終了時に cplat_tracer_stop() が 1 回呼び出されること。
                            // [Pre-Assert手順] - cplat_tracer_stop() から CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_dispose(_))
            .Times(1); // [Pre-Assert確認_異常系] - run 終了時に cplat_tracer_dispose() が 1 回呼び出されること。
    }

    // Act
    int console_rc = run_main({"service-sample", "console"}); // [手順] - "console" を指定して run_main() を呼び出す。
    int install_rc = run_main({"service-sample", "install"}); // [手順] - "install" を指定して run_main() を呼び出す。
    int run_rc = run_main({"service-sample", "run"});         // [手順] - "run" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(7, console_rc); // [確認_異常系] - console の run_main() の戻り値が 7 であること。
    EXPECT_EQ(tracer_handle,
              observed_console_tracer);  // [確認_異常系] - observed_console_tracer が tracer_handle と一致すること。
    EXPECT_EQ(EXIT_SUCCESS, install_rc); // [確認_異常系] - install の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(tracer_handle,
              observed_install_tracer); // [確認_異常系] - observed_install_tracer が tracer_handle と一致すること。
    EXPECT_EQ(EXIT_SUCCESS, run_rc);    // [確認_異常系] - run の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(tracer_handle,
              observed_run_tracer); // [確認_異常系] - observed_run_tracer が tracer_handle と一致すること。
}

// トレーサー開始に失敗し状態が STARTED でないとき、トレーサーを破棄して処理を続けることの確認
TEST(serviceSampleTest, tracer_start_failure_discards_unstarted_tracer)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    cplat_tracer *tracer_handle = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));
    const std::string start_diagnostic =
        "トレースを開始できませんでした (結果コード: " + std::to_string(CPLAT_ERR_UNKNOWN) + ")。";
    cplat_tracer *observed_console_tracer = tracer_handle;
    cplat_tracer *observed_install_tracer = tracer_handle;
    cplat_tracer *observed_run_tracer = tracer_handle;
    setup_tracer_defaults(mock_cplat, tracer_handle);
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_elevated_process_run_piped(_, _, _, _, _))
        .WillByDefault(DoAll(SetArgPointee<3>(0), SetArgPointee<4>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_service_prepare_run()).WillByDefault(Return(CPLAT_OK));

    // Pre-Assert
    {
        InSequence sequence;
        EXPECT_CALL(mock_cplat, cplat_tracer_start(tracer_handle))
            .WillOnce(Return(
                CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - console で cplat_tracer_start() が 1 回呼び出されること。
                                     // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(
                delegate_real_fprintf)); // [Pre-Assert確認_異常系] - console で診断が stderr へ 1 回出力されること。
                                         // [Pre-Assert手順] - fprintf() は libc の処理へ委譲する。
        EXPECT_CALL(mock_cplat, cplat_tracer_get_state(tracer_handle))
            .WillOnce(Return(
                CPLAT_TRACER_STATE_STOPPED)); // [Pre-Assert確認_異常系] - console で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STOPPED を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_stop(tracer_handle))
            .WillOnce(
                Return(CPLAT_OK)); // [Pre-Assert確認_異常系] - console で cplat_tracer_stop() が 1 回呼び出されること。
                                   // [Pre-Assert手順] - cplat_tracer_stop() から CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_dispose(_))
            .Times(1); // [Pre-Assert確認_異常系] - console で cplat_tracer_dispose() が 1 回呼び出されること。
        EXPECT_CALL(mock_cplat, cplat_service_run_console(NotNull(), NotNull()))
            .WillOnce(Invoke(
                [&observed_console_tracer](const cplat_service_definition *def, int *exit_code)
                {
                    observed_console_tracer = def->tracer;
                    *exit_code = 7;
                    return CPLAT_OK;
                })); // [Pre-Assert確認_異常系] - 破棄後に cplat_service_run_console() が 1 回呼び出されること。
                     // [Pre-Assert手順] - cplat_service_run_console() から終了コード 7 を設定して CPLAT_OK を返す。

        EXPECT_CALL(mock_cplat, cplat_tracer_start(tracer_handle))
            .WillOnce(Return(
                CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - install で cplat_tracer_start() が 1 回呼び出されること。
                                     // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(
                delegate_real_fprintf)); // [Pre-Assert確認_異常系] - install で診断が stderr へ 1 回出力されること。
                                         // [Pre-Assert手順] - fprintf() は libc の処理へ委譲する。
        EXPECT_CALL(mock_cplat, cplat_tracer_get_state(tracer_handle))
            .WillOnce(Return(
                CPLAT_TRACER_STATE_STOPPED)); // [Pre-Assert確認_異常系] - install で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STOPPED を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_stop(tracer_handle))
            .WillOnce(
                Return(CPLAT_OK)); // [Pre-Assert確認_異常系] - install で cplat_tracer_stop() が 1 回呼び出されること。
                                   // [Pre-Assert手順] - cplat_tracer_stop() から CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_dispose(_))
            .Times(1); // [Pre-Assert確認_異常系] - install で cplat_tracer_dispose() が 1 回呼び出されること。
        EXPECT_CALL(mock_cplat, cplat_service_install(NotNull(), nullptr))
            .WillOnce(Invoke(
                [&observed_install_tracer](const cplat_service_definition *def, const cplat_service_install_options *)
                {
                    observed_install_tracer = def->tracer;
                    return CPLAT_OK;
                })); // [Pre-Assert確認_異常系] - 破棄後に cplat_service_install() が 1 回呼び出されること。
                     // [Pre-Assert手順] - cplat_service_install() から CPLAT_OK を返す。

        EXPECT_CALL(mock_cplat, cplat_tracer_start(tracer_handle))
            .WillOnce(Return(
                CPLAT_ERR_UNKNOWN)); // [Pre-Assert確認_異常系] - run で cplat_tracer_start() が 1 回呼び出されること。
                                     // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(
                Invoke(delegate_real_fprintf)); // [Pre-Assert確認_異常系] - run で診断が stderr へ 1 回出力されること。
                                                // [Pre-Assert手順] - fprintf() は libc の処理へ委譲する。
        EXPECT_CALL(mock_cplat, cplat_tracer_get_state(tracer_handle))
            .WillOnce(Return(
                CPLAT_TRACER_STATE_STOPPED)); // [Pre-Assert確認_異常系] - run で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STOPPED を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_stop(tracer_handle))
            .WillOnce(
                Return(CPLAT_OK)); // [Pre-Assert確認_異常系] - run で cplat_tracer_stop() が 1 回呼び出されること。
                                   // [Pre-Assert手順] - cplat_tracer_stop() から CPLAT_OK を返す。
        EXPECT_CALL(mock_cplat, cplat_tracer_dispose(_))
            .Times(1); // [Pre-Assert確認_異常系] - run で cplat_tracer_dispose() が 1 回呼び出されること。
        EXPECT_CALL(mock_cplat, cplat_service_run(NotNull(), NotNull()))
            .WillOnce(Invoke(
                [&observed_run_tracer](const cplat_service_definition *def, int *exit_code)
                {
                    observed_run_tracer = def->tracer;
                    *exit_code = 0;
                    return CPLAT_OK;
                })); // [Pre-Assert確認_異常系] - 破棄後に cplat_service_run() が 1 回呼び出されること。
                     // [Pre-Assert手順] - cplat_service_run() から終了コード 0 を設定して CPLAT_OK を返す。
    }

    // Act
    int console_rc = run_main({"service-sample", "console"}); // [手順] - "console" を指定して run_main() を呼び出す。
    cplat_tracer *after_console = service_sample_get_tracer();
    int install_rc = run_main({"service-sample", "install"}); // [手順] - "install" を指定して run_main() を呼び出す。
    cplat_tracer *after_install = service_sample_get_tracer();
    int run_rc = run_main({"service-sample", "run"}); // [手順] - "run" を指定して run_main() を呼び出す。
    cplat_tracer *after_run = service_sample_get_tracer();

    // Assert
    EXPECT_EQ(7, console_rc);                    // [確認_異常系] - console の run_main() の戻り値が 7 であること。
    EXPECT_EQ(nullptr, observed_console_tracer); // [確認_異常系] - observed_console_tracer が nullptr であること。
    EXPECT_EQ(
        nullptr,
        after_console); // [確認_異常系] - console 後の service_sample_get_tracer() の戻り値が nullptr であること。
    EXPECT_EQ(EXIT_SUCCESS, install_rc); // [確認_異常系] - install の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(nullptr, observed_install_tracer); // [確認_異常系] - observed_install_tracer が nullptr であること。
    EXPECT_EQ(
        nullptr,
        after_install); // [確認_異常系] - install 後の service_sample_get_tracer() の戻り値が nullptr であること。
    EXPECT_EQ(EXIT_SUCCESS, run_rc);         // [確認_異常系] - run の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(nullptr, observed_run_tracer); // [確認_異常系] - observed_run_tracer が nullptr であること。
    EXPECT_EQ(nullptr,
              after_run); // [確認_異常系] - run 後の service_sample_get_tracer() の戻り値が nullptr であること。
}

// トレーサーを生成できないとき、開始と破棄を呼ばずに処理を続けることの確認
TEST(serviceSampleTest, tracer_create_failure_skips_start)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    ON_CALL(mock_cplat, cplat_elevated_process_attach_output_pipes(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_elevated_process_run_piped(_, _, _, _, _))
        .WillByDefault(DoAll(SetArgPointee<3>(0), SetArgPointee<4>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_service_prepare_run()).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_service_install(_, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_service_run(_, _)).WillByDefault(DoAll(SetArgPointee<1>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_service_run_console(_, _)).WillByDefault(DoAll(SetArgPointee<1>(0), Return(CPLAT_OK)));

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_tracer_create(_))
        .Times(3)
        .WillRepeatedly(Return(nullptr)); // [Pre-Assert確認_異常系] - cplat_tracer_create() が 3 回呼び出されること。
                                          // [Pre-Assert手順] - cplat_tracer_create() から nullptr を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_start(_))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_tracer_start() が呼び出されないこと。
    EXPECT_CALL(mock_cplat, cplat_tracer_get_state(_))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_tracer_get_state() が呼び出されないこと。
    EXPECT_CALL(mock_cplat, cplat_tracer_stop(_))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_tracer_stop() が呼び出されないこと。
    EXPECT_CALL(mock_cplat, cplat_tracer_dispose(_))
        .Times(0); // [Pre-Assert確認_異常系] - cplat_tracer_dispose() が呼び出されないこと。

    // Act
    int console_rc = run_main({"service-sample", "console"}); // [手順] - "console" を指定して run_main() を呼び出す。
    int install_rc = run_main({"service-sample", "install"}); // [手順] - "install" を指定して run_main() を呼び出す。
    int run_rc = run_main({"service-sample", "run"});         // [手順] - "run" を指定して run_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, console_rc); // [確認_異常系] - console の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(EXIT_SUCCESS, install_rc); // [確認_異常系] - install の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(EXIT_SUCCESS, run_rc);     // [確認_異常系] - run の run_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(
        nullptr,
        service_sample_get_tracer()); // [確認_異常系] - service_sample_get_tracer() の戻り値が nullptr であること。
}

// サンプル コールバック (on_start / on_run / on_stop / on_event / on_reload) の動作確認
TEST(serviceSampleTest, sample_callbacks_execute_cleanly)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    cplat_service_event_info ev = {CPLAT_SERVICE_EVENT_SESSION_LOGON, 0U, "100"};

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_service_wait_for_stop(1000))
        .WillOnce(Return(CPLAT_ERR_TIMEOUT))
        .WillOnce(Return(
            CPLAT_OK)); // [Pre-Assert確認_正常系] - cplat_service_wait_for_stop() が 1000 を指定して 2 回呼び出されること。
    // [Pre-Assert手順] - cplat_service_wait_for_stop() から 1 回目は CPLAT_ERR_TIMEOUT、2 回目は CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_service_set_status_text(NotNull()))
        .Times(
            1); // [Pre-Assert確認_正常系] - cplat_service_set_status_text() が nullptr 以外を指定して 1 回呼び出されること。

    // Act
    int actual_start = g_service_def.on_start(nullptr); // [手順] - g_service_def.on_start() を呼び出す。
    int actual_run = g_service_def.on_run(nullptr);     // [手順] - g_service_def.on_run() を呼び出す。
    int actual_stop = g_service_def.on_stop(nullptr);   // [手順] - g_service_def.on_stop() を呼び出す。
    g_service_def.on_event(&ev, nullptr);               // [手順] - g_service_def.on_event() を呼び出す。
    g_service_def.on_reload(nullptr);                   // [手順] - g_service_def.on_reload() を呼び出す。

    // Assert
    EXPECT_EQ(0, actual_start); // [確認_正常系] - g_service_def.on_start() の戻り値が 0 であること。
    EXPECT_EQ(0, actual_run);   // [確認_正常系] - g_service_def.on_run() の戻り値が 0 であること。
    EXPECT_EQ(0, actual_stop);  // [確認_正常系] - g_service_def.on_stop() の戻り値が 0 であること。
}
