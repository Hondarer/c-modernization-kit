/**
 *******************************************************************************
 *  @file           message_catalog_definition.h
 *  @brief          利用者が定義するメッセージ ID の列挙と、カタログの取得を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  本ヘッダーは `prod/src/cmd/message-catalog-sample/` のモジュール私有ヘッダーです。\n
 *  同ディレクトリの実装ファイルからだけ `#include "message_catalog_definition.h"` で取り込みます。
 *
 *  本ヘッダーと `message_catalog_definition.c` は、カタログ定義 (Excel など) からの生成物です。\n
 *  列挙と表は 1 組の生成単位であり、常に同時に生成してください。\n
 *  手作業で編集せず、生成元の定義を変更してから再生成してください。
 *
 *  この 2 ファイルが、メッセージ カタログを利用するアプリケーションが用意する部分です。\n
 *  言語、引数種別、レベル、書式の構文はライブラリが定めます。
 *
 *  列挙の名前をライブラリの接頭辞に揃えているのは、生成物の識別子がカタログ定義に由来するためです。\n
 *  ライブラリはこの名前を定義せず、メッセージ ID を `int` として受け取ります。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef MESSAGE_CATALOG_DEFINITION_H
#define MESSAGE_CATALOG_DEFINITION_H

#include <message_catalog/message_catalog_entry.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          メッセージの重大度を表すトレース レベルです。
     *
     *  カタログの分類値 (@ref message_catalog_entry::category) として使用します。\n
     *  ライブラリは分類値を解釈しないため、この意味付けは利用者側の取り決めです。
     *
     *  値は `app/c-platform` の `cplat_trace_level` と同一です。\n
     *  この app は標準 C だけで完結するサンプルであり、cplat に依存しないため、
     *  同じ値をここで再実装しています。\n
     *  cplat を利用する app へ移植する場合は、値が同じであるため変換表なしで置き換えられます。
     *
     *  | message_catalog_trace_level          | ETW Level         | syslog severity |
     *  | ------------------------------------ | ----------------- | --------------- |
     *  | MESSAGE_CATALOG_TRACE_LEVEL_CRITICAL | Critical (1)      | LOG_CRIT (2)    |
     *  | MESSAGE_CATALOG_TRACE_LEVEL_ERROR    | Error (2)         | LOG_ERR (3)     |
     *  | MESSAGE_CATALOG_TRACE_LEVEL_WARNING  | Warning (3)       | LOG_WARNING (4) |
     *  | MESSAGE_CATALOG_TRACE_LEVEL_INFO     | Informational (4) | LOG_INFO (6)    |
     *  | MESSAGE_CATALOG_TRACE_LEVEL_VERBOSE  | Verbose (5)       | LOG_DEBUG (7)   |
     *  | MESSAGE_CATALOG_TRACE_LEVEL_DEBUG    | Verbose (5)       | LOG_DEBUG (7)   |
     *
     *  @ref MESSAGE_CATALOG_TRACE_LEVEL_CRITICAL は 0 であり、分類なしと同じ値です。\n
     *  分類値を取得しただけでは、登録されていないメッセージと区別できません。
     *
     *  本 app はトレースの出力機構を持ちません。\n
     *  レベルは、利用側が出力先や絞り込みを決めるための情報として保持します。
     */
    typedef enum message_catalog_trace_level
    {
        MESSAGE_CATALOG_TRACE_LEVEL_CRITICAL = 0, /**< 致命的エラー。 */
        MESSAGE_CATALOG_TRACE_LEVEL_ERROR = 1,    /**< エラー。 */
        MESSAGE_CATALOG_TRACE_LEVEL_WARNING = 2,  /**< 警告。 */
        MESSAGE_CATALOG_TRACE_LEVEL_INFO = 3,     /**< 情報。 */
        MESSAGE_CATALOG_TRACE_LEVEL_VERBOSE = 4,  /**< 詳細な診断情報。 */
        MESSAGE_CATALOG_TRACE_LEVEL_DEBUG = 5,    /**< 最も詳細な診断情報。 */
        MESSAGE_CATALOG_TRACE_LEVEL_NONE = 6      /**< 出力しない。 */
    } message_catalog_trace_level;

    /**
     *  @brief          カタログに登録したメッセージを識別します。
     *
     *  各 ID の引数スキーマ、分類値、言語別の書式と備考は、同じ生成単位の表が保持します。\n
     *  値はログの解析対象として安定させ、既存の値は変更せず、追加は末尾への追記だけとします。
     */
    typedef enum message_catalog_id
    {
        MESSAGE_CATALOG_ID_STARTUP_COMPLETED = 1, /**< 起動完了。 */
        MESSAGE_CATALOG_ID_FILE_OPEN_FAILED = 2,  /**< ファイルのオープン失敗。 */
        MESSAGE_CATALOG_ID_MEMORY_SIGNATURE = 3,  /**< メモリー領域のシグネチャー。 */
        MESSAGE_CATALOG_ID_BUFFER_LIMIT = 4,      /**< バッファー上限の超過。 */
        MESSAGE_CATALOG_ID_RECORD_MISMATCH = 5,   /**< レコードのシグネチャー不一致。 */
        MESSAGE_CATALOG_ID_RETRY_SCHEDULED = 6,   /**< 再試行の予約。 */
        MESSAGE_CATALOG_ID_THROUGHPUT_REPORT = 7  /**< スループット報告。 */
    } message_catalog_id;

    /**
     *  @brief          カタログの先頭を返します。
     *  @return         カタログの配列です。NULL は返しません。
     *
     *  返すポインターは静的領域を指します。呼び出し側で解放してはなりません。\n
     *  @ref message_catalog_definition_entry_count とともに
     *  `message_catalog_set_catalog()` へ渡します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    const message_catalog_entry *message_catalog_definition_entries(void);

    /**
     *  @brief          カタログの件数を返します。
     *  @return         メッセージの件数です。1 以上を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    int message_catalog_definition_entry_count(void);

    /**
     *  @brief          メッセージ ID からカタログの添字を引く表を返します。
     *  @return         添字表です。NULL は返しません。
     *
     *  メッセージ ID を添字として、カタログの添字を格納します。\n
     *  登録していない添字には負の値を格納します。\n
     *  この表により、メッセージ ID からカタログを引く探索を線形探索から添字引きへ置き換えます。
     *
     *  返すポインターは静的領域を指します。呼び出し側で解放してはなりません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    const int *message_catalog_definition_id_index(void);

    /**
     *  @brief          添字表の要素数を返します。
     *  @return         添字表の要素数です。最大のメッセージ ID に 1 を加えた値です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    int message_catalog_definition_id_index_count(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MESSAGE_CATALOG_DEFINITION_H */
