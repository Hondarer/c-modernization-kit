/**
 *******************************************************************************
 *  @file           fake_catalog.h
 *  @brief          テストが内容を差し替えられるカタログを宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  製品のカタログは正しい内容だけを持つため、定義が壊れた場合の経路へ到達できません。\n
 *  本ファイルは、テストから内容を書き換えられるカタログを提供します。
 *
 *  カタログは利用者が定義する部分であり、メッセージ ID の列挙も利用者が名付けます。\n
 *  ここではライブラリの接頭辞に依存しない名前を用い、注入方式が名前に依存しないことを示します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef FAKE_CATALOG_PRIVATE_H
#define FAKE_CATALOG_PRIVATE_H

#include <message_catalog/message_catalog_entry.h>

/** 引数を取らないメッセージの添字です。 */
#define FAKE_CATALOG_INDEX_NO_ARGUMENT 0

/** 引数を 2 個取るメッセージの添字です。 */
#define FAKE_CATALOG_INDEX_TWO_ARGUMENTS 1

/** 引数を 1 個取るメッセージの添字です。 */
#define FAKE_CATALOG_INDEX_ONE_ARGUMENT 2

/** 偽のカタログが保持するメッセージの件数です。 */
#define FAKE_CATALOG_ENTRY_COUNT 3

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          偽のカタログのメッセージ ID です。
     */
    typedef enum fake_catalog_id
    {
        FAKE_CATALOG_ID_NO_ARGUMENT = 1,   /**< 引数を取らないメッセージです。 */
        FAKE_CATALOG_ID_TWO_ARGUMENTS = 2, /**< 引数を 2 個取るメッセージです。 */
        FAKE_CATALOG_ID_ONE_ARGUMENT = 4,  /**< 引数を 1 個取るメッセージです。 */
        FAKE_CATALOG_ID_UNKNOWN = 99       /**< カタログに登録しないメッセージです。 */
    } fake_catalog_id;

    /**
     *  @brief          偽のカタログを既定の内容へ戻し、プロセスへ注入します。
     */
    void fake_catalog_reset(void);

    /**
     *  @brief          指定したメッセージのメッセージ ID を書き換えます。
     *  @param[in]      index      書き換えるメッセージの添字。
     *  @param[in]      message_id 設定するメッセージ ID。
     */
    void fake_catalog_set_id(int index, int message_id);

    /**
     *  @brief          指定したメッセージの分類値を書き換えます。
     *  @param[in]      index    書き換えるメッセージの添字。
     *  @param[in]      category 設定する分類値。
     */
    void fake_catalog_set_category(int index, int category);

    /**
     *  @brief          指定したメッセージの引数個数を書き換えます。
     *  @param[in]      index 書き換えるメッセージの添字。
     *  @param[in]      count 設定する引数個数。
     */
    void fake_catalog_set_argument_count(int index, int count);

    /**
     *  @brief          指定したメッセージの引数種別を書き換えます。
     *  @param[in]      index          書き換えるメッセージの添字。
     *  @param[in]      argument_index 書き換える引数の添字。
     *  @param[in]      kind           設定する引数種別。
     */
    void fake_catalog_set_argument_kind(int index, int argument_index, message_catalog_argument_kind kind);

    /**
     *  @brief          指定したメッセージの言語別の備考を書き換えます。
     *  @param[in]      index    書き換えるメッセージの添字。
     *  @param[in]      language 書き換える言語。
     *  @param[in]      note     設定する備考。NULL を指定するとリソースが無い状態になります。
     */
    void fake_catalog_set_note(int index, message_catalog_language language, const char *note);

    /**
     *  @brief          指定したメッセージの言語別書式を書き換えます。
     *  @param[in]      index    書き換えるメッセージの添字。
     *  @param[in]      language 書き換える言語。
     *  @param[in]      text     設定する書式。NULL を指定するとリソースが無い状態になります。
     */
    void fake_catalog_set_text(int index, message_catalog_language language, const char *text);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAKE_CATALOG_PRIVATE_H */
