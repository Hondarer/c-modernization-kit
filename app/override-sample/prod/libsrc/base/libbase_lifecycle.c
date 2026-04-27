/**
 *******************************************************************************
 *  @file           libbase_lifecycle.c
 *  @brief          base.so / base.dll のロード・アンロード時処理。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  base.so / base.dll のロード時およびアンロード時に処理を行います。
 *
 *  プラットフォームごとのフック (Linux constructor/destructor, Windows DllMain)
 *  は shared_lib_lifecycle.h が提供します。このファイルは onLoad / onUnload を定義します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "symbol_loader_libbase.h"
#include <com_util/base/shared_lib_lifecycle.h>
#include <com_util/runtime/module_info.h>
#include <com_util/crt/path.h>
#include <stdio.h>

/**
 *******************************************************************************
 *  @brief          ライブラリロード時に呼び出されます。
 *******************************************************************************
 */
void onLoad(void)
{
    char basename[SYMBOL_LOADER_NAME_MAX] = {0};

    DLLMAIN_COM_UTIL_INFO_MSG("base: onLoad called");

    if (module_info_get_basename(basename, sizeof(basename), (const void *)onLoad) == 0)
    {
        {
            char tmpdir[PLATFORM_PATH_MAX];
            if (com_util_get_temp_dir(tmpdir, sizeof(tmpdir), NULL) == 0)
            {
                snprintf(symbol_loader_configpath, sizeof(symbol_loader_configpath),
                         "%s" PLATFORM_PATH_SEP "%s_extdef.txt", tmpdir, basename);
            }
        }
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
    DLLMAIN_COM_UTIL_INFO_MSG("base: onUnload called");
    symbol_loader_dispose(fobj_array_libbase, fobj_length_libbase);
}
