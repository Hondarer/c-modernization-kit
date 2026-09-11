/**
 *******************************************************************************
 *  @file           catalog.h
 *  @brief          注入されたカタログへの参照 API を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  本ヘッダーは string_catalog ライブラリ内の共有ヘッダーです。\n
 *  カタログの検索は `prod/libsrc/string_catalog/string_catalog_catalog.c` が担い、
 *  書式を展開する実装はこの境界を通してだけカタログを参照します。\n
 *  境界を設けることで、検索方式を変更しても展開側は影響を受けません。\n
 *  ライブラリはカタログを保持しません。呼び出し元が @ref string_catalog を渡します。\n
 *  see: app/string-catalog-sample/docs/architecture.md
 *
 *  カタログ 1 件分の型は公開ヘッダー `<string_catalog/string_catalog_entry.h>` を正とします。\n
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

#ifndef STRING_CATALOG_CATALOG_H
#define STRING_CATALOG_CATALOG_H

#include <string_catalog/string_catalog_entry.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          カタログの内容が参照できる形であることを確認します。
     *  @param[in]      catalog 確認するカタログ。NULL を渡せます。
     *  @return         参照できる場合は true を返します。
     *
     *  NULL、配列が NULL、要素数が負のいずれかであれば false を返します。\n
     *  カタログは利用者が静的初期化するため、参照する前にこの関数で形を確認します。\n
     *  書式や引数スキーマの妥当性は確認しません。@ref string_catalog_verify が担います。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    bool string_catalog_internal_is_usable(const string_catalog *catalog);

    /**
     *  @brief          カタログの件数を返します。
     *  @param[in]      catalog 参照するカタログ。NULL を渡せます。
     *  @return         文字列の件数です。0 以上を返します。
     *
     *  参照できないカタログでは 0 を返します。\n
     *  @ref string_catalog_internal_entry_at で走査するときの上限として使用します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    int string_catalog_internal_entry_count(const string_catalog *catalog);

    /**
     *  @brief          添字を指定してカタログの 1 件を取得します。
     *  @param[in]      catalog 参照するカタログ。NULL を渡せます。
     *  @param[in]      index   0 以上、件数未満の添字。
     *  @return         カタログの 1 件です。@p index が範囲外の場合は NULL を返します。
     *
     *  返すポインターは利用者が用意した領域を指します。ライブラリでは解放しません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    const string_catalog_entry *string_catalog_internal_entry_at(const string_catalog *catalog, int index);

    /**
     *  @brief          文字列 ID に対応するカタログの 1 件を取得します。
     *  @param[in]      catalog   検索するカタログ。NULL を渡せます。
     *  @param[in]      string_id 検索する文字列 ID。
     *  @return         カタログの 1 件です。見つからない場合は NULL を返します。
     *
     *  返すポインターは利用者が用意した領域を指します。ライブラリでは解放しません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    const string_catalog_entry *string_catalog_internal_find_entry(const string_catalog *catalog, int string_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRING_CATALOG_CATALOG_H */
