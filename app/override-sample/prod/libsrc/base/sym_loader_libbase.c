/**
 *******************************************************************************
 *  @file           sym_loader_libbase.c
 *  @brief          sym_loader が管理する関数ポインターの実定義。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  sym_loader_libbase.h には extern 宣言のみを宣言し、実体をここで定義します。
 *  関数を追加する場合は、sym_loader_libbase.h, sym_loader_libbase.c をメンテナンスします。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "sym_loader_libbase.h"
#include <stdio.h>

/* Doxygen コメントは、ヘッダーに記載 */

char sym_loader_configpath[SYM_LOADER_CONFIG_PATH_MAX] = {0};

/* --- 拡張可能な各関数のアクセス用のオブジェクトとアクセス用のポインター設定 --- */
/* --- 対応関数を追加した場合、以下に追加が必要です。                         --- */

/** sample_func 用の sym_loader エントリ実体。 */
static com_util_sym_loader_entry sfo_sample_func = COM_UTIL_SYM_LOADER_ENTRY_INIT("sample_func", sample_func_t);
/* Doxygen コメントは、ヘッダーに記載 */

com_util_sym_loader_entry *const pfo_sample_func = &sfo_sample_func;

/* static com_util_sym_loader_entry sfo_func_name = COM_UTIL_SYM_LOADER_ENTRY_INIT("func_name", func_name_t); */ /* 将来追加 */
/* com_util_sym_loader_entry *const pfo_func_name = &sfo_func_name; */ /* 将来追加 */

/* --- sym_loader に渡すポインター配列                --- */
/* --- 対応関数を追加した場合、以下に追加が必要です。 --- */

/* Doxygen コメントは、ヘッダーに記載 */

com_util_sym_loader_entry *const fobj_array_libbase[] = {
    &sfo_sample_func,
    /* &sfo_func_name, */ /* 将来追加 */
};

/* Doxygen コメントは、ヘッダーに記載 */

const size_t fobj_length_libbase = sizeof(fobj_array_libbase) / sizeof(fobj_array_libbase[0]);

/* Doxygen コメントは、ヘッダーに記載 */

int sym_loader_info_libbase()
{
    printf("- congigpath: %s\n", sym_loader_configpath);
    return com_util_sym_loader_info(fobj_array_libbase, fobj_length_libbase);
}
