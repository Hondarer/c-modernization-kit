/**
 *******************************************************************************
 *  @file           libbase_lifecycle.c
 *  @brief          base 共有ライブラリのロード時とアンロード時の処理を提供します。
 *  @author         c-modernization-kit sample team
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

static int shutdown_completed = 0;

/**
 *  @brief          ライブラリ ロード時に呼び出されます。
 */
void onLoad(void)
{
    char basename[CPLAT_SYM_LOADER_NAME_MAX] = {0};
    char leafname[CPLAT_SYM_LOADER_NAME_MAX + sizeof("_extdef.jsonc")] = {0};
    cplat_error error;

    CPLAT_DLLMAIN_INFO_MSG("base: onLoad called");

    /*
     * 自モジュールのベース名に _extdef.jsonc を付け、一時ディレクトリ上の定義ファイル パスにする。
     * ベース名は onLoad のアドレスが属するモジュールから得る。
     * パスが長すぎる場合は空文字にする。空のパスでは cplat_sym_loader_init() は設定を読まない。
     * 正本は app/cplat/prod/libsrc/cplat/runtime/README.md の「module_info」節と、
     * app/override-sample/prod/README.md の「設定ファイル」節である。
     */
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

    {
        int rc = cplat_sym_loader_init(fobj_array_libbase, fobj_length_libbase, sym_loader_configpath);
        if ((rc != CPLAT_OK) && (rc != CPLAT_SKIPPED))
        {
            CPLAT_DLLMAIN_INFO_MSG("base: sym_loader initialization failed");
        }
    }
}

/**
 *  @brief          ライブラリ アンロード時に呼び出されます。
 *  @param[in]      process_terminating プロセス終了時は 1、明示的アンロード時は 0。
 */
void onUnload(int process_terminating)
{
    /* see: https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-freelibrary */
    CPLAT_DLLMAIN_INFO_MSG("base: onUnload called");
#if defined(PLATFORM_WINDOWS)
    if ((process_terminating == 0) && (shutdown_completed == 0))
    {
        CPLAT_DLLMAIN_INFO_MSG("base: sym_loader shutdown was not completed");
    }
#else
    (void)process_terminating;
#endif /* PLATFORM_WINDOWS */
}

/* Doxygen コメントは、ヘッダーに記載 */

int base_sym_loader_shutdown(void)
{
    if (cplat_sym_loader_dispose(fobj_array_libbase, fobj_length_libbase) != CPLAT_OK)
    {
        return BASE_ERR_UNKNOWN;
    }

    shutdown_completed = 1;
    return BASE_OK;
}
