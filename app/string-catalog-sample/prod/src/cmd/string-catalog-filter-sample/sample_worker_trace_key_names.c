/**
 *******************************************************************************
 *  @file           sample_worker_trace_key_names.c
 *  @brief          sample_worker_trace の文字列キーの名前解決テーブルを定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/26
 *  @version        0.1.0
 *
 *  名前は列挙定数を文字列化して作ります。綴りを誤るとコンパイル エラーになります。\n
 *  件数とカタログの項目数の一致は、コマンドの起動時に確認します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_worker_trace_key_names.h"

#include "gen/sample_worker_trace.h"

/** 列挙定数から、名前解決テーブルの 1 件を作ります。 */
#define SAMPLE_WORKER_TRACE_KEY_NAME(key) {#key, (key), 0U}

/** 名前解決テーブルです。 */
static const sample_filter_key_name s_key_names[] = {
    SAMPLE_WORKER_TRACE_KEY_NAME(SAMPLE_WORKER_TRACE_KEY_WORKER_STARTED),
    SAMPLE_WORKER_TRACE_KEY_NAME(SAMPLE_WORKER_TRACE_KEY_JOB_RECEIVED),
    SAMPLE_WORKER_TRACE_KEY_NAME(SAMPLE_WORKER_TRACE_KEY_JOB_PROGRESS),
    SAMPLE_WORKER_TRACE_KEY_NAME(SAMPLE_WORKER_TRACE_KEY_BUFFER_ALLOCATED),
    SAMPLE_WORKER_TRACE_KEY_NAME(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED),
    SAMPLE_WORKER_TRACE_KEY_NAME(SAMPLE_WORKER_TRACE_KEY_COMMAND_RECEIVED),
    SAMPLE_WORKER_TRACE_KEY_NAME(SAMPLE_WORKER_TRACE_KEY_WORKER_STOPPED),
};

/* Doxygen コメントは、ヘッダーに記載 */

const sample_filter_key_name *sample_worker_trace_key_names(void)
{
    return s_key_names;
}

/* Doxygen コメントは、ヘッダーに記載 */

size_t sample_worker_trace_key_name_count(void)
{
    return sizeof(s_key_names) / sizeof(s_key_names[0]);
}
