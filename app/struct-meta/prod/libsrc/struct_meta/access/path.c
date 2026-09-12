/**
 *******************************************************************************
 *  @file           path.c
 *  @brief          C フィールド名と配列インデックスからなる文字列パスを解決します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/access/access.h>

#include <struct_meta/meta/index_internal.h>

#include <cplat/base/result.h>

#include <ctype.h>
#include <stdint.h>
#include <string.h>

static const struct_meta_field *find_field_segment(const struct_meta_descriptor *descriptor, const char *name,
                                                   size_t name_length)
{
    /* 登録済みの記述子は表引きで済む。未登録なら CPLAT_SKIPPED が返り、線形走査へ進む。 */
    size_t index;
    int index_ret = struct_meta_internal_index_find_field(descriptor, name, name_length, &index);
    if (index_ret == CPLAT_OK)
    {
        return &descriptor->fields[index];
    }
    if (index_ret == CPLAT_ERR_NOT_FOUND)
    {
        return NULL;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        const struct_meta_field *field = &descriptor->fields[i];
        if ((strlen(field->name) == name_length) && (strncmp(field->name, name, name_length) == 0))
        {
            return field;
        }
    }
    return NULL;
}

static int parse_index(const char **cursor_in_out, size_t *index_out)
{
    const char *cursor = *cursor_in_out;
    size_t index = 0;

    if (*cursor != '[')
    {
        return CPLAT_SKIPPED;
    }
    cursor++;
    if (isdigit((unsigned char)*cursor) == 0)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    while (isdigit((unsigned char)*cursor) != 0)
    {
        unsigned int digit = (unsigned int)(*cursor - '0');
        if (index > ((SIZE_MAX - digit) / 10U))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }
        index = (index * 10U) + digit;
        cursor++;
    }
    if (*cursor != ']')
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    *cursor_in_out = cursor + 1;
    *index_out = index;
    return CPLAT_OK;
}

static int resolve_path(const struct_meta_descriptor *descriptor, uintptr_t instance_address, const char *path,
                        const struct_meta_field **field_out, uintptr_t *value_address_out)
{
    const char *cursor = path;
    const struct_meta_descriptor *current_descriptor = descriptor;
    uintptr_t current_address = instance_address;

    while (*cursor != '\0')
    {
        const char *name = cursor;
        size_t name_length = 0;
        size_t index = 0;
        int has_index = 0;

        if ((isalpha((unsigned char)*cursor) == 0) && (*cursor != '_'))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
        do
        {
            cursor++;
            name_length++;
        } while ((isalnum((unsigned char)*cursor) != 0) || (*cursor == '_'));

        const struct_meta_field *field = find_field_segment(current_descriptor, name, name_length);
        if (field == NULL)
        {
            return CPLAT_ERR_NOT_FOUND;
        }

        uintptr_t value_address = current_address + field->offset;
        int ret = parse_index(&cursor, &index);
        if (ret == CPLAT_OK)
        {
            if ((field->kind == STRUCT_META_FIELD_CHAR_ARRAY) || (index >= field->element_count))
            {
                return CPLAT_ERR_OUT_OF_RANGE;
            }
            value_address += index * field->element_size;
            has_index = 1;
        }
        else if (ret != CPLAT_SKIPPED)
        {
            return ret;
        }

        if (*cursor == '\0')
        {
            *field_out = field;
            *value_address_out = value_address;
            return CPLAT_OK;
        }
        if (*cursor != '.')
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
        if ((field->kind != STRUCT_META_FIELD_STRUCT) || ((field->element_count > 1U) && (has_index == 0)))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }

        cursor++;
        if (*cursor == '\0')
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
        current_descriptor = field->nested;
        current_address = value_address;
    }

    return CPLAT_ERR_INVALID_ARGUMENT;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_path_resolve_const(const struct_meta_descriptor *descriptor, const void *instance, const char *path,
                                   const struct_meta_field **field_out, const void **value_out)
{
    if ((descriptor == NULL) || (instance == NULL) || (path == NULL) || (path[0] == '\0') || (field_out == NULL) ||
        (value_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *field_out = NULL;
    *value_out = NULL;

    int ret = struct_meta_descriptor_validate(descriptor);
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    uintptr_t value_address = 0U;
    ret = resolve_path(descriptor, (uintptr_t)instance, path, field_out, &value_address);
    if (ret == CPLAT_OK)
    {
        *value_out = (const void *)value_address;
    }
    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_path_resolve(const struct_meta_descriptor *descriptor, void *instance, const char *path,
                             const struct_meta_field **field_out, void **value_out)
{
    if (value_out == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *value_out = NULL;

    if ((descriptor == NULL) || (instance == NULL) || (path == NULL) || (path[0] == '\0') || (field_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *field_out = NULL;

    int ret = struct_meta_descriptor_validate(descriptor);
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    uintptr_t value_address = 0U;
    ret = resolve_path(descriptor, (uintptr_t)instance, path, field_out, &value_address);
    if (ret == CPLAT_OK)
    {
        *value_out = (void *)value_address;
    }
    return ret;
}
