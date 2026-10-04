/**
 *******************************************************************************
 *  @file           validate.c
 *  @brief          構造体メタデータの整合性を検査します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/meta/meta.h>

#include <struct_meta/meta/bytes.h>
#include <struct_meta/meta/index_internal.h>
#include <struct_meta/meta/integer.h>

#include <cplat/base/result.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct validation_context
{
    const struct_meta_descriptor **stack;
    size_t depth;
    size_t capacity;
} validation_context;

static int push_descriptor(validation_context *context, const struct_meta_descriptor *descriptor)
{
    for (size_t i = 0; i < context->depth; i++)
    {
        if (context->stack[i] == descriptor)
        {
            return CPLAT_ERR_CORRUPT_DESCRIPTOR;
        }
    }

    if (context->depth == context->capacity)
    {
        size_t capacity;
        if (context->capacity == 0U)
        {
            capacity = 8U;
        }
        else
        {
            capacity = context->capacity * 2U;
        }
        if (capacity < context->capacity || capacity > (SIZE_MAX / sizeof(*context->stack)))
        {
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
        const struct_meta_descriptor **stack =
            (const struct_meta_descriptor **)realloc(context->stack, capacity * sizeof(*context->stack));
        if (stack == NULL)
        {
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
        context->stack = stack;
        context->capacity = capacity;
    }

    context->stack[context->depth] = descriptor;
    context->depth++;
    return CPLAT_OK;
}

static int validate_attributes(const struct_meta_attribute *attributes, const size_t attribute_count)
{
    if ((attribute_count > 0U) && (attributes == NULL))
    {
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    for (size_t i = 0; i < attribute_count; i++)
    {
        const struct_meta_attribute *attribute = &attributes[i];
        if ((attribute->key == NULL) || (attribute->key[0] == '\0'))
        {
            return CPLAT_ERR_CORRUPT_DESCRIPTOR;
        }
    }

    for (size_t i = 0; i < attribute_count; i++)
    {
        const struct_meta_attribute *attribute = &attributes[i];
        for (size_t j = i + 1U; j < attribute_count; j++)
        {
            if (strcmp(attribute->key, attributes[j].key) == 0)
            {
                return CPLAT_ERR_CORRUPT_DESCRIPTOR;
            }
        }
    }
    return CPLAT_OK;
}

static int validate_descriptor(validation_context *context, const struct_meta_descriptor *descriptor)
{
    if ((descriptor->name == NULL) || (descriptor->name[0] == '\0') || (descriptor->size == 0U) ||
        ((descriptor->field_count > 0U) && (descriptor->fields == NULL)))
    {
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    int ret = push_descriptor(context, descriptor);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    ret = validate_attributes(descriptor->attributes, descriptor->attribute_count);
    for (size_t i = 0; (ret == CPLAT_OK) && (i < descriptor->field_count); i++)
    {
        const struct_meta_field *field = &descriptor->fields[i];
        size_t field_size;

        if ((field->name == NULL) || (field->name[0] == '\0') || (field->kind < STRUCT_META_FIELD_SIGNED_INTEGER) ||
            (field->kind > STRUCT_META_FIELD_STRUCT) || (field->element_size == 0U) || (field->element_count == 0U))
        {
            ret = CPLAT_ERR_CORRUPT_DESCRIPTOR;
            break;
        }

        /* 整数は幅を element_size が表すため、扱える幅であることをここで確かめる。 */
        if (((field->kind == STRUCT_META_FIELD_SIGNED_INTEGER) ||
             (field->kind == STRUCT_META_FIELD_UNSIGNED_INTEGER)) &&
            (struct_meta_internal_integer_is_supported_size(field->element_size) == 0))
        {
            ret = CPLAT_ERR_CORRUPT_DESCRIPTOR;
            break;
        }

        if (field->kind == STRUCT_META_FIELD_CHAR_ARRAY)
        {
            if ((field->char_buffer_size == 0U) || (field->element_count != 1U) || (field->nested != NULL))
            {
                ret = CPLAT_ERR_CORRUPT_DESCRIPTOR;
                break;
            }
            field_size = field->char_buffer_size;
        }
        else
        {
            if ((field->char_buffer_size != 0U) || (field->element_count > (SIZE_MAX / field->element_size)))
            {
                ret = CPLAT_ERR_CORRUPT_DESCRIPTOR;
                break;
            }
            field_size = field->element_count * field->element_size;
        }

        if ((field->offset > descriptor->size) || (field_size > (descriptor->size - field->offset)))
        {
            ret = CPLAT_ERR_CORRUPT_DESCRIPTOR;
            break;
        }

        if (field->kind == STRUCT_META_FIELD_STRUCT)
        {
            if ((field->nested == NULL) || (field->element_size != field->nested->size))
            {
                ret = CPLAT_ERR_CORRUPT_DESCRIPTOR;
                break;
            }
            ret = validate_descriptor(context, field->nested);
            if (ret != CPLAT_OK)
            {
                break;
            }
        }
        else if (field->nested != NULL)
        {
            ret = CPLAT_ERR_CORRUPT_DESCRIPTOR;
            break;
        }

        ret = validate_attributes(field->attributes, field->attribute_count);
        if (ret != CPLAT_OK)
        {
            break;
        }

        struct_meta_internal_byte_format byte_format;
        ret = struct_meta_internal_field_byte_format(field, &byte_format);
        if (ret != CPLAT_OK)
        {
            break;
        }
    }

    context->depth--;
    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_descriptor_validate(const struct_meta_descriptor *descriptor)
{
    if (descriptor == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    /* 登録済みの記述子は、登録時に求めた結果を控えている。公開入口が呼ばれるたびに
       記述子全体を再帰走査し直す費用を避ける。未登録なら控えが無く、以降で検査する。 */
    int cached;
    if (struct_meta_internal_index_find_validation(descriptor, &cached) == CPLAT_OK)
    {
        return cached;
    }

    validation_context context = {0};
    int ret = validate_descriptor(&context, descriptor);

    free(context.stack);
    return ret;
}
