/**
 *******************************************************************************
 *  @file           access.h
 *  @brief          メタデータを使って構造体のフィールドと要素へアクセスします。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_ACCESS_H
#define STRUCT_META_ACCESS_H

#include <struct_meta/meta/meta.h>

/**
 *  @addtogroup STRUCT_META_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** @brief インデックスでフィールドを取得します。@param[in] descriptor 記述子です。@param[in] index インデックスです。@param[out] field_out 取得結果です。@return 結果コードです。@par スレッド セーフ 共有状態を変更しません。 */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_descriptor_get_field(const struct_meta_descriptor *descriptor,
                                                                            size_t index,
                                                                            const struct_meta_field **field_out);
    /** @brief 名前でフィールドを検索します。@param[in] descriptor 記述子です。@param[in] name 名前です。@param[out] field_out 取得結果です。@return 結果コードです。@par スレッド セーフ 共有状態を変更しません。 */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_descriptor_find_field(const struct_meta_descriptor *descriptor,
                                                                             const char *name,
                                                                             const struct_meta_field **field_out);
    /**
     *  @brief          構造体記述子の属性を検索します。
     *  @param[in]      descriptor   構造体記述子です。NULL を渡してはなりません。
     *  @param[in]      key          属性キーです。NULL を渡してはなりません。
     *  @param[out]     attribute_out 検索結果です。NULL を渡してはなりません。
     *  @return         成功時は @c CPLAT_OK、未検出時は @c CPLAT_ERR_NOT_FOUND、引数不正時は
     *                  @c CPLAT_ERR_INVALID_ARGUMENT、記述子不正時は対応するエラー コードを返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_descriptor_find_attribute(
        const struct_meta_descriptor *descriptor, const char *key, const struct_meta_attribute **attribute_out);
    /** @brief 属性を検索します。@param[in] field フィールドです。@param[in] key 属性キーです。@param[out] attribute_out 取得結果です。@return 結果コードです。@par スレッド セーフ 共有状態を変更しません。 */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_field_find_attribute(
        const struct_meta_field *field, const char *key, const struct_meta_attribute **attribute_out);
    /** @brief 配列要素を取得します。@param[in] field フィールドです。@param[in,out] instance 親構造体です。@param[in] index インデックスです。@param[out] element_out 取得結果です。@return 結果コードです。@par スレッド セーフ 同じインスタンスを並行変更しない場合に限ります。 */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_field_get_element(const struct_meta_field *field, void *instance,
                                                                         size_t index, void **element_out);
    /** @brief 読み取り専用の配列要素を取得します。@param[in] field フィールドです。@param[in] instance 親構造体です。@param[in] index インデックスです。@param[out] element_out 取得結果です。@return 結果コードです。@par スレッド セーフ 同じインスタンスを並行変更しない場合に限ります。 */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_field_get_const_element(const struct_meta_field *field,
                                                                               const void *instance, size_t index,
                                                                               const void **element_out);
    /** @brief パスを変更可能な値へ解決します。@param[in] descriptor 記述子です。@param[in,out] instance 構造体です。@param[in] path パスです。@param[out] field_out 終端フィールドです。@param[out] value_out 終端値です。@return 結果コードです。@par スレッド セーフ 同じインスタンスを並行変更しない場合に限ります。 */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_path_resolve(const struct_meta_descriptor *descriptor,
                                                                    void *instance, const char *path,
                                                                    const struct_meta_field **field_out,
                                                                    void **value_out);
    /** @brief パスを読み取り専用の値へ解決します。@param[in] descriptor 記述子です。@param[in] instance 構造体です。@param[in] path パスです。@param[out] field_out 終端フィールドです。@param[out] value_out 終端値です。@return 結果コードです。@par スレッド セーフ 同じインスタンスを並行変更しない場合に限ります。 */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_path_resolve_const(const struct_meta_descriptor *descriptor,
                                                                          const void *instance, const char *path,
                                                                          const struct_meta_field **field_out,
                                                                          const void **value_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRUCT_META_ACCESS_H */
