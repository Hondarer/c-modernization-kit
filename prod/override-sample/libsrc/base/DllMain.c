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
#include <stddef.h>
#ifndef _WIN32
    #include <syslog.h>
#endif /* _WIN32 */

static void onUnload(void);

void onUnload(void)
{
#ifndef _WIN32
    syslog(LOG_INFO, "base: onUnload called");
#else  /* _WIN32 */
    OutputDebugStringA("base: onUnload called\n");
#endif /* _WIN32 */

    if (s_handle != NULL)
    {
#ifndef _WIN32
        dlclose(s_handle);
#else  /* _WIN32 */
        FreeLibrary(s_handle);
#endif /* _WIN32 */
        s_handle = NULL;
        s_func_override = NULL;
    }
}

#ifndef _WIN32

__attribute__((destructor)) static void unload_liboverride(void) { onUnload(); }

#else /* _WIN32 */

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hinstDLL;
    (void)lpvReserved;
    if (fdwReason == DLL_PROCESS_DETACH)
    {
        onUnload();
    }
    return TRUE;
}

#endif /* _WIN32 */
