/**
 *******************************************************************************
 *  @file           func_manager_config.c
 *  @brief          func_manager が管理する関数ポインタの実定義。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  func_manager_config.h には extern 宣言のみを宣言し、_func_objects の実体をここで定義します。\n
 *  関数を追加する場合は、func_manager_config.h, func_manager_config.c をメンテナンスします。\n
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "func_manager_config.h"

/* --- 拡張可能な各関数のアクセス用のオブジェクトとアクセス用のポインタ設定 --- */

static func_object s_sample_func = NEW_FUNC_OBJECT("sample_func", sample_func_t);
func_object *const p_sample_func = &s_sample_func;

/* --- func_manager に渡すポインタ配列 --- */

func_object *const func_objects[] = {
    &s_sample_func,
    /* &s_func_name, */ /* 将来追加 */
};

/* doxygen コメントは、ヘッダに記載 */
const size_t FUNC_OBEJCTS_COUNT = sizeof(func_objects) / sizeof(func_objects[0]);
