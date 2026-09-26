/**
 *******************************************************************************
 *  @file           sample_filter_share.h
 *  @brief          コンパイル済みの条件を共有メモリで配布する仕組みを宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/27
 *  @version        0.1.0
 *
 *  書き込み側はフィルター オブジェクトを共有メモリへ公開するのみで、読み取り側のフィルター スロットを直接更新しません。\n
 *  読み取り側は、トレース出力の都度世代番号をロック不要で読み取り (緩いチェック)、変化が検知された場合に
 *  ロックを取得して再確認 (最終チェック) した後、フィルター オブジェクトを複製してフィルター スロットへ適用します。
 *
 *  書き込み側と、読み取り側の複製は、同じミューテックスで排他します。\n
 *  PoC では、呼び出し側が用意した単純なミューテックス (`cplat_local_lock`) で、プロセスをまたぐ排他を模擬します。
 *  同じミューテックスを共有する複数のハンドルを、別々のプロセスの書き込み側と読み取り側に見立てます。\n
 *  実際に複数のプロセスで動かす段階では、このミューテックスをプロセス間で共有できるものに置き換えます。
 *
 *  設計の理由は `app/string-catalog-sample/docs/trace-filter-poc.md` の「共有メモリによる配布」に記録しています。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#ifndef SAMPLE_FILTER_SHARE_PRIVATE_H
#define SAMPLE_FILTER_SHARE_PRIVATE_H

#include "sample_filter.h"

#include <cplat/sync/atomic.h>
#include <cplat/sync/sync.h>

#include <stddef.h>
#include <stdint.h>

/** 配布ヘッダーの署名です。メモリ上では "SCFS" の順に並びます (リトル エンディアン)。 */
#define SAMPLE_FILTER_SHARE_SIGNATURE 0x53464353U

/** 配布ヘッダーの形式版です。 */
#define SAMPLE_FILTER_SHARE_FORMAT_VERSION 1U

/** 配布ヘッダーのバイト数です。フィルター オブジェクトはこの位置から始まります。 */
#define SAMPLE_FILTER_SHARE_HEADER_SIZE 64U

/** 世代番号のうち、未公開を表す値です。 */
#define SAMPLE_FILTER_SHARE_GENERATION_NONE 0U

/**
 *  @brief          行数の上限と行幅から、共有メモリのバイト数を求めます。
 *  @param[in]      lines 行数の上限。
 *  @param[in]      width 行幅。
 */
