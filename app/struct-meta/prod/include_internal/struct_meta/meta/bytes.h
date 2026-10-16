/**
 *******************************************************************************
 *  @file           bytes.h
 *  @brief          バイト配列の属性解釈と 16 進文字列変換を提供します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_INTERNAL_META_BYTES_H
#define STRUCT_META_INTERNAL_META_BYTES_H

#include <stddef.h>

#include <struct_meta/meta/meta.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef enum struct_meta_internal_byte_format
    {
        STRUCT_META_INTERNAL_BYTE_FORMAT_INTEGER = 0,
        STRUCT_META_INTERNAL_BYTE_FORMAT_HEX = 1
    } struct_meta_internal_byte_format;

    /** 1 バイト整数の固定長配列かどうかを返します。 */
    int struct_meta_internal_field_is_byte_array(const struct_meta_field *field);
    /** フィールド属性からバイト配列の表示形式を求めます。 */
    int struct_meta_internal_field_byte_format(const struct_meta_field *field,
                                               struct_meta_internal_byte_format *format_out);
    /** 空白区切り 16 進文字列に必要な、NUL を含むバイト数を求めます。 */
    int struct_meta_internal_bytes_hex_text_size(size_t byte_count, size_t *text_size_out);
    /** バイト配列を小文字 2 桁の空白区切り 16 進文字列へ変換します。 */
    int struct_meta_internal_bytes_to_hex(const unsigned char *bytes, size_t byte_count, char *dest, size_t dest_size);
    /** 空白区切り 16 進文字列を固定長バイト配列へ変換します。 */
    int struct_meta_internal_bytes_from_hex(unsigned char *bytes, size_t byte_count, const char *text);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRUCT_META_INTERNAL_META_BYTES_H */
