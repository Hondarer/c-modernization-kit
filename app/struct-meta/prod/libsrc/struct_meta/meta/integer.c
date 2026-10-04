/**
 *******************************************************************************
 *  @file           integer.c
 *  @brief          記述子が表す整数フィールドを、幅によらない形で読み書きします。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/meta/integer.h>

#include <cplat/base/result.h>

#include <string.h>

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_internal_integer_is_supported_size(size_t element_size)
{
    if ((element_size == 1U) || (element_size == 2U) || (element_size == 4U) || (element_size == 8U))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_internal_integer_load_signed(const unsigned char *field_ptr, size_t element_size, int64_t *value_out)
{
    if ((field_ptr == NULL) || (value_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    switch (element_size)
    {
    case 1U:
    {
        int8_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (int64_t)raw;
        return CPLAT_OK;
    }
    case 2U:
    {
        int16_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (int64_t)raw;
        return CPLAT_OK;
    }
    case 4U:
    {
        int32_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (int64_t)raw;
        return CPLAT_OK;
    }
    case 8U:
    {
        int64_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = raw;
        return CPLAT_OK;
    }
    default:
        return CPLAT_ERR_UNSUPPORTED;
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_internal_integer_load_unsigned(const unsigned char *field_ptr, size_t element_size,
                                               uint64_t *value_out)
{
    if ((field_ptr == NULL) || (value_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    switch (element_size)
    {
    case 1U:
    {
        uint8_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (uint64_t)raw;
        return CPLAT_OK;
    }
    case 2U:
    {
        uint16_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (uint64_t)raw;
        return CPLAT_OK;
    }
    case 4U:
    {
        uint32_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (uint64_t)raw;
        return CPLAT_OK;
    }
    case 8U:
    {
        uint64_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = raw;
        return CPLAT_OK;
    }
    default:
        return CPLAT_ERR_UNSUPPORTED;
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_internal_integer_store_signed(unsigned char *field_ptr, size_t element_size, int64_t value)
{
    if (field_ptr == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    switch (element_size)
    {
    case 1U:
    {
        if ((value < INT8_MIN) || (value > INT8_MAX))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }
        int8_t raw = (int8_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case 2U:
    {
        if ((value < INT16_MIN) || (value > INT16_MAX))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }
        int16_t raw = (int16_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case 4U:
    {
        if ((value < INT32_MIN) || (value > INT32_MAX))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }
        int32_t raw = (int32_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case 8U:
        memcpy(field_ptr, &value, sizeof(value));
        return CPLAT_OK;
    default:
        return CPLAT_ERR_UNSUPPORTED;
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_internal_integer_store_unsigned(unsigned char *field_ptr, size_t element_size, uint64_t value)
{
    if (field_ptr == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    switch (element_size)
    {
    case 1U:
    {
        if (value > UINT8_MAX)
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }
        uint8_t raw = (uint8_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case 2U:
    {
        if (value > UINT16_MAX)
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }
        uint16_t raw = (uint16_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case 4U:
    {
        if (value > UINT32_MAX)
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }
        uint32_t raw = (uint32_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case 8U:
        memcpy(field_ptr, &value, sizeof(value));
        return CPLAT_OK;
    default:
        return CPLAT_ERR_UNSUPPORTED;
    }
}
