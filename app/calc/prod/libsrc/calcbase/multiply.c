/**
 *******************************************************************************
 *  @file           libsrc/calcbase/multiply.c
 *  @brief          2 つの整数を乗算する multiply 関数を提供します。
 *  @author         c-modenization-kit sample team
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

int multiply(const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return CALC_ERROR;
    }
    *result = a * b;
    return CALC_SUCCESS;
}
