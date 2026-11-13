/**
 *******************************************************************************
 *  @file           decode.c
 *  @brief          cJSON オブジェクトの内容を、記述子 (@ref struct_meta_descriptor) に従って構造体インスタンスへ書き戻します。
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

static int struct_from_json(const struct_meta_descriptor *desc, const cJSON *json, unsigned char *base);

/**
 *  @brief          cJSON アイテム 1 個分をスカラー値としてメモリへ書き戻します。
 */
static int scalar_from_json(struct_meta_field_kind kind, const cJSON *item, unsigned char *field_ptr,
                            size_t element_size, size_t char_buffer_size)
{
    switch (kind)
    {
    case STRUCT_META_FIELD_SIGNED_INTEGER:
    {
        if (!cjson_app_cJSON_IsNumber(item))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        int64_t value;

        if (!cjson_app_cJSON_GetInt64Value(item, &value))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }

        return struct_meta_internal_integer_store_signed(field_ptr, element_size, value);
    }

    case STRUCT_META_FIELD_UNSIGNED_INTEGER:
    {
        if (!cjson_app_cJSON_IsNumber(item))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        uint64_t value;

        if (!cjson_app_cJSON_GetUInt64Value(item, &value))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }

        return struct_meta_internal_integer_store_unsigned(field_ptr, element_size, value);
    }

    case STRUCT_META_FIELD_FLOAT:
        if ((element_size != sizeof(float)) || (!cjson_app_cJSON_IsNumber(item)))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
        {
            float value = (float)cjson_app_cJSON_GetNumberValue(item);
            memcpy(field_ptr, &value, sizeof(value));
        }

        break;

    case STRUCT_META_FIELD_DOUBLE:
        if ((element_size != sizeof(double)) || (!cjson_app_cJSON_IsNumber(item)))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
        {
            double value = cjson_app_cJSON_GetNumberValue(item);
            memcpy(field_ptr, &value, sizeof(value));
        }

        break;

    case STRUCT_META_FIELD_CHAR_ARRAY:
        if (!cjson_app_cJSON_IsString(item))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
        {
            const char *text = cjson_app_cJSON_GetStringValue(item);
            size_t text_len = strlen(text);

            if ((char_buffer_size == 0U) || (text_len >= char_buffer_size))
            {
                return CPLAT_ERR_BUFFER_TOO_SMALL;
            }

            memcpy(field_ptr, text, text_len + 1U);
        }

        break;

    case STRUCT_META_FIELD_STRUCT:
    default:
        /* STRUCT_META_FIELD_STRUCT は element_from_json() が先に振り分けるため、ここには到達しない。 */
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    return CPLAT_OK;
}

/**
 *  @brief          cJSON アイテム 1 個分 (配列要素、またはネスト構造体 1 個) をメモリへ書き戻します。
 */
static int element_from_json(const struct_meta_field *field, const cJSON *item, unsigned char *elem_ptr)
{
    if (field->kind == STRUCT_META_FIELD_STRUCT)
    {
        return struct_from_json(field->nested, item, elem_ptr);
    }

    return scalar_from_json(field->kind, item, elem_ptr, field->element_size, field->char_buffer_size);
}

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

/**
 *  @brief          フィールド記述子 1 個分 (スカラー、char 配列、固定長配列のいずれか) を cJSON から書き戻します。
 *  @param[in]      field フィールド記述子です。
 *  @param[in]      json 読み出す JSON オブジェクトです。
 *  @param[in,out]  base 書き戻す構造体の先頭です。
 */
static int field_from_json(const struct_meta_field *field, const cJSON *json, unsigned char *base)
{
    const cJSON *item;

    const struct_meta_attribute *attribute = NULL;
    if (struct_meta_field_find_attribute(field, "json.ignore", &attribute) == CPLAT_OK)
    {
        return CPLAT_OK;
    }

    item = cjson_app_cJSON_GetObjectItemCaseSensitive(json, json_key(field));
    if (item == NULL)
    {
        if (struct_meta_field_find_attribute(field, "json.required", &attribute) == CPLAT_OK)
        {
            return CPLAT_ERR_MISSING_REQUIRED;
        }

        return CPLAT_OK;
    }

    struct_meta_internal_byte_format byte_format;
    int format_ret = struct_meta_internal_field_byte_format(field, &byte_format);
    if (format_ret != CPLAT_OK)
    {
        return format_ret;
    }

    /* 16 進文字列形式のバイト配列を復元する。 */
    if (byte_format == STRUCT_META_INTERNAL_BYTE_FORMAT_HEX)
    {
        if (!cjson_app_cJSON_IsString(item))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        void *element;
        int ret = struct_meta_field_get_element(field, base, 0U, &element);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        return struct_meta_internal_bytes_from_hex((unsigned char *)element, field->element_count,
                                                   cjson_app_cJSON_GetStringValue(item));
    }

    /* 単一要素 (スカラーまたは文字列) を復元する。 */
    if ((field->kind == STRUCT_META_FIELD_CHAR_ARRAY) || (field->element_count <= 1U))
    {
        void *element;
        int ret = struct_meta_field_get_element(field, base, 0U, &element);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        return element_from_json(field, item, (unsigned char *)element);
    }

    if (!cjson_app_cJSON_IsArray(item))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    int array_size = cjson_app_cJSON_GetArraySize(item);
    if ((array_size < 0) || ((size_t)array_size != field->element_count))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    /* 配列の各要素を復元する。 */
    for (size_t i = 0; i < field->element_count; i++)
    {
        const cJSON *elem = cjson_app_cJSON_GetArrayItem(item, (int)i);
        void *element;
        int ret = struct_meta_field_get_element(field, base, i, &element);
        if (ret == CPLAT_OK)
        {
            ret = element_from_json(field, elem, (unsigned char *)element);
        }

        if (ret != CPLAT_OK)
        {
            return ret;
        }
    }

    return CPLAT_OK;
}

/**
 *  @brief          cJSON オブジェクト 1 個分を構造体インスタンスへ書き戻します。
 */
static int struct_from_json(const struct_meta_descriptor *desc, const cJSON *json, unsigned char *base)
{
    if (!cjson_app_cJSON_IsObject(json))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    for (size_t i = 0; i < desc->field_count; i++)
    {
        int ret = field_from_json(&desc->fields[i], json, base);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
    }

    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_json_decode(const struct_meta_descriptor *desc, const cJSON *json, void *instance)
{
    if ((desc == NULL) || (json == NULL) || (instance == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    int ret = struct_meta_descriptor_validate(desc);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    return struct_from_json(desc, json, (unsigned char *)instance);
}
