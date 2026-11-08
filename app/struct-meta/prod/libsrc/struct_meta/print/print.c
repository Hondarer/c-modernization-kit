/**
 *******************************************************************************
 *  @file           print.c
 *  @brief          記述子を辿り、構造体インスタンスの値をテキストへ書き出します。
 *  @author         Tetsuo Honda
 *  @date           2026/08/16
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/print/print.h>

#include <struct_meta/access/access.h>
#include <struct_meta/meta/bytes.h>
#include <struct_meta/meta/integer.h>

#include <cplat/base/result.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_indent(FILE *out, int indent)
{
    int i;

    for (i = 0; i < indent; i++)
    {
        fputc(' ', out);
    }
}

/*
 * スカラー フィールドの値を表示用の文字列にする。
 * 成功時は CPLAT_OK を返し、dest に表示文字列を書く。
 * 整数を読み出せないとき、および構造体または未知の種別のときは CPLAT_ERR_INVALID_ARGUMENT を返す。
 * char 配列は print_element から直接出力するため、本関数では扱わない。
 */
static int format_scalar_value(struct_meta_field_kind kind, const unsigned char *field_ptr, size_t element_size,
                               char *dest, size_t dest_size)
{
    switch (kind)
    {
    case STRUCT_META_FIELD_SIGNED_INTEGER:
    {
        int64_t value;

        if (struct_meta_internal_integer_load_signed(field_ptr, element_size, &value) != CPLAT_OK)
        {
            snprintf(dest, dest_size, "?");
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        snprintf(dest, dest_size, "%" PRId64, value);
        return CPLAT_OK;
    }
    case STRUCT_META_FIELD_UNSIGNED_INTEGER:
    {
        uint64_t value;

        if (struct_meta_internal_integer_load_unsigned(field_ptr, element_size, &value) != CPLAT_OK)
        {
            snprintf(dest, dest_size, "?");
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        snprintf(dest, dest_size, "%" PRIu64, value);
        return CPLAT_OK;
    }
    case STRUCT_META_FIELD_FLOAT:
    {
        float value;

        memcpy(&value, field_ptr, sizeof(value));
        snprintf(dest, dest_size, "%g", (double)value);
        return CPLAT_OK;
    }
    case STRUCT_META_FIELD_DOUBLE:
    {
        double value;

        memcpy(&value, field_ptr, sizeof(value));
        snprintf(dest, dest_size, "%g", value);
        return CPLAT_OK;
    }
    case STRUCT_META_FIELD_CHAR_ARRAY:
        /* 文字列は print_element から直接出力する。 */
        return CPLAT_ERR_INVALID_ARGUMENT;
    case STRUCT_META_FIELD_STRUCT:
    default:
        snprintf(dest, dest_size, "{...}");
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
}

static int print_struct(const struct_meta_descriptor *desc, const unsigned char *base, FILE *out, int indent);

/*
 * 1 要素を label 付きで表示する。
 * ネスト構造体は子のフィールドを表示し、それ以外はスカラー値を 1 行に表示する。
 * 成功時は CPLAT_OK、失敗時はその結果コードを返す。
 */
static int print_element(const struct_meta_field *field, const unsigned char *elem_ptr, FILE *out, int indent,
                         const char *label)
{
    if (field->kind == STRUCT_META_FIELD_STRUCT)
    {
        if (field->nested == NULL)
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        print_indent(out, indent);
        if (fprintf(out, "%s:\n", label) < 0 || ferror(out))
        {
            return CPLAT_ERR_UNKNOWN;
        }
        return print_struct(field->nested, elem_ptr, out, indent + 2);
    }

    if (field->kind == STRUCT_META_FIELD_CHAR_ARRAY)
    {
        if (field->char_buffer_size == 0U || memchr(elem_ptr, '\0', field->char_buffer_size) == NULL)
        {
            return CPLAT_ERR_INVALID_ENCODING;
        }
        print_indent(out, indent);
        if (fprintf(out, "%s = \"%s\"\n", label, (const char *)elem_ptr) < 0 || ferror(out))
        {
            return CPLAT_ERR_UNKNOWN;
        }
        return CPLAT_OK;
    }

    {
        char current[64];
        int ret = format_scalar_value(field->kind, elem_ptr, field->element_size, current, sizeof(current));
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        print_indent(out, indent);
        if (fprintf(out, "%s = %s\n", label, current) < 0 || ferror(out))
        {
            return CPLAT_ERR_UNKNOWN;
        }
    }

    return CPLAT_OK;
}

/*
 * フィールド 1 個を表示する。
 * 成功時は CPLAT_OK、失敗時は下位の呼び出しが返した結果コードを返す。
 */
static int print_field(const struct_meta_field *field, const unsigned char *base, FILE *out, int indent)
{
    struct_meta_internal_byte_format byte_format;
    int format_ret = struct_meta_internal_field_byte_format(field, &byte_format);
    if (format_ret != CPLAT_OK)
    {
        return format_ret;
    }

    /*
     * meta.format=hex のバイト配列は、空白区切りの 16 進文字列として 1 行に表示する。
     * 要素の大きさは 1 [byte] で連続しているため、先頭要素から element_count [byte] を変換する。
     * 正本は app/struct-meta/docs/architecture.md の「文字列とバイト配列」節である。
     */
    if (byte_format == STRUCT_META_INTERNAL_BYTE_FORMAT_HEX)
    {
        const void *element;
        int ret = struct_meta_field_get_const_element(field, base, 0U, &element);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        size_t text_size;
        ret = struct_meta_internal_bytes_hex_text_size(field->element_count, &text_size);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        char *text = (char *)malloc(text_size);
        if (text == NULL)
        {
            return CPLAT_ERR_OUT_OF_MEMORY;
        }

        ret = struct_meta_internal_bytes_to_hex((const unsigned char *)element, field->element_count, text, text_size);
        if (ret == CPLAT_OK)
        {
            print_indent(out, indent);
            if (fprintf(out, "%s = %s\n", field->name, text) < 0 || ferror(out))
            {
                ret = CPLAT_ERR_UNKNOWN;
            }
        }

        free(text);
        return ret;
    }

    /*
     * char 配列は NUL 終端文字列 1 個として扱う。要素数 1 以下のフィールドも 1 個として表示する。
     * 正本は app/struct-meta/docs/architecture.md の「文字列とバイト配列」節である。
     */
    if ((field->kind == STRUCT_META_FIELD_CHAR_ARRAY) || (field->element_count <= 1U))
    {
        const void *element;
        int ret = struct_meta_field_get_const_element(field, base, 0U, &element);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        return print_element(field, (const unsigned char *)element, out, indent, field->name);
    }

    /* 要素数 2 以上の配列は、要素ごとに名前と添字を label として表示する。 */
    {
        size_t i;
        char label[128];

        for (i = 0; i < field->element_count; i++)
        {
            int ret;
            snprintf(label, sizeof(label), "%s[%zu]", field->name, i);
            const void *element;
            ret = struct_meta_field_get_const_element(field, base, i, &element);
            if (ret == CPLAT_OK)
            {
                ret = print_element(field, (const unsigned char *)element, out, indent, label);
            }

            if (ret != CPLAT_OK)
            {
                return ret;
            }
        }
    }

    return CPLAT_OK;
}

/*
 * フィールドを先頭から表示する。
 * 成功時は CPLAT_OK を返す。途中で失敗したら残りは表示せず、その結果コードを返す。
 */
static int print_struct(const struct_meta_descriptor *desc, const unsigned char *base, FILE *out, int indent)
{
    size_t i;

    for (i = 0; i < desc->field_count; i++)
    {
        int ret = print_field(&desc->fields[i], base, out, indent);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
    }

    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_print_write(const struct_meta_descriptor *desc, const void *instance, FILE *out)
{
    if ((desc == NULL) || (instance == NULL) || (out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    /*
     * 構造体の内容へアクセスする前に、記述子全体を検査する。
     * 正本は app/struct-meta/docs/architecture.md の「記述子と属性」節である。
     */
    int ret = struct_meta_descriptor_validate(desc);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    /* 構造体名を見出しにし、フィールドは空白 2 個で字下げして表示する。 */
    print_indent(out, 0);
    const char *name;

    if (desc->name != NULL)
    {
        name = desc->name;
    }
    else
    {
        name = "(unnamed)";
    }

    if (fprintf(out, "%s:\n", name) < 0 || ferror(out))
    {
        return CPLAT_ERR_UNKNOWN;
    }

    return print_struct(desc, (const unsigned char *)instance, out, 2);
}
