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

#include <libbase.h>
#include "libbase_local.h"
#include <stddef.h>

/* ハンドルと関数ポインタのキャッシュ (初回ロード時のみ取得)
 * アンロード時の解放は DllMain.c が担当する。 */
MODULE_HANDLE    s_handle        = NULL;
func_override_t  s_func_override = NULL;

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
#ifndef _WIN32
        if (s_handle == NULL)
        {
            s_handle = dlopen("liboverride.so", RTLD_LAZY);
            if (s_handle == NULL)
            {
                return -1;
            }
            s_func_override = (func_override_t)dlsym(s_handle, "func_override");
            if (s_func_override == NULL)
            {
                dlclose(s_handle);
                s_handle = NULL;
                return -1;
            }
        }
        console_output("func: func_override に移譲します\n");
        return s_func_override(useOverride, a, b, result);
#else /* _WIN32 */
        if (s_handle == NULL)
        {
            s_handle = LoadLibrary("liboverride.dll");
            if (s_handle == NULL)
            {
                return -1;
            }
            s_func_override = (func_override_t)GetProcAddress(s_handle, "func_override");
            if (s_func_override == NULL)
            {
                FreeLibrary(s_handle);
                s_handle = NULL;
                return -1;
            }
        }
        console_output("func: func_override に移譲します\n");
        return s_func_override(useOverride, a, b, result);
#endif /* _WIN32 */
    }
}
