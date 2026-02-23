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
 *  は dllmain.h が提供します。このファイルは onLoad / onUnload を定義します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "funcman_libbase.h"
#include <dllmain.h>
#include <libbase.h>
#include <stdio.h>

/* doxygen コメントは、ヘッダに記載 */
void onLoad(void)
{
    char basename[FUNCMAN_NAME_MAX] = {0};

    DLLMAIN_INFO_MSG("base: onLoad called");

    if (get_lib_basename(basename, sizeof(basename), (const void *)onLoad) == 0)
    {
#ifndef _WIN32
        /* Linux: 定義ファイルを /tmp から読み込み */
        snprintf(funcman_configpath, sizeof(funcman_configpath), "/tmp/%s_extdef.txt", basename);
#else  /* _WIN32 */
        /* Windows: 定義ファイルを %TEMP% から読み込み */
        wchar_t tmpw[MAX_PATH] = L"";
        DWORD n = GetTempPathW((DWORD)(sizeof(tmpw) / sizeof(tmpw[0])), tmpw);
        if (n > 0 && n < (DWORD)(sizeof(tmpw) / sizeof(tmpw[0])))
        {
            /* UTF-16 -> UTF-8 変換 */
            char tmpu8[MAX_PATH * 4] = {0};
            int m = WideCharToMultiByte(CP_UTF8, 0, tmpw, -1, tmpu8, (int)sizeof(tmpu8), NULL, NULL);
            if (m > 0)
            {
                /* GetTempPathW は通常末尾に '\' を付けて返す */
                snprintf(funcman_configpath, sizeof(funcman_configpath), "%s%s_extdef.txt", tmpu8, basename);
            }
        }
#endif /* _WIN32 */
    }

    funcman_init(fobj_array_libbase, fobj_length_libbase, funcman_configpath);
}

/* doxygen コメントは、ヘッダに記載 */
void onUnload(void)
{
    DLLMAIN_INFO_MSG("base: onUnload called");
    funcman_dispose(fobj_array_libbase, fobj_length_libbase);
}
