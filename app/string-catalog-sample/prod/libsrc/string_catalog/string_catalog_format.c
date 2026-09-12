/**
 *******************************************************************************
 *  @file           libsrc/string_catalog/string_catalog_format.c
 *  @brief          文字列の組み立てとメタデータ参照の公開 API を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  公開 API は、引数スキーマの取得、可変長引数の取り出し、書式の展開の 3 つの処理段階を順に呼び出します。\n
 *  可変長引数を事前に値の配列へ格納するため、言語ごとの語順の違いは書式の展開処理のみで対応できます。
 *
 *  出力する言語はプロセスの設定から取得します。言語の保持は `string_catalog_language.c`、
 *  カタログの保持は `string_catalog_catalog.c` が担います。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "format_engine.h"

#include <string_catalog/catalog.h>
#include <string_catalog/string_catalog_spec.h>
#include <stdarg.h>
#include <stddef.h>

/**
 *  @brief          言語別リソースから、指定した言語の要素を選びます。
 *  @param[in]      localized 言語をインデックスとするリソースの配列。NULL を渡してはなりません。
 *  @param[in]      language  出力する言語。範囲内の値を渡してください。
 *  @return         使用するリソースです。ニュートラル言語の要素も未設定の場合は NULL を返します。
 *
 *  指定した言語のリソースが未設定の場合は、ニュートラル言語の要素を代替として使用します。\n
 *  翻訳が完了していない言語であっても、文字列を出力できます。
 */
static const char *select_localized(const char *const *localized, const string_catalog_language language)
{
    const char *text = localized[(unsigned int)language];

    if (text == NULL)
    {
        text = localized[STRING_CATALOG_LANGUAGE_NEUTRAL];
    }

    return text;
}

/* Doxygen コメントは、ヘッダーに記載 */

int string_catalog_vformat(const string_catalog *const catalog, char *dest, const size_t dest_size, const int string_id,
                           va_list args)
{
    const string_catalog_entry *entry;
    const string_catalog_language language = string_catalog_get_language();
    const char *text;
    format_engine_argument_value values[STRING_CATALOG_ARGUMENT_MAX] = {0};
    int ret;

    if ((dest == NULL) || (dest_size == 0U))
    {
        return STRING_CATALOG_ERR_INVALID_ARGUMENT;
    }

    dest[0] = '\0';

    if (!string_catalog_internal_is_usable(catalog))
    {
        return STRING_CATALOG_ERR_INVALID_ARGUMENT;
    }

    entry = string_catalog_internal_find_entry(catalog, string_id);
    if (entry == NULL)
    {
        return STRING_CATALOG_ERR_NOT_FOUND;
    }

    if ((entry->argument_count < 0) || (entry->argument_count > STRING_CATALOG_ARGUMENT_MAX))
    {
        return STRING_CATALOG_ERR_INVALID_DEFINITION;
    }

    text = select_localized(entry->texts, language);
    if (text == NULL)
    {
        return STRING_CATALOG_ERR_INVALID_DEFINITION;
    }

    ret = format_engine_collect_arguments(entry, args, values);
    if (ret != STRING_CATALOG_OK)
    {
        return ret;
    }

    return format_engine_render_text(dest, dest_size, text, values, entry->argument_count);
}

/* Doxygen コメントは、ヘッダーに記載 */

int string_catalog_format(const string_catalog *const catalog, char *dest, const size_t dest_size, const int string_id,
                          ...)
{
    va_list args;
    int ret;

    va_start(args, string_id);
    ret = string_catalog_vformat(catalog, dest, dest_size, string_id, args);
    va_end(args);

    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int string_catalog_verify(const string_catalog *const catalog, int *string_id_out,
                          string_catalog_language *language_out)
{
    int entry_count;
    int entry_index;

    if (!string_catalog_internal_is_usable(catalog))
    {
        return STRING_CATALOG_ERR_INVALID_ARGUMENT;
    }

    entry_count = string_catalog_internal_entry_count(catalog);

    for (entry_index = 0; entry_index < entry_count; entry_index++)
    {
        const string_catalog_entry *entry;
        int language_index;

        entry = string_catalog_internal_entry_at(catalog, entry_index);

        /* 分類値はライブラリが解釈しないため、範囲は確認しない */
        if ((entry->argument_count < 0) || (entry->argument_count > STRING_CATALOG_ARGUMENT_MAX) ||
            (string_catalog_internal_find_entry(catalog, entry->id) != entry))
        {
            if (string_id_out != NULL)
            {
                *string_id_out = entry->id;
            }
            if (language_out != NULL)
            {
                /* 引数個数とインデックス表の不正は言語に依存しないため、言語ではない値を格納する */
                *language_out = STRING_CATALOG_LANGUAGE_COUNT;
            }
            return STRING_CATALOG_ERR_INVALID_DEFINITION;
        }

        for (language_index = 0; language_index < (int)STRING_CATALOG_LANGUAGE_COUNT; language_index++)
        {
            const char *text = select_localized(entry->texts, (string_catalog_language)language_index);
            int ret = STRING_CATALOG_OK;

            if ((text == NULL) || (select_localized(entry->notes, (string_catalog_language)language_index) == NULL))
            {
                ret = STRING_CATALOG_ERR_INVALID_DEFINITION;
            }
            else
            {
                ret = format_engine_validate_text(text, entry->argument_count);
            }

            if (ret != STRING_CATALOG_OK)
            {
                if (string_id_out != NULL)
                {
                    *string_id_out = entry->id;
                }
                if (language_out != NULL)
                {
                    *language_out = (string_catalog_language)language_index;
                }
                return ret;
            }
        }
    }

    return STRING_CATALOG_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int string_catalog_category(const string_catalog *const catalog, const int string_id)
{
    const string_catalog_entry *entry;

    entry = string_catalog_internal_find_entry(catalog, string_id);
    if (entry == NULL)
    {
        return 0;
    }

    return entry->category;
}

/* Doxygen コメントは、ヘッダーに記載 */

const char *string_catalog_id_text(const string_catalog *const catalog, const int string_id)
{
    const string_catalog_entry *entry;

    entry = string_catalog_internal_find_entry(catalog, string_id);
    if (entry == NULL)
    {
        return NULL;
    }

    return entry->id_text;
}

/* Doxygen コメントは、ヘッダーに記載 */

const char *string_catalog_note(const string_catalog *const catalog, const int string_id)
{
    const string_catalog_entry *entry;

    entry = string_catalog_internal_find_entry(catalog, string_id);
    if (entry == NULL)
    {
        return NULL;
    }

    return select_localized(entry->notes, string_catalog_get_language());
}
