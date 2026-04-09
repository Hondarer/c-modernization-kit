/**
 *******************************************************************************
 *  @file           symbol_loader_libbase.c
 *  @brief          symbol_loader が管理する関数ポインタの実定義。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  symbol_loader_libbase.h には extern 宣言のみを宣言し、実体をここで定義します。
 *  関数を追加する場合は、symbol_loader_libbase.h, symbol_loader_libbase.c をメンテナンスします。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "symbol_loader_libbase.h"
#include <stdio.h>

/* doxygen コメントは、ヘッダに記載 */
char symbol_loader_configpath[SYMBOL_LOADER_CONFIG_PATH_MAX] = {0};

/* --- 拡張可能な各関数のアクセス用のオブジェクトとアクセス用のポインタ設定 --- */
/* --- 対応関数を追加した場合、以下に追加が必要です。                       --- */

/** sample_func 用の symbol_loader エントリ実体。 */
static symbol_loader_entry_t sfo_sample_func = SYMBOL_LOADER_ENTRY_INIT("sample_func", sample_func_t);
/* doxygen コメントは、ヘッダに記載 */
symbol_loader_entry_t *const pfo_sample_func = &sfo_sample_func;

/* static symbol_loader_entry_t sfo_func_name = SYMBOL_LOADER_ENTRY_INIT("func_name", func_name_t); */ /* 将来追加 */
/* symbol_loader_entry_t *const pfo_func_name = &sfo_func_name; */                               /* 将来追加 */

/* --- symbol_loader に渡すポインタ配列                     --- */
/* --- 対応関数を追加した場合、以下に追加が必要です。 --- */

/* doxygen コメントは、ヘッダに記載 */
symbol_loader_entry_t *const fobj_array_libbase[] = {
    &sfo_sample_func,
    /* &sfo_func_name, */ /* 将来追加 */
};

/* doxygen コメントは、ヘッダに記載 */
const size_t fobj_length_libbase = sizeof(fobj_array_libbase) / sizeof(fobj_array_libbase[0]);

/* doxygen コメントは、ヘッダに記載 */
BASE_EXPORT int BASE_API symbol_loader_info_libbase()
{
    printf("- congigpath: %s\n", symbol_loader_configpath);
    return symbol_loader_info(fobj_array_libbase, fobj_length_libbase);
}
