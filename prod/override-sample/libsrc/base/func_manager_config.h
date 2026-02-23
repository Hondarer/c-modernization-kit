/**
 *******************************************************************************
 *  @file           func_manager_config.h
 *  @brief          func_manager が管理する関数ポインタの extern 定義。
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

#ifndef FUNC_MANAGER_CONFIG_H
#define FUNC_MANAGER_CONFIG_H

#include <func_manager.h>

/* --- 拡張可能な各関数のポインタ型とアクセス用のオブジェクトへのポインタ --- */

typedef int (*sample_func_t)(const int, const int, int *);
extern func_object *const p_sample_func;

/** func_manager に設定するポインタ配列。 */
extern func_object *const func_objects[];

/** func_manager に設定するポインタ配列の要素数 */
extern const size_t FUNC_OBEJCTS_COUNT;

#endif /* FUNC_MANAGER_CONFIG_H */
