/**
 *******************************************************************************
 *  @file           sym_loader_libbase.h
 *  @brief          sym_loader が管理する関数ポインタの extern 定義。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase 内の機能拡張対応関数の型定義および変数の extern 宣言を提供します。
 *  関数を追加する場合は、sym_loader_libbase.h, sym_loader_libbase.c をメンテナンスします。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SYM_LOADER_LIBBASE_H
#define SYM_LOADER_LIBBASE_H

#include <libbase.h>
#include <com_util/crt/path.h>

/* --- 拡張可能な各関数のポインタ型とアクセス用のオブジェクトへのポインタ --- */
/* --- 対応関数を追加した場合、以下に追加が必要です。                     --- */

/** sample_func に対応する関数ポインタの型定義。 */
typedef int (*sample_func_t)(const int, const int, int *);

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** sample_func に対応する sym_loader エントリへのポインタ。 */
extern com_util_sym_loader_entry_t *const pfo_sample_func;

/* typedef any (*func_name_t)(...); */            /* 将来追加 */
/* extern com_util_sym_loader_entry_t *const pfo_func_name; */ /* 将来追加 */

/** sym_loader に設定するポインタ配列。 */
extern com_util_sym_loader_entry_t *const fobj_array_libbase[];

/** sym_loader に設定するポインタ配列の要素数 */
extern const size_t fobj_length_libbase;

/** sym_loader 設定ファイルのパス長 (終端 '\0' を含む) */
#define SYM_LOADER_CONFIG_PATH_MAX PLATFORM_PATH_MAX

/** sym_loader 設定ファイルのパス */
extern char sym_loader_configpath[SYM_LOADER_CONFIG_PATH_MAX];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYM_LOADER_LIBBASE_H */
