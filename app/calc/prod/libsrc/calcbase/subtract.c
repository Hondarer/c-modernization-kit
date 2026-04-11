/**
 *******************************************************************************
 *  @file           libsrc/calcbase/subtract.c
 *  @brief          subtract 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  2 つの整数を減算する関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalcbase.h>
#include <stddef.h>

/* doxygen コメントは、ヘッダに記載 */
int subtract(const int a, const int b, int *result)
{
    return add(a, -1 * b, result); /* 再帰的な関数呼び出しの実装例として add を呼ぶ */
}
