/**
 *******************************************************************************
 *  @file           sample_worker_trace_key_names.h
 *  @brief          sample_worker_trace の文字列キーの名前解決テーブルを宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/26
 *  @version        0.1.0
 *
 *  設計資料の第 2 段階では、カタログ定義生成器が `{module}_key_names()` を出力します。\n
 *  試作では生成器を変更しないため、同じ形の関数を手書きしています。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#ifndef SAMPLE_WORKER_TRACE_KEY_NAMES_PRIVATE_H
#define SAMPLE_WORKER_TRACE_KEY_NAMES_PRIVATE_H

#include "sample_filter.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          名前解決テーブルの先頭を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の表を返します。
     */
    const sample_filter_key_name *sample_worker_trace_key_names(void);

    /**
     *  @brief          名前解決テーブルの要素数を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    size_t sample_worker_trace_key_name_count(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_WORKER_TRACE_KEY_NAMES_PRIVATE_H */
