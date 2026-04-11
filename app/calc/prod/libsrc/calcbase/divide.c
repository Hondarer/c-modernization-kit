/**
 *******************************************************************************
 *  @file           libsrc/calcbase/divide.c
 *  @brief          divide 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  2 つの整数を除算する関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalcbase.h>
#include <stddef.h>

/* doxygen コメントは、ヘッダに記載 */
int divide(const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return CALC_ERROR;
    }
    if (b == 0)
    {
        return CALC_ERROR;
    }
    *result = a / b;
    return CALC_SUCCESS;
}
