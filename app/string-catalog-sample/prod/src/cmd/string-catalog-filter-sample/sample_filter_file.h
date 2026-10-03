/**
 *******************************************************************************
 *  @file           sample_filter_file.h
 *  @brief          条件式リストのファイルの読み込みと書き出しを宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/10/03
 *  @version        0.1.0
 *
 *  条件式リストのファイルは、1 行に 1 つの条件式を書いた UTF-8 のテキストです。\n
 *  空行、空白だけの行、先頭が `#` の行は判定に使わず、コメントや一時的に無効にした条件を書けます。
 *  改行は LF と CRLF のどちらでも読み込み、先頭の BOM は読み飛ばします。
 *
 *  読み込みは、ファイルの各行をそのまま条件式リストの 1 行としてコンパイルします。
 *  そのため、診断情報の行の位置 (0 起点) に 1 を加えた値が、ファイルの行番号になります。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#ifndef SAMPLE_FILTER_FILE_PRIVATE_H
#define SAMPLE_FILTER_FILE_PRIVATE_H

#include <cplat/string_catalog/filter.h>

#include <stddef.h>

/** 読み込める条件式リストのファイルの行数の上限です。コメント行と空行を含みます。 */
#define SAMPLE_FILTER_FILE_LINE_MAX 4096U

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 条件式リストのファイルを読み込んだ結果です。 */
    typedef struct sample_filter_file_result
    {
        size_t line_count;      /**< ファイルの行数。コメント行と空行を含みます。 */
        size_t condition_count; /**< フィルター オブジェクトへ格納した条件式の数。 */
        size_t invalid_count;   /**< 無効にした行の総数。診断情報の格納先が足りない場合も、すべての数を数えます。 */
    } sample_filter_file_result;

    /**
     *  @brief          条件式リストのファイルを読み込み、フィルター オブジェクトへコンパイルします。
     *  @param[in]      path                ファイルのパス (UTF-8)。
     *  @param[in]      line_capacity       フィルター オブジェクトの行数の上限。
     *  @param[in]      line_width          フィルター オブジェクトの行幅。
     *  @param[out]     image               フィルター オブジェクトの格納先。失敗した場合は変更しません。
     *  @param[in]      image_size          @p image のバイト数。
     *  @param[out]     diagnostics         無効にした行の診断情報の格納先。ファイルの行の順に並びます。NULL を指定できます。
     *  @param[in]      diagnostic_capacity @p diagnostics の要素数。
     *  @param[out]     result_out          読み込んだ結果の格納先。NULL を指定できます。
     *  @return         成功時は `CPLAT_OK` を返します。無効にした行があっても成功です。
     *  @return         引数が不正な場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         ファイルを開けない、または読み取れない場合は `CPLAT_ERR_UNKNOWN` を返します。
     *  @return         ファイルの行数が @ref SAMPLE_FILTER_FILE_LINE_MAX を超える場合は `CPLAT_ERR_LIMIT_EXCEEDED` を返します。
     *  @return         そのほかの失敗は `cplat_string_catalog_filter_compile` と同じ結果コードを返します。
     *
     *  行幅を超える行は、その行だけを @ref CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_LIMIT_EXCEEDED として無効にします。
     *  行の途中で切り詰めると、別の条件式としてコンパイルされるおそれがあるためです。\n
     *  構文の誤りなど、コンパイルで無効にした行の扱いは `cplat_string_catalog_filter_compile` と同じです。
     *  名前の解決など、適用の時点で検出する誤りは含みません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int sample_filter_file_load(const char *path, size_t line_capacity, size_t line_width, void *image,
                                size_t image_size, cplat_string_catalog_filter_diagnostic *diagnostics,
                                size_t diagnostic_capacity, sample_filter_file_result *result_out);

    /**
     *  @brief          フィルター オブジェクトの条件式を、条件式リストのファイルへ書き出します。
     *  @param[in]      path       ファイルのパス (UTF-8)。既存のファイルは置き換えます。
     *  @param[in]      image      フィルター オブジェクト。
     *  @param[in]      image_size @p image のバイト数。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が不正な場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         @p image が検証に失敗した場合は、その結果コードを返します。
     *  @return         ファイルを開けない、または書き込めない場合は `CPLAT_ERR_UNKNOWN` を返します。
     *
     *  各行を復元した条件式を、LF 区切りで 1 行ずつ書き出します。
     *  書き出したファイルを読み込むと、元と同じフィルター オブジェクトになります。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int sample_filter_file_save(const char *path, const void *image, size_t image_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_FILTER_FILE_PRIVATE_H */
