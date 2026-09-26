/**
 *******************************************************************************
 *  @file           sample_filter_output.c
 *  @brief          条件式フィルターを通してトレースを出力する入口を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/26
 *  @version        0.1.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_filter_output.h"

#include <cplat/base/result.h>

#include <stdarg.h>

/** 出力に使用するカタログです。 */
static const cplat_string_catalog *s_catalog = NULL;

/** 判定に使用するフィルター スロットです。 */
static sample_filter_slot *s_slot = NULL;

/** 出力先のトレーサーです。 */
static cplat_tracer *s_tracer = NULL;

/** 条件式を取り込む配布ハンドルです。NULL の場合は取り込みません。 */
static sample_filter_share *s_share = NULL;

/* Doxygen コメントは、ヘッダーに記載 */

void sample_filter_output_set_share(sample_filter_share *share)
{
    s_share = share;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_output_configure(const cplat_string_catalog *catalog, sample_filter_slot *slot, cplat_tracer *tracer)
{
    const int is_cleared = (catalog == NULL) && (slot == NULL) && (tracer == NULL);

    if (!is_cleared && ((catalog == NULL) || (slot == NULL) || (tracer == NULL)))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    s_catalog = catalog;
    s_slot = slot;
    s_tracer = tracer;
    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_output_write(const int string_key, ...)
{
    char text[CPLAT_STRING_CATALOG_TEXT_MAX];
    cplat_trace_level level = CPLAT_TRACE_LEVEL_NONE;
    va_list args;
    int category;
    int is_matched = 0;
    int ret;

    /* 出力先が未設定の場合は、組み立てを行わずに失敗を返す。設定の漏れを成功として隠蔽しないためです。 */
    if ((s_catalog == NULL) || (s_slot == NULL) || (s_tracer == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    /* 公開内容が変わっていれば、判定の前に取り込む。通常は世代番号の比較 1 回で戻る。
       取り込みの失敗はトレースの出力を妨げないため、結果は配布の状態で確認する */
    if (s_share != NULL)
    {
        (void)sample_filter_share_refresh(s_share, s_slot, NULL);
    }

    va_start(args, string_key);
    ret = sample_filter_slot_vformat(s_slot, text, sizeof(text), &is_matched, string_key, args);
    va_end(args);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    category = cplat_string_catalog_get_category(s_catalog, string_key);
    if ((category >= (int)CPLAT_TRACE_LEVEL_CRITICAL) && (category <= (int)CPLAT_TRACE_LEVEL_NONE))
    {
        level = (cplat_trace_level)category;
    }

    /* 条件式に一致したトレースは、出力先のしきい値によらず出力する */
    if (is_matched != 0)
    {
        level = CPLAT_TRACE_LEVEL_TO_FORCE(level);
    }

    return cplat_tracer_write_at(s_tracer, level, NULL, text);
}
