/**
 *******************************************************************************
 *  @file           parse.h
 *  @brief          解析対象ヘッダーの構文解析が返す診断を定義します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_PARSE_PARSE_H
#define STRUCT_META_PARSE_PARSE_H

#include <struct_meta/struct_meta_export.h>

/**
 *  @addtogroup STRUCT_META_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** 診断メッセージの最大バイト数です (終端の NUL を含みます)。 */
#define STRUCT_META_DIAGNOSTIC_MESSAGE_SIZE 256

    /**
     *  @brief          解析対象ヘッダーの構文解析が返す診断です。
     *
     *  エラー経路で記憶域を確保しないよう、メッセージは固定長の配列で持ちます。\n
     *  最初に検出した 1 件だけを保持します。後続の診断では上書きしません。
     *  最初の診断が原因で、後続はその波及であることが多いためです。
     */
    typedef struct struct_meta_diagnostic
    {
        int line;                                          /**< 対象行です。行が定まらない場合は 0 です。 */
        char message[STRUCT_META_DIAGNOSTIC_MESSAGE_SIZE]; /**< 診断メッセージです。診断が無ければ空文字列です。 */
    } struct_meta_diagnostic;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRUCT_META_PARSE_PARSE_H */
