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
 *
 *  Windows: DllMain の DLL_PROCESS_DETACH により FreeLibrary(base.dll) または
 *           プロセス正常終了時に自動的に呼び出されます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "libbase_local.h"
#include <stddef.h>

/* DLL アンロード時の API 呼び出し制約に対応したログ出力マクロ */
#ifndef _WIN32
    #include <syslog.h>
    #define LOG_INFO_MSG(msg) syslog(LOG_INFO, (msg))
#else /* _WIN32 */
    #define LOG_INFO_MSG(msg) OutputDebugStringA(msg)
#endif /* _WIN32 */

static void onUnload(void);

void onUnload(void)
{
    LOG_INFO_MSG("base: onUnload called");
    libbase_unload_all();
}

#ifndef _WIN32

/**
 *  @brief          共有ライブラリアンロード時のデストラクタ (Linux 専用)。
 *  @details        __attribute__((destructor)) により、dlclose(base.so) または
 *                  プロセス正常終了時に自動的に呼び出されます。
 */
__attribute__((destructor)) static void unload_liboverride(void)
{
    onUnload();
}

#else /* _WIN32 */

/**
 *  @brief          DLL エントリーポイント (Windows 専用)。
 *  @details        DLL_PROCESS_DETACH を受け取った際に onUnload() を呼び出し、
 *                   liboverride のハンドルと関数ポインタを解放します。
 *
 *  @param[in]      hinstDLL    DLL のインスタンスハンドル (本実装では未使用)。
 *  @param[in]      fdwReason   呼び出し理由。DLL_PROCESS_DETACH の場合のみ処理します。
 *  @param[in]      lpvReserved 予約済みパラメータ (本実装では未使用)。
 *  @return         常に TRUE を返します。
 */
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
