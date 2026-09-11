/**
 *******************************************************************************
 *  @file           string_catalog_entry.h
 *  @brief          利用者が注入するカタログ 1 件分の表現を定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  カタログはライブラリが抱え込まず、利用者が定義します。\n
 *  配列と添字表を @ref string_catalog へまとめ、組み立て API の呼び出しごとに渡します。\n
 *  利用者が用意するのは文字列 ID の列挙とこの型の配列の 2 つだけです。\n
 *  言語、引数種別、レベル、書式の構文はライブラリが定めます。
 *
 *  文字列 ID の型を列挙にせず `int` としているのは、列挙を利用者側で定義できるようにするためです。\n
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

#ifndef STRING_CATALOG_STRING_CATALOG_ENTRY_H
#define STRING_CATALOG_STRING_CATALOG_ENTRY_H

#include <string_catalog/string_catalog_argument.h>
#include <string_catalog/string_catalog_const.h>
#include <string_catalog/string_catalog_language.h>

/**
 *  @ingroup        STRING_CATALOG_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          1 つの文字列 ID が持つカタログの 1 件分です。
     *
     *  引数スキーマ、分類値、メタデータ、言語別の書式と備考を 1 つの表で保持します。\n
     *  @ref string_catalog_entry::arguments の先頭から
     *  @ref string_catalog_entry::argument_count 個までが有効です。
     *
     *  @ref string_catalog_entry::category はライブラリが解釈しない補足情報です。\n
     *  値の意味と有効な範囲は利用者が決めます。ライブラリは保持して返すだけです。
     *
     *  @ref string_catalog_entry::texts と @ref string_catalog_entry::notes は、
     *  言語を添字として引きます。\n
     *  ニュートラル言語以外の要素が NULL の場合は、ニュートラル言語の要素を使用します。\n
     *  ニュートラル言語の要素は NULL にできません。
     *
     *  この 2 つの配列は、@ref string_catalog_language をキーとした指示付き初期化子で記載できます。\n
     *  記載しなかった言語の要素は暗黙にヌル ポインターとなるため、リソースを持たない言語を省略できます。
     *
     *  @ref string_catalog_entry::pad は明示的アラインメントです。\n
     *  配列の初期化子では 0 を指定してください。
     */
    typedef struct string_catalog_entry
    {
        int id;             /**< 文字列 ID です。利用者の列挙の値を指定します。 */
        int category;       /**< 利用者が意味を決める分類値です。0 は分類なしを表します。 */
        int argument_count; /**< 引数の個数です。0 以上、上限以下です。 */
        unsigned int pad;   /**< 明示的アラインメントです。0 を指定します。 */
        string_catalog_argument_kind arguments[STRING_CATALOG_ARGUMENT_MAX]; /**< 引数の種別です。 */
        const char *id_text;                                                 /**< 文字列 ID の固定文字列です。 */
        const char *texts[STRING_CATALOG_LANGUAGE_COUNT]; /**< 言語別の書式です。NULL は自動選択です。 */
        const char *notes[STRING_CATALOG_LANGUAGE_COUNT]; /**< 言語別の備考です。NULL は自動選択です。 */
    } string_catalog_entry;

    /**
     *  @brief          1 つのカタログを識別します。
     *
     *  カタログの配列と、文字列 ID から配列の添字を引く表を 1 つにまとめた値です。\n
     *  ライブラリはこの値を保持せず、API の呼び出しごとに受け取ります。\n
     *  1 つのプロセスで複数のカタログを扱えます。
     *
     *  すべてのメンバーを初期化子で与えられるため、静的記憶域期間を持つ `const` として定義できます。\n
     *  配列はコピーせず、ポインターだけを保持します。
     *  指す領域は、このカタログを使用する間ずっと有効である必要があります。
     *
     *  @ref string_catalog::id_index は、文字列 ID からカタログを引く探索コストを下げる表です。\n
     *  文字列 ID を添字として @ref string_catalog::entries の添字を格納し、
     *  登録していない添字には負の値を格納します。\n
     *  不要な場合は NULL と 0 を指定します。この場合は線形探索になります。
     *
     *  メンバーは配列を先に、要素数をあとにまとめています。\n
     *  この順序であれば暗黙のパディングが生じません。
     *
     *  内容の妥当性は @ref string_catalog_verify で確認します。
     */
    typedef struct string_catalog
    {
        const string_catalog_entry *entries; /**< カタログの配列です。NULL にできません。 */
        const int *id_index; /**< 文字列 ID を添字として entries の添字を引く表です。不要な場合は NULL です。 */
        int entry_count;     /**< entries の要素数です。0 以上を指定します。 */
        int id_index_count;  /**< id_index の要素数です。id_index が NULL の場合は 0 を指定します。 */
    } string_catalog;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRING_CATALOG_STRING_CATALOG_ENTRY_H */
