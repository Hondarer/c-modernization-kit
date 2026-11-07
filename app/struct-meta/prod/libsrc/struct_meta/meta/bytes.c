/**
 *******************************************************************************
 *  @file           bytes.c
 *  @brief          バイト配列の属性解釈と 16 進文字列変換を実装します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/meta/bytes.h>

#include <cplat/base/result.h>

#include <stdint.h>
#include <string.h>

/*
 * フィールドの属性一覧から指定されたキーを持つ属性を検索する。
 * 一致する属性へのポインターを返す。見つからない場合は NULL を返す。
 */
static const struct_meta_attribute *find_attribute(const struct_meta_field *field, const char *key)
{
    for (size_t i = 0; i < field->attribute_count; i++)
    {
        if (strcmp(field->attributes[i].key, key) == 0)
        {
            return &field->attributes[i];
        }
    }

    return NULL;
}

int struct_meta_internal_field_is_byte_array(const struct_meta_field *field)
{
    if (field == NULL)
    {
        return 0;
    }

    if (((field->kind == STRUCT_META_FIELD_SIGNED_INTEGER) || (field->kind == STRUCT_META_FIELD_UNSIGNED_INTEGER)) &&
        (field->element_size == 1U) && (field->element_count > 1U))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int struct_meta_internal_field_byte_format(const struct_meta_field *field, struct_meta_internal_byte_format *format_out)
{
    if ((field == NULL) || (format_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    const struct_meta_attribute *kind = find_attribute(field, "meta.kind");
    if (kind != NULL)
    {
        if ((kind->value == NULL) || (strcmp(kind->value, "bytes") != 0) ||
            (struct_meta_internal_field_is_byte_array(field) == 0))
        {
            return CPLAT_ERR_CORRUPT_DESCRIPTOR;
        }
    }

    const struct_meta_attribute *format = find_attribute(field, "meta.format");
    if (format == NULL)
    {
        *format_out = STRUCT_META_INTERNAL_BYTE_FORMAT_INTEGER;
        return CPLAT_OK;
    }

    if ((format->value == NULL) || (strcmp(format->value, "hex") != 0) ||
        (struct_meta_internal_field_is_byte_array(field) == 0))
    {
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    *format_out = STRUCT_META_INTERNAL_BYTE_FORMAT_HEX;

    return CPLAT_OK;
}

int struct_meta_internal_bytes_hex_text_size(const size_t byte_count, size_t *text_size_out)
{
    if (text_size_out == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    /* 1 [byte] あたり 3 文字を要するため、乗算のオーバーフローを防ぐ上限を検査する。 */
    if ((byte_count == 0U) || (byte_count > ((SIZE_MAX - 1U) / 3U)))
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    *text_size_out = byte_count * 3U;

    return CPLAT_OK;
}

int struct_meta_internal_bytes_to_hex(const unsigned char *bytes, const size_t byte_count, char *dest,
                                      const size_t dest_size)
{
    static const char s_hex_digits[] = "0123456789abcdef";
    size_t required_size;

    if ((bytes == NULL) || (dest == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    int ret = struct_meta_internal_bytes_hex_text_size(byte_count, &required_size);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    if (dest_size < required_size)
    {
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }

    size_t offset = 0U;

    /* 各バイトを 2 桁の 16 進文字へ変換し、空白で区切って格納する。 */
    for (size_t i = 0; i < byte_count; i++)
    {
        if (i > 0U)
        {
            dest[offset] = ' ';
            offset++;
        }

        dest[offset] = s_hex_digits[bytes[i] >> 4U];
        dest[offset + 1U] = s_hex_digits[bytes[i] & 0x0fU];
        offset += 2U;
    }

    dest[offset] = '\0';

    return CPLAT_OK;
}

/*
 * 16 進文字を 4 ビットの数値へ変換する。
 * 成功時は CPLAT_OK、16 進文字でない場合は CPLAT_ERR_INVALID_ENCODING を返す。
 */
static int hex_value(const char ch, unsigned char *value_out)
{
    if ((ch >= '0') && (ch <= '9'))
    {
        *value_out = (unsigned char)(ch - '0');
        return CPLAT_OK;
    }

    if ((ch >= 'a') && (ch <= 'f'))
    {
        *value_out = (unsigned char)(ch - 'a' + 10);
        return CPLAT_OK;
    }

    if ((ch >= 'A') && (ch <= 'F'))
    {
        *value_out = (unsigned char)(ch - 'A' + 10);
        return CPLAT_OK;
    }

    return CPLAT_ERR_INVALID_ENCODING;
}

/*
 * 16 進文字列が空白区切りの形式を満たしているかを検証する。
 * 妥当な場合は CPLAT_OK、不正な文字や形式違反がある場合は CPLAT_ERR_INVALID_ENCODING を返す。
 */
static int validate_hex_text(const char *text, const size_t byte_count)
{
    const char *cursor = text;

    /* 各バイトに相当する 2 文字の 16 進数と区切りの空白文字を走査する。 */
    for (size_t i = 0; i < byte_count; i++)
    {
        unsigned char ignored;

        if ((hex_value(cursor[0], &ignored) != CPLAT_OK) || (hex_value(cursor[1], &ignored) != CPLAT_OK))
        {
            return CPLAT_ERR_INVALID_ENCODING;
        }

        cursor += 2;

        if (i + 1U < byte_count)
        {
            if (*cursor != ' ')
            {
                return CPLAT_ERR_INVALID_ENCODING;
            }

            do
            {
                cursor++;
            } while (*cursor == ' ');
        }
    }

    if (*cursor == '\0')
    {
        return CPLAT_OK;
    }
    else
    {
        return CPLAT_ERR_INVALID_ENCODING;
    }
}

int struct_meta_internal_bytes_from_hex(unsigned char *bytes, const size_t byte_count, const char *text)
{
    if ((bytes == NULL) || (text == NULL) || (byte_count == 0U))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    int ret = validate_hex_text(text, byte_count);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    const char *cursor = text;

    for (size_t i = 0; i < byte_count; i++)
    {
        unsigned char high = 0U;
        unsigned char low = 0U;

        /* 事前の validate_hex_text() で妥当性を検証済みのため、hex_value() は失敗しない。 */
        (void)hex_value(cursor[0], &high);
        (void)hex_value(cursor[1], &low);
        bytes[i] = (unsigned char)((high << 4U) | low);
        cursor += 2;
        while (*cursor == ' ')
        {
            cursor++;
        }
    }

    return CPLAT_OK;
}
