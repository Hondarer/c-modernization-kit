/**
 *******************************************************************************
 *  @file           index.h
 *  @brief          記述子を登録し、検査結果とフィールド名の索引を保持します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_META_INDEX_H
#define STRUCT_META_META_INDEX_H

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
     *  @brief          記述子を索引へ登録します。
     *  @param[in]      descriptor 登録する記述子です。NULL を渡してはなりません。
     *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
     *                  @c CPLAT_ERR_OUT_OF_MEMORY を返します。
     *
     *  登録した記述子では、@ref struct_meta_descriptor_validate の検査結果を控えて
     *  再検査を省き、フィールド名の検索を線形走査からハッシュ表引きへ切り替えます。\n
     *  ネストした記述子も再帰的に登録します。登録済みの記述子を再度登録しても
     *  @c CPLAT_OK を返し、内容は変わりません。\n
     *  索引を作れなかった場合はエラーを返します。索引の無い状態を黙って作りません。
     *
     *  登録できるのは、寿命が登録期間より長く、内容が変化しない記述子だけです。\n
     *  記述子のアドレスを鍵にするため、解放または再利用されるアドレスの記述子を
     *  登録したままにすると、別の記述子へ誤って一致します。\n
     *  自動変数として組み立てた記述子は、スコープを抜ける前に
     *  @ref struct_meta_index_unregister してください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  登録と登録解除 (カタログの作成と破棄、生成コードによるカタログの初回取得を含む) は呼び出し側で直列化し、
     *  索引を参照する操作 (@ref struct_meta_descriptor_validate 、フィールドの検索、
     *  パスの解決など) と同時に実行しないでください。\n
     *  通常は初期化時に登録を済ませてください。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_index_register(const struct_meta_descriptor *descriptor);

    /**
     *  @brief          記述子を索引から登録解除します。
     *  @param[in]      descriptor 登録解除する記述子です。NULL を渡してはなりません。
     *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
     *                  @c CPLAT_ERR_NOT_FOUND を返します。
     *
     *  ネストした記述子も再帰的に登録解除します。\n
     *  登録解除した記述子は、登録前と同じ線形走査の経路へ戻ります。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  登録と登録解除 (カタログの作成と破棄、生成コードによるカタログの初回取得を含む) は呼び出し側で直列化し、
     *  索引を参照する操作 (@ref struct_meta_descriptor_validate 、フィールドの検索、
     *  パスの解決など) と同時に実行しないでください。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_index_unregister(const struct_meta_descriptor *descriptor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRUCT_META_META_INDEX_H */
