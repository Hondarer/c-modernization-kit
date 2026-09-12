/**
 *******************************************************************************
 *  @file           sample_func.c
 *  @brief          外部ライブラリに処理を委譲できるサンプル関数を提供します。
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

/* Doxygen コメントは、ヘッダーに記載 */

int sample_func(const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return BASE_ERR_INVALID_ARGUMENT;
    }

    sample_func_fn fp = cplat_sym_loader_resolve_as(pfo_sample_func, sample_func_fn);
    if (fp != NULL)
    {
        /* 拡張 (オーバーライド) 処理 */
        base_console_output("sample_func: 拡張処理が見つかりました。拡張処理に移譲します\n");
        return fp(a, b, result);
    }
    else
    {
        /* 基底 (既定) 処理 */
        base_console_output("sample_func: a=%d, b=%d の処理 (*result = a + b;) を行います\n", a, b);
        *result = a + b;
        return BASE_OK;
    }
}
