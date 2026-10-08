/**
 *******************************************************************************
 *  @file           build.h
 *  @brief          構文解析結果から記述子を組み立てます。
 *
 *  レイアウトは `layout` が求めます。生成コードが出力する @c offsetof / @c sizeof と
 *  同じ値になることは、生成コードへ出力する @c _Static_assert が毎ビルド検査します。
 *  see: app/struct-meta/docs/architecture.md
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_CATALOG_BUILD_H
#define STRUCT_META_CATALOG_BUILD_H

#include <struct_meta/catalog/arena.h>
#include <struct_meta/meta/meta.h>
#include <struct_meta/parse/ast.h>
#include <struct_meta/parse/parse.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          構文解析結果のすべての構造体から記述子を組み立てます。
     *  @param[in]      structs          構文解析結果。NULL を渡してはなりません。
     *  @param[in,out]  arena            記述子の記憶域。NULL を渡してはなりません。
     *  @param[out]     descriptors_out  記述子の配列の格納先。NULL を渡してはなりません。
     *  @param[out]     count_out        記述子の数の格納先。NULL を渡してはなりません。
     *  @param[out]     diagnostic       診断の書き込み先。NULL を渡せます。
     *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、@c CPLAT_ERR_OUT_OF_MEMORY 、
     *                  @c CPLAT_ERR_NOT_FOUND 、または @c CPLAT_ERR_CORRUPT_DESCRIPTOR を返します。
     *
     *  記述子の並びは構文解析結果の宣言順です。ネストした構造体も同じ配列に含まれます。\n
     *  記述子とその参照先はすべて @p arena から確保します。呼び出し側は個別に解放せず、
     *  アリーナごと破棄します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。同じアリーナを複数のスレッドから
     *  同時に操作してはなりません。
     */
    int struct_meta_internal_build_descriptors(const struct_meta_internal_parse_struct_list *structs,
                                               struct_meta_internal_arena *arena,
                                               const struct_meta_descriptor *const **descriptors_out, size_t *count_out,
                                               struct_meta_diagnostic *diagnostic);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRUCT_META_CATALOG_BUILD_H */
