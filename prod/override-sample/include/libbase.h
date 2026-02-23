/**
 *******************************************************************************
 *  @file           libbase.h
 *  @brief          ベースライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  このライブラリは動的ライブラリのオーバーライド機能を示すサンプルです。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBBASE_H
#define LIBBASE_H

#include <stddef.h>

/**
 *  @def            BASE_API
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                              | 値                       |
 *  | ------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時               | (空)                     |
 *  | Windows / `BASE_STATIC` 定義時 (静的リンク)       | (空)                     |
 *  | Windows / `BASE_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
 *  | Windows / `BASE_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
 */

/**
 *  @def            WINAPI
 *  @brief          Windows 呼び出し規約マクロ。
 *
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#ifndef _WIN32
    #define BASE_API
    #define WINAPI
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef BASE_STATIC
            #ifdef BASE_EXPORTS
                #define BASE_API __declspec(dllexport)
            #else /* BASE_EXPORTS */
                #define BASE_API __declspec(dllimport)
            #endif /* BASE_EXPORTS */
        #else      /* BASE_STATIC */
            #define BASE_API
        #endif /* BASE_STATIC */
    #else      /* __INTELLISENSE__ */
        #define BASE_API
    #endif /* __INTELLISENSE__ */
    #ifndef WINAPI
        #define WINAPI __stdcall
    #endif /* WINAPI */
#endif     /* _WIN32 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *******************************************************************************
     *  @brief          計算処理を行います。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @warning        result が NULL の場合は -1 を返します。
     *******************************************************************************
     */
    BASE_API extern int WINAPI sample_func(const int a, const int b, int *result);

    /**
     *******************************************************************************
     *  @brief          printf と同じ書式でコンソールに出力します。
     *  @param[in]      format printf 互換の書式文字列。
     *  @param[in]      ... 書式文字列に対応する引数。
     *
     *  @details        この関数は printf のラッパーです。\n
     *                  動的ライブラリ内から呼び出し元プロセスのコンソールに出力します。
     *
     *  @par            使用例
     *  @code{.c}
     *  console_output("result: %d\n", 42);  // 出力: result: 42
     *  @endcode
     *******************************************************************************
     */
    BASE_API extern void WINAPI console_output(const char *format, ...);

    /**
     *******************************************************************************
     *  @brief          指定した関数が所属する共有ライブラリ (.so/.dll) の絶対パスを取得します。
     *
     *  @param[out]     out_path    出力バッファ。
     *  @param[in]      out_path_sz 出力バッファサイズ[byte]。
     *  @param[in]      func_addr   所属モジュールを特定するための関数アドレス。
     *  @return         関数が成功した場合、0 を返します。失敗した場合は 0 以外を返します。
     *******************************************************************************
     */
    BASE_API extern int WINAPI get_lib_path(char *out_path, const size_t out_path_sz, const void *func_addr);

    /**
     *******************************************************************************
     *  @brief          指定した関数が所属する共有ライブラリ (.so/.dll) の basename (パスなし・拡張子なし)
     *                  を取得します。
     *
     *  Linux の .so.1.2.3 のようなバージョン付きは ".so." より前を basename とみなします。
     *
     *  @param[out]     out_basename    出力バッファ。
     *  @param[in]      out_basename_sz 出力バッファサイズ[byte]。
     *  @param[in]      func_addr       所属モジュールを特定するための関数アドレス。
     *  @return         関数が成功した場合、0 を返します。失敗した場合は 0 以外を返します。
     *******************************************************************************
     */
    BASE_API extern int WINAPI get_lib_basename(char *out_basename, const size_t out_basename_sz,
                                                const void *func_addr);

#ifdef __cplusplus
}
#endif

#endif /* LIBBASE_H */
