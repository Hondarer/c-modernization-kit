/**
 *******************************************************************************
 *  @file           calc_const.h
 *  @brief          calc ライブラリの定数ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef CALC_CONST_H
#define CALC_CONST_H

/**
 *  @defgroup       CALC_CONST 定数
 *  @ingroup        CALC_PUBLIC_API
 *  @brief          calc ライブラリで使用する定数です。
 *  @{
 */

#define CALC_SUCCESS 0  /**< 成功の戻り値を表します。 */
#define CALC_ERROR   -1 /**< 失敗の戻り値を表します。 */

#define CALC_KIND_ADD      1 /**< 加算の演算種別を表します。 */
#define CALC_KIND_SUBTRACT 2 /**< 減算の演算種別を表します。 */
#define CALC_KIND_MULTIPLY 3 /**< 乗算の演算種別を表します。 */
#define CALC_KIND_DIVIDE   4 /**< 除算の演算種別を表します。 */

/** @} */

#endif /* CALC_CONST_H */
