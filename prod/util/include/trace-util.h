#ifndef TRACE_UTIL_H
#define TRACE_UTIL_H

/**
 *  @file           trace-util.h
 *  @brief          クロスプラットフォーム トレーシング API。
 *  @details        Windows (ETW) と Linux (syslog) の差異を抽象化し、
 *                  共通のトレースレベルとインターフェースを提供します。\n
 *                  内部で @c trace-etw-util.h (Windows) または @c trace-syslog-util.h (Linux) を
 *                  使用します。
 *
 *  @par            アーキテクチャ
 *  @code
 *  Application
 *        |
 *        v
 *  trace-util.h (共通 API)
 *        |
 *  +-----+-----+-----+
 *  |           |     |
 *  ETW        syslog File
 *  (Windows)  (Linux) (両OS)
 *  @endcode
 *
 *  @par            使用例 (共通)
 *  @code
 *  #include <trace-util.h>
 *
 *  trace_provider_t *logger = trace_init();
 *  trace_modify_name(logger, "myapp", 0);
 *  trace_start(logger);
 *  trace_write(logger, TRACE_LV_INFO, "application started");
 *  trace_stop(logger);
 *  trace_dispose(logger);
 *  @endcode
 *
 *  @par            使用例 (設定変更)
 *  @code
 *  trace_provider_t *logger = trace_init();
 *  trace_modify_name(logger, "myapp", 0);
 *  trace_modify_ostrc(logger, TRACE_LV_VERBOSE);
 *  trace_start(logger);
 *  trace_write(logger, TRACE_LV_INFO, "running as myapp");
 *  trace_stop(logger);
 *  trace_modify_name(logger, "myapp", 1); // "myapp-1" として再開
 *  trace_start(logger);
 *  trace_write(logger, TRACE_LV_INFO, "running as myapp-1");
 *  trace_stop(logger);
 *  trace_dispose(logger);
 *  @endcode
 */

/* size_t (trace_hex_write / trace_hex_writef で使用) */
#include <stddef.h>
/* int64_t (trace_modify_name で使用) */
#include <inttypes.h>

/* 内部で使用するプラットフォーム固有ヘッダー */
#ifdef _WIN32
#include <trace-etw-util.h>
#else /* !_WIN32 */
#include <trace-syslog-util.h>
#endif /* _WIN32 */

/* ===== デフォルトプロバイダ定義 (Windows) ===== */

#ifdef _WIN32

/**
 *  @def            TRACE_DEFAULT_PROVIDER_NAME
 *  @brief          trace_init が使用するデフォルト ETW プロバイダ名。
 */
#define TRACE_DEFAULT_PROVIDER_NAME "Company.Product"

/**
 *  @def            TRACE_DEFAULT_PROVIDER_GUID
 *  @brief          デフォルト ETW プロバイダの GUID (TraceLogging タプル形式)。
 *  @details        TRACELOGGING_DEFINE_PROVIDER で使用する形式です。
 */
#define TRACE_DEFAULT_PROVIDER_GUID \
    (0xc3a7b5d1, 0x4e2f, 0x4a89, 0x96, 0xc8, 0xd7, 0xe9, 0xf1, 0xa2, 0xb3, 0xc4)

/**
 *  @def            TRACE_DEFAULT_PROVIDER_GUID_STR
 *  @brief          デフォルト ETW プロバイダの GUID (文字列形式)。
 *  @details        etw_session_start に渡す場合など、文字列形式の GUID が
 *                  必要な場面で使用します。
 */
#define TRACE_DEFAULT_PROVIDER_GUID_STR \
    "c3a7b5d1-4e2f-4a89-96c8-d7e9f1a2b3c4"

#endif /* _WIN32 */

/* ===== DLL エクスポート / インポート制御マクロ ===== */

#ifdef DOXYGEN

/**
 *  @def            TRACE_UTIL_EXPORT
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                   | 値                       |
 *  | ------------------------------------------------------ | ------------------------ |
 *  | Linux (非 Windows)                                     | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                    | (空)                     |
 *  | Windows / `TRACE_UTIL_STATIC` 定義時 (静的リンク)      | (空)                     |
 *  | Windows / `TRACE_UTIL_EXPORTS` 定義時 (DLL ビルド)     | `__declspec(dllexport)`  |
 *  | Windows / `TRACE_UTIL_EXPORTS` 未定義時 (DLL 利用側)   | `__declspec(dllimport)`  |
 */
#define TRACE_UTIL_EXPORT

/**
 *  @def            TRACE_UTIL_API
 *  @brief          呼び出し規約マクロ。
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#define TRACE_UTIL_API

#else /* !DOXYGEN */

