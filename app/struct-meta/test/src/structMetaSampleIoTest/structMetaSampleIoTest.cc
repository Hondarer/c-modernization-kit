#include <cplat/base/platform.h>
#include <cplat/crt/path.h>
#include <cplat/crt/stdio.h>
#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_cplat.h>
#include <struct_meta/catalog/catalog.h>
#include <cerrno>
#include <cstring>
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
    FILE *binary = cplat_fopen_temp("sio", "w+b", binary_path, sizeof(binary_path), nullptr);
    ASSERT_NE(nullptr, json);   // [状態確認] - cplat_fopen_temp() が JSON 用の入力先を返すこと。
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
