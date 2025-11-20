/**
 *******************************************************************************
 *  @file           libsrc/calcbase/divide.c
 *  @brief          divide 関数の実装ファイル。
 *  @author         doxygen-sample team
 *  @date           2025/01/30
 *  @version        1.0.0
 *
 *  2 つの整数を除算する関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalcbase.h>

/* doxygen コメントは、ヘッダに記載 */
int divide(int a, int b, int *result)
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
