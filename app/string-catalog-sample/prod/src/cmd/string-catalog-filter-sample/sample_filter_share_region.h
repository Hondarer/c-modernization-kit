/**
 *******************************************************************************
 *  @file           sample_filter_share_region.h
 *  @brief          配布に使う受け渡し用のメモリ領域と、その排他を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/27
 *  @version        0.2.0
 *
 *  受け渡し用のメモリ領域の確保、確保後の先頭アドレスの取得、および受け渡しの排他を、
 *  配布の手順 (`sample_filter_share.c`) から切り離したモジュールです。\n
 *  領域の方式や排他の方式を差し替える場合は、本ヘッダーの宣言を保ったまま、実装ファイル
 *  (`sample_filter_share_region.c`) を置き換えます。
 *
 *  本ヘッダーは cplat に依存しません。結果は本モジュールの @ref sample_filter_share_region_result で返します。\n
 *  本モジュールは領域の中身の型を知りません。先頭アドレスは `void *` で返し、利用者がキャストします。
 *
 *  現在の実装は、領域をファイルのメモリ マップで、排他をファイル ロックで実現し、複数のプロセスで共有します。\n
 *  ファイルは削除しません。OS の再起動を越えて残り、次に開いたプロセスが最後の公開内容を引き継ぎます。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#ifndef SAMPLE_FILTER_SHARE_REGION_PRIVATE_H
#define SAMPLE_FILTER_SHARE_REGION_PRIVATE_H

#include <stddef.h>

/** 排他を取得できるまで待つことを表す待ち時間です。 */
#define SAMPLE_FILTER_SHARE_WAIT_FOREVER (-1)

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 本モジュールの関数の結果です。 */
    typedef enum sample_filter_share_region_result
    {
        SAMPLE_FILTER_SHARE_REGION_OK = 0,               /**< 成功。 */
        SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT = 1, /**< 引数が不正。 */
        SAMPLE_FILTER_SHARE_REGION_SIZE_MISMATCH = 2,    /**< 既存の領域の大きさが、要求した大きさと一致しない。 */
        SAMPLE_FILTER_SHARE_REGION_TIMEOUT = 3,          /**< 待ち時間のうちに排他を取得できない。 */
        SAMPLE_FILTER_SHARE_REGION_FAILED = 4            /**< OS の操作の失敗やメモリ不足。 */
    } sample_filter_share_region_result;

    /** 受け渡し用のメモリ領域 (不透明型)。 */
    typedef struct sample_filter_share_region sample_filter_share_region;

    /** 受け渡しの排他 (不透明型)。 */
    typedef struct sample_filter_share_lock sample_filter_share_lock;

    /**
     *  @brief          受け渡し用のメモリ領域を開きます。存在しない場合は作成します。
     *  @param[in]      path       領域のファイルのパス。ローカルのファイル システム上のパスを指定します。
     *  @param[in]      size       必要なバイト数。0 より大きい値を指定します。
     *  @param[out]     region_out 開いた領域の格納先。
     *  @return         成功時は @ref SAMPLE_FILTER_SHARE_REGION_OK を返します。
     *  @return         引数が不正な場合は @ref SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT を返します。
     *  @return         既存の領域の大きさが @p size と一致しない場合は @ref SAMPLE_FILTER_SHARE_REGION_SIZE_MISMATCH
     *                  を返します。既存の領域は作り直しません。
     *  @return         領域を開けない場合は @ref SAMPLE_FILTER_SHARE_REGION_FAILED を返します。
     *
     *  新しく作成した領域は 0 で埋まっています。\n
     *  同じパスを複数のプロセス、または同じプロセスの複数のハンドルで開くと、同じ内容を共有します。\n
     *  複数のプロセスが同時に作成しないよう、同じパスの排他を取得した状態で呼び出してください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    sample_filter_share_region_result sample_filter_share_region_open(const char *path, size_t size,
                                                                      sample_filter_share_region **region_out);

    /**
     *  @brief          受け渡し用のメモリ領域を閉じます。ファイルは削除しません。
     *  @param[in,out]  region 閉じる領域を保持する変数のアドレス。閉じたあとは NULL を設定します。
     *                         NULL または *region が NULL の場合は何もしません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  同じ領域への他の呼び出しが完了していることを、呼び出し側で保証してください。
     */
    void sample_filter_share_region_close(sample_filter_share_region **region);

    /**
     *  @brief          受け渡し用のメモリ領域の先頭アドレスを取得します。
     *  @param[in]      region 領域。
     *  @return         先頭アドレスを返します。少なくとも 8 バイト境界に揃っています。
     *                  領域を閉じるまで有効です。@p region が NULL の場合は NULL を返します。
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
     *                           実装は、このパスに `.lock` を付けたファイルをロックに使います。
     *  @param[out]     lock_out 作成した排他の格納先。
     *  @return         成功時は @ref SAMPLE_FILTER_SHARE_REGION_OK を返します。
     *  @return         引数が NULL の場合は @ref SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT を返します。
     *  @return         作成できない場合は @ref SAMPLE_FILTER_SHARE_REGION_FAILED を返します。
     *
     *  同じパスで作成した排他は、プロセスの間と、同じ排他を使うスレッドの間の両方で排他になります。\n
     *  排他を保持したプロセスが異常終了した場合は、OS がファイル ロックを解放します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    sample_filter_share_region_result sample_filter_share_lock_create(const char *path,
                                                                      sample_filter_share_lock **lock_out);

    /**
     *  @brief          受け渡しの排他を破棄します。ロックのファイルは削除しません。
     *  @param[in,out]  lock 破棄する排他を保持する変数のアドレス。破棄したあとは NULL を設定します。
     *                       NULL または *lock が NULL の場合は何もしません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  排他を取得中のスレッドがないことを、呼び出し側で保証してください。
     */
    void sample_filter_share_lock_dispose(sample_filter_share_lock **lock);

    /**
     *  @brief          受け渡しの排他を取得します。
     *  @param[in]      lock       排他。
     *  @param[in]      timeout_ms 待ち時間 (ミリ秒)。@ref SAMPLE_FILTER_SHARE_WAIT_FOREVER で取得できるまで待ちます。
     *                             0 の場合は待ちません。
     *  @return         成功時は @ref SAMPLE_FILTER_SHARE_REGION_OK を返します。
     *  @return         @p lock が NULL の場合、または待ち時間が負の値 (@ref SAMPLE_FILTER_SHARE_WAIT_FOREVER を除く) の場合は
     *                  @ref SAMPLE_FILTER_SHARE_REGION_INVALID_ARGUMENT を返します。
     *  @return         待ち時間のうちに取得できない場合は @ref SAMPLE_FILTER_SHARE_REGION_TIMEOUT を返します。
     *  @return         そのほかの失敗は @ref SAMPLE_FILTER_SHARE_REGION_FAILED を返します。
     *
     *  再入には対応しません。同じスレッドから取得済みの排他を取得しないでください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    sample_filter_share_region_result sample_filter_share_lock_acquire(sample_filter_share_lock *lock, int timeout_ms);

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
