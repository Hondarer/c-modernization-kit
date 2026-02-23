/**
 *******************************************************************************
 *  @file           funcman_libbase.h
 *  @brief          funcman が管理する関数ポインタの extern 定義。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase 内の機能拡張対応関数の型定義および変数の extern 宣言を提供します。
 *  libbase 内での利用を前提とするため、外部公開 API には含めません。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef FUNCMAN_LIBBASE_H
#define FUNCMAN_LIBBASE_H

#include <libbase.h>

/* --- 拡張可能な各関数のポインタ型とアクセス用のオブジェクトへのポインタ --- */

typedef int (*sample_func_t)(const int, const int, int *);
extern funcman_object *const pfo_sample_func;

/** funcman に設定するポインタ配列。 */
extern funcman_object *const fobj_array_libbase[];

/** funcman に設定するポインタ配列の要素数 */
extern const size_t fobj_length_libbase;

#endif /* FUNCMAN_LIBBASE_H */
