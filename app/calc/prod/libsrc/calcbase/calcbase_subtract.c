/**
 *******************************************************************************
 *  @file           calcbase_subtract.c
 *  @brief          2 つの整数を減算する calcbase_subtract 関数を提供します。
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
#include <limits.h>

/* Doxygen コメントは、ヘッダーに記載 */

int calcbase_subtract(const int a, const int b, int *result)
{
    int difference;
    int ret;

    if (result == NULL)
    {
        return CALC_ERR_INVALID_ARGUMENT;
    }

    if (((b > 0) && (a < (INT_MIN + b))) || ((b < 0) && (a > (INT_MAX + b))))
    {
        return CALC_ERR_INVALID_ARGUMENT;
    }

    /* 関数呼び出しの経路を示すサンプルとして、calcbase_add() を経由する。 */
    if (b == INT_MIN)
    {
        int intermediate;

        ret = calcbase_add(a, INT_MAX, &intermediate);
        if (ret != CALC_OK)
        {
            return ret;
        }
        ret = calcbase_add(intermediate, 1, &difference);
    }
    else
    {
        ret = calcbase_add(a, -b, &difference);
    }

    if (ret != CALC_OK)
    {
        return ret;
    }

    *result = difference;
    return CALC_OK;
}
