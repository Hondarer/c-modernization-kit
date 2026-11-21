/**
 *******************************************************************************
 *  @file           samplecatalog.c
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

/** 初期化時に点検するカタログの 1 件です。 */
typedef struct samplecatalog_catalog_check
{
    const char *name; /**< トレースへ出力するカタログ名。 */
    int (*verify)(int *string_key_out, cplat_string_catalog_language *language_out); /**< カタログの点検関数。 */
} samplecatalog_catalog_check;

/** 本ライブラリが保持するカタログの表です。初期化時に先頭から順に点検します。 */
static const samplecatalog_catalog_check s_catalogs[] = {
    {"samplecatalog_messages", samplecatalog_messages_verify},
    {"samplecatalog_trace", samplecatalog_trace_verify},
};

/** 本ライブラリが保持するカタログの数です。初期化のトレースへ出力します。 */
#define SAMPLECATALOG_CATALOG_COUNT ((uint32_t)(sizeof(s_catalogs) / sizeof(s_catalogs[0])))

/** 検索対象とする項目のテーブルです。ライブラリがデータを保持することを示すための固定テーブルです。 */
static const char *const s_items[] = {"alpha", "beta", "gamma"};

/** `s_items` の要素数です。 */
#define SAMPLECATALOG_ITEM_COUNT ((size_t)(sizeof(s_items) / sizeof(s_items[0])))

/* Doxygen コメントは、ヘッダーに記載 */

int samplecatalog_initialize(cplat_tracer *const tracer)
{
    uint32_t index;

    /* 出力先を事前に設定する。点検に失敗した場合もトレースへ記録するためである。 */
    samplecatalog_trace_set_tracer(tracer);

    /* カタログの点検は提供元の責務とし、利用側へ点検 API を公開しない。 */
    for (index = 0U; index < SAMPLECATALOG_CATALOG_COUNT; index++)
    {
        int string_key = 0;
        const int ret = s_catalogs[index].verify(&string_key, NULL);

        if (ret != CPLAT_OK)
        {
            /*
             * トレースは補助の記録であり、出力先が NULL の場合の失敗も含めて初期化の結果を変えないため、
             * この関数のトレースの戻り値は明示的に破棄する。
             */
            (void)samplecatalog_trace_key_verify_failed(s_catalogs[index].name, string_key);
            return ret;
        }
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

    /* トレースは補助の記録であり、出力の失敗で検索の結果を変えないため、戻り値は明示的に破棄する。 */
    (void)samplecatalog_trace_key_lookup_performed(item_name);

    /* 保持する項目テーブルを走査し、一致する項目を検索する。 */
    for (index = 0U; index < SAMPLECATALOG_ITEM_COUNT; index++)
    {
        if (strcmp(s_items[index], item_name) == 0)
        {
            return CPLAT_OK;
        }
    }

    /*
     * 項目が存在しない理由は、ライブラリ自身のカタログから組み立てて利用側へ返却する。
     * カタログは初期化時に点検済みで、残る失敗は dest に収まらない場合の切り詰めだけである。
     * 理由の文字列を切り詰めても項目が存在しない結果は変わらないため、戻り値は明示的に破棄する。
     */
    (void)samplecatalog_messages_key_item_not_found(dest, dest_size, item_name, (uint32_t)SAMPLECATALOG_ITEM_COUNT);

    return CPLAT_ERR_NOT_FOUND;
}
