/**
 *******************************************************************************
 *  @file           libsrc/samplecatalog/samplecatalog.c
 *  @brief          カタログを公開するサンプル ライブラリの API を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/20
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <cplat/base/result.h>
#include <samplecatalog/samplecatalog.h>
#include <samplecatalog/samplecatalog_messages.h>
#include <samplecatalog/samplecatalog_trace.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/** 本ライブラリが保持するカタログの数です。初期化のトレースへ出力します。 */
#define SAMPLECATALOG_CATALOG_COUNT 2

/** 探索の対象とする項目の表です。ライブラリが値を持つことを示すための固定の表です。 */
static const char *const s_items[] = {"alpha", "beta", "gamma"};

/** @ref s_items の要素数です。 */
#define SAMPLECATALOG_ITEM_COUNT ((size_t)(sizeof(s_items) / sizeof(s_items[0])))

/* Doxygen コメントは、ヘッダーに記載 */

int samplecatalog_initialize(cplat_tracer *const tracer)
{
    int string_key = 0;
    int ret;

    /* 出力先を先に設定する。点検に失敗した場合もトレースへ残すため */
    samplecatalog_trace_set_tracer(tracer);

    /* カタログの点検は提供元の責務とし、利用側へ点検の API を公開しない */
    ret = samplecatalog_messages_verify(&string_key, NULL);
    if (ret != CPLAT_OK)
    {
        (void)samplecatalog_trace_key_verify_failed("samplecatalog_messages", string_key);
        return ret;
    }

    ret = samplecatalog_trace_verify(&string_key, NULL);
    if (ret != CPLAT_OK)
    {
        (void)samplecatalog_trace_key_verify_failed("samplecatalog_trace", string_key);
        return ret;
    }

    (void)samplecatalog_trace_key_library_initialized(SAMPLECATALOG_CATALOG_COUNT);

    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int samplecatalog_find_item(const char *const item_name, char *const dest, const size_t dest_size)
{
    size_t index;

    if ((item_name == NULL) || (dest == NULL) || (dest_size == 0U))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    (void)samplecatalog_trace_key_lookup_performed(item_name);

    for (index = 0U; index < SAMPLECATALOG_ITEM_COUNT; index++)
    {
        if (strcmp(s_items[index], item_name) == 0)
        {
            return CPLAT_OK;
        }
    }

    /* 見つからない理由は、ライブラリ自身のカタログから組み立てて利用側へ返す */
    (void)samplecatalog_messages_key_item_not_found(dest, dest_size, item_name, (uint32_t)SAMPLECATALOG_ITEM_COUNT);

    return CPLAT_ERR_NOT_FOUND;
}
