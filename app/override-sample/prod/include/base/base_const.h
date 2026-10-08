/**
 *******************************************************************************
 *  @file           base_const.h
 *  @brief          base ライブラリで使用する定数を定義します。
 *  @author         c-modernization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef BASE_CONST_H
#define BASE_CONST_H

/**
 *  @ingroup        BASE_PUBLIC_API
 *  @{
 */

#define BASE_OK                   0    /**< 成功の戻り値を表します。 */
#define BASE_ERR_UNKNOWN          (-1) /**< 分類済みコードに該当しないその他のエラーです。 */
#define BASE_ERR_INVALID_ARGUMENT (-2) /**< API 引数が不正です (NULL など)。 */

/** @} */

#endif /* BASE_CONST_H */
