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

#ifdef DOXYGEN

    /**
     *  @def            CALC_EXPORT
     *  @brief          DLL エクスポート/インポート制御マクロ。
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
    #define CALC_EXPORT

    /**
     *  @def            CALC_API
     *  @brief          呼び出し規約マクロ。
     *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *                  Linux (非 Windows) 環境では空に展開されます。
     */
    #define CALC_API

#else /* !DOXYGEN */

    #define UTIL_DLL_EXPORT_PREFIX CALC
    #include <util/base/dll_exports.h>
    #define CALC_EXPORT UTIL_DLL_EXPORT_VALUE
    #define CALC_API    UTIL_DLL_API_VALUE

#endif /* DOXYGEN */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

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
    CALC_EXPORT extern int CALC_API calcHandler(const int kind, const int a, const int b, int *result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCALC_H */
