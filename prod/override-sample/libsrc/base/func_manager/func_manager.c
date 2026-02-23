/**
 *******************************************************************************
 *  @file           func_manager.c
 *  @brief          動的関数管理機能の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/22
 *  @version        1.0.0
 *
 *  特定の関数の機能を定義ベースによって拡張可能な動的関数管理機能を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "func_manager_local.h"
#include <func_manager.h>

/** 管理対象のキャッシュポインタ配列 (func_manager_init で設定される) */
func_object *const *_func_objects = NULL;

/** _func_objects の要素数 */
size_t _func_objects_count = 0;
