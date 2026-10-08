/**
 *******************************************************************************
 *  @file           context.h
 *  @brief          構文解析 1 回分の状態を、字句解析器と構文解析器の間で共有します。
 *
 *  本ヘッダーの参照範囲は `prod/libsrc/struct_meta/parse/` の中だけです。
 *  see: app/general/docs/coding-guideline.md の「モジュール私有ヘッダー」
 *
 *  flex と bison は既定でグローバル変数へ状態を置きます。ライブラリとして提供する
 *  以上、同一プロセスで繰り返し呼べる必要があるため、再入可能な構成 (flex の
 *  `reentrant` と bison の `api.pure`) を用い、解析結果と診断をこの構造体で運びます。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef CONTEXT_PRIVATE_H
#define CONTEXT_PRIVATE_H

#include <struct_meta/parse/ast.h>
#include <struct_meta/parse/parse.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          構文解析 1 回分の状態です。
     */
    typedef struct struct_meta_internal_parse_context
    {
        struct_meta_internal_parse_struct_list *structs; /**< 解析できた構造体のリストです。 */
        struct_meta_diagnostic *diagnostic;              /**< 診断の書き込み先です。NULL を渡せます。 */
        int failed;                                      /**< 0 以外なら解析が失敗しました。 */
        int pad;                                         /**< 明示的アラインメントです。0 を指定します。 */
    } struct_meta_internal_parse_context;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONTEXT_PRIVATE_H */
