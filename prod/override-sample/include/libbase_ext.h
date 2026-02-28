/**
 *******************************************************************************
 *  @file           libbase_ext.h
 *  @brief          オーバーライドライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  このライブラリは libbase から動的にロードされ、\n
 *  処理を引き受けるオーバーライド関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBBASE_EXT_H
#define LIBBASE_EXT_H

/**
 *  @def            BASE_EXT_API
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                  | 値                       |
 *  | ----------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                    | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                   | (空)                     |
 *  | Windows / `BASE_EXT_STATIC` 定義時 (静的リンク)       | (空)                     |
 *  | Windows / `BASE_EXT_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
 *  | Windows / `BASE_EXT_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
 */

#ifndef _WIN32
    #define BASE_EXT_API
    #define WINAPI
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef BASE_EXT_STATIC
            #ifdef BASE_EXT_EXPORTS
                #define BASE_EXT_API __declspec(dllexport)
            #else /* BASE_EXT_EXPORTS */
                #define BASE_EXT_API __declspec(dllimport)
            #endif /* BASE_EXT_EXPORTS */
        #else      /* BASE_EXT_STATIC */
            #define BASE_EXT_API
        #endif /* BASE_EXT_STATIC */
    #else      /* __INTELLISENSE__ */
        #define BASE_EXT_API
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
     *  @brief          sample_func のオーバーライド実装。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @details
     *                  libbase の sample_func から動的にロードされ呼び出されます。\n
     *                  a * b を計算して result に格納します。
     *
     *  @par            使用例
        @code{.c}
        int result;
        if (override_func(1, 2, &result) == 0) {
            console_output("result: %d\n", result);  // 出力: result: 2
        }
        @endcode
     *
     *  @warning        result が NULL の場合は -1 を返します。
     *******************************************************************************
     */
    BASE_EXT_API extern int WINAPI override_func(const int a, const int b, int *result);

#ifdef __cplusplus
}
#endif

#endif /* LIBBASE_EXT_H */
