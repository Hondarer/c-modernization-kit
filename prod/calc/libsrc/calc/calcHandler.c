/**
 *******************************************************************************
 *  @file           calcHandler.c
 *  @brief          calcHandler 関数の実装ファイル。
 *  @author         doxygen-sample team
 *  @date           2025/01/30
 *  @version        1.0.0
 *
 *  演算種別に基づいて適切な計算関数を呼び出すハンドラーを提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalcbase.h>
#include <libcalc.h>

/* doxygen コメントは、ヘッダに記載 */
int calcHandler(int kind, int a, int b)
{
    switch (kind)
    {
    case CALC_KIND_ADD:
        return add(a, b);
    default:
        return -1;
    }
}
