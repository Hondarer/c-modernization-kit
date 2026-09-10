/**
 *******************************************************************************
 *  @file           src/cmd/message-catalog-sample/message_catalog_definition.c
 *  @brief          メッセージ ID ごとの引数スキーマ、レベル、メタデータ、言語別リソースを保持します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  本ファイルは、カタログ定義 (Excel など) からの生成物です。\n
 *  同じ生成元から作る `message_catalog_definition.h` と合わせて 1 組の生成単位です。\n
 *  手作業で編集せず、生成元の定義を変更してから再生成してください。
 *
 *  この表は利用者が用意する部分であり、ライブラリは抱え込みません。\n
 *  `message_catalog_set_catalog()` で注入します。
 *
 *  カタログの配列に加えて、メッセージ ID を添字とする添字表を持ちます。\n
 *  ライブラリはこの表によってメッセージ ID からカタログを直接引き、線形探索を避けます。
 *
 *  各要素は、メッセージ ID、レベル、引数個数、明示的アラインメント、引数スキーマ、
 *  メッセージ ID の固定文字列、言語別の書式、言語別の備考の順です。\n
 *  `texts` と `notes` の並びは @ref message_catalog_language の値の順です。
 *
 *  引数の型と文字列表現はこの表が決め、言語別リソースは語順だけを決めます。\n
 *  書式中の `{0}` から `{9}` は引数の位置を表します。\n
 *  `{` と `}` そのものを出力する場合は `{{` と `}}` を使用します。
 *
 *  ニュートラル言語の書式は、英語と同じ表現とします。\n
 *  英語の要素は NULL とし、ニュートラル言語の書式へ読み替えます。\n
 *  英語をニュートラル言語と分ける必要が生じた時点で、英語の要素を追加してください。
 *
 *  ソース ファイルの文字コードは UTF-8 です。出力するメッセージも UTF-8 です。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "message_catalog_definition.h"

#include <stddef.h>

/** メッセージ ID ごとのカタログです。メッセージ ID の昇順に並べます。 */
static const message_catalog_entry s_entries[] = {
    {MESSAGE_CATALOG_ID_STARTUP_COMPLETED,
     MESSAGE_CATALOG_TRACE_LEVEL_INFO,
     0,
     0,   /* 明示的アラインメント */
     {0}, /* 引数なし */
     "MSG_ID_0001",
     {/* NEUTRAL  */ "Startup completed. The default setting is {{ default }}.",
      /* JAPANESE */ "起動が完了しました。既定の設定は {{ default }} です。",
      /* ENGLISH  */ NULL},
     {/* NEUTRAL  */ "Informational message emitted when startup finishes with the default settings.",
      /* JAPANESE */ "運用開始を知らせる情報メッセージです。既定設定で起動したことを示します。",
      /* ENGLISH  */ NULL}},
    {MESSAGE_CATALOG_ID_FILE_OPEN_FAILED,
     MESSAGE_CATALOG_TRACE_LEVEL_ERROR,
     2,
     0, /* 明示的アラインメント */
     {MESSAGE_CATALOG_ARGUMENT_KIND_STRING, MESSAGE_CATALOG_ARGUMENT_KIND_ERROR_CODE},
     "MSG_ID_0002",
     {/* NEUTRAL  */ "Failed to open file {0}. Error code={1}",
      /* JAPANESE */ "ファイル {0} を開けませんでした。エラー コード={1}",
      /* ENGLISH  */ NULL},
     {/* NEUTRAL  */ "The path is emitted as given by the caller. The error is an errno or Win32 error number.",
      /* JAPANESE */ "パスは利用者の指定をそのまま出力します。エラー コードは errno または Win32 のエラー番号です。",
      /* ENGLISH  */ NULL}},
    {MESSAGE_CATALOG_ID_MEMORY_SIGNATURE,
     MESSAGE_CATALOG_TRACE_LEVEL_DEBUG,
     2,
     0, /* 明示的アラインメント */
     {MESSAGE_CATALOG_ARGUMENT_KIND_POINTER, MESSAGE_CATALOG_ARGUMENT_KIND_HEX64},
     "MSG_ID_0003",
     {/* NEUTRAL  */ "The signature at memory address {0} is {1}.",
      /* JAPANESE */ "メモリー アドレス {0} のシグネチャーは {1} です。",
      /* ENGLISH  */ NULL},
     {/* NEUTRAL  */ "The address changes on every run. It is emitted to identify a region during failure analysis.",
      /* JAPANESE */ "アドレスは実行のたびに変わります。障害解析で領域の同一性を確認する目的で出力します。",
      /* ENGLISH  */ NULL}},
    {MESSAGE_CATALOG_ID_BUFFER_LIMIT,
     MESSAGE_CATALOG_TRACE_LEVEL_WARNING,
     2,
     0, /* 明示的アラインメント */
     {MESSAGE_CATALOG_ARGUMENT_KIND_SIZE, MESSAGE_CATALOG_ARGUMENT_KIND_SIZE},
     "MSG_ID_0004",
     {/* NEUTRAL  */ "The requested size of {0} bytes exceeds the limit of {1} bytes.",
      /* JAPANESE */ "要求サイズ {0} バイトが上限 {1} バイトを超えました。",
      /* ENGLISH  */ NULL},
     {/* NEUTRAL  */ "The requested size and the limit are in bytes. The limit is configurable.",
      /* JAPANESE */ "要求サイズと上限サイズの単位はバイトです。上限は構成で変更できます。",
      /* ENGLISH  */ NULL}},
    {MESSAGE_CATALOG_ID_RECORD_MISMATCH,
     MESSAGE_CATALOG_TRACE_LEVEL_ERROR,
     2,
     0, /* 明示的アラインメント */
     {MESSAGE_CATALOG_ARGUMENT_KIND_UINT32, MESSAGE_CATALOG_ARGUMENT_KIND_HEX32},
     "MSG_ID_0005",
     {/* NEUTRAL  */ "Record {0} has an unexpected signature {1}.",
      /* JAPANESE */ "シグネチャー {1} は、レコード {0} の想定と一致しません。",
      /* ENGLISH  */ NULL},
     {/* NEUTRAL  */ "The Japanese text swaps the placeholders. The caller passes the arguments in the same order "
                     "for every language.",
      /* JAPANESE */ "日本語の書式は位置指定を入れ替えています。呼び出し側の引数順序は言語によらず同じです。",
      /* ENGLISH  */ "The Japanese text swaps the placeholders, while the argument order stays the same."}},
    {MESSAGE_CATALOG_ID_RETRY_SCHEDULED,
     MESSAGE_CATALOG_TRACE_LEVEL_WARNING,
     2,
     0, /* 明示的アラインメント */
     {MESSAGE_CATALOG_ARGUMENT_KIND_INT32, MESSAGE_CATALOG_ARGUMENT_KIND_INT64},
     "MSG_ID_0006",
     {/* NEUTRAL  */ "Retrying connection {0} in {1} milliseconds.",
      /* JAPANESE */ "接続 {0} を {1} ミリ秒後に再試行します。",
      /* ENGLISH  */ NULL},
     {/* NEUTRAL  */ "The delay is in milliseconds. The caller manages the retry count.",
      /* JAPANESE */ "待ち時間の単位はミリ秒です。再試行の回数は呼び出し側が管理します。",
      /* ENGLISH  */ NULL}},
    {MESSAGE_CATALOG_ID_THROUGHPUT_REPORT,
     MESSAGE_CATALOG_TRACE_LEVEL_VERBOSE,
     2,
     0, /* 明示的アラインメント */
     {MESSAGE_CATALOG_ARGUMENT_KIND_DOUBLE, MESSAGE_CATALOG_ARGUMENT_KIND_UINT64},
     "MSG_ID_0007",
     {/* NEUTRAL  */ "The throughput is {0} records per second. {1} records were processed at {0} records per "
                     "second.",
      /* JAPANESE */ "処理速度は {0} 件/秒です。累計 {1} 件を {0} 件/秒で処理しました。",
      /* ENGLISH  */ NULL},
     {/* NEUTRAL  */ "The rate is referenced twice. It shows that one placeholder can appear more than once.",
      /* JAPANESE */ "毎秒件数を 2 か所で参照します。同じ位置指定を複数回書けることを示す例です。",
      /* ENGLISH  */ NULL}}};

