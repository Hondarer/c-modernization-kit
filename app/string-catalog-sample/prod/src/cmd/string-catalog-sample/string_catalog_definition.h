/**
 *******************************************************************************
 *  @file           string_catalog_definition.h
 *  @brief          利用者が定義する文字列 ID の列挙と、カタログの取得を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  本ヘッダーは `prod/src/cmd/string-catalog-sample/` のモジュール私有ヘッダーです。\n
 *  同ディレクトリの実装ファイルからだけ `#include "string_catalog_definition.h"` で取り込みます。
 *
 *  本ヘッダーと `string_catalog_definition.c` は、カタログ定義 (Excel など) からの生成物です。\n
 *  列挙と表は 1 組の生成単位であり、常に同時に生成してください。\n
 *  手作業で編集せず、生成元の定義を変更してから再生成してください。
 *
 *  この 2 ファイルが、文字列カタログを利用するアプリケーションが用意する部分です。\n
 *  言語、引数種別、レベル、書式の構文はライブラリが定めます。
 *
 *  列挙の名前をライブラリの接頭辞に揃えているのは、生成物の識別子がカタログ定義に由来するためです。\n
 *  ライブラリはこの名前を定義せず、文字列 ID を `int` として受け取ります。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRING_CATALOG_DEFINITION_H
#define STRING_CATALOG_DEFINITION_H

#include <string_catalog/string_catalog_spec.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          文字列の重大度を表すトレース レベルです。
     *
     *  カタログの分類値 (@ref string_catalog_entry::category) として使用します。\n
     *  ライブラリは分類値を解釈しないため、この意味付けは利用者側の取り決めです。
     *
     *  値は `app/c-platform` の `cplat_trace_level` と同一です。\n
     *  この app は標準 C だけで完結するサンプルであり、cplat に依存しないため、
     *  同じ値をここで再実装しています。\n
     *  cplat を利用する app へ移植する場合は、値が同じであるため変換表なしで置き換えられます。
     *
     *  | string_catalog_trace_level          | ETW Level         | syslog severity |
     *  | ------------------------------------ | ----------------- | --------------- |
     *  | STRING_CATALOG_TRACE_LEVEL_CRITICAL | Critical (1)      | LOG_CRIT (2)    |
     *  | STRING_CATALOG_TRACE_LEVEL_ERROR    | Error (2)         | LOG_ERR (3)     |
     *  | STRING_CATALOG_TRACE_LEVEL_WARNING  | Warning (3)       | LOG_WARNING (4) |
     *  | STRING_CATALOG_TRACE_LEVEL_INFO     | Informational (4) | LOG_INFO (6)    |
     *  | STRING_CATALOG_TRACE_LEVEL_VERBOSE  | Verbose (5)       | LOG_DEBUG (7)   |
     *  | STRING_CATALOG_TRACE_LEVEL_DEBUG    | Verbose (5)       | LOG_DEBUG (7)   |
     *
     *  @ref STRING_CATALOG_TRACE_LEVEL_CRITICAL は 0 であり、分類なしと同じ値です。\n
     *  分類値を取得しただけでは、登録されていない文字列と区別できません。
     *
     *  本 app はトレースの出力機構を持ちません。\n
     *  レベルは、利用側が出力先や絞り込みを決めるための情報として保持します。
     */
    typedef enum string_catalog_trace_level
    {
        STRING_CATALOG_TRACE_LEVEL_CRITICAL = 0, /**< 致命的エラー。 */
        STRING_CATALOG_TRACE_LEVEL_ERROR = 1,    /**< エラー。 */
        STRING_CATALOG_TRACE_LEVEL_WARNING = 2,  /**< 警告。 */
        STRING_CATALOG_TRACE_LEVEL_INFO = 3,     /**< 情報。 */
        STRING_CATALOG_TRACE_LEVEL_VERBOSE = 4,  /**< 詳細な診断情報。 */
        STRING_CATALOG_TRACE_LEVEL_DEBUG = 5,    /**< 最も詳細な診断情報。 */
        STRING_CATALOG_TRACE_LEVEL_NONE = 6      /**< 出力しない。 */
    } string_catalog_trace_level;

    /**
     *  @brief          カタログに登録した文字列を識別します。
     *
     *  各 ID の引数スキーマ、分類値、言語別の書式と備考は、同じ生成単位の表が保持します。\n
     *  値はログの解析対象として安定させ、既存の値は変更せず、追加は末尾への追記だけとします。
     */
    typedef enum string_catalog_id
    {
        STRING_CATALOG_ID_STARTUP_COMPLETED = 1, /**< 起動完了。 */
        STRING_CATALOG_ID_FILE_OPEN_FAILED = 2,  /**< ファイルのオープン失敗。 */
        STRING_CATALOG_ID_MEMORY_SIGNATURE = 3,  /**< メモリー領域のシグネチャー。 */
        STRING_CATALOG_ID_BUFFER_LIMIT = 4,      /**< バッファー上限の超過。 */
        STRING_CATALOG_ID_RECORD_MISMATCH = 5,   /**< レコードのシグネチャー不一致。 */
        STRING_CATALOG_ID_RETRY_SCHEDULED = 6,   /**< 再試行の予約。 */
        STRING_CATALOG_ID_THROUGHPUT_REPORT = 7  /**< スループット報告。 */
    } string_catalog_id;

    /**
     *  @brief          カタログの先頭を返します。
     *  @return         カタログの配列です。NULL は返しません。
     *
     *  返すポインターは静的領域を指します。呼び出し側で解放してはなりません。\n
     *  @ref string_catalog_definition_entry_count とともに @ref string_catalog を組み立てる材料です。\n
     *  組み立て済みのカタログは @ref string_catalog_definition_catalog が返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    const string_catalog_entry *string_catalog_definition_entries(void);

    /**
     *  @brief          カタログの件数を返します。
     *  @return         文字列の件数です。1 以上を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    int string_catalog_definition_entry_count(void);

    /**
     *  @brief          文字列 ID からカタログの添字を引く表を返します。
     *  @return         添字表です。NULL は返しません。
     *
     *  文字列 ID を添字として、カタログの添字を格納します。\n
     *  登録していない添字には負の値を格納します。\n
     *  この表により、文字列 ID からカタログを引く探索を線形探索から添字引きへ置き換えます。
     *
     *  返すポインターは静的領域を指します。呼び出し側で解放してはなりません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    const int *string_catalog_definition_id_index(void);

    /**
     *  @brief          添字表の要素数を返します。
     *  @return         添字表の要素数です。最大の文字列 ID に 1 を加えた値です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    int string_catalog_definition_id_index_count(void);

    /**
     *  @brief          このカタログ定義のカタログ識別オブジェクトを返します。
     *  @return         カタログ識別オブジェクトです。NULL は返しません。
     *
     *  配列と添字表を 1 つのカタログへまとめた値です。\n
     *  ライブラリはカタログを保持しないため、組み立て API へはこの値を渡します。
     *
     *  返すポインターは静的領域を指します。呼び出し側で解放してはなりません。\n
     *  ほかのカタログ定義と組み合わせる場合は、それぞれのカタログを使い分けます。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    const string_catalog *string_catalog_definition_catalog(void);

    /**
     *  @brief          このカタログ定義を使用して、文字列を組み立てます。
     *  @param[out]     dest      文字列の格納先。NULL を渡してはなりません。
     *  @param[in]      dest_size @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      string_id 組み立てる文字列の ID。
     *  @param[in]      ...       引数スキーマが定める順序と型の値。
     *  @return         戻り値は @ref string_catalog_format と同じです。
     *
     *  カタログを省略して呼び出す口です。\n
     *  @ref string_catalog_definition_catalog を補って @ref string_catalog_format を呼び出します。
     *
     *  @par            スレッド セーフ
     *  スレッド セーフ性は @ref string_catalog_format と同じです。
     */
    int string_catalog_definition_format(char *dest, size_t dest_size, int string_id, ...);

    /**
     *  @brief          このカタログ定義を使用して、@c va_list から文字列を組み立てます。
     *  @param[out]     dest      文字列の格納先。NULL を渡してはなりません。
     *  @param[in]      dest_size @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      string_id 組み立てる文字列の ID。
     *  @param[in]      args      引数スキーマが定める順序と型の値を保持する引数リスト。
     *  @return         戻り値は @ref string_catalog_vformat と同じです。
     *
     *  @par            スレッド セーフ
     *  スレッド セーフ性は @ref string_catalog_vformat と同じです。
     */
    int string_catalog_definition_vformat(char *dest, size_t dest_size, int string_id, va_list args);

    /**
     *  @brief          このカタログ定義の内容を確認します。
     *  @param[out]     string_id_out 不正を検出した文字列の ID。不要な場合は NULL を指定できます。
     *  @param[out]     language_out  不正を検出した言語。不要な場合は NULL を指定できます。
     *  @return         戻り値は @ref string_catalog_verify と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int string_catalog_definition_verify(int *string_id_out, string_catalog_language *language_out);

    /**
     *  @brief          このカタログ定義から、文字列の分類値を返します。
     *  @param[in]      string_id 参照する文字列の ID。
     *  @return         戻り値は @ref string_catalog_category と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int string_catalog_definition_category(int string_id);

    /**
     *  @brief          このカタログ定義から、文字列 ID の固定文字列を返します。
     *  @param[in]      string_id 参照する文字列の ID。
     *  @return         戻り値は @ref string_catalog_id_text と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    const char *string_catalog_definition_id_text(int string_id);

    /**
     *  @brief          このカタログ定義から、現在の言語で文字列の備考を返します。
     *  @param[in]      string_id 参照する文字列の ID。
     *  @return         戻り値は @ref string_catalog_note と同じです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    const char *string_catalog_definition_note(int string_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRING_CATALOG_DEFINITION_H */
