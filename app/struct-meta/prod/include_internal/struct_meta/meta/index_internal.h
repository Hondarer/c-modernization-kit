/**
 *******************************************************************************
 *  @file           index_internal.h
 *  @brief          索引 (@ref struct_meta_index_register) の引き当てをライブラリ内へ提供します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_META_INDEX_INTERNAL_H
#define STRUCT_META_META_INDEX_INTERNAL_H

#include <struct_meta/meta/meta.h>

#include <stddef.h>

/**
 *  @brief          記述子に控えた検査結果を引き当てます。
 *  @param[in]      descriptor 記述子です。NULL を渡してはなりません。
 *  @param[out]     result_out 控えていた検査結果の格納先です。NULL を渡してはなりません。
 *  @return         控えがあれば @c CPLAT_OK 、未登録なら @c CPLAT_SKIPPED 、
 *                  引数が不正なら @c CPLAT_ERR_INVALID_ARGUMENT を返します。
 *
 *  @c CPLAT_SKIPPED は異常ではありません。呼び出し側は記述子を検査し直してください。
 *
 *  @par            スレッド セーフ
 *  本関数は条件付きスレッド セーフです。\n
 *  @ref struct_meta_index_register および @ref struct_meta_index_unregister と
 *  同時に呼び出してはなりません。
 */
int struct_meta_internal_index_find_validation(const struct_meta_descriptor *descriptor, int *result_out);

/**
 *  @brief          フィールド名から @c fields のインデックスを引き当てます。
 *  @param[in]      descriptor  記述子です。NULL を渡してはなりません。
 *  @param[in]      name        フィールド名です。NUL 終端していなくても構いません。
 *                              NULL を渡してはなりません。
 *  @param[in]      name_length @p name の長さです。
 *  @param[out]     index_out   インデックスの格納先です。NULL を渡してはなりません。
 *  @return         見つかれば @c CPLAT_OK 、索引にあって不一致なら @c CPLAT_ERR_NOT_FOUND 、
 *                  索引がなければ @c CPLAT_SKIPPED 、引数が不正なら
 *                  @c CPLAT_ERR_INVALID_ARGUMENT を返します。
 *
 *  @c CPLAT_SKIPPED は異常ではありません。呼び出し側は線形走査で検索してください。
 *
 *  @par            スレッド セーフ
 *  本関数は条件付きスレッド セーフです。\n
 *  @ref struct_meta_index_register および @ref struct_meta_index_unregister と
 *  同時に呼び出してはなりません。
 */
int struct_meta_internal_index_find_field(const struct_meta_descriptor *descriptor, const char *name,
                                          size_t name_length, size_t *index_out);

#endif /* STRUCT_META_META_INDEX_INTERNAL_H */
