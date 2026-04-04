#ifndef TRACE_FILE_H
#define TRACE_FILE_H

/**
 *  @file           trace_file.h
 *  @brief          ファイルベーストレースプロバイダライブラリ。
 *  @details        トレースメッセージをローテーション付きテキストファイルへ同期書き込みする
 *                  クロスプラットフォームプロバイダです。\n
 *                  trace_etw (Windows ETW) および trace_syslog (Linux syslog) と
 *                  同じ init / write / dispose インターフェースを提供します。
 *
 *  @par            出力フォーマット
 *  @code
 *  2026-03-31 12:34:56.789 I メッセージテキスト
 *  @endcode
 *  レベル文字: C=CRITICAL / E=ERROR / W=WARNING / I=INFO / V=VERBOSE
 */

#include <stddef.h>
#include <util/trace/trace.h>

/* ===== DLL エクスポート / インポート制御マクロ ===== */

#ifdef DOXYGEN

/**
 *  @def            TRACE_FILE_EXPORT
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                          | 値                       |
 *  | ------------------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                            | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                           | (空)                     |
 *  | Windows / `TRACE_FILE_STATIC` 定義時 (静的リンク)         | (空)                     |
 *  | Windows / `TRACE_FILE_EXPORTS` 定義時 (DLL ビルド)        | `__declspec(dllexport)`  |
 *  | Windows / `TRACE_FILE_EXPORTS` 未定義時 (DLL 利用側)      | `__declspec(dllimport)`  |
 */
#define TRACE_FILE_EXPORT

/**
 *  @def            TRACE_FILE_API
 *  @brief          呼び出し規約マクロ。
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#define TRACE_FILE_API

#else /* !DOXYGEN */

#ifndef _WIN32
    #define TRACE_FILE_EXPORT
    #define TRACE_FILE_API
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef TRACE_FILE_STATIC
            #ifdef TRACE_FILE_EXPORTS
                #define TRACE_FILE_EXPORT __declspec(dllexport)
            #else /* !TRACE_FILE_EXPORTS */
                #define TRACE_FILE_EXPORT __declspec(dllimport)
            #endif /* TRACE_FILE_EXPORTS */
        #else      /* TRACE_FILE_STATIC */
            #define TRACE_FILE_EXPORT
        #endif /* TRACE_FILE_STATIC */
    #else      /* __INTELLISENSE__ */
        #define TRACE_FILE_EXPORT
    #endif /* __INTELLISENSE__ */
    #ifndef TRACE_FILE_API
        #define TRACE_FILE_API __stdcall
    #endif /* TRACE_FILE_API */
#endif     /* _WIN32 */

#endif /* DOXYGEN */

/* ===== 設定定数 ===== */

/**
 *  @def            TRACE_FILE_SINK_DEFAULT_MAX_BYTES
 *  @brief          トレースファイル 1 世代あたりの既定最大サイズ (バイト)。
 *  @details        この値を超えるとローテーションが実行されます。\n
 *                  trace_file_sink_create の max_bytes に 0 を指定した場合に使用されます。
 */
#define TRACE_FILE_SINK_DEFAULT_MAX_BYTES  ((size_t)(10 * 1024 * 1024))

/**
 *  @def            TRACE_FILE_SINK_DEFAULT_GENERATIONS
 *  @brief          保持するトレースファイル世代数の既定値。
 *  @details        ローテーション時に path.1 〜 path.N のファイルを保持します。\n
 *                  trace_file_sink_create の generations に 0 以下を指定した場合に使用されます。
 */
#define TRACE_FILE_SINK_DEFAULT_GENERATIONS  5

/* ===== 不透明ハンドル型 ===== */

/** ファイルトレースプロバイダハンドル (不透明型)。 */
typedef struct trace_file_sink trace_file_sink_t;

/* ===== API 関数 ===== */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          ファイルトレースプロバイダを初期化する。
     *  @details        指定されたファイルパスへの書き込みを開始します。\n
     *                  ファイルが存在する場合は追記します。存在しない場合は新規作成します。\n
     *                  max_bytes に 0 を指定した場合は TRACE_FILE_SINK_DEFAULT_MAX_BYTES を使用します。\n
     *                  generations に 0 以下を指定した場合は TRACE_FILE_SINK_DEFAULT_GENERATIONS を使用します。
     *
     *  @par            ローテーション動作
     *                  書き込み後にファイルサイズが max_bytes に達すると、以下の名前でファイルを保持します。
     *                  @code
     *                  path     ... 現在のトレースファイル (新規作成)
     *                  path.1   ... 直前の世代
     *                  path.2   ... 2 世代前
     *                  path.N   ... N 世代前 (N = generations)
     *                  @endcode
     *                  generations 世代を超えた古いファイルは削除されます。
     *
     *  @param[in]      path         出力ファイルパス。NULL の場合は NULL を返します。
     *  @param[in]      max_bytes    1 ファイルあたりの最大バイト数。0 でデフォルト値を使用。
     *  @param[in]      generations  保持する旧世代数。0 以下でデフォルト値を使用。
     *  @return         成功時: ハンドル。失敗時: NULL。
     *
     *  @par            使用例
     *  @code
     *  trace_file_sink_t *h = trace_file_sink_create(
     *      "C:\\logs\\myapp.log", 0, 0);
     *  @endcode
     */
    TRACE_FILE_EXPORT trace_file_sink_t *TRACE_FILE_API
        trace_file_sink_create(const char *path, size_t max_bytes, int generations);

    /**
     *  @brief          ファイルへトレースメッセージを書き込む。
     *  @details        タイムスタンプとトレースレベル文字を付加し 1 行で書き込みます。\n
     *                  書き込み後にファイルサイズが max_bytes に達した場合はローテーションします。\n
     *                  handle または message が NULL の場合は何もせず 0 を返します。\n
     *                  ファイルがロックされている等の I/O 失敗は呼び出し元をブロックせず -1 を返します。
     *
     *  @param[in]      handle   trace_file_sink_create の戻り値。NULL は無視。
     *  @param[in]      level    トレースレベル (TRACE_FILE_LV_* 定数)。
     *  @param[in]      message  null 終端 UTF-8 文字列。NULL は無視。
     *  @return         成功 0 / 失敗 -1。
     */
    TRACE_FILE_EXPORT int TRACE_FILE_API
        trace_file_sink_write(trace_file_sink_t *handle, int level,
                                  const char *message);

    /**
     *  @brief          ファイルトレースプロバイダを終了する。
     *  @details        ファイルハンドルを閉じ、ロック / mutex を解放してメモリを解放します。\n
     *                  ハンドルが NULL の場合は何もしません。
     *
     *  @param[in]      handle   trace_file_sink_create の戻り値。
     */
    TRACE_FILE_EXPORT void TRACE_FILE_API
        trace_file_sink_destroy(trace_file_sink_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TRACE_FILE_H */
