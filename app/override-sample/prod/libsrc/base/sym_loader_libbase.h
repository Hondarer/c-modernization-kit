/**
 *******************************************************************************
 *  @file           sym_loader_libbase.h
 *  @brief          sym_loader が管理する関数ポインターを extern 宣言します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase 内の差し替え可能な関数に対応する変数の extern 宣言を提供します。
 *  関数を追加する場合は、sym_loader_libbase.h と sym_loader_libbase.c を更新します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SYM_LOADER_LIBBASE_PRIVATE_H
#define SYM_LOADER_LIBBASE_PRIVATE_H

#include <base/base_spec.h>
#include <cplat/crt/path.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /* --- 差し替え可能な各関数の sym_loader エントリへのポインター      --- */
    /* --- 対応関数を追加した場合、次への追加が必要です。                --- */
    /* --- 関数ポインター型 <lib>_<name>_fn は公開ヘッダーに定義します。 --- */

    /** base_calc に対応する sym_loader エントリへのポインター。 */
    extern cplat_sym_loader_entry *const pfo_base_calc;

    /* extern cplat_sym_loader_entry *const pfo_base_name; */ /* 将来追加 */

    /** sym_loader に設定するポインター配列。 */
    extern cplat_sym_loader_entry *const fobj_array_libbase[];

    /** sym_loader に設定するポインター配列の要素数です。 */
    extern const size_t fobj_length_libbase;

/** sym_loader 設定ファイルのパス長 (終端 '\0' を含む) */
#define SYM_LOADER_CONFIG_PATH_MAX PLATFORM_PATH_MAX

    /** sym_loader 設定ファイルのパスです。 */
    extern char sym_loader_configpath[SYM_LOADER_CONFIG_PATH_MAX];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYM_LOADER_LIBBASE_PRIVATE_H */
