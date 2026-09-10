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
     *  @brief          カタログに登録したメッセージを識別します。
     *
     *  各 ID の引数スキーマ、レベル、言語別の書式と備考は、同じ生成単位の表が保持します。\n
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
