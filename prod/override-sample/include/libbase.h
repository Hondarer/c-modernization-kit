/**
 *******************************************************************************
 *  @file           libbase.h
 *  @brief          ベースライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  このライブラリは動的ライブラリのオーバーライド機能を示すサンプルです。\n
 *  useOverride フラグによって自身の処理と外部ライブラリへの委譲を切り替えます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBBASE_H
#define LIBBASE_H

/* DLL エクスポート/インポート定義 */
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
     *  @brief          useOverride フラグに従い計算処理を行います。
     *  @param[in]      useOverride 0 の場合は自身で処理、1 の場合は liboverride に委譲。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @details
     *  useOverride が 0 の場合、この関数自身が a + b を計算して result に格納します。\n
     *  useOverride が 1 の場合、liboverride.so (Linux) または liboverride.dll (Windows)
     *  を動的にロードし、func_override 関数に処理を委譲します。
     *
     *  @par            使用例
     *  @code{.c}
     *  int result;
     *  if (func(0, 1, 2, &result) == 0) {
     *      console_output("result: %d\n", result);  // 出力: result: 3
     *  }
     *  @endcode
     *
     *  @warning        result が NULL の場合は -1 を返します。
     *******************************************************************************
     */
    BASE_API extern int WINAPI func(const int useOverride, const int a, const int b, int *result);

    /**
     *******************************************************************************
     *  @brief          printf と同じ書式でコンソールに出力します。
     *  @param[in]      format printf 互換の書式文字列。
     *  @param[in]      ... 書式文字列に対応する引数。
     *
     *  @details
     *  この関数は printf のラッパーです。\n
     *  動的ライブラリ内から呼び出し元プロセスのコンソールに出力します。
     *
     *  @par            使用例
     *  @code{.c}
     *  console_output("result: %d\n", 42);  // 出力: result: 42
     *  @endcode
     *******************************************************************************
     */
    BASE_API extern void WINAPI console_output(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* LIBBASE_H */
