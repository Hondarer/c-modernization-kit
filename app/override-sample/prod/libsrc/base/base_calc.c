/**
 *******************************************************************************
 *  @file           base_calc.c
 *  @brief          外部ライブラリの実装に差し替え可能な計算関数を提供します。
 *  @author         c-modernization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "sym_loader_libbase.h"
#include <base/base_spec.h>

/* Doxygen コメントは、ヘッダーに記載 */

int base_calc(const int a, const int b, int *result)
{
    size_t count;
    void *address;
    base_calc_fn fp;

    if (result == NULL)
    {
        return BASE_ERR_INVALID_ARGUMENT;
    }

    /*
     * 移譲先が 1 件以上なら先頭だけを呼び、0 件なら組み込みの加算を行う。
     */
    if (cplat_sym_loader_count(pfo_base_calc, &count) != CPLAT_OK)
    {
        return BASE_ERR_UNKNOWN;
    }

    if (count == 0)
    {
        base_console_output("base_calc: a=%d, b=%d の処理 (*result = a + b;) を行います\n", a, b);
        *result = a + b;
        return BASE_OK;
    }

    if (cplat_sym_loader_get(pfo_base_calc, 0u, &address) != CPLAT_OK)
    {
        return BASE_ERR_UNKNOWN;
    }
    base_console_output("base_calc: 差し替え実装が見つかりました。差し替え実装に移譲します\n");
    fp = (base_calc_fn)address;
    return fp(a, b, result);
}
