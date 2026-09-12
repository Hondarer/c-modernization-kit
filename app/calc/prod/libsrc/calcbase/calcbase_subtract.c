/**
 *******************************************************************************
 *  @file           libsrc/calcbase/calcbase_subtract.c
 *  @brief          2 つの整数を減算する calcbase_subtract 関数を提供します。
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

int calcbase_subtract(const int a, const int b, int *result)
{
    return calcbase_add(a, -1 * b, result); /* 再帰的な関数呼び出しの実装例として calcbase_add を呼び出す */
}
