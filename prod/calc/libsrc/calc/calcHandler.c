/**
 *******************************************************************************
 *  @file           calcHandler.c
 *  @brief          calcHandler 関数の実装ファイル。
 *  @author         doxygen-sample team
 *  @date           2025/01/30
 *  @version        1.0.0
 *
 *  演算種別に基づいて適切な計算関数を呼び出すハンドラーを提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalcbase.h>
#include <libcalc.h>

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
int calcHandler(int kind, int a, int b) {
    switch(kind){
        case CALC_KIND_ADD:
            return add(a,b);
        default:
            return -1;
    }
}
