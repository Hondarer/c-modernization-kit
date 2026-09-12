/**
 *******************************************************************************
 *  @file           catalog.h
 *  @brief          記述子の集合を、取得方法によらない共通の形で扱います。
 *
 *  記述子を得る経路は 2 系統あります。どちらも同じカタログ ハンドルになるため、
 *  利用側のコードは経路を意識しません。
 *
 *  - 事前組み込み型: `struct-meta-gen` が生成した C ソースを実行体へ組み込みます。
 *    レイアウトはコンパイラが決めます。生成コードが
 *    @ref struct_meta_catalog_attach_static を呼びます。
 *  - 事後解析型: 実行時に C ヘッダーを構文解析し、レイアウトを自前で求めます。
 *    利用側が @ref struct_meta_catalog_create_from_header_file を呼びます。
 *
 *  see: app/struct-meta/docs/architecture.md
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_CATALOG_CATALOG_H
#define STRUCT_META_CATALOG_CATALOG_H

#include <struct_meta/meta/meta.h>
#include <struct_meta/parse/parse.h>
#include <struct_meta/struct_meta_export.h>

#include <stddef.h>

/**
 *  @addtogroup STRUCT_META_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 記述子の集合です。内容は実装が保持します。 */
    typedef struct struct_meta_catalog struct_meta_catalog;

    /**
     *  @brief          C ヘッダー ファイルを実行時に構文解析し、カタログを作ります。
     *  @param[in]      path            解析対象ヘッダーのパス。NULL を渡してはなりません。
     *  @param[out]     catalog_out     カタログの格納先。NULL を渡してはなりません。
     *  @param[out]     diagnostic_out  診断の格納先。NULL を渡せます。
     *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、@c CPLAT_ERR_NOT_FOUND 、
     *                  @c CPLAT_ERR_OUT_OF_MEMORY 、@c CPLAT_ERR_INVALID_PATTERN 、または
     *                  @c CPLAT_ERR_CORRUPT_DESCRIPTOR を返します。
     *
     *  コンパイラを使いません。レイアウトは x86_64 の規則から求めます。\n
     *  成功したときだけ @p catalog_out へ値を入れます。使い終えたら
     *  @ref struct_meta_catalog_destroy で破棄します。\n
     *  失敗したときは @p diagnostic_out に原因と行番号が入ります。
     *
     *  @attention      本関数が返す記述子は、解析対象ヘッダーの宣言だけから求めたものです。
     *                  実行中のプログラムが静的に知っている型へ適用する場合は、利用側で
     *                  @c descriptor->size と @c sizeof の一致を確認してください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。解析の状態はすべて呼び出しごとに確保します。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_catalog_create_from_header_file(
        const char *path, struct_meta_catalog **catalog_out, struct_meta_diagnostic *diagnostic_out);

    /**
     *  @brief          メモリ上の C ヘッダー内容を構文解析し、カタログを作ります。
     *  @param[in]      text            解析対象の内容。NULL を渡してはなりません。
     *  @param[in]      length          @p text のバイト数。
     *  @param[out]     catalog_out     カタログの格納先。NULL を渡してはなりません。
     *  @param[out]     diagnostic_out  診断の格納先。NULL を渡せます。
     *  @return         @ref struct_meta_catalog_create_from_header_file と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。解析の状態はすべて呼び出しごとに確保します。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_catalog_create_from_header_text(
        const char *text, size_t length, struct_meta_catalog **catalog_out,
        struct_meta_diagnostic *diagnostic_out);

    /**
     *  @brief          静的な記述子と埋め込み索引イメージからカタログを作ります。
     *  @param[in]      descriptors       記述子の配列。NULL を渡してはなりません。
     *  @param[in]      descriptor_count  @p descriptors の要素数。0 を渡してはなりません。
     *  @param[in]      index_image_mgmt  索引イメージの管理領域。NULL を渡してはなりません。
     *  @param[in]      index_image_mgmt_size  @p index_image_mgmt のバイト数。
     *  @param[in]      index_image_data  索引イメージのデータ領域。NULL を渡してはなりません。
     *  @param[in]      index_image_data_size  @p index_image_data のバイト数。
     *  @param[out]     catalog_out       カタログの格納先。NULL を渡してはなりません。
     *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
     *                  @c CPLAT_ERR_OUT_OF_MEMORY を返します。
     *
     *  `struct-meta-gen` が生成したコードが呼びます。イメージは読み取り専用領域に置けます。
     *  接続はイメージへ書き込まず、ハンドル 1 個を確保するだけです。\n
     *  記述子とイメージの寿命は、カタログの寿命より長くなければなりません。\n
     *  各記述子を @ref struct_meta_index_register へも登録します。登録に失敗しても
     *  カタログの作成は成功します。登録は検索を速くするだけであり、未登録の記述子も
     *  正しく扱えるためです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。生成コードは
     *  @c cplat_call_once から 1 回だけ呼びます。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_catalog_attach_static(
        const struct_meta_descriptor *const *descriptors, size_t descriptor_count, const void *index_image_mgmt,
        size_t index_image_mgmt_size, const void *index_image_data, size_t index_image_data_size,
        struct_meta_catalog **catalog_out);

    /**
     *  @brief          カタログを破棄します。
     *  @param[in,out]  catalog  破棄するカタログ。NULL を渡せます。
     *
     *  実行時に構文解析して作ったカタログでは、記述子の記憶域もまとめて解放します。
     *  破棄後に取得済みの記述子を参照してはなりません。\n
     *  静的な記述子から作ったカタログでは、索引のハンドルだけを解放します。
     *  記述子とイメージは静的領域であり、解放しません。\n
     *  どちらの場合も、登録した記述子を索引から登録解除します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。破棄と検索を同時に実行しないでください。
     */
    STRUCT_META_EXPORT void STRUCT_META_API struct_meta_catalog_destroy(struct_meta_catalog *catalog);

    /**
     *  @brief          カタログが持つ記述子の数を求めます。
     *  @param[in]      catalog    対象。NULL を渡してはなりません。
     *  @param[out]     count_out  数の格納先。NULL を渡してはなりません。
     *  @return         @c CPLAT_OK または @c CPLAT_ERR_INVALID_ARGUMENT を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_catalog_get_count(const struct_meta_catalog *catalog,
                                                                        size_t *count_out);

    /**
     *  @brief          インデックスで記述子を取得します。
     *  @param[in]      catalog         対象。NULL を渡してはなりません。
     *  @param[in]      index           取得する記述子のインデックス。
     *  @param[out]     descriptor_out  記述子の格納先。NULL を渡してはなりません。
     *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
     *                  @c CPLAT_ERR_NOT_FOUND を返します。
     *
     *  並びは解析対象ヘッダーの宣言順です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。内部に共有状態を持ちません。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_catalog_get(const struct_meta_catalog *catalog, size_t index,
                                                                  const struct_meta_descriptor **descriptor_out);

    /**
     *  @brief          構造体名で記述子を検索します。
     *  @param[in]      catalog         対象。NULL を渡してはなりません。
     *  @param[in]      name            構造体名。NULL を渡してはなりません。
     *  @param[out]     descriptor_out  記述子の格納先。NULL を渡してはなりません。
     *  @return         @c CPLAT_OK 、@c CPLAT_ERR_INVALID_ARGUMENT 、または
     *                  @c CPLAT_ERR_NOT_FOUND を返します。
     *
     *  検索にはハッシュ表を使います。線形走査の経路はありません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。カタログの内容は作成後に変わりません。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_catalog_find(const struct_meta_catalog *catalog,
                                                                   const char *name,
                                                                   const struct_meta_descriptor **descriptor_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRUCT_META_CATALOG_CATALOG_H */
