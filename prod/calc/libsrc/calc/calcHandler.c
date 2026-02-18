/**
 *******************************************************************************
 *  @file           calcHandler.c
 *  @brief          calcHandler 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  演算種別に基づいて適切な計算関数を呼び出すハンドラーを提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalc.h>
#include <libcalcbase.h>
#include <stddef.h>

/* doxygen コメントは、ヘッダに記載 */
CALC_API int WINAPI calcHandler(const int kind, const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return CALC_ERROR;
    }

    switch (kind)
    {
    case CALC_KIND_ADD:
        return add(a, b, result);
    case CALC_KIND_SUBTRACT:
        return subtract(a, b, result);
    case CALC_KIND_MULTIPLY:
        return multiply(a, b, result);
    case CALC_KIND_DIVIDE:
        return divide(a, b, result);
    default:
        return CALC_ERROR;
    }
}
