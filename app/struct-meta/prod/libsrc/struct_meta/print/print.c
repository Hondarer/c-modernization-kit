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

static int format_scalar_value(struct_meta_field_kind kind, const unsigned char *field_ptr, size_t element_size,
                               size_t char_buffer_size, char *dest, size_t dest_size)
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
        if ((char_buffer_size == 0U) || (memchr(field_ptr, '\0', char_buffer_size) == NULL))
        {
            return CPLAT_ERR_INVALID_ENCODING;
        }
        snprintf(dest, dest_size, "\"%s\"", (const char *)field_ptr);
        return CPLAT_OK;
    case STRUCT_META_FIELD_STRUCT:
    default:
        snprintf(dest, dest_size, "{...}");
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
}

static int print_struct(const struct_meta_descriptor *desc, const unsigned char *base, FILE *out, int indent);

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
        fprintf(out, "%s:\n", label);
        return print_struct(field->nested, elem_ptr, out, indent + 2);
    }

    {
        char current[64];
        int ret = format_scalar_value(field->kind, elem_ptr, field->element_size, field->char_buffer_size, current,
                                      sizeof(current));
        if (ret != CPLAT_OK)
        {
            return ret;
        }
        print_indent(out, indent);
        fprintf(out, "%s = %s\n", label, current);
    }
    return CPLAT_OK;
}

static int print_field(const struct_meta_field *field, const unsigned char *base, FILE *out, int indent)
{
    struct_meta_internal_byte_format byte_format;
    int format_ret = struct_meta_internal_field_byte_format(field, &byte_format);
    if (format_ret != CPLAT_OK)
    {
        return format_ret;
    }
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
            fprintf(out, "%s = %s\n", field->name, text);
        }
        free(text);
        return ret;
    }

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

    int ret = struct_meta_descriptor_validate(desc);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

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
    fprintf(out, "%s:\n", name);
    return print_struct(desc, (const unsigned char *)instance, out, 2);
}
