/**
 *******************************************************************************
 *  @file           sample_func.c
 *  @brief          sample_func 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  外部ライブラリに処理を委譲可能なサンプル関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "func_manager_config.h"
#include <libbase.h>

/* doxygen コメントは、ヘッダに記載 */
int WINAPI sample_func(const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return -1;
    }

    sample_func_t fp = func_manager_get_func(p_sample_func, sample_func_t);
    if (fp != NULL)
    {
        /* 拡張 (オーバーライド) 処理 */
        console_output("sample_func: 拡張処理が見つかりました。拡張処理に移譲します\n");
        return fp(a, b, result);
    }
    else
    {
        /* 基底 (デフォルト) 処理 */
        console_output("sample_func: a=%d, b=%d の処理 (*result = a + b;) を行います\n", a, b);
        *result = a + b;
        return 0;
    }
}
