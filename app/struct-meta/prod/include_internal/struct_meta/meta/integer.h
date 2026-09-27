/**
 *******************************************************************************
 *  @file           integer.h
 *  @brief          記述子が表す整数フィールドを、幅によらない形で読み書きします。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_META_INTEGER_H
#define STRUCT_META_META_INTEGER_H

#include <stddef.h>
#include <stdint.h>

/**
 *  @brief          整数フィールドとして扱える要素サイズかを判定します。
 *  @param[in]      element_size  判定する要素サイズ (バイト数)。
 *  @return         扱えるなら 0 以外、扱えないなら 0 を返します。
 *
 *  1、2、4、8 バイトだけを扱います。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_integer_is_supported_size(size_t element_size);

/**
 *  @brief          符号付き整数フィールドを @c int64_t として読み出します。
 *  @param[in]      field_ptr     読み出し元。NULL を渡してはなりません。
 *  @param[in]      element_size  フィールドの要素サイズ (バイト数)。
 *  @param[out]     value_out     読み出した値の格納先。NULL を渡してはなりません。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
 *                  @c CPLAT_ERR_UNSUPPORTED を返します。
 *
 *  読み出した値は符号拡張します。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_integer_load_signed(const unsigned char *field_ptr, size_t element_size, int64_t *value_out);

/**
 *  @brief          符号なし整数フィールドを @c uint64_t として読み出します。
 *  @param[in]      field_ptr     読み出し元。NULL を渡してはなりません。
 *  @param[in]      element_size  フィールドの要素サイズ (バイト数)。
 *  @param[out]     value_out     読み出した値の格納先。NULL を渡してはなりません。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
 *                  @c CPLAT_ERR_UNSUPPORTED を返します。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_integer_load_unsigned(const unsigned char *field_ptr, size_t element_size,
                                               uint64_t *value_out);

/**
 *  @brief          @c int64_t の値を符号付き整数フィールドへ書き込みます。
 *  @param[out]     field_ptr     書き込み先。NULL を渡してはなりません。
 *  @param[in]      element_size  フィールドの要素サイズ (バイト数)。
 *  @param[in]      value         書き込む値。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、
 *                  @c CPLAT_ERR_UNSUPPORTED 、または @c CPLAT_ERR_OUT_OF_RANGE を返します。
 *
 *  @p value が @p element_size で表せる範囲を超える場合は、書き込まずに
 *  @c CPLAT_ERR_OUT_OF_RANGE を返します。切り詰めは行いません。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_integer_store_signed(unsigned char *field_ptr, size_t element_size, int64_t value);

/**
 *  @brief          @c uint64_t の値を符号なし整数フィールドへ書き込みます。
 *  @param[out]     field_ptr     書き込み先。NULL を渡してはなりません。
 *  @param[in]      element_size  フィールドの要素サイズ (バイト数)。
 *  @param[in]      value         書き込む値。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、
 *                  @c CPLAT_ERR_UNSUPPORTED 、または @c CPLAT_ERR_OUT_OF_RANGE を返します。
 *
 *  @p value が @p element_size で表せる範囲を超える場合は、書き込まずに
 *  @c CPLAT_ERR_OUT_OF_RANGE を返します。切り詰めは行いません。
 *
 *  @par            スレッド セーフ
 *  本関数はスレッド セーフです。
 */
int struct_meta_internal_integer_store_unsigned(unsigned char *field_ptr, size_t element_size, uint64_t value);

#endif /* STRUCT_META_META_INTEGER_H */
