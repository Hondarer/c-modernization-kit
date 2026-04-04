/**
 *******************************************************************************
 *  @file           symbol_loader_libbase.h
 *  @brief          symbol_loader が管理する関数ポインタの extern 定義。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase 内の機能拡張対応関数の型定義および変数の extern 宣言を提供します。
 *  関数を追加する場合は、symbol_loader_libbase.h, symbol_loader_libbase.c をメンテナンスします。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SYMBOL_LOADER_LIBBASE_H
#define SYMBOL_LOADER_LIBBASE_H

#include <libbase.h>

/* --- 拡張可能な各関数のポインタ型とアクセス用のオブジェクトへのポインタ --- */
/* --- 対応関数を追加した場合、以下に追加が必要です。                     --- */

/** sample_func に対応する関数ポインタの型定義。 */
typedef int (*sample_func_t)(const int, const int, int *);
/** sample_func に対応する symbol_loader エントリへのポインタ。 */
extern symbol_loader_entry_t *const pfo_sample_func;

/* typedef any (*func_name_t)(...); */            /* 将来追加 */
/* extern symbol_loader_entry_t *const pfo_func_name; */ /* 将来追加 */

/** symbol_loader に設定するポインタ配列。 */
extern symbol_loader_entry_t *const fobj_array_libbase[];

/** symbol_loader に設定するポインタ配列の要素数 */
extern const size_t fobj_length_libbase;

/** symbol_loader 設定ファイルのパス長 (終端 '\0' を含む) */
#define SYMBOL_LOADER_CONFIG_PATH_MAX 1024

/** symbol_loader 設定ファイルのパス */
extern char symbol_loader_configpath[SYMBOL_LOADER_CONFIG_PATH_MAX];

#endif /* SYMBOL_LOADER_LIBBASE_H */
