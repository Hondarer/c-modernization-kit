/**
 *******************************************************************************
 *  @file           message_catalog_argument.h
 *  @brief          メッセージ引数の種別を定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  引数種別は、可変長引数から取り出す C の型と、文字列化の書式を 1 つにまとめた列挙です。\n
 *  型と書式を別々の列挙に分けると、意味を持たない組み合わせを表現できてしまうため、
 *  1 つの列挙として扱います。
 *
 *  引数種別はメッセージ ID 側の定義が保持します。言語別リソースは語順だけを決め、
 *  文字列表現には関与しません。\n
 *  同じメッセージ ID の `{0}` は、どの言語でも同じ表現になります。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef MESSAGE_CATALOG_MESSAGE_CATALOG_ARGUMENT_H
#define MESSAGE_CATALOG_MESSAGE_CATALOG_ARGUMENT_H

/**
 *  @ingroup        MESSAGE_CATALOG_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          メッセージ引数として渡す値の種別を表します。
     *
     *  種別ごとに、可変長引数へ渡す型と文字列表現が次のとおり決まります。
     *
     *  | 種別                                          | 渡す型          | 文字列表現の例         |
     *  | --------------------------------------------- | --------------- | ---------------------- |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_STRING     | `const char *`  | `config.json`          |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_CHAR       | `char`          | `'A'` / `138 (0x8a)`   |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_INT8       | `int8_t`        | `-12`                  |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_UINT8      | `uint8_t`       | `200`                  |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_INT16      | `int16_t`       | `-1200`                |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_UINT16     | `uint16_t`      | `48000`                |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_INT32      | `int32_t`       | `-12`                  |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_UINT32     | `uint32_t`      | `12`                   |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_INT64      | `int64_t`       | `-4294967296`          |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_UINT64     | `uint64_t`      | `4294967296`           |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_HEX8       | `uint8_t`       | `0x8a`                 |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_HEX16      | `uint16_t`      | `0xbeef`               |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_HEX32      | `uint32_t`      | `0x1234abcd`           |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_HEX64      | `uint64_t`      | `0x00000000deadbeef`   |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_SIZE       | `size_t`        | `4096`                 |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_SSIZE      | `int64_t`       | `-1`                   |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_POINTER    | `const void *`  | `0x00007fffa1234567`   |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_DOUBLE     | `double`        | `12.5`                 |
     *  | @ref MESSAGE_CATALOG_ARGUMENT_KIND_ERROR_CODE | `int`           | `2 (0x00000002)`       |
     *
     *  16 進表現は、桁数を種別で固定し、英小文字で出力します。\n
     *  @ref MESSAGE_CATALOG_ARGUMENT_KIND_CHAR は、ASCII の印字可能な範囲 (`0x20` から `0x7e`) を
     *  単引用符で囲んだ 1 文字として出力し、それ以外は値を 10 進数と 16 進数で併記します。
     *  印字可能かどうかの判定はロケールへ依存させず、プラットフォーム間で表現を揃えます。\n
     *  @ref MESSAGE_CATALOG_ARGUMENT_KIND_POINTER は、プラットフォーム間で表現を揃えるため、
     *  `%p` ではなくポインター幅の 16 進表現へ変換します。
     *
     *  @ref MESSAGE_CATALOG_ARGUMENT_KIND_SSIZE は、符号付きのバイト数や要素数を受け取ります。\n
     *  POSIX の `ssize_t` は Windows に無く、幅も処理系で異なるため、公開契約の型は `int64_t` とします。
     *  呼び出し側で `int64_t` へ変換してから渡してください。
     *
     *  既定引数拡張のため、`char` と 8 bit、16 bit の整数は `int` へ、`float` は `double` へ昇格します。\n
     *  8 bit と 16 bit の種別は、`va_arg` で `int` として取り出したあと、種別が表す幅へ変換します。
     *  昇格後の型と一致しない `va_arg` の指定は行いません。\n
     *  単精度浮動小数点数の種別を設けていないのは、`double` へ昇格した値を倍精度として扱えば足りるためです。
     *
     *  @ref MESSAGE_CATALOG_ARGUMENT_KIND_ERROR_CODE は、Linux の `errno` と
     *  Windows のエラー コードのどちらも `int` として受け取り、10 進数と 16 進数を併記します。
     */
    typedef enum message_catalog_argument_kind
    {
        MESSAGE_CATALOG_ARGUMENT_KIND_STRING = 0,     /**< NUL 終端の文字列。NULL は `(null)` と表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_CHAR = 1,       /**< 1 文字。印字できない場合は 10 進数と 16 進数を併記します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_INT8 = 2,       /**< 符号付き 8 bit 整数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_UINT8 = 3,      /**< 符号なし 8 bit 整数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_INT16 = 4,      /**< 符号付き 16 bit 整数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_UINT16 = 5,     /**< 符号なし 16 bit 整数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_INT32 = 6,      /**< 符号付き 32 bit 整数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_UINT32 = 7,     /**< 符号なし 32 bit 整数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_INT64 = 8,      /**< 符号付き 64 bit 整数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_UINT64 = 9,     /**< 符号なし 64 bit 整数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_HEX8 = 10,      /**< 符号なし 8 bit 整数。2 桁の 16 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_HEX16 = 11,     /**< 符号なし 16 bit 整数。4 桁の 16 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_HEX32 = 12,     /**< 符号なし 32 bit 整数。8 桁の 16 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_HEX64 = 13,     /**< 符号なし 64 bit 整数。16 桁の 16 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_SIZE = 14,      /**< オブジェクトのバイト数や要素数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_SSIZE = 15,     /**< 符号付きのバイト数や要素数。10 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_POINTER = 16,   /**< オブジェクトのアドレス。16 進数で表現します。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_DOUBLE = 17,    /**< 倍精度浮動小数点数。有効桁を保つ短い表現にします。 */
        MESSAGE_CATALOG_ARGUMENT_KIND_ERROR_CODE = 18 /**< OS のエラー コード。10 進数と 16 進数を併記します。 */
    } message_catalog_argument_kind;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* MESSAGE_CATALOG_MESSAGE_CATALOG_ARGUMENT_H */
