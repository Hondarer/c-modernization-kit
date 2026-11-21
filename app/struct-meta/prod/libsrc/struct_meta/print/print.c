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
#include <cplat/crt/stdio.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** 階層ごとの字下げの空白の数です。print_indent() の引数が int のため、U を付けません。 */
#define STRUCT_META_PRINT_INDENT 2

/*
 * 字下げ分の空白を出力する。
 * 個々の fputc() の失敗はストリームのエラー指示に残り、呼び出し元の print_label() が
 * 直後の fprintf() の結果と ferror(out) でまとめて検査するため、この関数の fputc の戻り値は明示的に破棄する。
 */
static void print_indent(FILE *out, int indent)
{
    int i;

    for (i = 0; i < indent; i++)
    {
        (void)fputc(' ', out);
    }
}

/*
 * 字下げ、フィールド名、および配列要素のインデックスをストリームへ直接出力する。
 * 固定長の作業配列や動的確保を用いずに完全なラベルを出力し、字下げ時の
 * fputc() 失敗も fprintf() 後の ferror(out) でまとめて検出する。
 * 成功時は CPLAT_OK、出力失敗時は CPLAT_ERR_UNKNOWN を返す。
 */
static int print_label(FILE *out, const int indent, const char *name, const int has_index, const size_t index)
{
    print_indent(out, indent);

    if (has_index != 0)
    {
        if ((fprintf(out, "%s[%zu]", name, index) < 0) || (ferror(out) != 0))
        {
            return CPLAT_ERR_UNKNOWN;
        }
    }
    else
    {
        if ((fprintf(out, "%s", name) < 0) || (ferror(out) != 0))
        {
            return CPLAT_ERR_UNKNOWN;
        }
    }

    return CPLAT_OK;
}

/*
 * スカラー フィールドの値を表示用の文字列にする。
 * 成功時は CPLAT_OK を返し、dest に表示文字列を書く。
 * 整数を読み出せないとき、および構造体または未知の種別のときは CPLAT_ERR_INVALID_ARGUMENT を返す。
 * 数値の書式化に失敗した場合は、cplat_snprintf() が返した結果コードをそのまま返す。
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
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        return cplat_snprintf(dest, dest_size, "%" PRId64, value);
    }
    case STRUCT_META_FIELD_UNSIGNED_INTEGER:
    {
        uint64_t value;

        if (struct_meta_internal_integer_load_unsigned(field_ptr, element_size, &value) != CPLAT_OK)
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        return cplat_snprintf(dest, dest_size, "%" PRIu64, value);
    }
    case STRUCT_META_FIELD_FLOAT:
    {
        float value;

        /*
         * 記述子検証で要素幅が sizeof(float) であることは担保済みだが、任意バイト列の
         * ポインターを float* へ直接変換すると整列条件や有効型規則に依存する。
         * memcpy() で型付きのローカル変数へ複製し、整列に依存せず読み出す。
         * see: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf
         */
        memcpy(&value, field_ptr, sizeof(value));
        return cplat_snprintf(dest, dest_size, "%g", (double)value);
    }
    case STRUCT_META_FIELD_DOUBLE:
    {
        double value;

        /*
         * 記述子検証で要素幅が sizeof(double) であることは担保済みだが、任意バイト列の
         * ポインターを double* へ直接変換すると整列条件や有効型規則に依存する。
         * memcpy() で型付きのローカル変数へ複製し、整列に依存せず読み出す。
         * see: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf
         */
        memcpy(&value, field_ptr, sizeof(value));
        return cplat_snprintf(dest, dest_size, "%g", value);
    }
    case STRUCT_META_FIELD_CHAR_ARRAY:
        /* 文字列は print_element から直接出力する。 */
        return CPLAT_ERR_INVALID_ARGUMENT;
    case STRUCT_META_FIELD_STRUCT:
    default:
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
}

static int print_struct(const struct_meta_descriptor *desc, const unsigned char *base, FILE *out, int indent);

/*
 * 1 要素をフィールド名 (配列要素時はインデックス付き) で表示する。
 * ネスト構造体は子のフィールドを表示し、それ以外はスカラー値を 1 行に表示する。
 * 成功時は CPLAT_OK、失敗時はその結果コードを返す。
 */
static int print_element(const struct_meta_field *field, const unsigned char *elem_ptr, FILE *out, int indent,
                         int has_index, size_t index)
{
    if (field->kind == STRUCT_META_FIELD_STRUCT)
    {
        if ((print_label(out, indent, field->name, has_index, index) != CPLAT_OK) || (fprintf(out, ":\n") < 0) ||
            (ferror(out) != 0))
        {
            return CPLAT_ERR_UNKNOWN;
        }
        return print_struct(field->nested, elem_ptr, out, indent + STRUCT_META_PRINT_INDENT);
    }

    if (field->kind == STRUCT_META_FIELD_CHAR_ARRAY)
    {
        if (field->char_buffer_size == 0U || memchr(elem_ptr, '\0', field->char_buffer_size) == NULL)
        {
            return CPLAT_ERR_INVALID_ENCODING;
        }
        if ((print_label(out, indent, field->name, has_index, index) != CPLAT_OK) ||
            (fprintf(out, " = \"%s\"\n", (const char *)elem_ptr) < 0) || (ferror(out) != 0))
        {
            return CPLAT_ERR_UNKNOWN;
        }
        return CPLAT_OK;
    }

    {
        char current[STRUCT_META_SCALAR_TEXT_BYTES];
        int ret = format_scalar_value(field->kind, elem_ptr, field->element_size, current, sizeof(current));
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        if ((print_label(out, indent, field->name, has_index, index) != CPLAT_OK) ||
            (fprintf(out, " = %s\n", current) < 0) || (ferror(out) != 0))
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
            if ((print_label(out, indent, field->name, 0, 0U) != CPLAT_OK) || (fprintf(out, " = %s\n", text) < 0) ||
                (ferror(out) != 0))
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

        return print_element(field, (const unsigned char *)element, out, indent, 0, 0U);
    }

    /* 要素数 2 以上の配列は、要素ごとに名前とインデックスを直接ストリームへ出力する。 */
    for (size_t i = 0; i < field->element_count; i++)
    {
        const void *element;
        int ret = struct_meta_field_get_const_element(field, base, i, &element);
        if (ret == CPLAT_OK)
        {
            ret = print_element(field, (const unsigned char *)element, out, indent, 1, i);
        }

        if (ret != CPLAT_OK)
        {
            return ret;
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
    if ((print_label(out, 0, desc->name, 0, 0U) != CPLAT_OK) || (fprintf(out, ":\n") < 0) || (ferror(out) != 0))
    {
        return CPLAT_ERR_UNKNOWN;
    }

    return print_struct(desc, (const unsigned char *)instance, out, STRUCT_META_PRINT_INDENT);
}
