/**
 *******************************************************************************
 *  @file           doc.h
 *  @brief          Doxygen コメントから短い説明と汎用属性を取り出します。
 *
 *  本ヘッダーの参照範囲は `prod/libsrc/struct_meta/parse/` の中だけです。
 *  see: app/general/docs/coding-guideline.md の「モジュール私有ヘッダー」
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef DOC_PRIVATE_H
#define DOC_PRIVATE_H

#include <struct_meta/parse/ast.h>
#include <struct_meta/parse/parse.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          1 コメントから取り出した説明と汎用属性です。
     */
    typedef struct struct_meta_internal_parse_doc_attrs
    {
        char *brief;                                      /**< 短い説明です。無いときは NULL です。 */
        struct_meta_internal_parse_attribute *attributes; /**< 汎用属性の連結リストです。 */
        int invalid;                                      /**< 0 以外なら記述が不正です。診断は書き込み済みです。 */
        int pad;                                          /**< 明示的アラインメントです。0 を指定します。 */
    } struct_meta_internal_parse_doc_attrs;

    /**
     *  @brief          Doxygen コメントが `@file` タグを持つかどうかを返します。
     *  @param[in]      raw  コメント原文。NULL を渡せます。
     *  @return         持つなら 1、持たないか NULL なら 0 を返します。
     */
    int struct_meta_internal_parse_doc_has_file_tag(const char *raw);

    /**
     *  @brief          Doxygen コメントから `brief` 文字列を取り出します。
     *  @param[in]      raw         コメント原文。NULL を渡せます。
     *  @param[in]      is_postfix  後置コメントなら 0 以外を渡します。
     *  @return         取り出した文字列です。取れないときは NULL を返します。呼び出し側が解放します。
     *
     *  前置は `@brief` 本文だけを取ります。後置はコメント本体を `brief` とします。\n
     *  後置に `@brief` があればその本文を優先します。
     */
    char *struct_meta_internal_parse_brief_from_doc(const char *raw, int is_postfix);

    /**
     *  @brief          前置と後置の `brief` から、採用する一方を返します。
     *  @param[in,out]  prefix_brief   前置の説明。所有権を受け取ります。
     *  @param[in,out]  postfix_brief  後置の説明。所有権を受け取ります。
     *  @return         採用した文字列です。呼び出し側が解放します。
     *
     *  後置があれば後置を返して前置を解放します。後置が無ければ前置を返します。
     */
    char *struct_meta_internal_parse_brief_choose(char *prefix_brief, char *postfix_brief);

    /**
     *  @brief          連続する Doxygen コメントの原文を改行でつなぎます。
     *  @param[in,out]  first   先行するコメント原文。所有権を受け取ります。
     *  @param[in,out]  second  後続のコメント原文。所有権を受け取ります。
     *  @return         つないだ新しい文字列です。呼び出し側が解放します。
     */
    char *struct_meta_internal_parse_doc_concat(char *first, char *second);

    /**
     *  @brief          コメント原文から brief と汎用属性を取り出します。
     *  @param[in]      raw         コメント原文。NULL を渡せます。
     *  @param[in]      is_postfix  後置コメントなら 0 以外を渡します。
     *  @param[in]      line        診断へ出す行番号。
     *  @param[out]     diagnostic  診断の書き込み先。NULL を渡せます。
     *  @return         取り出した結果です。記述が不正なら `invalid` が 0 以外になります。
     */
    struct_meta_internal_parse_doc_attrs
    struct_meta_internal_parse_doc_attrs_from_raw(const char *raw, int is_postfix, int line,
                                                  struct_meta_diagnostic *diagnostic);

    /**
     *  @brief          前置と後置の属性をマージします。同じ属性名は不正とします。
     *  @param[in]      prefix      前置の結果。所有権を受け取ります。
     *  @param[in]      postfix     後置の結果。所有権を受け取ります。
     *  @param[in]      line        診断へ出す行番号。
     *  @param[out]     diagnostic  診断の書き込み先。NULL を渡せます。
     *  @return         マージした結果です。属性名が重複していれば `invalid` が 0 以外になります。
     */
    struct_meta_internal_parse_doc_attrs
    struct_meta_internal_parse_doc_attrs_choose(struct_meta_internal_parse_doc_attrs prefix,
                                                struct_meta_internal_parse_doc_attrs postfix, int line,
                                                struct_meta_diagnostic *diagnostic);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DOC_PRIVATE_H */
