/**
 *******************************************************************************
 *  @file           libbase_lifecycle.c
 *  @brief          base 共有ライブラリのロード時とアンロード時の処理を提供します。
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
#include <cplat/base/error.h>
#include <cplat/base/shared_lib_lifecycle.h>
#include <cplat/crt/path.h>
#include <cplat/runtime/module.h>

/**
 *  @brief          ライブラリ ロード時に呼び出されます。
 */
void onLoad(void)
{
    char basename[CPLAT_SYM_LOADER_NAME_MAX] = {0};
    char leafname[CPLAT_SYM_LOADER_NAME_MAX + sizeof("_extdef.jsonc")] = {0};
    cplat_error error;

    CPLAT_DLLMAIN_INFO_MSG("base: onLoad called");

    if (cplat_module_get_basename(basename, sizeof(basename), (const void *)onLoad) == CPLAT_OK)
    {
        if (cplat_path_concat(leafname, sizeof(leafname), &error, basename, "_extdef.jsonc") != CPLAT_OK)
        {
            sym_loader_configpath[0] = '\0';
            CPLAT_DLLMAIN_INFO_MSG("base: config path too long; override disabled");
        }
        else
        {
            char tmpdir[PLATFORM_PATH_MAX];
            if (cplat_path_get_temp_dir(tmpdir, sizeof(tmpdir), &error) == CPLAT_OK)
            {
                if (cplat_path_concat(sym_loader_configpath, sizeof(sym_loader_configpath), &error, tmpdir,
                                         PLATFORM_PATH_SEP, leafname) != CPLAT_OK)
                {
                    sym_loader_configpath[0] = '\0';
                    CPLAT_DLLMAIN_INFO_MSG("base: config path too long; override disabled");
                }
            }
            else if (cplat_error_is(&error, CPLAT_CAUSE_NAME_TOO_LONG) != 0)
            {
                sym_loader_configpath[0] = '\0';
                CPLAT_DLLMAIN_INFO_MSG("base: config path too long; override disabled");
            }
        }
    }

    cplat_sym_loader_init(fobj_array_libbase, fobj_length_libbase, sym_loader_configpath);
}

/**
 *  @brief          ライブラリ アンロード時に呼び出されます。
 *  @param[in]      process_terminating プロセス終了時は 1、明示的アンロード時は 0。
 */
void onUnload(int process_terminating)
{
    (void)process_terminating;
    CPLAT_DLLMAIN_INFO_MSG("base: onUnload called");
    cplat_sym_loader_dispose(fobj_array_libbase, fobj_length_libbase);
}
