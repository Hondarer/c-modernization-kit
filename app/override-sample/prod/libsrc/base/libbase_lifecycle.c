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

#include "sym_loader_libbase.h"
#include <com_util/base/shared_lib_lifecycle.h>
#include <com_util/runtime/module.h>
#include <com_util/crt/path.h>
#include <errno.h>
#include <stdio.h>

/**
 *******************************************************************************
 *  @brief          ライブラリロード時に呼び出されます。
 *******************************************************************************
 */
void onLoad(void)
{
    char basename[COM_UTIL_SYM_LOADER_NAME_MAX] = {0};
    char leafname[COM_UTIL_SYM_LOADER_NAME_MAX + sizeof("_extdef.txt")] = {0};
    int err = 0;

    DLLMAIN_COM_UTIL_INFO_MSG("base: onLoad called");

    if (com_util_module_get_basename(basename, sizeof(basename), (const void *)onLoad) == 0)
    {
        if (com_util_path_concat(leafname, sizeof(leafname), &err, basename, "_extdef.txt") != 0)
        {
            sym_loader_configpath[0] = '\0';
            DLLMAIN_COM_UTIL_INFO_MSG("base: config path too long; override disabled");
        }
        else
        {
            char tmpdir[PLATFORM_PATH_MAX];
            if (com_util_get_temp_dir(tmpdir, sizeof(tmpdir), &err) == 0)
            {
                if (com_util_path_concat(sym_loader_configpath, sizeof(sym_loader_configpath), &err,
                                         tmpdir, PLATFORM_PATH_SEP, leafname) != 0)
                {
                    sym_loader_configpath[0] = '\0';
                    DLLMAIN_COM_UTIL_INFO_MSG("base: config path too long; override disabled");
                }
            }
            else if (err == ENAMETOOLONG)
            {
                sym_loader_configpath[0] = '\0';
                DLLMAIN_COM_UTIL_INFO_MSG("base: config path too long; override disabled");
            }
        }
    }

    com_util_sym_loader_init(fobj_array_libbase, fobj_length_libbase, sym_loader_configpath);
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
    com_util_sym_loader_dispose(fobj_array_libbase, fobj_length_libbase);
}
