/**
 *******************************************************************************
 *  @file           sample_filter_share.h
 *  @brief          コンパイル済みの条件を共有メモリで配布する仕組みを宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/27
 *  @version        0.2.0
 *
 *  共有メモリを cplat のソース領域として使い、書き込み側の公開を担います。\n
 *  公開の形式と、読み取り側の変化の検知と取り込みは cplat が行います。
 *  読み取り側は、@ref sample_filter_share_get_source が返す領域を
 *  `cplat_string_catalog_filter_slot_attach_source` でフィルター スロットへ結び付けます。
 *  以降は、トレース出力のたびにスロットが公開時刻を比べ、変化した場合に取り込みます。
 *
 *  書き込み側どうしは、同じ排他 (@ref sample_filter_share_lock) で直列化します。\n
 *  読み取り側にも @ref sample_filter_share_get_source_lock で同じ排他を渡し、二重確認で取り込みます。
 *  公開時刻の変化はロックを取らずに確かめ、変化した場合だけ排他を取って公開時刻を読み直してから複製します。\n
 *  受け渡し用のメモリ領域の確保と先頭アドレスの取得、および排他は `sample_filter_share_region.h` に切り出しており、
 *  方式の差し替えはそちらの実装で行います。
 *
 *  設計の理由は `string-catalog-filter-migration-design.md` の「条件の取り込みを公開時刻の比較で検知する」に記録しています。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#ifndef SAMPLE_FILTER_SHARE_PRIVATE_H
#define SAMPLE_FILTER_SHARE_PRIVATE_H

#include <cplat/string_catalog/filter.h>
#include "sample_filter_share_region.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 共有メモリによる配布のハンドル (不透明型)。 */
    typedef struct sample_filter_share sample_filter_share;

    /**
     *  @brief          共有メモリを開きます。存在しない場合は作成します。
     *  @param[in]      path          共有メモリを識別するパス。
     *  @param[in]      lock          書き込み側どうしの排他。呼び出し側が所有し、
     *                                ハンドルを閉じるまで有効である必要があります。
     *                                同じ共有メモリを開くすべてのハンドルに、同じ排他を渡します。
     *  @param[in]      line_capacity 配布するフィルター オブジェクトの行数の上限。
     *  @param[in]      line_width    配布するフィルター オブジェクトの行幅。
     *  @param[out]     share_out     開いたハンドルの格納先。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が不正な場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         既存の共有メモリが必要な大きさに満たない場合は `CPLAT_ERR_CORRUPT_DESCRIPTOR` を返します。
     *  @return         メモリまたは共有メモリを確保できない場合は `CPLAT_ERR_OUT_OF_MEMORY` または
     *                  `CPLAT_ERR_UNKNOWN` を返します。
     *
     *  共有メモリの大きさは `CPLAT_STRING_CATALOG_FILTER_SOURCE_SIZE` で求めます。\n
     *  新しく作成した共有メモリは 0 で埋まっており、cplat は未公開のソース領域として扱います。\n
     *  同じパスを、同じプロセスの中で複数のハンドルから開くこともできます。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  共有メモリを開く処理と閉じる処理は、同時に呼び出さないことを呼び出し側で保証してください。
     */
    int sample_filter_share_open(const char *path, sample_filter_share_lock *lock, size_t line_capacity,
                                 size_t line_width, sample_filter_share **share_out);

    /**
     *  @brief          共有メモリを閉じます。
     *  @param[in,out]  share 閉じるハンドルを保持する変数のアドレス。閉じたあとは NULL を設定します。
     *                        NULL または *share が NULL の場合は何もしません。
     *
     *  共有メモリをフィルター スロットへ結び付けている場合は、閉じる前に結び付けを解除してください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  同じハンドルへの他の呼び出しが完了していること、および共有メモリを開く処理と同時に呼び出さないことを、
     *  呼び出し側で保証してください。
     */
    void sample_filter_share_close(sample_filter_share **share);

    /**
     *  @brief          フィルター オブジェクトを共有メモリへ公開します。
     *  @param[in]      share         ハンドル。
     *  @param[in]      image         公開するフィルター オブジェクト。
     *  @param[in]      image_size    @p image のバイト数。
     *  @param[out]     timestamp_out 公開時刻の格納先。NULL を指定できます。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が NULL の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         @p image の行数の上限と行幅がハンドルと一致しない場合は `CPLAT_ERR_CORRUPT_DESCRIPTOR` を返します。
     *                  共有メモリは変更しません。
     *  @return         排他を取れない場合は、その結果コードを返します。
     *  @return         そのほかの失敗は `cplat_string_catalog_filter_source_publish` と同じ結果コードを返します。
     *
     *  読み取り側のフィルター スロットは直接更新しません。読み取り側は次のトレース出力で公開時刻の変化を検知して反映します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。書き込み側どうし、および排他を結び付けた読み取り側の複製とは、排他で直列化します。
     */
    int sample_filter_share_publish(sample_filter_share *share, const void *image, size_t image_size,
                                    uint64_t *timestamp_out);

    /**
     *  @brief          共有メモリをソース領域として取得します。
     *  @param[in]      share       ハンドル。
     *  @param[out]     size_out    ソース領域のバイト数の格納先。
     *  @return         ソース領域の先頭アドレスを返します。引数が NULL の場合は NULL を返します。
     *
     *  返した領域は、`cplat_string_catalog_filter_slot_attach_source` と
     *  `cplat_string_catalog_filter_source_get_info` へ渡せます。ハンドルを閉じるまで有効です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    const void *sample_filter_share_get_source(const sample_filter_share *share, size_t *size_out);

    /**
     *  @brief          書き込み側の排他を、読み取り側のフィルター スロットへ渡す形で取得します。
     *  @param[in]      share    ハンドル。
     *  @param[out]     lock_out 排他の関数の組の格納先。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が NULL の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *
     *  格納した内容は `cplat_string_catalog_filter_slot_attach_source` へ渡します。
     *  スロットは、公開時刻の変化を検知した場合だけこの排他を取り、複製の間に公開が重ならないようにします。\n
     *  排他は、ハンドルを開くときに渡したものです。結び付けを解除するまで破棄しないでください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int sample_filter_share_get_source_lock(const sample_filter_share *share,
                                            cplat_string_catalog_filter_source_lock *lock_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_FILTER_SHARE_PRIVATE_H */
