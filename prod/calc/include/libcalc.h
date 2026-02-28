/**
 *******************************************************************************
 *  @file           libcalc.h
 *  @brief          計算ライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  このライブラリは基本的な整数演算を提供します。\n
 *  動的リンクによる機能外提供用の API を模しています。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBCALC_H
#define LIBCALC_H

#include <libcalc_const.h>

/**
 *  @def            CALC_API
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                              | 値                       |
 *  | ------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時               | (空)                     |
 *  | Windows / `CALC_STATIC` 定義時 (静的リンク)       | (空)                     |
 *  | Windows / `CALC_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
 *  | Windows / `CALC_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
 */

#ifndef _WIN32
    #define CALC_API
    #define WINAPI
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef CALC_STATIC
            #ifdef CALC_EXPORTS
                #define CALC_API __declspec(dllexport)
            #else /* CALC_EXPORTS */
                #define CALC_API __declspec(dllimport)
            #endif /* CALC_EXPORTS */
        #else      /* CALC_STATIC */
            #define CALC_API
        #endif /* CALC_STATIC */
    #else      /* __INTELLISENSE__ */
        #define CALC_API
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
     *  @brief          指定された演算種別に基づいて計算を実行します。
     *  @param[in]      kind 演算の種別 (CALC_KIND_ADD など)。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は CALC_SUCCESS、失敗時は CALC_SUCCESS 以外の値を返します。
     *
     *  @details
     *  この関数は演算種別を受け取り、対応する計算関数を呼び出します。
     *  現在サポートされている演算種別は以下の通りです。
     *
     *  | 演算種別            | 説明             |
     *  | ------------------- | ---------------- |
     *  | CALC_KIND_ADD       | 加算を実行       |
     *  | CALC_KIND_SUBTRACT  | 減算を実行       |
     *  | CALC_KIND_MULTIPLY  | 乗算を実行       |
     *  | CALC_KIND_DIVIDE    | 除算を実行       |
     *
     *  @par            使用例
        @code{.c}
        int result;
        if (calcHandler(CALC_KIND_ADD, 10, 20, &result) == CALC_SUCCESS) {
            printf("Result: %d\n", result);  // 出力: Result: 30
        }
        @endcode
     *
     *  @warning        無効な kind を指定した場合、ゼロ除算の場合、
     *                  または result が NULL の場合は失敗を返します。
     *                  呼び出し側で戻り値のチェックを行ってください。
     *******************************************************************************
     */
    CALC_API extern int WINAPI calcHandler(const int kind, const int a, const int b, int *result);

#ifdef __cplusplus
}
#endif

#endif /* LIBCALC_H */