#ifndef _WIN32
    #define TRACE_UTIL_EXPORT
    #define TRACE_UTIL_API
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef TRACE_UTIL_STATIC
            #ifdef TRACE_UTIL_EXPORTS
                #define TRACE_UTIL_EXPORT __declspec(dllexport)
            #else /* !TRACE_UTIL_EXPORTS */
                #define TRACE_UTIL_EXPORT __declspec(dllimport)
            #endif /* TRACE_UTIL_EXPORTS */
        #else      /* TRACE_UTIL_STATIC */
            #define TRACE_UTIL_EXPORT
        #endif /* TRACE_UTIL_STATIC */
    #else      /* __INTELLISENSE__ */
        #define TRACE_UTIL_EXPORT
    #endif /* __INTELLISENSE__ */
    #ifndef TRACE_UTIL_API
        #define TRACE_UTIL_API __stdcall
    #endif /* TRACE_UTIL_API */
#endif     /* _WIN32 */

#endif /* DOXYGEN */

/* ===== メッセージ長制限 ===== */

/**
 *  @def            TRACE_MESSAGE_MAX_BYTES
 *  @brief          trace_write が受け付けるメッセージの最大バイト数 (null 終端含む)。
 *  @details        ETW (約 65,000 バイト) と syslog (RFC 3164: 1,024 バイト) の
 *                  推奨上限のうち小さい方を採用し、クロスプラットフォームでの
 *                  安全な転送を保証します。\n
 *                  本文の最大長は @c TRACE_MESSAGE_MAX_BYTES @c - @c 1 (= 1,023) バイトです。
 */
#define TRACE_MESSAGE_MAX_BYTES 1024

/**
 *  @def            TRACE_HEX_MAX_DATA_BYTES
 *  @brief          trace_hex_write がラベルなしで HEX 出力できるバイナリデータの最大バイト数。
 *  @details        1 バイトあたり 3 文字 (HH + スペース) を消費し、最終バイトは 2 文字です。\n
 *                  ラベル (@p message) を指定した場合はラベル長 + セパレータ (": ") 分だけ
 *                  出力可能なバイナリデータ量が減少します。\n
 *                  データがこの上限を超える場合は切り詰めが行われ、
 *                  末尾に @c "..." が付与されます。
 */
#define TRACE_HEX_MAX_DATA_BYTES 341

/* ===== 共通トレースレベル ===== */

/**
 *  @enum           trace_level
 *  @brief          アプリケーション共通トレースレベル。
 *  @details        OS 非依存のトレースレベルを定義します。重大度は上から下へ低下します。\n
 *                  内部で ETW Level (1-5) および syslog severity へマッピングされます。
 *
 *  | trace_level       | ETW Level        | syslog severity |
 *  | ----------------- | ---------------- | --------------- |
 *  | TRACE_LV_CRITICAL | Critical (1)     | LOG_CRIT (2)    |
 *  | TRACE_LV_ERROR    | Error (2)        | LOG_ERR (3)     |
 *  | TRACE_LV_WARNING  | Warning (3)      | LOG_WARNING (4) |
 *  | TRACE_LV_INFO     | Informational (4)| LOG_INFO (6)    |
 *  | TRACE_LV_VERBOSE  | Verbose (5)      | LOG_DEBUG (7)   |
 */
enum trace_level
{
    TRACE_LV_CRITICAL = 0, /**< 致命的エラー。 */
    TRACE_LV_ERROR    = 1, /**< エラー。 */
    TRACE_LV_WARNING  = 2, /**< 警告。 */
    TRACE_LV_INFO     = 3, /**< 情報。 */
    TRACE_LV_VERBOSE  = 4, /**< 詳細 (デバッグ)。 */
    TRACE_LV_NONE     = 5  /**< 出力しない。 */
};

/* ===== デフォルトトレースレベル ===== */

/**
 *  @def            TRACE_DEFAULT_OS_LEVEL
 *  @brief          trace_init() が設定する OS トレース (ETW / syslog) のデフォルトレベル。
 *  @details        ユーザーが trace_modify_ostrc() で変更するまで有効な初期値です。
 */
#define TRACE_DEFAULT_OS_LEVEL   TRACE_LV_INFO

/**
 *  @def            TRACE_DEFAULT_FILE_LEVEL
 *  @brief          trace_init() が設定するファイルトレースのデフォルトレベル。
 *  @details        ユーザーが trace_modify_filetrc() で変更するまで有効な初期値です。
 */
#define TRACE_DEFAULT_FILE_LEVEL TRACE_LV_ERROR

