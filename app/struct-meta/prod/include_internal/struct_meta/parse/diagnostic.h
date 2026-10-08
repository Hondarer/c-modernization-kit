/**
 *******************************************************************************
 *  @file           diagnostic.h
 *  @brief          解析と記述子構築が使う診断の書き込みを提供します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_PARSE_DIAGNOSTIC_H
#define STRUCT_META_PARSE_DIAGNOSTIC_H

#include <struct_meta/parse/parse.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          診断を書き込みます。
     *  @param[out]     diagnostic  書き込み先。NULL を渡せます。
     *  @param[in]      line        対象行。行が定まらない場合は 0 を渡します。
     *  @param[in]      format      printf 形式の書式。NULL を渡してはなりません。
     *
     *  最初の 1 件だけを記録し、既に診断があるときは何もしません。最初の診断が原因で、
     *  後続はその波及であることが多いためです。\n
     *  メッセージが入りきらない場合は切り詰めます。診断は原因を伝えるためのものであり、
     *  切り詰めを別のエラーとして扱う必要はありません。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  異なる診断情報に対する呼び出しは同時に実行できます。\n
     *  同一診断情報に対する操作は、呼び出し側で同期してください。
     */
    void struct_meta_internal_diagnose(struct_meta_diagnostic *diagnostic, int line, const char *format, ...);

    /**
     *  @brief          診断を空にします。
     *  @param[out]     diagnostic  初期化する診断。NULL を渡せます。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    void struct_meta_internal_diagnostic_clear(struct_meta_diagnostic *diagnostic);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRUCT_META_PARSE_DIAGNOSTIC_H */
