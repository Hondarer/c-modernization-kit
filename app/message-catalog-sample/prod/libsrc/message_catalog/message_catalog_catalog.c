/**
 *******************************************************************************
 *  @file           libsrc/message_catalog/message_catalog_catalog.c
 *  @brief          プロセスが使用するカタログの保持と検索を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  カタログはライブラリが抱え込まず、利用者が注入します。\n
 *  ライブラリが定めるのは、言語、引数種別、分類、書式の構文です。\n
 *  利用者が用意するのは、メッセージ ID の列挙とカタログの配列です。
 *
 *  設定はプロセス グローバルな状態であり、同期を持ちません。\n
 *  プロセスの初期化時に設定し、メッセージを組み立てている間は変更しない前提です。\n
 *  配列はコピーせず、ポインターだけを保持します。
 *
 *  メッセージ ID からカタログを引く探索は、添字表があれば添字表を、無ければ線形探索を使います。\n
 *  添字表の内容は利用者が用意するため、参照する前に範囲を確認します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <message_catalog/catalog.h>
#include <message_catalog/message_catalog_spec.h>
#include <stddef.h>

/** 注入されたカタログの先頭です。注入前は NULL です。 */
static const message_catalog_entry *s_entries = NULL;

/** 注入されたカタログの件数です。注入前は 0 です。 */
static int s_entry_count = 0;

/** メッセージ ID を添字として、カタログの添字を引く表です。注入していない場合は NULL です。 */
static const int *s_id_index = NULL;

/** @ref s_id_index の要素数です。添字表が無い場合は 0 です。 */
static int s_id_index_count = 0;

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_set_catalog(const message_catalog_entry *entries, const int entry_count, const int *id_index,
                                const int id_index_count)
{
    if ((entries == NULL) || (entry_count < 0) || (id_index_count < 0))
    {
        return MESSAGE_CATALOG_ERR_INVALID_ARGUMENT;
    }

    s_entries = entries;
    s_entry_count = entry_count;
    s_id_index = id_index;

    if (id_index == NULL)
    {
        s_id_index_count = 0;
    }
    else
    {
        s_id_index_count = id_index_count;
    }

    return MESSAGE_CATALOG_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_internal_entry_count(void)
{
    return s_entry_count;
}

/* Doxygen コメントは、ヘッダーに記載 */

const message_catalog_entry *message_catalog_internal_entry_at(const int index)
{
    if ((index < 0) || (index >= s_entry_count))
    {
        return NULL;
    }

    return &s_entries[index];
}

/* Doxygen コメントは、ヘッダーに記載 */

const message_catalog_entry *message_catalog_internal_find_entry(const int message_id)
{
    int index;

    if ((message_id >= 0) && (message_id < s_id_index_count))
    {
        /* 添字表の内容は利用者が用意するため、参照する前に範囲を確認する */
        const int entry_index = s_id_index[message_id];

        if ((entry_index < 0) || (entry_index >= s_entry_count))
        {
            return NULL;
        }

        return &s_entries[entry_index];
    }

    for (index = 0; index < s_entry_count; index++)
    {
        if (s_entries[index].id == message_id)
        {
            return &s_entries[index];
        }
    }

    return NULL;
}