/** @ref s_entries の要素数です。 */
#define ENTRY_COUNT ((int)(sizeof(s_entries) / sizeof(s_entries[0])))

/** 添字表で、メッセージ ID を登録していないことを表す値です。 */
#define ID_INDEX_ABSENT (-1)

/**
 *  @brief          メッセージ ID を添字として、@ref s_entries の添字を引く表です。
 *
 *  メッセージ ID は 1 から始まるため、添字 0 は使用しません。\n
 *  メッセージ ID を歯抜けにする場合は、該当する添字へ @ref ID_INDEX_ABSENT を格納します。
 */
static const int s_id_index[] = {
    ID_INDEX_ABSENT, /* 0: 未使用 */
    0,               /* MESSAGE_CATALOG_ID_STARTUP_COMPLETED */
    1,               /* MESSAGE_CATALOG_ID_FILE_OPEN_FAILED */
    2,               /* MESSAGE_CATALOG_ID_MEMORY_SIGNATURE */
    3,               /* MESSAGE_CATALOG_ID_BUFFER_LIMIT */
    4,               /* MESSAGE_CATALOG_ID_RECORD_MISMATCH */
    5,               /* MESSAGE_CATALOG_ID_RETRY_SCHEDULED */
    6                /* MESSAGE_CATALOG_ID_THROUGHPUT_REPORT */
};

/** @ref s_id_index の要素数です。 */
#define ID_INDEX_COUNT ((int)(sizeof(s_id_index) / sizeof(s_id_index[0])))

/* Doxygen コメントは、ヘッダーに記載 */

const message_catalog_entry *message_catalog_definition_entries(void)
{
    return s_entries;
}

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_definition_entry_count(void)
{
    return ENTRY_COUNT;
}

/* Doxygen コメントは、ヘッダーに記載 */

const int *message_catalog_definition_id_index(void)
{
    return s_id_index;
}

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_definition_id_index_count(void)
{
    return ID_INDEX_COUNT;
}
