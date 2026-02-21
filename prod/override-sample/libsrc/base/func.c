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

/* ハンドルと関数ポインタのキャッシュ (初回ロード時のみ取得) */
#ifndef _WIN32
static void            *s_handle        = NULL;
#else  /* _WIN32 */
static HMODULE          s_handle        = NULL;
#endif /* _WIN32 */
static func_override_t  s_func_override = NULL;

/*
 * base.so / base.dll がアンロードされるときに s_handle を解放する。
 *
 * Linux  : __attribute__((destructor)) により、dlclose(base.so) または
 *          プロセス正常終了時に自動的に呼び出される。
 * Windows: DllMain の DLL_PROCESS_DETACH により、FreeLibrary(base.dll) または
 *          プロセス正常終了時に自動的に呼び出される。
 */
#ifndef _WIN32

__attribute__((destructor))
static void unload_liboverride(void)
{
    if (s_handle != NULL)
    {
        dlclose(s_handle);
        s_handle        = NULL;
        s_func_override = NULL;
    }
}

#else /* _WIN32 */

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hinstDLL;
    (void)lpvReserved;
    if (fdwReason == DLL_PROCESS_DETACH)
    {
        if (s_handle != NULL)
        {
            FreeLibrary(s_handle);
            s_handle        = NULL;
            s_func_override = NULL;
        }
    }
    return TRUE;
}

#endif /* _WIN32 */

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
