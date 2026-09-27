/**
 *******************************************************************************
 *  @file           meta.h
 *  @brief          C 構造体を表すメタデータと検査 API を提供します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_META_H
#define STRUCT_META_META_H

#include <stddef.h>

#include <struct_meta/struct_meta_export.h>

/**
 *  @defgroup       STRUCT_META_PUBLIC_API 公開 API (struct_meta)
 *  @brief          struct_meta ライブラリの公開 API です。
 */

/**
 *  @ingroup        STRUCT_META_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          フィールド値の種別です。
     *
     *  整数は符号の有無だけを種別で表し、幅は @ref struct_meta_field::element_size が
     *  表します。`int`、`long long`、`int32_t` のような C の型名ごとに種別を増やさず、
     *  対応する型が増えても列挙が変わらないようにするためです。\n
     *  整数の @ref struct_meta_field::element_size は 1、2、4、8 のいずれかです。
     *  `char[N]` は既定で NUL 終端文字列です。`signed char[N]`、`unsigned char[N]`、
     *  `int8_t[N]`、`uint8_t[N]` はバイト配列です。`char[N]` に
     *  `meta.kind=bytes` 属性を指定した場合も、符号付きバイト配列として扱います。
     */
    typedef enum struct_meta_field_kind
    {
        STRUCT_META_FIELD_SIGNED_INTEGER = 0,   /**< 符号付き整数です。幅は element_size が表します。 */
        STRUCT_META_FIELD_UNSIGNED_INTEGER = 1, /**< 符号なし整数です。幅は element_size が表します。 */
        STRUCT_META_FIELD_FLOAT = 2,            /**< float 値です。 */
        STRUCT_META_FIELD_DOUBLE = 3,           /**< double 値です。 */
        STRUCT_META_FIELD_CHAR_ARRAY = 4,       /**< NUL 終端文字列として扱う既定の char 配列です。 */
        STRUCT_META_FIELD_STRUCT = 5            /**< ネストした構造体です。 */
    } struct_meta_field_kind;

    typedef struct struct_meta_descriptor struct_meta_descriptor;

    /** フィールドへ付与する拡張属性です。 */
    typedef struct struct_meta_attribute
    {
        const char *key;   /**< 属性名です。NULL を指定してはなりません。 */
        const char *value; /**< 属性値です。値を持たない属性では NULL です。 */
    } struct_meta_attribute;

    /** 構造体の 1 フィールドを表す記述子です。 */
    typedef struct struct_meta_field
    {
        const char *name;                        /**< C フィールド名です。 */
        struct_meta_field_kind kind;             /**< フィールド値の種別です。 */
        unsigned int pad;                        /**< 明示的アラインメントです。0 を指定します。 */
        size_t offset;                           /**< 親構造体の先頭からのバイト オフセットです。 */
        size_t element_size;                     /**< 配列要素 1 個のバイト数です。 */
        size_t element_count;                    /**< 要素数です。スカラーでは 1 です。 */
        size_t char_buffer_size;                 /**< char 配列全体のバイト数です。その他では 0 です。 */
        const struct_meta_descriptor *nested;    /**< ネスト先です。その他では NULL です。 */
        const char *brief;                       /**< 短い説明です。説明がなければ NULL です。 */
        const struct_meta_attribute *attributes; /**< 拡張属性の配列です。 */
        size_t attribute_count;                  /**< @p attributes の要素数です。 */
    } struct_meta_field;

    /** 構造体全体を表す記述子です。 */
    struct struct_meta_descriptor
    {
        const char *name;                        /**< 構造体名です。 */
        size_t size;                             /**< 構造体全体のバイト数です。 */
        const struct_meta_field *fields;         /**< フィールド記述子の配列です。 */
        size_t field_count;                      /**< @p fields の要素数です。 */
        const char *brief;                       /**< 短い説明です。説明がなければ NULL です。 */
        const struct_meta_attribute *attributes; /**< 拡張属性の配列です。 */
        size_t attribute_count;                  /**< @p attributes の要素数です。 */
    };

    /**
     *  @brief          記述子を再帰的に検査します。
     *  @param[in]      descriptor 検査対象です。
     *  @return         @c CPLAT_OK、@c CPLAT_ERR_INVALID_ARGUMENT、または
     *                  @c CPLAT_ERR_CORRUPT_DESCRIPTOR、または
     *                  @c CPLAT_ERR_OUT_OF_MEMORY を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_descriptor_validate(const struct_meta_descriptor *descriptor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRUCT_META_META_H */
