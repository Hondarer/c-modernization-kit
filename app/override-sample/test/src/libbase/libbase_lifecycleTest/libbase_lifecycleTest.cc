#include <cplat/base/platform.h>
#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_cplat.h>
#if defined(PLATFORM_WINDOWS)
    #include <mock_windows.h>
#endif

#include <cplat/base/error.h>
#include <cplat/base/result.h>

#include <cerrno>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include "sym_loader_libbase.h"
#include "libbase_lifecycle.inject.h"

namespace
{

/** 定義ファイルのパスへ、空でないことが分かる値を入れます。 */
void fill_configpath(void)
{
    std::memset(sym_loader_configpath, 0, sizeof(sym_loader_configpath));
    std::memcpy(sym_loader_configpath, "previous", sizeof("previous"));
}

/** ロード時の診断の出力を有効にした状態の cplat_getenv() です。 */
int getenv_enabled(const char *, char *, size_t, int *found, cplat_error *)
{
    if (found != nullptr)
    {
        *found = 1;
    }
    return 0;
}

} // namespace

class libbase_lifecycleTest : public Test
{
  protected:
    NiceMock<Mock_cplat> mock_cplat_;
#if defined(PLATFORM_WINDOWS)
    NiceMock<Mock_windows> mock_windows_;
#endif
    std::vector<std::wstring> debug_messages_;

    // [サブ手順 名前=libbase_lifecycleTest.SetUp]
    void SetUp() override
    {
        fill_configpath(); // [状態] - 定義ファイルのパスへ空でない値を入れる。
        ON_CALL(mock_cplat_, cplat_sym_loader_init(_, _, _)).WillByDefault(Return(CPLAT_OK));
        // [状態] - cplat_sym_loader_init() から CPLAT_OK を返す。
        ON_CALL(mock_cplat_, cplat_getenv(_, _, _, _, _)).WillByDefault(Invoke(getenv_enabled));
        // [状態] - ロード時の診断の出力を有効にする。
#if defined(PLATFORM_WINDOWS)
        ON_CALL(mock_windows_, OutputDebugStringW(_, _, _, _))
            .WillByDefault(Invoke([this](const char *, const int, const char *, LPCWSTR text)
                                  { debug_messages_.push_back(text); }));
        // [状態] - OutputDebugStringW() へ渡された文面を記録する。
#endif
    }
    // [サブ手順終了]

    /** 記録した診断の文面に、指定した文字列を含むものがあるかを返します。 */
    bool has_debug_message(const wchar_t *text) const
    {
        for (const std::wstring &message : debug_messages_)
        {
            if (message.find(text) != std::wstring::npos)
            {
                return true;
            }
        }
        return false;
    }
};

// モジュールのベース名を取得できない場合に、定義ファイルのパスを空にすることの確認
// [サブ手順参照 名前=libbase_lifecycleTest.SetUp]
TEST_F(libbase_lifecycleTest, on_load_clears_config_path_when_basename_unavailable)
{
    // Arrange

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_module_get_basename(_, _, _)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
    // [Pre-Assert確認_異常系] - cplat_module_get_basename() が 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_module_get_basename() から CPLAT_ERR_UNKNOWN を返す。
    EXPECT_CALL(mock_cplat_, cplat_sym_loader_init(_, _, StrEq(""))).WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - cplat_sym_loader_init() が空のパスを指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_sym_loader_init() から CPLAT_OK を返す。

    // Act
    test_libbase_on_load(); // [手順] - onLoad() を呼び出す。

    // Assert
    EXPECT_STREQ("", sym_loader_configpath); // [確認_異常系] - 定義ファイルのパスが空であること。
#if defined(PLATFORM_WINDOWS)
    EXPECT_TRUE(has_debug_message(L"base: module basename unavailable; override disabled"));
    // [確認_異常系] - ベース名を取得できないことを診断へ出すこと。(Windows)
#endif
}

// 一時ディレクトリを取得できない場合に、定義ファイルのパスを空にすることの確認
// [サブ手順参照 名前=libbase_lifecycleTest.SetUp]
TEST_F(libbase_lifecycleTest, on_load_clears_config_path_when_temp_dir_unavailable)
{
    // Arrange
    ON_CALL(mock_cplat_, cplat_module_get_basename(_, _, _))
        .WillByDefault(Invoke(
            [](char *name, size_t size, const void *) -> int
            {
                if (size < sizeof("libbase"))
                {
                    return CPLAT_ERR_BUFFER_TOO_SMALL;
                }
                std::memcpy(name, "libbase", sizeof("libbase"));
                return CPLAT_OK;
            })); // [状態] - モジュールのベース名として libbase を返す。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_path_get_temp_dir(_, _, _))
        .WillOnce(Invoke(
            [](char *, size_t, cplat_error *error) -> int
            {
                cplat_error_capture_errno(error, EIO);
                return CPLAT_ERR_UNKNOWN;
            }));
    // [Pre-Assert確認_異常系] - cplat_path_get_temp_dir() が 1 回呼び出されること。
    // [Pre-Assert手順] - 名前が長すぎる以外の原因 (EIO) を記録して CPLAT_ERR_UNKNOWN を返す。
    EXPECT_CALL(mock_cplat_, cplat_sym_loader_init(_, _, StrEq(""))).WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - cplat_sym_loader_init() が空のパスを指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_sym_loader_init() から CPLAT_OK を返す。

    // Act
    test_libbase_on_load(); // [手順] - onLoad() を呼び出す。

    // Assert
    EXPECT_STREQ("", sym_loader_configpath); // [確認_異常系] - 定義ファイルのパスが空であること。
#if defined(PLATFORM_WINDOWS)
    EXPECT_TRUE(has_debug_message(L"base: temp dir unavailable; override disabled"));
    // [確認_異常系] - 一時ディレクトリを取得できないことを診断へ出すこと。(Windows)
#endif
}

#if defined(PLATFORM_WINDOWS)
// ワイド文字列への変換に失敗した場合に、OutputDebugStringW() を呼ばないことの確認 (Windows)
// [サブ手順参照 名前=libbase_lifecycleTest.SetUp]
TEST_F(libbase_lifecycleTest, info_msg_skips_output_when_conversion_fails)
{
    // Arrange

    // Pre-Assert
    EXPECT_CALL(mock_windows_, MultiByteToWideChar(_, _, _, CP_UTF8, 0U, _, -1, _, _))
        .WillOnce(Invoke(delegate_real_MultiByteToWideChar))
        .WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - MultiByteToWideChar() が大きさの取得と変換の合計 2 回呼び出されること。
    // [Pre-Assert手順] - 大きさの取得は実関数へ委譲し、変換では 0 を返す。
    EXPECT_CALL(mock_windows_, OutputDebugStringW(_, _, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - OutputDebugStringW() が呼び出されないこと。

    // Act
    test_libbase_info_msg("base: conversion test"); // [手順] - ロード時の診断の出力を呼び出す。

    // Assert
}
#endif