/* ===== 不透明ハンドル型 ===== */

/** トレースプロバイダハンドル (不透明型)。 */
typedef struct trace_provider trace_provider_t;

/* ===== API 関数 ===== */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          トレースプロバイダを初期化する。
     *  @details        自プロセスの実行ファイル名をデフォルト識別名として初期化します
     *                  (例: Linux @c /usr/bin/myapp → @c "myapp",
     *                  Windows @c C:\\bin\\myapp.exe → @c "myapp.exe")。\n
     *                  プロセス名の取得に失敗した場合は @c "unknown" を使用します。\n
     *                  Linux 環境では syslog を LOG_USER facility で初期化します。\n
     *                  Windows 環境ではライブラリ内蔵の ETW デフォルトプロバイダ
     *                  (@c TRACE_DEFAULT_PROVIDER_NAME) を使用します。\n
     *                  識別名を変更するには trace_modify_name を呼び出してください。
     *
     *  @return         成功時: ハンドル (stopped 状態)。失敗時: NULL。
     *
     *  @post           戻り値のハンドルは stopped 状態です。
     *                  出力関数を使用するには trace_start を呼び出してください。\n
     *                  stopped 状態では設定関数 (trace_modify_name, trace_modify_ostrc,
     *                  trace_modify_filetrc) をスレッド安全に使用できます。
     *
     *  @par            使用例
     *  @code
     *  trace_provider_t *logger = trace_init();
     *  trace_modify_name(logger, "myapp", 0);
     *  trace_start(logger);
     *  trace_write(logger, TRACE_LV_INFO, "application started");
     *  trace_stop(logger);
     *  trace_dispose(logger);
     *  @endcode
     */
    TRACE_UTIL_EXPORT trace_provider_t *TRACE_UTIL_API
        trace_init(void);

    /**
     *  @brief          トレースプロバイダを開始する。
     *  @details        ハンドルを実行中 (started) 状態に遷移させます。\n
     *                  started 状態では出力関数 (trace_write 等) が有効になり、
     *                  設定関数 (trace_modify_name, trace_modify_ostrc, trace_modify_filetrc) は
     *                  使用できなくなります (-1 を返します)。\n
     *                  既に started 状態の場合は何もせず 0 を返します (冪等)。
     *
     *  @param[in]      handle   trace_init の戻り値。
     *                           NULL の場合は -1 を返します。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @see            trace_stop
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_start(trace_provider_t *handle);

    /**
     *  @brief          トレースプロバイダを停止する。
     *  @details        ハンドルを停止中 (stopped) 状態に遷移させます。\n
     *                  stopped 状態では出力関数 (trace_write 等) は -1 を返し、
     *                  設定関数 (trace_modify_name, trace_modify_ostrc, trace_modify_filetrc) が
     *                  スレッド安全に使用できるようになります。\n
     *                  既に stopped 状態の場合は何もせず 0 を返します (冪等)。
     *
     *  @param[in]      handle   trace_init の戻り値。
     *                           NULL の場合は -1 を返します。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @see            trace_start
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_stop(trace_provider_t *handle);

    /**
     *  @brief          トレースメッセージを書き込む。
     *  @details        指定されたトレースレベルでメッセージを書き込みます。\n
     *                  内部で enum trace_level を ETW Level または syslog severity に
     *                  マッピングして書き込みます。
     *
     *  @param[in]      handle   trace_init の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    トレースレベル (enum trace_level)。
     *  @param[in]      message  null 終端 UTF-8 文字列。NULL の場合は何もせず 0 を返します。
     *
     *  メッセージが @c TRACE_MESSAGE_MAX_BYTES (1,024) バイト
     *  (null 終端含む) を超える場合、本関数は内部で
     *  1,023 バイト以内に切り詰めて下層 API に渡します。\n
     *  切り詰め時は UTF-8 マルチバイト文字の途中で切断しないよう
     *  バイト境界を調整します。\n
     *  この上限は ETW (約 65,000 バイト) と syslog (RFC 3164:
     *  1,024 バイト) の推奨値のうち小さい方に合わせています。
     *
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが started 状態であること (trace_start 呼び出し済み)。
     *                  stopped 状態では -1 を返します。
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_write(trace_provider_t *handle, enum trace_level level, const char *message);

    /**
     *  @brief          printf 形式でトレースメッセージを書き込む。
     *  @details        フォーマット文字列と可変長引数で構成されたメッセージを書き込みます。\n
     *                  内部で @c vsnprintf を使用し、@c TRACE_MESSAGE_MAX_BYTES (1,024) バイトの
     *                  バッファにフォーマットします。バッファを超える部分は切り詰められます。
     *
     *  @param[in]      handle   trace_init の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    トレースレベル (enum trace_level)。
     *  @param[in]      format   printf 形式のフォーマット文字列。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      ...      フォーマット文字列に対応する可変長引数。
     *
     *  @par            使用例
     *  @code
     *  trace_writef(logger, TRACE_LV_INFO, "user=%s count=%d", username, count);
     *  @endcode
     *
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが started 状態であること (trace_start 呼び出し済み)。
     *                  stopped 状態では -1 を返します。
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_writef(trace_provider_t *handle, enum trace_level level, const char *format, ...);

    /**
     *  @brief          バイナリデータを HEX テキスト形式でトレースに書き込む。
     *  @details        バイナリデータを大文字スペース区切りの HEX 文字列に変換して
     *                  トレースに書き込みます。@p message が非 NULL の場合はラベルとして
     *                  先頭に付与されます。
     *
     *  @par            出力フォーマット
     *  @code
     *  // message 指定あり (全データ表示):
     *  "Received data: 48 65 6C 6C 6F"
     *
     *  // message が NULL (全データ表示):
     *  "48 65 6C 6C 6F"
     *
     *  // データが収まらない場合 (切り詰め):
     *  "Received data: 48 65 6C ..."
     *  @endcode
     *
     *  @param[in]      handle   trace_init の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    トレースレベル (enum trace_level)。
     *  @param[in]      data     バイナリデータへのポインタ。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      size     バイナリデータのバイト数。
     *                           0 の場合は何もせず 0 を返します。
     *  @param[in]      message  HEX データの手前に付与するラベル文字列。
     *                           NULL の場合はラベルなしで HEX のみ出力します。
     *
     *  ラベルなしの場合、最大 @c TRACE_HEX_MAX_DATA_BYTES (341) バイトの
     *  バイナリデータを出力できます。ラベル指定時はラベル長 + セパレータ
     *  (": ", 2 バイト) 分だけ出力可能なデータ量が減少します。\n
     *  データが収まらない場合は切り詰め、末尾に @c "..." を付与します。
     *
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが started 状態であること (trace_start 呼び出し済み)。
     *                  stopped 状態では -1 を返します。
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_hex_write(trace_provider_t *handle, enum trace_level level,
                      const void *data, size_t size, const char *message);

    /**
     *  @brief          バイナリデータを HEX テキスト形式でトレースに書き込む (printf 形式ラベル)。
     *  @details        trace_hex_write と同等ですが、ラベル文字列を printf 形式の
     *                  フォーマット文字列と可変長引数で指定できます。
     *
     *  @par            出力フォーマット
     *  @code
     *  // フォーマット済みラベル付き:
     *  "packet[3]: 48 65 6C 6C 6F"
     *
     *  // データが収まらない場合:
     *  "packet[3]: 48 65 6C ..."
     *  @endcode
     *
     *  @param[in]      handle   trace_init の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    トレースレベル (enum trace_level)。
     *  @param[in]      data     バイナリデータへのポインタ。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      size     バイナリデータのバイト数。
     *                           0 の場合は何もせず 0 を返します。
     *  @param[in]      format   printf 形式のフォーマット文字列 (ラベル)。
     *                           NULL の場合はラベルなしで HEX のみ出力します。
     *  @param[in]      ...      フォーマット文字列に対応する可変長引数。
     *
     *  @par            使用例
     *  @code
     *  trace_hex_writef(logger, TRACE_LV_INFO, data, len, "packet[%d]", index);
     *  @endcode
     *
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが started 状態であること (trace_start 呼び出し済み)。
     *                  stopped 状態では -1 を返します。
     *  @see            trace_hex_write
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_hex_writef(trace_provider_t *handle, enum trace_level level,
                       const void *data, size_t size, const char *format, ...);

    /**
     *  @brief          トレースプロバイダの識別名と識別番号を設定する。
     *  @details        初期化済みのハンドルを維持したまま、識別名と識別番号を変更します。\n
     *                  dispose して再 init する場合と異なり、以下の利点があります。
     *                  - ハンドルが無効化されないため、呼び出し側でポインタを差し替える必要がない
     *                  - Windows では ETW プロバイダの登録・解除を行わない
     *                  - トレースが書き込めない空白期間が発生しない
     *
     *                  内部識別名は @p name と @p identifier から以下のルールで生成します。\n
     *                  - @p identifier @c == @c 0 : 識別名 = @p name そのもの\n
     *                  - @p identifier @c > @c 0 : 識別名 = @c "<name>-<identifier>" (ハイフン区切り)\n
     *                  - @p identifier @c < @c 0 : -1 を返す (無効値)\n
     *                  Linux 環境では内部で closelog / openlog が呼び出されます。\n
     *                  Windows 環境では ETW イベントの "Service" フィールド値のみが変更されます。
     *
     *  @param[in]      handle      trace_init の戻り値。
     *                              NULL の場合は何もせず -1 を返します。
     *  @param[in]      name        ベース識別名。\n
     *                              NULL の場合は自プロセスの実行ファイル名を使用します
     *                              (trace_init と同じ動作)。
     *  @param[in]      identifier  アプリケーション管理識別番号 (0 以上)。\n
     *                              0 の場合は @p name をそのまま使用します。\n
     *                              正の値の場合は @c "<name>-<identifier>" を識別名とします。\n
     *                              負の値の場合は -1 を返します。
     *  @return         成功 0 / 失敗 (無効な引数・メモリ確保失敗等) -1。
     *
     *  @par            使用例
     *  @code
     *  trace_provider_t *logger = trace_init();
     *  trace_modify_name(logger, "worker", 2); // 識別名 = "worker-2"
     *  trace_start(logger);
     *  trace_write(logger, TRACE_LV_INFO, "running as worker-2");
     *  trace_dispose(logger);
     *  @endcode
     *
     *  @pre            ハンドルが stopped 状態であること。
     *                  started 状態では -1 を返します。\n
     *                  stopped 状態ではスレッド安全です (内部で排他制御)。
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_modify_name(trace_provider_t *handle, const char *name, int64_t identifier);

    /**
     *  @brief          OS トレースのスレッショルドレベルを設定する。
     *  @details        ETW (Windows) または syslog (Linux) に出力するメッセージの
     *                  最低重要度レベルを変更します。\n
     *                  デフォルト値は @c TRACE_DEFAULT_OS_LEVEL (TRACE_LV_INFO) です。\n
     *                  @c TRACE_LV_NONE を指定すると OS トレース出力を完全に抑止します。
     *
     *  @param[in]      handle   trace_init の戻り値。NULL の場合は -1 を返します。
     *  @param[in]      level    新しいスレッショルドレベル (enum trace_level)。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが stopped 状態であること。
     *                  started 状態では -1 を返します。\n
     *                  stopped 状態ではスレッド安全です (内部で排他制御)。
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_modify_ostrc(trace_provider_t *handle, enum trace_level level);

    /**
     *  @brief          ファイルトレースの出力先と設定を変更する。
     *  @details        ファイルトレースを有効化または再構成します。\n
     *                  @p path に NULL を指定するとファイルトレースを無効化します
     *                  (既存のファイルプロバイダを解放して閉じます)。\n
     *                  既にファイルトレースが有効な場合は既存のプロバイダを解放してから
     *                  新しいプロバイダを初期化します。\n
     *                  デフォルトのファイルトレースレベルは @c TRACE_DEFAULT_FILE_LEVEL
     *                  (TRACE_LV_ERROR) です。\n
     *                  ファイルトレースは trace_init の直後は無効 (パス未指定) です。
     *
     *  @param[in]      handle       trace_init の戻り値。NULL の場合は -1 を返します。
     *  @param[in]      path         出力ファイルパス。NULL の場合はファイルトレースを無効化。
     *  @param[in]      level        ファイルトレースのスレッショルドレベル (enum trace_level)。
     *  @param[in]      max_bytes    1 ファイルあたりの最大バイト数。0 で既定値
     *                               (TRACE_FILE_DEFAULT_MAX_BYTES = 10 MB) を使用。
     *  @param[in]      generations  保持する旧世代数。0 以下で既定値
     *                               (TRACE_FILE_DEFAULT_GENERATIONS = 5) を使用。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが stopped 状態であること。
     *                  started 状態では -1 を返します。\n
     *                  stopped 状態ではスレッド安全です (内部で排他制御)。
     */
    TRACE_UTIL_EXPORT int TRACE_UTIL_API
        trace_modify_filetrc(trace_provider_t *handle, const char *path,
                             enum trace_level level, size_t max_bytes, int generations);

    /**
     *  @brief          トレースプロバイダを終了し、リソースを解放する。
     *  @details        ハンドルが NULL の場合は何もしません。\n
     *                  started 状態のハンドルに対しても呼び出し可能です
     *                  (内部で自動的に停止してから解放します)。
     *
     *  @param[in]      handle   trace_init の戻り値。
     */
    TRACE_UTIL_EXPORT void TRACE_UTIL_API
        trace_dispose(trace_provider_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TRACE_UTIL_H */
