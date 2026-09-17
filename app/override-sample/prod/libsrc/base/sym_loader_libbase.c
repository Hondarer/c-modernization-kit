/**
 *******************************************************************************
 *  @file           sym_loader_libbase.c
 *  @brief          sym_loader が管理する関数ポインターの実体を定義します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  sym_loader_libbase.h には extern 宣言のみを記述し、実体をここで定義します。
 *  関数を追加する場合は、sym_loader_libbase.h と sym_loader_libbase.c を更新します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "sym_loader_libbase.h"
#include <cplat/base/result.h>
#include <stdio.h>

/* Doxygen コメントはヘッダーに記述 */

char sym_loader_configpath[SYM_LOADER_CONFIG_PATH_MAX] = {0};

/* --- 差し替え可能な各関数のアクセス用オブジェクトとアクセス用ポインターの設定 --- */
/* --- 対応関数を追加した場合、次への追加が必要です。                             --- */

/** base_calc 用の sym_loader エントリ実体です。差し替えキーは公開関数名と同じにします。 */
static cplat_sym_loader_entry sfo_base_calc = CPLAT_SYM_LOADER_ENTRY_INIT("base_calc", base_calc_fn);
/* Doxygen コメントはヘッダーに記述 */

cplat_sym_loader_entry *const pfo_base_calc = &sfo_base_calc;

/* static cplat_sym_loader_entry sfo_base_name = CPLAT_SYM_LOADER_ENTRY_INIT("base_name", base_name_fn); */ /* 将来追加 */
/* cplat_sym_loader_entry *const pfo_base_name = &sfo_base_name; */ /* 将来追加 */

/* --- sym_loader に渡すポインター配列                --- */
/* --- 対応関数を追加した場合、次への追加が必要です。 --- */

/* Doxygen コメントはヘッダーに記述 */

cplat_sym_loader_entry *const fobj_array_libbase[] = {
    &sfo_base_calc,
    /* &sfo_base_name, */ /* 将来追加 */
};

/* Doxygen コメントはヘッダーに記述 */

const size_t fobj_length_libbase = sizeof(fobj_array_libbase) / sizeof(fobj_array_libbase[0]);

/* Doxygen コメントはヘッダーに記述 */

int base_sym_loader_info(void)
{
    int ret;

    printf("- congigpath: %s\n", sym_loader_configpath);
    ret = cplat_sym_loader_info(fobj_array_libbase, fobj_length_libbase);
    if (ret != CPLAT_OK)
    {
        return BASE_ERR_UNKNOWN;
    }
    return BASE_OK;
}
