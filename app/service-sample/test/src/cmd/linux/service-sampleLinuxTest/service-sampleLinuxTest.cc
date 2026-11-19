#include <testfw.h>
#include <mock_cplat.h>
#include <mock_stdio.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "service-sample.h"

using testing::_;
using testing::AnyNumber;
using testing::DoAll;
using testing::HasSubstr;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;
using testing::SetArgPointee;
using testing::StrEq;

/** svc_os_install() に渡すサービス定義。 */
static const svc_definition g_test_def = {"service-sample-linux-test",
                                          "Service Sample Linux Test",
                                          "service-sample_linux.c の単体テスト用定義です。",
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr};

// ユニット ファイルを閉じる処理が失敗した場合に、ERROR のトレースを出して EXIT_FAILURE を返すことの確認
TEST(service_sampleLinuxTest, install_fails_when_unit_file_close_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    FILE *unit_file = tmpfile();
    ASSERT_NE(nullptr, unit_file); // [状態確認] - ユニット ファイルの代わりに書き込む一時ファイルを開けること。
    ON_CALL(mock_cplat, cplat_elevated_process_run_if_needed(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(0))); // [状態] - root 権限があり、別プロセスへ委譲しない状態とする。
    ON_CALL(mock_cplat, cplat_process_get_executable_path(_, _))
        .WillByDefault(Invoke(
            [](char *path, size_t size) -> int
            {
                (void)snprintf(path, size, "%s", "/usr/local/bin/service-sample");
                return CPLAT_OK;
            })); // [状態] - 実行ファイルのパスを返す。

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, HasSubstr("/service-sample-linux-test.service"), StrEq("w")))
        .WillOnce(Return(unit_file));
    // [Pre-Assert確認_異常系] - fopen() がユニット ファイルのパスと "w" を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - fopen() から一時ファイルを返す。
    EXPECT_CALL(mock_stdio, fclose(_, _, _, unit_file))
        .WillOnce(Invoke(
            [](const char *, const int, const char *, FILE *stream) -> int
            {
                (void)::fclose(stream);
                return EOF;
            }));
    // [Pre-Assert確認_異常系] - fclose() が一時ファイルを指定して 1 回呼び出されること。
    // [Pre-Assert手順] - 一時ファイルを閉じてから EOF を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(_, _, _, _)).Times(AnyNumber()).WillRepeatedly(Return(CPLAT_OK));
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _, HasSubstr("への書き込みに失敗しました")))
        .WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - 書き込みの失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_process_run_sync(_, _)).Times(0);
    // [Pre-Assert確認_異常系] - systemctl daemon-reload を実行しないこと。

    // Act
    int actual_ret = svc_os_install(&g_test_def); // [手順] - svc_os_install() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_install() の戻り値が EXIT_FAILURE であること。
}
