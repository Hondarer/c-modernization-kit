/**
 *******************************************************************************
 *  @file           func.c
 *  @brief          func 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  useOverride フラグに従い自身で処理するか外部ライブラリに委譲する関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "libbase_local.h"
#include <libbase.h>
#include <stddef.h>

/* lib_loader.c に定義しているキャッシュエントリ */
extern LibFuncCache s_cache_func_override;

/* doxygen コメントは、ヘッダに記載 */
int WINAPI func(const int useOverride, const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return -1;
    }

    if (useOverride == 0)
    {
        console_output("func: a=%d, b=%d の処理 (*result = a + b;) を行います\n", a, b);
        *result = a + b;
        return 0;
    }

    /* useOverride != 0: liboverride に委譲 */
    {
        func_override_t fp = libbase_load_func(&s_cache_func_override, func_override_t);
        if (fp == NULL)
        {
            return -1;
        }
        console_output("func: func_override に移譲します\n");
        return fp(useOverride, a, b, result);
    }
}
