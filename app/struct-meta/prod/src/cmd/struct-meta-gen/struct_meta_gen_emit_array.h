/**
 *******************************************************************************
 *  @file           struct_meta_gen_emit_array.h
 *  @brief          バイト列を C の静的配列リテラルとして出力します。
 *  @author         Tetsuo Honda
 *  @date           2026/08/30
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_GEN_EMIT_ARRAY_PRIVATE_H
#define STRUCT_META_GEN_EMIT_ARRAY_PRIVATE_H

#include <stddef.h>
#include <stdio.h>

/**
 *  @brief          バイト列を @c static @c const @c uint64_t の配列リテラルとして書き出します。
 *  @param[in,out]  out         出力先です。NULL を渡してはなりません。
 *  @param[in]      array_name  配列名です。NULL を渡してはなりません。
 *  @param[in]      data        出力するバイト列です。NULL を渡してはなりません。
 *  @param[in]      data_size   @p data のバイト数です。1 以上を指定してください。
 *
 *  8 [byte] に満たない末尾は 0 で埋め、1 行あたり 4 要素を出力します。
 */
void struct_meta_gen_emit_uint64_array(FILE *out, const char *array_name, const void *data, size_t data_size);

#endif /* STRUCT_META_GEN_EMIT_ARRAY_PRIVATE_H */
