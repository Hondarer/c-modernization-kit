/**
 *******************************************************************************
 *  @file           libsrc/string_catalog/string_catalog_catalog.c
 *  @brief          カタログの検索を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  カタログはライブラリが抱え込まず、利用者が用意します。\n
 *  ライブラリが定めるのは、言語、引数種別、分類、書式の構文です。\n
 *  利用者が用意するのは、文字列 ID の列挙とカタログの配列です。
 *
 *  本ファイルは状態を持ちません。呼び出し元が渡すカタログだけを参照します。\n
 *  1 つのプロセスで複数のカタログを扱え、カタログどうしは互いに影響しません。
 *
 *  カタログは利用者が静的初期化するため、値の形を関数の入口で確認します。\n
 *  文字列 ID からカタログを引く探索は、添字表があれば添字表を、無ければ線形探索を使います。\n
 *  添字表の内容も利用者が用意するため、参照する前に範囲を確認します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <string_catalog/catalog.h>
#include <string_catalog/string_catalog_spec.h>
#include <stdbool.h>
#include <stddef.h>

/* Doxygen コメントは、ヘッダーに記載 */

bool string_catalog_internal_is_usable(const string_catalog *const catalog)
{
    if (catalog == NULL)
    {
        return false;
    }

    if ((catalog->entries == NULL) || (catalog->entry_count < 0) || (catalog->id_index_count < 0))
    {
        return false;
    }

    return true;
}

/* Doxygen コメントは、ヘッダーに記載 */

int string_catalog_internal_entry_count(const string_catalog *const catalog)
{
    if (!string_catalog_internal_is_usable(catalog))
    {
        return 0;
    }

    return catalog->entry_count;
}

/* Doxygen コメントは、ヘッダーに記載 */

const string_catalog_entry *string_catalog_internal_entry_at(const string_catalog *const catalog, const int index)
{
    if (!string_catalog_internal_is_usable(catalog))
    {
        return NULL;
    }

    if ((index < 0) || (index >= catalog->entry_count))
    {
        return NULL;
    }

    return &catalog->entries[index];
}

/* Doxygen コメントは、ヘッダーに記載 */

const string_catalog_entry *string_catalog_internal_find_entry(const string_catalog *const catalog, const int string_id)
{
    int index;

    if (!string_catalog_internal_is_usable(catalog))
    {
        return NULL;
    }

    if ((catalog->id_index != NULL) && (string_id >= 0) && (string_id < catalog->id_index_count))
    {
        /* 添字表の内容は利用者が用意するため、参照する前に範囲を確認する */
        const int entry_index = catalog->id_index[string_id];

        if ((entry_index < 0) || (entry_index >= catalog->entry_count))
        {
            return NULL;
        }

        return &catalog->entries[entry_index];
    }

    for (index = 0; index < catalog->entry_count; index++)
    {
        if (catalog->entries[index].id == string_id)
        {
            return &catalog->entries[index];
        }
    }

    return NULL;
}
