/**
 *******************************************************************************
 *  @file           parse_internal.h
 *  @brief          解析対象ヘッダーを構文解析し、AST を返します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_PARSE_PARSE_INTERNAL_H
#define STRUCT_META_PARSE_PARSE_INTERNAL_H

#include <struct_meta/parse/ast.h>
#include <struct_meta/parse/parse.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *  @brief          ヘッダー ファイルを構文解析し、AST を返します。
 *  @param[in]      path            解析対象ヘッダーのパス。NULL を渡してはなりません。
 *  @param[out]     structs_out     AST の格納先。NULL を渡してはなりません。
 *  @param[out]     diagnostic_out  診断の格納先。NULL を渡せます。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、@c CPLAT_ERR_NOT_FOUND 、
 *                  @c CPLAT_ERR_OUT_OF_MEMORY 、または @c CPLAT_ERR_INVALID_PATTERN を返します。
 *
 *  成功したときだけ @p structs_out へ値を入れます。呼び出し側が
 *  @ref struct_meta_internal_parse_struct_list_destroy で解放します。\n
 *  構造体が 1 個も無いヘッダーは @c CPLAT_ERR_NOT_FOUND とします。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_parse_header_file(const char *path, struct_meta_internal_parse_struct_list **structs_out,
                                           struct_meta_diagnostic *diagnostic_out);

/**
 *  @brief          メモリ上のヘッダー内容を構文解析し、AST を返します。
 *  @param[in]      text            解析対象の内容。NULL を渡してはなりません。
 *  @param[in]      length          @p text のバイト数。
 *  @param[out]     structs_out     AST の格納先。NULL を渡してはなりません。
 *  @param[out]     diagnostic_out  診断の格納先。NULL を渡せます。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、@c CPLAT_ERR_NOT_FOUND 、
 *                  @c CPLAT_ERR_OUT_OF_MEMORY 、または @c CPLAT_ERR_INVALID_PATTERN を返します。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_parse_header_text(const char *text, size_t length,
                                           struct_meta_internal_parse_struct_list **structs_out,
                                           struct_meta_diagnostic *diagnostic_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRUCT_META_PARSE_PARSE_INTERNAL_H */
