/**
 *******************************************************************************
 *  @file           sample_filter_warning.h
 *  @brief          型が合わない比較要素の警告を、表示用の文へ整える関数を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/10/07
 *  @version        0.1.0
 *
 *  cplat は警告を文字列キーと引数の位置で返し、人へ見せる文は利用側が作ります。\n
 *  このコマンドは、カタログの ID と引数の名前、種別を使って日本語の文にします。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#ifndef SAMPLE_FILTER_WARNING_PRIVATE_H
#define SAMPLE_FILTER_WARNING_PRIVATE_H

#include <cplat/string_catalog/filter.h>
#include <cplat/string_catalog/string_catalog.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          警告 1 件を、行番号と比較要素の番号を含む日本語の 1 文へ整えます。
     *  @param[in]      catalog   警告を返したスロットのカタログ。
     *  @param[in]      warning   警告。
     *  @param[out]     dest      文の格納先。
     *  @param[in]      dest_size @p dest のバイト数。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が NULL の場合、@p dest_size が 0 の場合、または警告の項目と引数がカタログにない場合は
     *                  `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         @p dest に収まらない場合は `CPLAT_ERR_BUFFER_TOO_SMALL` を返し、@p dest を空文字列にします。
     *
     *  行番号と比較要素の番号は 1 起点で表します。行番号は draft の一覧と同じ、編集中イメージの行です。
     */
    int sample_filter_warning_format(const cplat_string_catalog *catalog,
                                     const cplat_string_catalog_filter_warning *warning, char *dest,
                                     size_t dest_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_FILTER_WARNING_PRIVATE_H */
