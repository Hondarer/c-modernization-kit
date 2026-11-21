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
    if ((element_size == sizeof(int8_t)) || (element_size == sizeof(int16_t)) || (element_size == sizeof(int32_t)) ||
        (element_size == sizeof(int64_t)))
    {
        return 1;
    }

    return 0;
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
    case sizeof(int8_t):
    {
        int8_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (int64_t)raw;
        return CPLAT_OK;
    }
    case sizeof(int16_t):
    {
        int16_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (int64_t)raw;
        return CPLAT_OK;
    }
    case sizeof(int32_t):
    {
        int32_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (int64_t)raw;
        return CPLAT_OK;
    }
    case sizeof(int64_t):
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

int struct_meta_internal_integer_load_unsigned(const unsigned char *field_ptr, size_t element_size, uint64_t *value_out)
{
    if ((field_ptr == NULL) || (value_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    switch (element_size)
    {
    case sizeof(uint8_t):
    {
        uint8_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (uint64_t)raw;
        return CPLAT_OK;
    }
    case sizeof(uint16_t):
    {
        uint16_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (uint64_t)raw;
        return CPLAT_OK;
    }
    case sizeof(uint32_t):
    {
        uint32_t raw;
        memcpy(&raw, field_ptr, sizeof(raw));
        *value_out = (uint64_t)raw;
        return CPLAT_OK;
    }
    case sizeof(uint64_t):
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
    case sizeof(int8_t):
    {
        if ((value < INT8_MIN) || (value > INT8_MAX))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }

        int8_t raw = (int8_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case sizeof(int16_t):
    {
        if ((value < INT16_MIN) || (value > INT16_MAX))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }

        int16_t raw = (int16_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case sizeof(int32_t):
    {
        if ((value < INT32_MIN) || (value > INT32_MAX))
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }

        int32_t raw = (int32_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case sizeof(int64_t):
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
    case sizeof(uint8_t):
    {
        if (value > UINT8_MAX)
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }

        uint8_t raw = (uint8_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case sizeof(uint16_t):
    {
        if (value > UINT16_MAX)
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }

        uint16_t raw = (uint16_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case sizeof(uint32_t):
    {
        if (value > UINT32_MAX)
        {
            return CPLAT_ERR_OUT_OF_RANGE;
        }

        uint32_t raw = (uint32_t)value;
        memcpy(field_ptr, &raw, sizeof(raw));
        return CPLAT_OK;
    }
    case sizeof(uint64_t):
        memcpy(field_ptr, &value, sizeof(value));
        return CPLAT_OK;
    default:
        return CPLAT_ERR_UNSUPPORTED;
    }
}
