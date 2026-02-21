/**
 *******************************************************************************
 *  @file           DllMain.c
 *  @brief          base.so / base.dll のアンロード時処理。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  base.so / base.dll がアンロードされるとき、func.c がキャッシュした
 *  liboverride のハンドルと関数ポインタを解放します。
 *
 *  Linux  : __attribute__((destructor)) により dlclose(base.so) または
 *           プロセス正常終了時に自動的に呼び出されます。
 *  Windows: DllMain の DLL_PROCESS_DETACH により FreeLibrary(base.dll) または
 *           プロセス正常終了時に自動的に呼び出されます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "libbase_local.h"

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
