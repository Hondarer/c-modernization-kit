/**
 *******************************************************************************
 *  @file           string_catalog_const.h
 *  @brief          string_catalog ライブラリの結果コードと上限値を定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  本ライブラリの共通結果コードと、引数個数の上限を定義します。\n
 *  結果コードの値は ABI として凍結し、追加はより小さい負値への追記だけとします。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef STRING_CATALOG_STRING_CATALOG_CONST_H
#define STRING_CATALOG_STRING_CATALOG_CONST_H

/**
 *  @ingroup        STRING_CATALOG_PUBLIC_API
 *  @{
 */

#define STRING_CATALOG_OK                   0  /**< 成功の戻り値を表します。 */
#define STRING_CATALOG_ERR                  -1 /**< 分類済みコードに該当しないその他のエラーです。 */
#define STRING_CATALOG_ERR_INVALID_ARGUMENT -2 /**< API 引数が不正です (NULL、容量 0、範囲外の言語など)。 */
#define STRING_CATALOG_ERR_NOT_FOUND        -3 /**< 指定した文字列 ID がカタログに存在しません。 */
#define STRING_CATALOG_ERR_INVALID_DEFINITION \
    -4                                   /**< カタログの定義が不正です (書式の構文誤り、引数種別の誤りなど)。 */
#define STRING_CATALOG_ERR_TRUNCATED -5 /**< 書き込み先の容量が不足し、結果を切り詰めました。 */

/**
 *  @brief          1 つの文字列が取り得る引数の最大個数です。
 *
 *  書式中の位置指定は `{0}` から `{9}` までとなります。\n
 *  書式解析は 1 桁の添字だけを受け付けるため、この値の上限は 10 です。\n
 *  10 を超える値を設定する場合は、書式解析を複数桁の添字へ対応させてください。
 */
#define STRING_CATALOG_ARGUMENT_MAX 10

/**
 *  @brief          サンプルで使用する文字列バッファーの推奨サイズです。
 *
 *  ライブラリ側の制限ではありません。呼び出し側が任意の容量を指定できます。
 */
#define STRING_CATALOG_TEXT_MAX 512

/** @} */

#endif /* STRING_CATALOG_STRING_CATALOG_CONST_H */
