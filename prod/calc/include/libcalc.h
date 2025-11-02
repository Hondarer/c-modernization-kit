/**
 *******************************************************************************
 *  @file           libcalc.h
 *  @brief          計算ライブラリのヘッダーファイル。
 *  @author         doxygen-sample team
 *  @date           2025/01/30
 *  @version        1.0.0
 *
 *  このライブラリは基本的な整数演算を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBCALC_H
#define LIBCALC_H

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
 *  @note           現在サポートされている演算は加算 (CALC_KIND_ADD) のみです。
 *******************************************************************************
 */
extern int calcHandler(int kind, int a, int b);

/**
 *******************************************************************************
 *  @ingroup        calc_api
 *  @brief          2 つの整数を加算します。
 *  @param[in]      a 第一オペランド。
 *  @param[in]      b 第二オペランド。
 *  @return         a と b の合計値。
 *******************************************************************************
 */
extern int add(int a, int b);

#ifdef __cplusplus
}
#endif

#endif /* LIBCALC_H */
