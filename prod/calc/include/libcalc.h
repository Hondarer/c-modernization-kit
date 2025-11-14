/**
 *******************************************************************************
 *  @file           libcalc.h
 *  @brief          計算ライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         doxygen-sample team
 *  @date           2025/01/30
 *  @version        1.0.0
 *
 *  このライブラリは基本的な整数演算を提供します。<br />
 *  動的リンクによる機能外提供用の API を模しています。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBCALC_H
#define LIBCALC_H

/* DLL エクスポート/インポート定義 */
#ifdef _WIN32
    #ifdef CALC_EXPORTS
        #define CALC_API __declspec(dllexport)
    #else
        #define CALC_API __declspec(dllimport)
    #endif
#else
    #define CALC_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup       calc_api Calculation API
 *  @brief          基本的な計算機能を提供します。
 */

/**
 *  @ingroup        calc_api
 *  @brief          加算の演算種別を表す定数。
 */
#define CALC_KIND_ADD 1

/**
 *******************************************************************************
 *  @ingroup        calc_api
 *  @brief          指定された演算種別に基づいて計算を実行します。
 *  @param[in]      kind 演算の種別 (CALC_KIND_ADD など)。
 *  @param[in]      a 第一オペランド。
 *  @param[in]      b 第二オペランド。
 *  @return         計算結果。kind が無効な場合は -1 を返します。
 *
 *  @details
 *  この関数は演算種別を受け取り、対応する計算関数を呼び出します。
 *  現在サポートされている演算種別は以下の通りです。
 *
 *  | 演算種別        | 説明             |
 *  | --------------- | ---------------- |
 *  | CALC_KIND_ADD   | 加算を実行       |
 *
 *  使用例:
 *
 *  @code{.c}
 *  int result = calcHandler(CALC_KIND_ADD, 10, 20);
 *  printf("Result: %d\n", result);  // 出力: Result: 30
 *  @endcode
 *
 *  @note           現在サポートされている演算は加算 (CALC_KIND_ADD) のみです。
 *  @warning        無効な kind を指定した場合、-1 を返します。
 *                  呼び出し側で戻り値のチェックを行ってください。
 *******************************************************************************
 */
CALC_API extern int calcHandler(int kind, int a, int b);

#ifdef __cplusplus
}
#endif

#endif /* LIBCALC_H */
