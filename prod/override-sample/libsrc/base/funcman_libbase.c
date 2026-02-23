/**
 *******************************************************************************
 *  @file           funcman_libbase.c
 *  @brief          funcman が管理する関数ポインタの実定義。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  funcman_libbase.h には extern 宣言のみを宣言し、実体をここで定義します。\n
 *  関数を追加する場合は、funcman_libbase.h, funcman_libbase.c をメンテナンスします。\n
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "funcman_libbase.h"

/* --- 拡張可能な各関数のアクセス用のオブジェクトとアクセス用のポインタ設定 --- */

static funcman_object sfo_sample_func = NEW_FUNCMAN_OBJECT("sample_func", sample_func_t);
funcman_object *const pfo_sample_func = &sfo_sample_func;

/* --- funcman に渡すポインタ配列 --- */

funcman_object *const fobj_array_libbase[] = {
    &sfo_sample_func,
    /* &s_func_name, */ /* 将来追加 */
};

/* doxygen コメントは、ヘッダに記載 */
const size_t fobj_length_libbase = sizeof(fobj_array_libbase) / sizeof(fobj_array_libbase[0]);

void funcman_info_libbase()
{
    funcman_info(fobj_array_libbase, fobj_length_libbase);
    return;
}