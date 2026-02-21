/**
 *******************************************************************************
 *  @file           libbase_local.h
 *  @brief          libbase 内部共有ヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  func.c と DllMain.c の間で共有する型定義および変数宣言を提供します。
 *  libbase の外部公開 API には含めません。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBBASE_LOCAL_H
#define LIBBASE_LOCAL_H

#ifndef _WIN32
#    include <dlfcn.h>
#else  /* _WIN32 */
#    include <windows.h>
#endif /* _WIN32 */

/** liboverride の関数ポインタ型 */
typedef int (*func_override_t)(const int, const int, const int, int *);

/**
 * liboverride の DLL/SO ハンドルキャッシュ。
 * func.c で定義し、DllMain.c からも参照する。
 */
#ifndef _WIN32
extern void            *s_handle;
#else  /* _WIN32 */
extern HMODULE          s_handle;
#endif /* _WIN32 */

/**
 * liboverride の関数ポインタキャッシュ。
 * func.c で定義し、DllMain.c からも参照する。
 */
extern func_override_t  s_func_override;

#endif /* LIBBASE_LOCAL_H */
