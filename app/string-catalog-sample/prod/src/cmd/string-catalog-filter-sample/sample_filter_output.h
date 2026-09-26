/**
 *******************************************************************************
 *  @file           sample_filter_output.h
 *  @brief          条件式フィルターを通してトレースを出力する入口を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/26
 *  @version        0.1.0
 *
 *  条件式のいずれかの行に一致したトレースは、レベルを強制出力のレベルへ引き上げて出力します。\n
 *  一致しないトレースは、定義のレベルのまま出力し、出力先のしきい値で選別されます。
 *
 *  生成物の型付きラッパーは生成物の `{module}_write()` を直接呼び出すため、試作では使用できません。\n
 *  代わりに @ref sample_filter_output マクロが呼び出し位置と実行コンテキストを付与します。\n
 *  このマクロは引数の型を検査しません。引数スキーマと一致する型で渡してください。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#ifndef SAMPLE_FILTER_OUTPUT_PRIVATE_H
#define SAMPLE_FILTER_OUTPUT_PRIVATE_H

#include "sample_filter.h"
#include "sample_filter_share.h"
#include "sample_worker_context.h"

#include <cplat/crt/path.h>
#include <cplat/runtime/process.h>
#include <cplat/string_catalog/string_catalog.h>
#include <cplat/trace/tracer.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          出力に使用するカタログ、フィルター スロット、トレーサーを設定します。
     *  @param[in]      catalog トレース種別のカタログ。
     *  @param[in]      slot    @p catalog で作成したフィルター スロット。
     *  @param[in]      tracer  出力先のトレーサー。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が NULL の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *
     *  NULL をすべてに指定した場合は設定を解除します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。出力を開始する前に設定してください。
     */
    int sample_filter_output_configure(const cplat_string_catalog *catalog, sample_filter_slot *slot,
                                       cplat_tracer *tracer);

    /**
     *  @brief          条件式を共有メモリから取り込むための配布ハンドルを設定します。
     *  @param[in]      share 配布ハンドル。NULL の場合は共有メモリから取り込みません。
     *                        設定したスロットと、行数の上限と行幅が一致する必要があります。
     *
     *  設定した場合、@ref sample_filter_output_write は出力のたびに、出力の前に
     *  @ref sample_filter_share_refresh を呼び出します。
     *  公開内容が変わっていれば、出力したスレッドで取り込んでから判定します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。出力を開始する前に設定してください。
     */
    void sample_filter_output_set_share(sample_filter_share *share);

    /**
     *  @brief          条件式で判定したうえで、トレースを出力します。
     *  @param[in]      string_key 文字列キー。
     *  @param[in]      ...        引数スキーマに従う値。文脈引数 (`{40}` から `{45}`) を含みます。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         設定されていない場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         文字列の組み立てに失敗した場合は、その結果コードを返し、出力しません。
     *
     *  一致した場合のレベルは、分類値を `CPLAT_TRACE_LEVEL_TO_FORCE` で変換した値です。\n
     *  分類値が範囲外の場合は `CPLAT_TRACE_LEVEL_NONE` として扱います。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。フィルター スロットへの適用と並行して呼び出せます。
     */
    int sample_filter_output_write(int string_key, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 *  @brief          呼び出し位置と実行コンテキストを付与して、条件式フィルターを通したトレースを出力します。
 *  @param[in]      string_key 文字列キー。
 *
 *  利用者の引数に続けて、生成物の型付きラッパーと同じ順序で文脈引数を渡します。\n
 *  `{40}` から `{45}` は cplat が定める文脈引数、`{46}` は app が定義するラウンド トリップ ID です。\n
 *  app が定義するコンテキスト引数を増減した場合は、`catalog_settings.jsonc` と本マクロを同じ変更で更新してください。
 */
#define sample_filter_output(string_key, ...) \
    sample_filter_output_write((string_key), ##__VA_ARGS__, __FILE__, cplat_path_basename(__FILE__), \
                               (int32_t)__LINE__, __func__, cplat_process_get_pid(), cplat_process_get_tid(), \
                               sample_worker_next_sequence_number())

#endif /* SAMPLE_FILTER_OUTPUT_PRIVATE_H */