#define SAMPLE_FILTER_SHARE_SIZE(lines, width) \
    (SAMPLE_FILTER_SHARE_HEADER_SIZE + SAMPLE_FILTER_IMAGE_SIZE(lines, width))

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          共有メモリの先頭に置く配布ヘッダーです。
     *
     *  複数のプロセスが同じレイアウトで読み書きするため、固定幅の整数型だけで構成します。\n
     *  @ref sample_filter_share_header::generation は、ロックを取らずにアトミックに読まれます。
     *  それ以外のメンバーは、ミューテックスの下でだけ読み書きします。
     */
    typedef struct sample_filter_share_header
    {
        uint32_t signature;            /**< @ref SAMPLE_FILTER_SHARE_SIGNATURE 。未初期化の間は 0。 */
        uint16_t format_version;       /**< @ref SAMPLE_FILTER_SHARE_FORMAT_VERSION */
        uint16_t header_size;          /**< @ref SAMPLE_FILTER_SHARE_HEADER_SIZE */
        uint32_t line_capacity;        /**< フィルター オブジェクトの行数の上限。 */
        uint32_t line_width;           /**< フィルター オブジェクトの行幅。 */
        uint64_t image_size;           /**< フィルター オブジェクトのバイト数。 */
        cplat_atomic_u64 generation;   /**< 世代番号。公開のたびに 1 増える。0 は未公開。 */
        int64_t published_seconds;     /**< 公開した実時刻の秒部 (Unix epoch からの経過秒)。 */
        int64_t published_nanoseconds; /**< 公開した実時刻のナノ秒部。 */
        uint32_t publisher_process_id; /**< 公開したプロセスの ID。 */
        uint8_t reserved[12];          /**< 予約。0 を格納します。 */
    } sample_filter_share_header;

    /**
     *  @brief          配布の状態です。@ref sample_filter_share_get_status が返します。
     */
    typedef struct sample_filter_share_status
    {
        uint64_t published_generation;  /**< 共有メモリの世代番号。未公開は 0。 */
        uint64_t taken_generation;      /**< このハンドルで取り込み済みの世代番号。未取り込みは 0。 */
        int64_t published_seconds;      /**< 公開した実時刻の秒部。未公開は 0。 */
        int64_t published_nanoseconds;  /**< 公開した実時刻のナノ秒部。 */
        uint32_t publisher_process_id;  /**< 公開したプロセスの ID。未公開は 0。 */
        int last_take_result;           /**< 直近の取り込みで適用した結果コード。未取り込みは `CPLAT_OK`。 */
        size_t last_take_invalid_count; /**< 直近の取り込みで無効にした行の数。 */
    } sample_filter_share_status;

    /** 共有メモリによる配布のハンドル (不透明型)。 */
    typedef struct sample_filter_share sample_filter_share;

    /**
     *  @brief          共有メモリを開きます。ファイルが存在しない場合は作成します。
     *  @param[in]      path          共有メモリに対応付けるファイルのパス。ローカル ファイル システム上を指定します。
     *  @param[in]      lock          書き込みと取り込みを排他するミューテックス。呼び出し側が所有し、
     *                                ハンドルを閉じるまで有効である必要があります。
     *                                同じ共有メモリを開くすべてのハンドルに、同じミューテックスを渡します。
     *  @param[in]      line_capacity 配布するフィルター オブジェクトの行数の上限。
     *  @param[in]      line_width    配布するフィルター オブジェクトの行幅。
     *  @param[out]     share_out     開いたハンドルの格納先。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が不正な場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         既存のファイルが必要な大きさに満たない場合は `CPLAT_ERR_CORRUPT_DESCRIPTOR` を返します。
     *  @return         メモリまたは共有メモリを確保できない場合は `CPLAT_ERR_OUT_OF_MEMORY` または
     *                  `CPLAT_ERR_UNKNOWN` を返します。
     *
     *  新しく作成したファイルは 0 で埋まっており、最初の公開で配布ヘッダーを初期化します。\n
     *  同じファイルを、同じプロセスの中で複数のハンドルから開くこともできます。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int sample_filter_share_open(const char *path, cplat_local_lock *lock, size_t line_capacity, size_t line_width,
                                 sample_filter_share **share_out);

    /**
     *  @brief          共有メモリを閉じます。ファイルは削除しません。
     *  @param[in,out]  share 閉じるハンドルを保持する変数のアドレス。閉じたあとは NULL を設定します。
     *                        NULL または *share が NULL の場合は何もしません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  同じハンドルへの他の呼び出しが完了していることを、呼び出し側で保証してください。
     */
    void sample_filter_share_close(sample_filter_share **share);

    /**
     *  @brief          フィルター オブジェクトを共有メモリへ公開します。
     *  @param[in]      share             ハンドル。
     *  @param[in]      image             公開するフィルター オブジェクト。
     *  @param[in]      image_size        @p image のバイト数。
     *  @param[out]     generation_out    公開した世代番号の格納先。NULL を指定できます。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が NULL の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         @p image が検証に失敗した場合、行数の上限と行幅がハンドルと一致しない場合、
     *                  または共有メモリの配布ヘッダーが別の形式の場合は `CPLAT_ERR_CORRUPT_DESCRIPTOR` を返します。
     *                  共有メモリは変更しません。
     *  @return         ロックを取れない場合は、その結果コードを返します。
     *
     *  読み取り側のフィルター スロットは直接更新しません。読み取り側は次回の取り込み時に世代の変化を検知して反映します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  同じハンドルと異なるハンドル、および別のプロセスからの公開と取り込みと並行して呼び出せます。
     */
    int sample_filter_share_publish(sample_filter_share *share, const void *image, size_t image_size,
                                    uint64_t *generation_out);

    /**
     *  @brief          公開内容が変わっていれば、フィルター スロットへ取り込みます。
     *  @param[in]      share           ハンドル。
     *  @param[in,out]  slot            取り込み先のフィルター スロット。行数の上限と行幅がハンドルと一致すること。
     *  @param[out]     is_taken_out    今回取り込んだ場合は 0 以外、変化がなかった場合は 0 の格納先。NULL を指定できます。
     *  @return         成功時は `CPLAT_OK` を返します。変化がなかった場合も成功です。
     *  @return         引数が NULL の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         ロックを取れない場合は、その結果コードを返します。
     *
     *  通常時は、世代番号をロック不要で 1 回読み取り、取り込み済みの世代と一致していれば直ちに関数を終了します。\n
     *  一致しない場合はロックを取得して世代を再確認し、変化していれば複製してスロットへ適用します。\n
     *  適用に失敗した場合でも取り込み済み世代を更新し、同一世代に対する取り込みの再試行を防ぎます。
     *  適用の結果は @ref sample_filter_share_get_status で確認できます。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  取り込みはミューテックスの下で直列に行われ、後から来たスレッドは先の取り込みの完了を待ちます。
     */
    int sample_filter_share_refresh(sample_filter_share *share, sample_filter_slot *slot, int *is_taken_out);

    /**
     *  @brief          配布の状態を取得します。
     *  @param[in]      share      ハンドル。
     *  @param[out]     status_out 状態の格納先。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が NULL の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         ロックを取れない場合は、その結果コードを返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int sample_filter_share_get_status(sample_filter_share *share, sample_filter_share_status *status_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_FILTER_SHARE_PRIVATE_H */
