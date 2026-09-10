/**
 *******************************************************************************
 *  @file           catalog.h
 *  @brief          注入されたカタログへの参照 API を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  本ヘッダーは message_catalog ライブラリ内の共有ヘッダーです。\n
 *  カタログの保持と検索は `prod/libsrc/message_catalog/message_catalog_catalog.c` が担い、
 *  書式を展開する実装はこの境界を通してだけカタログを参照します。\n
 *  境界を設けることで、カタログの保持方式を変更しても展開側は影響を受けません。\n
 *  see: app/message-catalog-sample/docs/architecture.md
 *
 *  カタログ 1 件分の型は公開ヘッダー `<message_catalog/message_catalog_entry.h>` を正とします。\n
 *  利用者が配列を組み立てるため、内部型にはできません。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef MESSAGE_CATALOG_CATALOG_H
#define MESSAGE_CATALOG_CATALOG_H

#include <message_catalog/message_catalog_entry.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          注入されているカタログの件数を返します。
     *  @return         メッセージの件数です。0 以上を返します。
     *
     *  カタログを注入していないプロセスでは 0 を返します。\n
     *  @ref message_catalog_internal_entry_at で走査するときの上限として使用します。
     *
     *  @par            スレッド セーフ
     *  本関数は、カタログの設定を変更しない限りスレッド セーフです。
     */
    int message_catalog_internal_entry_count(void);

    /**
     *  @brief          添字を指定してカタログの 1 件を取得します。
     *  @param[in]      index 0 以上、件数未満の添字。
     *  @return         カタログの 1 件です。@p index が範囲外の場合は NULL を返します。
     *
     *  返すポインターは利用者が注入した領域を指します。ライブラリでは解放しません。
     *
     *  @par            スレッド セーフ
     *  本関数は、カタログの設定を変更しない限りスレッド セーフです。
     */
    const message_catalog_entry *message_catalog_internal_entry_at(int index);

    /**
     *  @brief          メッセージ ID に対応するカタログの 1 件を取得します。
     *  @param[in]      message_id 検索するメッセージ ID。
     *  @return         カタログの 1 件です。注入されていない場合は NULL を返します。
     *
     *  返すポインターは利用者が注入した領域を指します。ライブラリでは解放しません。
     *
     *  @par            スレッド セーフ
     *  本関数は、カタログの設定を変更しない限りスレッド セーフです。
     */
    const message_catalog_entry *message_catalog_internal_find_entry(int message_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MESSAGE_CATALOG_CATALOG_H */
