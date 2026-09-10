/**
 *******************************************************************************
 *  @file           libsrc/message_catalog/message_catalog_format.c
 *  @brief          メッセージの組み立てとメタデータ参照の公開 API を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  公開 API は、引数スキーマの取得、可変長引数の取り出し、書式の展開の 3 段を順に呼び出します。\n
 *  可変長引数を最初に値の配列へ写すため、言語ごとの語順の違いは書式の展開だけで吸収できます。
 *
 *  出力する言語はプロセスの設定から取得します。言語の保持は `message_catalog_language.c`、
 *  カタログの保持は `message_catalog_catalog.c` が担います。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "format_engine.h"

#include <message_catalog/catalog.h>
#include <message_catalog/message_catalog_spec.h>
#include <stdarg.h>
#include <stddef.h>

/**
 *  @brief          言語別リソースから、指定した言語の要素を選びます。
 *  @param[in]      localized 言語を添字とするリソースの配列。NULL を渡してはなりません。
 *  @param[in]      language  出力する言語。範囲内の値を渡してください。
 *  @return         使用するリソースです。ニュートラル言語の要素も無い場合は NULL を返します。
 *
 *  指定した言語のリソースが無い場合は、ニュートラル言語の要素へ読み替えます。\n
 *  翻訳が追い付いていない言語でも、メッセージを出力できます。
 */
static const char *select_localized(const char *const *localized, const message_catalog_language language)
{
    const char *text = localized[(unsigned int)language];

    if (text == NULL)
    {
        text = localized[MESSAGE_CATALOG_LANGUAGE_NEUTRAL];
    }

    return text;
}

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_vformat(char *dest, const size_t dest_size, const int message_id, va_list args)
{
    const message_catalog_entry *entry;
    const message_catalog_language language = message_catalog_get_language();
    const char *text;
    format_engine_argument_value values[MESSAGE_CATALOG_ARGUMENT_MAX] = {0};
    int ret;

    if ((dest == NULL) || (dest_size == 0U))
    {
        return MESSAGE_CATALOG_ERR_INVALID_ARGUMENT;
    }

    dest[0] = '\0';

    entry = message_catalog_internal_find_entry(message_id);
    if (entry == NULL)
    {
        return MESSAGE_CATALOG_ERR_NOT_FOUND;
    }

    if ((entry->argument_count < 0) || (entry->argument_count > MESSAGE_CATALOG_ARGUMENT_MAX))
    {
        return MESSAGE_CATALOG_ERR_INVALID_DEFINITION;
    }

    text = select_localized(entry->texts, language);
    if (text == NULL)
    {
        return MESSAGE_CATALOG_ERR_INVALID_DEFINITION;
    }

    ret = format_engine_collect_arguments(entry, args, values);
    if (ret != MESSAGE_CATALOG_OK)
    {
        return ret;
    }

    return format_engine_render_text(dest, dest_size, text, values, entry->argument_count);
}

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_format(char *dest, const size_t dest_size, const int message_id, ...)
{
    va_list args;
    int ret;

    va_start(args, message_id);
    ret = message_catalog_vformat(dest, dest_size, message_id, args);
    va_end(args);

    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_verify(int *message_id_out, message_catalog_language *language_out)
{
    const int entry_count = message_catalog_internal_entry_count();
    int entry_index;

    for (entry_index = 0; entry_index < entry_count; entry_index++)
    {
        const message_catalog_entry *entry;
        int language_index;

        entry = message_catalog_internal_entry_at(entry_index);

        /* 分類値はライブラリが解釈しないため、範囲は確認しない */
        if ((entry->argument_count < 0) || (entry->argument_count > MESSAGE_CATALOG_ARGUMENT_MAX) ||
            (message_catalog_internal_find_entry(entry->id) != entry))
        {
            if (message_id_out != NULL)
            {
                *message_id_out = entry->id;
            }
            if (language_out != NULL)
            {
                /* 引数個数と添字表の不正は言語に依らないため、言語ではない値を格納する */
                *language_out = MESSAGE_CATALOG_LANGUAGE_COUNT;
            }
            return MESSAGE_CATALOG_ERR_INVALID_DEFINITION;
        }

        for (language_index = 0; language_index < (int)MESSAGE_CATALOG_LANGUAGE_COUNT; language_index++)
        {
            const char *text = select_localized(entry->texts, (message_catalog_language)language_index);
            int ret = MESSAGE_CATALOG_OK;

            if ((text == NULL) || (select_localized(entry->notes, (message_catalog_language)language_index) == NULL))
            {
                ret = MESSAGE_CATALOG_ERR_INVALID_DEFINITION;
            }
            else
            {
                ret = format_engine_validate_text(text, entry->argument_count);
            }

            if (ret != MESSAGE_CATALOG_OK)
            {
                if (message_id_out != NULL)
                {
                    *message_id_out = entry->id;
                }
                if (language_out != NULL)
                {
                    *language_out = (message_catalog_language)language_index;
                }
                return ret;
            }
        }
    }

    return MESSAGE_CATALOG_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_category(const int message_id)
{
    const message_catalog_entry *entry;

    entry = message_catalog_internal_find_entry(message_id);
    if (entry == NULL)
    {
        return 0;
    }

    return entry->category;
}

/* Doxygen コメントは、ヘッダーに記載 */

const char *message_catalog_id_text(const int message_id)
{
    const message_catalog_entry *entry;

    entry = message_catalog_internal_find_entry(message_id);
    if (entry == NULL)
    {
        return NULL;
    }

    return entry->id_text;
}

/* Doxygen コメントは、ヘッダーに記載 */

const char *message_catalog_note(const int message_id)
{
    const message_catalog_entry *entry;

    entry = message_catalog_internal_find_entry(message_id);
    if (entry == NULL)
    {
        return NULL;
    }

    return select_localized(entry->notes, message_catalog_get_language());
}
