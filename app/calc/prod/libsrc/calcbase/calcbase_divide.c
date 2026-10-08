/**
 *******************************************************************************
 *  @file           calcbase_divide.c
 *  @brief          2 つの整数を除算する calcbase_divide 関数を提供します。
 *  @author         c-modernization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <calcbase/calcbase_spec.h>
#include <stddef.h>

/* Doxygen コメントは、ヘッダーに記載 */

int calcbase_divide(const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return CALC_ERR_INVALID_ARGUMENT;
    }
    if (b == 0)
    {
        return CALC_ERR_INVALID_ARGUMENT;
    }
    *result = a / b;
    return CALC_OK;
}
