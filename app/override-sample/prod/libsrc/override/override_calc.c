/**
 *******************************************************************************
 *  @file           override_calc.c
 *  @brief          base_calc の差し替え実装として積を計算する関数を提供します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase の base_calc から動的にロードされ、呼び出される差し替え実装を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <base/base_spec.h>
#include <override/override_spec.h>
#include <stddef.h>

/* Doxygen コメントはヘッダーに記述 */

int override_calc(const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return BASE_ERR_INVALID_ARGUMENT;
    }
    base_console_output("override_calc: a=%d, b=%d の処理 (*result = a * b;) を行います\n", a, b);
    *result = a * b;
    return BASE_OK;
}
