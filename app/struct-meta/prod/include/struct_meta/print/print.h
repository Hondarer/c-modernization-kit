/**
 *******************************************************************************
 *  @file           print.h
 *  @brief          メタデータを使って構造体の内容をテキスト出力します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_PRINT_H
#define STRUCT_META_PRINT_H

#include <stdio.h>

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
     *  @brief          記述子に従って構造体インスタンスの内容をテキスト出力します。
     *  @param[in]      descriptor  構造体の記述子です。NULL を渡してはなりません。
     *  @param[in]      instance    出力対象の構造体インスタンスです。NULL を渡してはなりません。
     *  @param[in]      stream      出力先ストリームです。NULL を渡してはなりません。
     *  @return         成功時は @c CPLAT_OK 、引数不正時は @c CPLAT_ERR_INVALID_ARGUMENT 、
     *                  記述子不正時は @c CPLAT_ERR_CORRUPT_DESCRIPTOR 、文字列に終端 NUL がない場合は
     *                  @c CPLAT_ERR_INVALID_ENCODING 、作業領域の確保に失敗した場合は
     *                  @c CPLAT_ERR_OUT_OF_MEMORY 、数値の書式化で容量不足となった場合は
     *                  @c CPLAT_ERR_BUFFER_TOO_SMALL 、出力ストリームへの書き込みまたはその他の書式化に
     *                  失敗した場合は @c CPLAT_ERR_UNKNOWN を返します。
     *
     *  フィールド名と配列要素のインデックスは固定長で切り詰めず、完全な名前を @p stream へ出力します。\n
     *  要素の取得、数値の書式化、または出力ストリームへの書き込みに失敗した場合は、その時点で処理を中断して
     *  対応する結果コードを返します。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なる @p instance と @p stream に対する操作は同時に実行できます。\n
     *  同一 @p instance への書き込み、または同一 @p stream に対する操作とは同時に呼び出さないでください。\n
     *  記述子の索引 (@ref struct_meta_index_register) を参照するため、
     *  索引を更新する操作 (@ref struct_meta_index_register 、@ref struct_meta_index_unregister 、カタログの作成と破棄、
     *  生成コードによるカタログの初回取得) とも同時に呼び出さないでください。
     */
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_print_write(const struct_meta_descriptor *descriptor,
                                                                   const void *instance, FILE *stream);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRUCT_META_PRINT_H */
