/**
 *******************************************************************************
 *  @file           base_calc.c
 *  @brief          外部ライブラリの実装に差し替え可能な計算関数を提供します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "sym_loader_libbase.h"
#include <base/base_spec.h>

/* Doxygen コメントはヘッダーに記述 */

int base_calc(const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return BASE_ERR_INVALID_ARGUMENT;
    }

    base_calc_fn fp = cplat_sym_loader_resolve_as(pfo_base_calc, base_calc_fn);
    if (fp != NULL)
    {
        /* 差し替え実装 */
        base_console_output("base_calc: 差し替え実装が見つかりました。差し替え実装に移譲します\n");
        return fp(a, b, result);
    }
    else
    {
        /* 既定の実装 */
        base_console_output("base_calc: a=%d, b=%d の処理 (*result = a + b;) を行います\n", a, b);
        *result = a + b;
        return BASE_OK;
    }
}
