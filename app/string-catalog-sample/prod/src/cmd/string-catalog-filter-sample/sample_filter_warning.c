/**
 *******************************************************************************
 *  @file           sample_filter_warning.c
 *  @brief          型が合わない比較要素の警告を、表示用の文へ整えます。
 *  @author         Tetsuo Honda
 *  @date           2026/10/07
 *  @version        0.1.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_filter_warning.h"

#include <cplat/base/result.h>
#include <cplat/crt/stdio.h>

/** 文字列以外の引数をまとめて表す語です。cplat は文字列と文字列以外の 2 区分で混在を判断します。 */
static const char *const s_other_class_label = "数値";

/** 引数種別の表示名です。cplat_string_catalog_argument_kind の値をインデックスとして参照します。 */
static const char *const s_argument_kind_labels[] = {
    "UNUSED", "STRING", "CHAR",  "INT8",  "UINT8", "INT16", "UINT16", "INT32",   "UINT32", "INT64",
    "UINT64", "HEX8",   "HEX16", "HEX32", "HEX64", "SIZE",  "SSIZE",  "POINTER", "DOUBLE", "ERROR_CODE",
};

static const char *argument_kind_label(const cplat_string_catalog_argument_kind kind)
{
    if ((unsigned int)kind >= (sizeof(s_argument_kind_labels) / sizeof(s_argument_kind_labels[0])))
    {
        return "?";
    }
    return s_argument_kind_labels[(unsigned int)kind];
}

/** 文字列キーと引数の位置から、カタログの引数定義を求めます。見つからない場合は NULL です。 */
static const cplat_string_catalog_argument *find_argument(const cplat_string_catalog *catalog, const int string_key,
                                                          const int argument_index,
                                                          const cplat_string_catalog_entry **entry_out)
{
    const cplat_string_catalog_entry *entry = cplat_string_catalog_get_entry(catalog, string_key);

    if ((entry == NULL) || (argument_index < 0) || (argument_index >= entry->argument_count))
    {
        return NULL;
    }
    *entry_out = entry;
    return &entry->arguments[argument_index];
}

/** 項目を表す名前です。ID を省略した項目は brief で表します。 */
static const char *entry_label(const cplat_string_catalog_entry *entry)
{
    if (entry->id != NULL)
    {
        return entry->id;
    }
    return entry->brief;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_warning_format(const cplat_string_catalog *catalog,
                                 const cplat_string_catalog_filter_warning *warning, char *dest, const size_t dest_size)
{
    const cplat_string_catalog_entry *entry = NULL;
    const cplat_string_catalog_entry *other_entry = NULL;
    const cplat_string_catalog_argument *argument;
    const cplat_string_catalog_argument *other_argument;

    if ((catalog == NULL) || (warning == NULL) || (dest == NULL) || (dest_size == 0U))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    dest[0] = '\0';

    argument = find_argument(catalog, warning->string_key, warning->argument_index, &entry);
    if ((argument == NULL) || (argument->name == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    switch (warning->kind)
    {
    case CPLAT_STRING_CATALOG_FILTER_WARNING_TYPE_MISMATCH:
        return cplat_snprintf(dest, dest_size,
                              "%u 行目の %u 番目の比較: %s の引数 %s は %s のため、この項目では比較が偽になります",
                              (unsigned int)(warning->line_index + 1U), (unsigned int)(warning->predicate_index + 1U),
                              entry_label(entry), argument->name, argument_kind_label(argument->kind));

    case CPLAT_STRING_CATALOG_FILTER_WARNING_MIXED_ARGUMENT_TYPES:
        other_argument = find_argument(catalog, warning->other_string_key, warning->other_argument_index, &other_entry);
        if (other_argument == NULL)
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
        return cplat_snprintf(dest, dest_size,
                              "%u 行目: 引数 %s は、%s では文字列、%s では%s (%s) です。"
                              "型の合わない項目では比較が偽になり、意図した判定結果にならない可能性があります",
                              (unsigned int)(warning->line_index + 1U), argument->name, entry_label(entry),
                              entry_label(other_entry), s_other_class_label, argument_kind_label(other_argument->kind));

    case CPLAT_STRING_CATALOG_FILTER_WARNING_NONE:
    default:
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
}
