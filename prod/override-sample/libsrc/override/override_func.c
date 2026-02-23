/**
 *******************************************************************************
 *  @file           override_func.c
 *  @brief          override_func 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase の func から動的にロードされ呼び出されるオーバーライド関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <libbase.h>
#include <libbase_ext.h>
#include <stddef.h>

/* doxygen コメントは、ヘッダに記載 */
int WINAPI override_func(const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return -1;
    }
    console_output("override_func: a=%d, b=%d の処理 (*result = a * b;) を行います\n", a, b);
    *result = a * b;
    return 0;
}
