/**
 *******************************************************************************
 *  @file           patch.h
 *  @brief          メタデータを使った対話形式の構造体編集を提供します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_PATCH_H
#define STRUCT_META_PATCH_H

#include <struct_meta/meta/meta.h>

/**
 *  @addtogroup STRUCT_META_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          フィールドをメニュー形式で選択し、構造体を対話編集します。
     *  @param[in]      descriptor 構造体の記述子です。NULL は指定できません。
     *  @param[in,out]  instance 編集対象の構造体です。NULL は指定できません。
     *  @return         成功時は @c CPLAT_OK、失敗時はエラー コードを返します。
     *
     *  メニューには現在位置と、パス指定に利用できる各候補の C フィールドパスを表示します。
     *  フィールドは番号または現在位置のメンバー名の完全一致で選択できます。
     *  配列要素の選択は番号で行います。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  同一 @p instance と標準入出力を、ほかのスレッドから同時に操作しないでください。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_patch_interactive(const struct_meta_descriptor *descriptor,
                                                                         void *instance);

    /**
     *  @brief          C フィールド名のパスで対象を選択し、構造体を対話編集します。
     *  @param[in]      descriptor 構造体の記述子です。NULL は指定できません。
     *  @param[in,out]  instance 編集対象の構造体です。NULL は指定できません。
     *  @param[in]      path `addresses[0].city` 形式のパスです。NULL と空文字列は指定できません。
     *  @return         成功時は @c CPLAT_OK、失敗時はエラー コードを返します。
     *
     *  インデックスを省略した配列では要素選択を表示します。\n
     *  構造体で終わるパスでは、その構造体のフィールド選択を表示します。
     *  これらのメニューには現在位置と、続けて選択できる各候補の完全パスを表示します。
     *  フィールドは番号または現在位置のメンバー名の完全一致で選択でき、配列要素は番号で選択します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  同一 @p instance と標準入出力を、ほかのスレッドから同時に操作しないでください。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_patch_path_interactive(const struct_meta_descriptor *descriptor,
                                                                              void *instance, const char *path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRUCT_META_PATCH_H */
