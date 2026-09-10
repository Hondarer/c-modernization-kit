/**
 *******************************************************************************
 *  @file           message_catalog_entry.h
 *  @brief          利用者が注入するカタログ 1 件分の表現を定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  カタログはライブラリが抱え込まず、利用者が定義して
 *  @ref message_catalog_set_catalog で注入します。\n
 *  利用者が用意するのはメッセージ ID の列挙とこの型の配列の 2 つだけです。\n
 *  言語、引数種別、レベル、書式の構文はライブラリが定めます。
 *
 *  メッセージ ID の型を列挙にせず `int` としているのは、列挙を利用者側で定義できるようにするためです。\n
 *  利用者は任意の名前の列挙を定義し、その定数をそのまま渡せます。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef MESSAGE_CATALOG_MESSAGE_CATALOG_ENTRY_H
#define MESSAGE_CATALOG_MESSAGE_CATALOG_ENTRY_H

#include <message_catalog/message_catalog_argument.h>
#include <message_catalog/message_catalog_const.h>
#include <message_catalog/message_catalog_language.h>

/**
 *  @ingroup        MESSAGE_CATALOG_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          1 つのメッセージ ID が持つカタログの 1 件分です。
     *
     *  引数スキーマ、分類値、メタデータ、言語別の書式と備考を 1 つの表で保持します。\n
     *  @ref message_catalog_entry::arguments の先頭から
     *  @ref message_catalog_entry::argument_count 個までが有効です。
     *
     *  @ref message_catalog_entry::category はライブラリが解釈しない補足情報です。\n
     *  値の意味と有効な範囲は利用者が決めます。ライブラリは保持して返すだけです。
     *
     *  @ref message_catalog_entry::texts と @ref message_catalog_entry::notes は、
     *  言語を添字として引きます。\n
     *  ニュートラル言語以外の要素が NULL の場合は、ニュートラル言語の要素を使用します。\n
     *  ニュートラル言語の要素は NULL にできません。
     *
     *  この 2 つの配列は、@ref message_catalog_language をキーとした指示付き初期化子で記載できます。\n
     *  記載しなかった言語の要素は暗黙にヌル ポインターとなるため、リソースを持たない言語を省略できます。
     *
     *  @ref message_catalog_entry::pad は明示的アラインメントです。\n
     *  配列の初期化子では 0 を指定してください。
     */
    typedef struct message_catalog_entry
    {
        int id;             /**< メッセージ ID です。利用者の列挙の値を指定します。 */
        int category;       /**< 利用者が意味を決める分類値です。0 は分類なしを表します。 */
        int argument_count; /**< 引数の個数です。0 以上、上限以下です。 */
        unsigned int pad;   /**< 明示的アラインメントです。0 を指定します。 */
        message_catalog_argument_kind arguments[MESSAGE_CATALOG_ARGUMENT_MAX]; /**< 引数の種別です。 */
        const char *id_text;                                                   /**< メッセージ ID の固定文字列です。 */
        const char *texts[MESSAGE_CATALOG_LANGUAGE_COUNT]; /**< 言語別の書式です。NULL は自動選択です。 */
        const char *notes[MESSAGE_CATALOG_LANGUAGE_COUNT]; /**< 言語別の備考です。NULL は自動選択です。 */
    } message_catalog_entry;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* MESSAGE_CATALOG_MESSAGE_CATALOG_ENTRY_H */
