/**
 *******************************************************************************
 *  @file           encode.c
 *  @brief          構造体インスタンスを、記述子 (@ref struct_meta_descriptor) に従って cJSON オブジェクトへ変換します。
 *  @author         Tetsuo Honda
 *  @date           2026/08/16
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/json/json.h>

#include <struct_meta/access/access.h>
#include <struct_meta/meta/bytes.h>
#include <struct_meta/meta/integer.h>

#include <cJSON_Integer.h>
#include <cplat/base/result.h>

#include <string.h>
#include <stdlib.h>

static int struct_to_json(const struct_meta_descriptor *desc, const unsigned char *base, cJSON **json_out);

/**
 *  @brief          スカラー値 1 個分のメモリ内容から cJSON アイテムを作成します。
 */
static int scalar_to_json(struct_meta_field_kind kind, const unsigned char *field_ptr, size_t element_size,
                          size_t char_buffer_size, cJSON **item_out)
{
    cJSON *item = NULL;

    switch (kind)
    {
    case STRUCT_META_FIELD_SIGNED_INTEGER:
    {
        int64_t value;
        int ret = struct_meta_internal_integer_load_signed(field_ptr, element_size, &value);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        item = cJSON_CreateInt64(value);
        break;
    }

    case STRUCT_META_FIELD_UNSIGNED_INTEGER:
    {
        uint64_t value;
        int ret = struct_meta_internal_integer_load_unsigned(field_ptr, element_size, &value);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        item = cJSON_CreateUInt64(value);
        break;
    }

    case STRUCT_META_FIELD_FLOAT:
        if (element_size != sizeof(float))
        {
            return CPLAT_ERR_UNSUPPORTED;
        }
        {
            float value;
            memcpy(&value, field_ptr, sizeof(value));
            item = cJSON_CreateNumber((double)value);
        }

        break;

    case STRUCT_META_FIELD_DOUBLE:
        if (element_size != sizeof(double))
        {
            return CPLAT_ERR_UNSUPPORTED;
        }
        {
            double value;
            memcpy(&value, field_ptr, sizeof(value));
            item = cJSON_CreateNumber(value);
        }

        break;

    case STRUCT_META_FIELD_CHAR_ARRAY:
        /* field_ptr は char[N] の先頭を指す。NUL 終端文字列として扱う。 */
        if ((char_buffer_size == 0U) || (memchr(field_ptr, '\0', char_buffer_size) == NULL))
        {
            return CPLAT_ERR_INVALID_ENCODING;
        }
        item = cJSON_CreateString((const char *)field_ptr);
        break;

    case STRUCT_META_FIELD_STRUCT:
    default:
        /* STRUCT_META_FIELD_STRUCT は element_to_json() が先に振り分けるため、ここには到達しない。 */
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    if (item == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    *item_out = item;

    return CPLAT_OK;
}

/**
 *  @brief          フィールド 1 要素分 (配列要素、またはネスト構造体 1 個) を cJSON アイテムへ変換します。
 */
static int element_to_json(const struct_meta_field *field, const unsigned char *elem_ptr, cJSON **item_out)
{
    if (field->kind == STRUCT_META_FIELD_STRUCT)
    {
        return struct_to_json(field->nested, elem_ptr, item_out);
    }

    return scalar_to_json(field->kind, elem_ptr, field->element_size, field->char_buffer_size, item_out);
}

/*
 * バイト配列フィールドの値を 16 進文字列へ変換し、cJSON 文字列アイテムを作成する。
 * 成功時は CPLAT_OK を返し、*item_out に作成した cJSON アイテムを格納する。
 * 失敗時は要素の取得、16 進変換の結果、または CPLAT_ERR_OUT_OF_MEMORY を返す。
 */
static int byte_array_to_json(const struct_meta_field *field, const unsigned char *base, cJSON **item_out)
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
        *item_out = cJSON_CreateString(text);
        if (*item_out == NULL)
        {
            ret = CPLAT_ERR_OUT_OF_MEMORY;
        }
    }

    free(text);

    return ret;
}

/**
 *  @brief          フィールド記述子 1 個分 (スカラー、char 配列、固定長配列のいずれか) を cJSON アイテムへ変換します。
 */
static int field_to_json(const struct_meta_field *field, const unsigned char *base, cJSON **item_out)
{
    struct_meta_internal_byte_format byte_format;
    int format_ret = struct_meta_internal_field_byte_format(field, &byte_format);
    if (format_ret != CPLAT_OK)
    {
        return format_ret;
    }

    if (byte_format == STRUCT_META_INTERNAL_BYTE_FORMAT_HEX)
    {
        return byte_array_to_json(field, base, item_out);
    }

    /* char[N] は配列であるが、単一の JSON 文字列として扱う。 */
    if ((field->kind == STRUCT_META_FIELD_CHAR_ARRAY) || (field->element_count <= 1U))
    {
        const void *element;
        int ret = struct_meta_field_get_const_element(field, base, 0U, &element);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        return element_to_json(field, (const unsigned char *)element, item_out);
    }

    cJSON *array = cJSON_CreateArray();
    if (array == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    /* 配列の各要素を cJSON アイテムへ変換して配列に追加する。 */
    for (size_t i = 0; i < field->element_count; i++)
    {
        cJSON *elem = NULL;
        const void *element;
        int ret = struct_meta_field_get_const_element(field, base, i, &element);
        if (ret == CPLAT_OK)
        {
            ret = element_to_json(field, (const unsigned char *)element, &elem);
        }

        if (ret != CPLAT_OK)
        {
            cJSON_Delete(array);
            return ret;
        }

        if (!cJSON_AddItemToArray(array, elem))
        {
            cJSON_Delete(elem);
            cJSON_Delete(array);
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
    }

    *item_out = array;

    return CPLAT_OK;
}

/**
 *  @brief          構造体インスタンス 1 個分を cJSON オブジェクトへ変換します。
 */
static const char *json_key(const struct_meta_field *field)
{
    const struct_meta_attribute *attribute = NULL;
    int ret = struct_meta_field_find_attribute(field, "json.name", &attribute);
    if ((ret == CPLAT_OK) && (attribute->value != NULL) && (attribute->value[0] != '\0'))
    {
        return attribute->value;
    }

    return field->name;
}

static int struct_to_json(const struct_meta_descriptor *desc, const unsigned char *base, cJSON **json_out)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    for (size_t i = 0; i < desc->field_count; i++)
    {
        const struct_meta_field *field = &desc->fields[i];
        cJSON *item = NULL;
        int ret;

        const struct_meta_attribute *attribute = NULL;
        if (struct_meta_field_find_attribute(field, "json.ignore", &attribute) == CPLAT_OK)
        {
            continue;
        }

        ret = field_to_json(field, base, &item);
        if (ret != CPLAT_OK)
        {
            cJSON_Delete(obj);
            return ret;
        }

        if (!cJSON_AddItemToObject(obj, json_key(field), item))
        {
            cJSON_Delete(item);
            cJSON_Delete(obj);
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
    }

    *json_out = obj;

    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_json_encode(const struct_meta_descriptor *desc, const void *instance, cJSON **json_out)
{
    if ((desc == NULL) || (instance == NULL) || (json_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    *json_out = NULL;
    int ret = struct_meta_descriptor_validate(desc);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    return struct_to_json(desc, (const unsigned char *)instance, json_out);
}
