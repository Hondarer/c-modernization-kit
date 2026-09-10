/**
 *******************************************************************************
 *  @file           format_engine.h
 *  @brief          可変長引数の取り出しと、位置指定書式の展開を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  本ヘッダーは `prod/libsrc/message_catalog/` のモジュール私有ヘッダーです。\n
 *  同ディレクトリの実装ファイルからだけ `#include "format_engine.h"` で取り込みます。\n
 *  公開契約は公開ヘッダー `<message_catalog/message_catalog_spec.h>` を正とします。
 *
 *  ディレクトリ名と同じ `message_catalog.h` としないのは、公開アンブレラ ヘッダー
 *  `prod/include/message_catalog.h` がインクルード パスの直下に同名で存在し、
 *  取り違えを招くためです。
 *
 *  本ヘッダーで宣言する関数は、呼び出し元が同一ディレクトリに閉じるため NULL を検査しません。\n
 *  前提条件は各関数の Doxygen コメントに記載します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef FORMAT_ENGINE_PRIVATE_H
#define FORMAT_ENGINE_PRIVATE_H

#include <message_catalog/catalog.h>
#include <message_catalog/message_catalog_argument.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          可変長引数から取り出した値 1 個分です。
     *
     *  @ref format_engine_argument_value::kind が、共用体のどのメンバーが有効かを表します。\n
     *  可変長引数は順番にしか取り出せないため、書式を展開する前にこの型の配列へ写します。\n
     *  これにより、位置指定の並べ替えと繰り返し参照を行えます。
     */
    typedef struct format_engine_argument_value
    {
        message_catalog_argument_kind kind; /**< 有効な共用体メンバーを表す引数種別です。 */
        unsigned int pad;                   /**< 明示的アラインメントです。0 を指定します。 */

        /**
     *  @brief          引数種別ごとの値です。
     */
        union format_engine_argument_storage
        {
            const char *string_value;  /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_STRING の値です。 */
            int32_t int32_value;       /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_INT32 の値です。 */
            uint32_t uint32_value;     /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_UINT32 と HEX32 の値です。 */
            int64_t int64_value;       /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_INT64 の値です。 */
            uint64_t uint64_value;     /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_UINT64 と HEX64 の値です。 */
            size_t size_value;         /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_SIZE の値です。 */
            const void *pointer_value; /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_POINTER の値です。 */
            double double_value;       /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_DOUBLE の値です。 */
            int error_code_value;      /**< @ref MESSAGE_CATALOG_ARGUMENT_KIND_ERROR_CODE の値です。 */
        } value;
    } format_engine_argument_value;

    /**
     *  @brief          引数スキーマに従って、可変長引数を値の配列へ取り出します。
     *  @param[in]      entry  カタログの 1 件。NULL を渡してはなりません。
     *  @param[in]      args       取り出す引数リスト。
     *  @param[out]     values     取り出した値の格納先。
     *                             @ref MESSAGE_CATALOG_ARGUMENT_MAX 個の要素が必要です。
     *  @return         成功時は @ref MESSAGE_CATALOG_OK を返します。
     *  @return         引数種別が未知の場合は @ref MESSAGE_CATALOG_ERR_INVALID_DEFINITION を返します。
     *
     *  @p args は先頭から @ref message_catalog_entry::argument_count 個だけ読み進めます。\n
     *  引数種別が未知の場合は、その時点で読み取りを打ち切ります。以降の値は取り出せません。
     *
     *  既定引数拡張と一致しない `va_arg` の指定は未定義動作となるため、
     *  引数種別ごとの取り出し型を本関数へ閉じ込めています。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    int format_engine_collect_arguments(const message_catalog_entry *entry, va_list args,
                                        format_engine_argument_value *values);

    /**
     *  @brief          位置指定書式を展開し、メッセージを組み立てます。
     *  @param[out]     dest        メッセージの格納先。NULL を渡してはなりません。常に NUL 終端します。
     *  @param[in]      dest_size   @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      text        展開する書式。NULL を渡してはなりません。
     *  @param[in]      values      展開に使用する値の配列。NULL を渡してはなりません。
     *  @param[in]      value_count @p values の有効な要素数。
     *  @return         成功時は @ref MESSAGE_CATALOG_OK を返します。
     *  @return         書式の構文が不正な場合、または位置指定が @p value_count 以上の添字を指す場合は
     *                  @ref MESSAGE_CATALOG_ERR_INVALID_DEFINITION を返します。
     *  @return         結果が @p dest に収まらない場合は、切り詰めたうえで
     *                  @ref MESSAGE_CATALOG_ERR_TRUNCATED を返します。
     *
     *  書式の構文は `{0}` から `{9}` までの位置指定と、`{{` と `}}` のエスケープだけです。\n
     *  書式指定は解釈しません。文字列表現は引数種別が決めます。
     *
     *  構文が不正な場合でも @p dest は NUL 終端します。内容は保証しません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    int format_engine_render_text(char *dest, size_t dest_size, const char *text,
                                  const format_engine_argument_value *values, int value_count);

    /**
     *  @brief          位置指定書式の構文と、位置指定の範囲を確認します。
     *  @param[in]      text        確認する書式。NULL を渡してはなりません。
     *  @param[in]      value_count 位置指定が指してよい引数の個数。
     *  @return         書式が正しい場合は @ref MESSAGE_CATALOG_OK を返します。
     *  @return         構文が不正な場合、または位置指定が @p value_count 以上の添字を指す場合は
     *                  @ref MESSAGE_CATALOG_ERR_INVALID_DEFINITION を返します。
     *
     *  値を持たずに書式だけを確認するため、カタログ全体の点検に使用します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    int format_engine_validate_text(const char *text, int value_count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FORMAT_ENGINE_PRIVATE_H */
