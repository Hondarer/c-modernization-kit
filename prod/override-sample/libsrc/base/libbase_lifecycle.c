/**
 *******************************************************************************
 *  @file           dllmain_libbase.c
 *  @brief          base.so / base.dll のロード・アンロード時処理。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  base.so / base.dll のロード時およびアンロード時に処理を行います。
 *
 *  プラットフォームごとのフック (Linux constructor/destructor, Windows DllMain)
 *  は dllmain-util.h が提供します。このファイルは onLoad / onUnload を定義します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "symbol_loader_libbase.h"
#include <util/base/shared_lib_lifecycle.h>
#include <util/runtime/module_info.h>
#include <util/fs/path_max.h>
#include <stdio.h>

/**
 *******************************************************************************
 *  @brief          ライブラリロード時に呼び出されます。
 *******************************************************************************
 */
void onLoad(void)
{
    char basename[SYMBOL_LOADER_NAME_MAX] = {0};

    DLLMAIN_UTIL_INFO_MSG("base: onLoad called");

    if (module_info_get_basename(basename, sizeof(basename), (const void *)onLoad) == 0)
    {
#if defined(PLATFORM_LINUX)
        /* Linux: 定義ファイルを /tmp から読み込み */
        snprintf(symbol_loader_configpath, sizeof(symbol_loader_configpath), "/tmp/%s_extdef.txt", basename);
#elif defined(PLATFORM_WINDOWS)
        /* Windows: 定義ファイルを %TEMP% から読み込み */
        wchar_t tmpw[PLATFORM_PATH_MAX] = L"";
        DWORD n = GetTempPathW((DWORD)(sizeof(tmpw) / sizeof(tmpw[0])), tmpw);
        if (n > 0 && n < (DWORD)(sizeof(tmpw) / sizeof(tmpw[0])))
        {
            /* UTF-16 -> UTF-8 変換 */
            char tmpu8[PLATFORM_PATH_MAX * 4] = {0};
            int m = WideCharToMultiByte(CP_UTF8, 0, tmpw, -1, tmpu8, (int)sizeof(tmpu8), NULL, NULL);
            if (m > 0)
            {
                /* GetTempPathW は通常末尾に '\' を付けて返す */
                snprintf(symbol_loader_configpath, sizeof(symbol_loader_configpath), "%s%s_extdef.txt", tmpu8, basename);
            }
        }
#endif /* PLATFORM_ */
    }

    symbol_loader_init(fobj_array_libbase, fobj_length_libbase, symbol_loader_configpath);
}

/**
 *******************************************************************************
 *  @brief          ライブラリアンロード時に呼び出されます。
 *  @param[in]      process_terminating プロセス終了時は 1、明示的アンロード時は 0。
 *******************************************************************************
 */
void onUnload(int process_terminating)
{
    (void)process_terminating;
    DLLMAIN_UTIL_INFO_MSG("base: onUnload called");
    symbol_loader_dispose(fobj_array_libbase, fobj_length_libbase);
}
