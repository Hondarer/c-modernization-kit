/**
 *******************************************************************************
 *  @file           sample_filter_share_region.h
 *  @brief          配布に使う受け渡し用のメモリ領域と、その排他を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/27
 *  @version        0.1.0
 *
 *  受け渡し用のメモリ領域の確保、確保後の先頭アドレスの取得、および受け渡しの排他を、
 *  配布の手順 (`sample_filter_share.c`) から切り離したモジュールです。\n
 *  領域の方式や排他の方式を差し替える場合は、本ヘッダーの宣言を保ったまま、実装ファイル
 *  (`sample_filter_share_region.c`) を置き換えます。
 *
 *  本モジュールは領域の中身の型を知りません。先頭アドレスは `void *` で返し、利用者がキャストします。
 *
 *  PoC の実装では、共有メモリを calloc で確保したプロセス内の領域で模擬し、
 *  排他を単純なミューテックス (`cplat_local_lock`) で模擬します。
 *  同じ排他を共有する複数のハンドルを、別々のプロセスの書き込み側と読み取り側に見立てます。\n
 *  実際に複数のプロセスで動かす段階では、排他をプロセス間で共有できるものに置き換えます。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#ifndef SAMPLE_FILTER_SHARE_REGION_PRIVATE_H
#define SAMPLE_FILTER_SHARE_REGION_PRIVATE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 受け渡し用のメモリ領域 (不透明型)。 */
    typedef struct sample_filter_share_region sample_filter_share_region;

    /** 受け渡しの排他 (不透明型)。 */
    typedef struct sample_filter_share_lock sample_filter_share_lock;

    /**
     *  @brief          受け渡し用のメモリ領域を確保します。存在しない場合は作成します。
     *  @param[in]      path       領域を識別するパス。PoC の実装では、プロセス内で領域を識別する名前としてだけ使います。
     *  @param[in]      size       必要なバイト数。0 より大きい値を指定します。
     *  @param[out]     region_out 確保した領域の格納先。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が不正な場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         既存の領域が @p size に満たない場合は `CPLAT_ERR_CORRUPT_DESCRIPTOR` を返します。
     *  @return         メモリまたは領域を確保できない場合は `CPLAT_ERR_OUT_OF_MEMORY` またはその他の結果コードを返します。
     *
     *  新しく作成した領域は 0 で埋まっています。\n
     *  同じパスを、同じプロセスの中で複数回確保することもできます。確保した領域は同じ内容を共有します。\n
     *  PoC の実装では、領域はプロセス内にだけ存在し、最後の利用者が解放すると内容は失われます。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  領域の確保と解放は、同時に呼び出さないことを呼び出し側で保証してください。
     */
    int sample_filter_share_region_open(const char *path, size_t size, sample_filter_share_region **region_out);

    /**
     *  @brief          受け渡し用のメモリ領域を解放します。
     *  @param[in,out]  region 解放する領域を保持する変数のアドレス。解放したあとは NULL を設定します。
     *                         NULL または *region が NULL の場合は何もしません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  同じ領域への他の呼び出しが完了していること、および領域の確保と同時に呼び出さないことを、
     *  呼び出し側で保証してください。
     */
    void sample_filter_share_region_close(sample_filter_share_region **region);

    /**
     *  @brief          受け渡し用のメモリ領域の先頭アドレスを取得します。
     *  @param[in]      region 領域。
     *  @return         先頭アドレスを返します。少なくとも 8 バイト境界に揃っています。
     *                  領域を解放するまで有効です。@p region が NULL の場合は NULL を返します。
     *
     *  領域の中身の型は利用者が決め、戻り値をキャストして使います。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    void *sample_filter_share_region_get_address(const sample_filter_share_region *region);

    /**
     *  @brief          受け渡しの排他を作成します。
     *  @param[in]      path     排他を識別するパス。受け渡し用のメモリ領域と同じパスを指定します。
     *                           PoC の実装では使いません。プロセス間で共有する排他へ差し替える際の識別子です。
     *  @param[out]     lock_out 作成した排他の格納先。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         引数が NULL の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         作成できない場合は `CPLAT_ERR_OUT_OF_MEMORY` またはその他の結果コードを返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int sample_filter_share_lock_create(const char *path, sample_filter_share_lock **lock_out);

    /**
     *  @brief          受け渡しの排他を破棄します。
     *  @param[in,out]  lock 破棄する排他を保持する変数のアドレス。破棄したあとは NULL を設定します。
     *                       NULL または *lock が NULL の場合は何もしません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  排他を取得中のスレッドがないことを、呼び出し側で保証してください。
     */
    void sample_filter_share_lock_dispose(sample_filter_share_lock **lock);

    /**
     *  @brief          受け渡しの排他を取得します。取得できるまで待ちます。
     *  @param[in]      lock 排他。
     *  @return         成功時は `CPLAT_OK` を返します。
     *  @return         @p lock が NULL の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *  @return         取得できない場合は、その結果コードを返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int sample_filter_share_lock_acquire(sample_filter_share_lock *lock);

    /**
     *  @brief          受け渡しの排他を解放します。
     *  @param[in]      lock 排他。取得済みであること。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    void sample_filter_share_lock_release(sample_filter_share_lock *lock);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_FILTER_SHARE_REGION_PRIVATE_H */
