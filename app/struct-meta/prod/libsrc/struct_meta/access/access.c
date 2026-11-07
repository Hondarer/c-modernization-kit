/**
 *******************************************************************************
 *  @file           access.c
 *  @brief          メタデータを使ったフィールドと配列要素の参照を提供します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/access/access.h>

#include <struct_meta/meta/index_internal.h>

#include <cplat/base/result.h>

#include <string.h>

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_descriptor_get_field(const struct_meta_descriptor *descriptor, size_t index,
                                     const struct_meta_field **field_out)
{
    if ((descriptor == NULL) || (field_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    *field_out = NULL;

    int ret = struct_meta_descriptor_validate(descriptor);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    if (index >= descriptor->field_count)
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    *field_out = &descriptor->fields[index];

    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_descriptor_find_field(const struct_meta_descriptor *descriptor, const char *name,
                                      const struct_meta_field **field_out)
{
    if ((descriptor == NULL) || (name == NULL) || (field_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    *field_out = NULL;

    int ret = struct_meta_descriptor_validate(descriptor);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    /* 登録済みの記述子は表引きで済む。未登録なら CPLAT_SKIPPED が返り、線形走査へ進む。 */
    size_t index;
    int index_ret = struct_meta_internal_index_find_field(descriptor, name, strlen(name), &index);
    if (index_ret == CPLAT_OK)
    {
        *field_out = &descriptor->fields[index];
        return CPLAT_OK;
    }

    if (index_ret == CPLAT_ERR_NOT_FOUND)
    {
        return CPLAT_ERR_NOT_FOUND;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (strcmp(descriptor->fields[i].name, name) == 0)
        {
            *field_out = &descriptor->fields[i];
            return CPLAT_OK;
        }
    }

    return CPLAT_ERR_NOT_FOUND;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_descriptor_find_attribute(const struct_meta_descriptor *descriptor, const char *key,
                                          const struct_meta_attribute **attribute_out)
{
    if ((descriptor == NULL) || (key == NULL) || (attribute_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    *attribute_out = NULL;

    int ret = struct_meta_descriptor_validate(descriptor);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    for (size_t i = 0; i < descriptor->attribute_count; i++)
    {
        if (strcmp(descriptor->attributes[i].key, key) == 0)
        {
            *attribute_out = &descriptor->attributes[i];
            return CPLAT_OK;
        }
    }

    return CPLAT_ERR_NOT_FOUND;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_field_find_attribute(const struct_meta_field *field, const char *key,
                                     const struct_meta_attribute **attribute_out)
{
    if ((field == NULL) || (key == NULL) || (attribute_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    *attribute_out = NULL;

    for (size_t i = 0; i < field->attribute_count; i++)
    {
        if (strcmp(field->attributes[i].key, key) == 0)
        {
            *attribute_out = &field->attributes[i];
            return CPLAT_OK;
        }
    }

    return CPLAT_ERR_NOT_FOUND;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_field_get_element(const struct_meta_field *field, void *instance, size_t index, void **element_out)
{
    if ((field == NULL) || (instance == NULL) || (element_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    *element_out = NULL;

    if (index >= field->element_count)
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    *element_out = (unsigned char *)instance + field->offset + (index * field->element_size);

    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_field_get_const_element(const struct_meta_field *field, const void *instance, size_t index,
                                        const void **element_out)
{
    if ((field == NULL) || (instance == NULL) || (element_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    *element_out = NULL;

    if (index >= field->element_count)
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    *element_out = (const unsigned char *)instance + field->offset + (index * field->element_size);

    return CPLAT_OK;
}
