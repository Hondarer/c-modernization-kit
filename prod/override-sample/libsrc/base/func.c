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
#ifndef _WIN32
    #include <dlfcn.h>
#else /* _WIN32 */
    #include <windows.h>
#endif /* _WIN32 */
#include <stddef.h>

typedef int (*func_override_t)(const int, const int, const int, int *);

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
        void *handle = dlopen("liboverride.so", RTLD_LAZY);
        if (handle == NULL)
        {
            return -1;
        }
        func_override_t func_override = (func_override_t)dlsym(handle, "func_override");
        if (func_override == NULL)
        {
            dlclose(handle);
            return -1;
        }
        console_output("func: func_override に移譲します\n");
        int ret = func_override(useOverride, a, b, result);
        dlclose(handle);
        return ret;
#else /* _WIN32 */
        HMODULE handle = LoadLibrary("liboverride.dll");
        if (handle == NULL)
        {
            return -1;
        }
        func_override_t func_override = (func_override_t)GetProcAddress(handle, "func_override");
        if (func_override == NULL)
        {
            FreeLibrary(handle);
            return -1;
        }
        console_output("func: func_override に移譲します\n");
        int ret = func_override(useOverride, a, b, result);
        FreeLibrary(handle);
        return ret;
#endif /* _WIN32 */
    }
}
