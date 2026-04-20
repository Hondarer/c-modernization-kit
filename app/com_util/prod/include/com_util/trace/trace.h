/**
 *******************************************************************************
 *  @file           trace.h
 *  @brief          クロスプラットフォーム トレーシング API。
 *  @author         Tetsuo Honda
 *  @date           2026/03/05
 *  @version        1.0.0
 *
 *  Windows (ETW) と Linux (syslog) の差異を抽象化し、
 *  共通のトレースレベルとインターフェースを提供します。\n
 *  内部で @c com_util/trace/trace_etw.h (Windows) または @c com_util/trace/trace_syslog.h (Linux) を
 *  使用します。
 *
 *  @par            アーキテクチャ
 *  @code
    Application
          |
          v           trace.h (共通 API)
          |
    +-----+-----+------+--------+
    |           |      |        |
    ETW        syslog File    stderr
    (Windows)  (Linux) (両OS)  (両OS)
 *  @endcode
 *
 *  @par            使用例 (共通)
 *  @code{.c}
    #include <com_util/trace/trace.h>

    trace_logger_t *logger = trace_logger_create();
    trace_logger_set_name(logger, "myapp", 0);
    trace_logger_start(logger);
    trace_logger_write(logger, TRACE_LEVEL_INFO, "application started");
    trace_logger_stop(logger);
    trace_logger_destroy(logger);
 *  @endcode
 *
 *  @par            使用例 (設定変更)
 *  @code{.c}
    trace_logger_t *logger = trace_logger_create();
    trace_logger_set_name(logger, "myapp", 0);
    trace_logger_set_os_level(logger, TRACE_LEVEL_VERBOSE);
    trace_logger_start(logger);
    trace_logger_write(logger, TRACE_LEVEL_INFO, "running as myapp");
    trace_logger_stop(logger);
    trace_logger_set_name(logger, "myapp", 1); // "myapp-1" として再開
    trace_logger_start(logger);
    trace_logger_write(logger, TRACE_LEVEL_INFO, "running as myapp-1");
    trace_logger_stop(logger);
    trace_logger_destroy(logger);
 *  @endcode
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef TRACE_LOGGER_H
#define TRACE_LOGGER_H

/* size_t (trace_logger_write_hex / trace_logger_write_hexf で使用) */
#include <stddef.h>
/* int64_t (trace_logger_set_name で使用) */
#include <inttypes.h>
/* strrchr (_TRACE_LOGGER_BASENAME マクロで使用) */
#include <string.h>
#include <com_util/base/platform.h>
#include <com_util_export.h>

/* 内部で使用するプラットフォーム固有ヘッダー */
#if defined(PLATFORM_LINUX)
#include <com_util/trace/trace_syslog.h>
#elif defined(PLATFORM_WINDOWS)
#include <com_util/trace/trace_etw.h>
#endif /* PLATFORM_ */

/* ===== デフォルトプロバイダ定義 (Windows) ===== */

#if defined(PLATFORM_WINDOWS)

/**
 *  @def            TRACE_LOGGER_DEFAULT_PROVIDER_NAME
 *  @brief          trace_logger_create が使用するデフォルト ETW プロバイダ名。
 */
#define TRACE_LOGGER_DEFAULT_PROVIDER_NAME "Company.Product"

/**
 *  @def            TRACE_LOGGER_DEFAULT_PROVIDER_GUID
 *  @brief          デフォルト ETW プロバイダの GUID (TraceLogging タプル形式)。
 *
 *  TRACELOGGING_DEFINE_PROVIDER で使用する形式です。
 */
#define TRACE_LOGGER_DEFAULT_PROVIDER_GUID \
    (0xc3a7b5d1, 0x4e2f, 0x4a89, 0x96, 0xc8, 0xd7, 0xe9, 0xf1, 0xa2, 0xb3, 0xc4)

/**
 *  @def            TRACE_LOGGER_DEFAULT_PROVIDER_GUID_STR
 *  @brief          デフォルト ETW プロバイダの GUID (文字列形式)。
 *
 *  trace_etw_session_start に渡す場合など、文字列形式の GUID が
 *  必要な場面で使用します。
 */
#define TRACE_LOGGER_DEFAULT_PROVIDER_GUID_STR \
    "c3a7b5d1-4e2f-4a89-96c8-d7e9f1a2b3c4"

#endif /* PLATFORM_WINDOWS */

/* ===== メッセージ長制限 ===== */

/**
 *  @def            TRACE_LOGGER_MESSAGE_MAX_BYTES
 *  @brief          trace_logger_write が受け付けるメッセージの最大バイト数 (null 終端含む)。
 *
 *  ETW (約 65,000 バイト) と syslog (RFC 3164: 1,024 バイト) の
 *  推奨上限のうち小さい方を採用し、クロスプラットフォームでの
 *  安全な転送を保証します。\n
 *  本文の最大長は @c TRACE_LOGGER_MESSAGE_MAX_BYTES @c - @c 1 (= 1,023) バイトです。
 */
#define TRACE_LOGGER_MESSAGE_MAX_BYTES 1024

/**
 *  @def            TRACE_LOGGER_HEX_MAX_DATA_BYTES
 *  @brief          trace_logger_write_hex がラベルなしで HEX 出力できるバイナリデータの最大バイト数。
 *
 *  1 バイトあたり 3 文字 (HH + スペース) を消費し、最終バイトは 2 文字です。\n
 *  ラベル (@p message) を指定した場合はラベル長 + セパレータ (": ") 分だけ
 *  出力可能なバイナリデータ量が減少します。\n
 *  データがこの上限を超える場合は切り詰めが行われ、
 *  末尾に @c "..." が付与されます。
 */
#define TRACE_LOGGER_HEX_MAX_DATA_BYTES 341

/* ===== 共通トレースレベル ===== */

/**
 *  @enum           trace_level_t
 *  @brief          アプリケーション共通トレースレベル。
 *
 *  OS 非依存のトレースレベルを定義します。重大度は上から下へ低下します。\n
 *  内部で ETW Level (1-5) および syslog severity へマッピングされます。\n
 *  TRACE_LEVEL_DEBUG は ETW / syslog では TRACE_LEVEL_VERBOSE と同じ詳細度で扱われます。
 *
 *  | trace_level          | ETW Level         | syslog severity |
 *  | -------------------- | ----------------- | --------------- |
 *  | TRACE_LEVEL_CRITICAL | Critical (1)      | LOG_CRIT (2)    |
 *  | TRACE_LEVEL_ERROR    | Error (2)         | LOG_ERR (3)     |
 *  | TRACE_LEVEL_WARNING  | Warning (3)       | LOG_WARNING (4) |
 *  | TRACE_LEVEL_INFO     | Informational (4) | LOG_INFO (6)    |
 *  | TRACE_LEVEL_VERBOSE  | Verbose (5)       | LOG_DEBUG (7)   |
 *  | TRACE_LEVEL_DEBUG    | Verbose (5)       | LOG_DEBUG (7)   |
 */
typedef enum trace_level_t
{
    TRACE_LEVEL_CRITICAL = 0, /**< 致命的エラー。 */
    TRACE_LEVEL_ERROR    = 1, /**< エラー。 */
    TRACE_LEVEL_WARNING  = 2, /**< 警告。 */
    TRACE_LEVEL_INFO     = 3, /**< 情報。 */
    TRACE_LEVEL_VERBOSE  = 4, /**< 詳細な診断情報。 */
    TRACE_LEVEL_DEBUG    = 5, /**< 最も詳細な診断情報。 */
    TRACE_LEVEL_NONE     = 6  /**< 出力しない。 */
} trace_level_t;

/* ===== デフォルトトレースレベル ===== */

/**
 *  @def            TRACE_LOGGER_DEFAULT_OS_LEVEL
 *  @brief          trace_logger_create() が設定する OS トレース (ETW / syslog) のデフォルトレベル。
 *
 *  ユーザーが trace_logger_set_os_level() で変更するまで有効な初期値です。
 */
#define TRACE_LOGGER_DEFAULT_OS_LEVEL   TRACE_LEVEL_INFO

/**
 *  @def            TRACE_LOGGER_DEFAULT_FILE_LEVEL
 *  @brief          trace_logger_create() が設定するファイルトレースのデフォルトレベル。
 *
 *  ユーザーが trace_logger_set_file_level() で変更するまで有効な初期値です。
 */
#define TRACE_LOGGER_DEFAULT_FILE_LEVEL TRACE_LEVEL_ERROR

/**
 *  @def            TRACE_LOGGER_DEFAULT_STDERR_LEVEL
 *  @brief          trace_logger_create() が設定する stderr トレースのデフォルトレベル。
 *
 *  ユーザーが trace_logger_set_stderr_level() で変更するまで有効な初期値です。\n
 *  デフォルトは TRACE_LEVEL_NONE (無効) です。
 */
#define TRACE_LOGGER_DEFAULT_STDERR_LEVEL TRACE_LEVEL_NONE

/* ===== 不透明ハンドル型 ===== */

/** トレースプロバイダハンドル (不透明型)。 */
typedef struct trace_logger trace_logger_t;

/* ===== API 関数 ===== */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *******************************************************************************
     *  @brief          トレースプロバイダを初期化する。
     *
     *  自プロセスの実行ファイル名をデフォルト識別名として初期化します
     *  (例: Linux @c /usr/bin/myapp → @c "myapp",
     *  Windows @c C:\\bin\\myapp.exe → @c "myapp.exe")。\n
     *  プロセス名の取得に失敗した場合は @c "unknown" を使用します。\n
     *  Linux 環境では syslog を LOG_USER facility で初期化します。\n
     *  Windows 環境ではライブラリ内蔵の ETW デフォルトプロバイダ
     *  (@c TRACE_LOGGER_DEFAULT_PROVIDER_NAME) を使用します。\n
     *  識別名を変更するには trace_logger_set_name を呼び出してください。
     *
     *  @return         成功時: ハンドル (stopped 状態)。失敗時: NULL。
     *
     *  @post           戻り値のハンドルは stopped 状態です。
     *                  出力関数を使用するには trace_logger_start を呼び出してください。\n
     *                  stopped 状態では設定関数 (trace_logger_set_name, trace_logger_set_os_level,
     *                  trace_logger_set_file_level, trace_logger_set_stderr_level) をスレッド安全に使用できます。
     *
     *  @par            使用例
     *  @code{.c}
        trace_logger_t *logger = trace_logger_create();
        trace_logger_set_name(logger, "myapp", 0);
        trace_logger_start(logger);
        trace_logger_write(logger, TRACE_LEVEL_INFO, "application started");
        trace_logger_stop(logger);
        trace_logger_destroy(logger);
     *  @endcode
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  複数スレッドから独立したハンドルを取得するために並行して呼び出すことができます。
     *******************************************************************************
     */
    COM_UTIL_EXPORT trace_logger_t *COM_UTIL_API
        trace_logger_create(void);

    /**
     *******************************************************************************
     *  @brief          トレースプロバイダを開始する。
     *
     *  ハンドルを実行中 (started) 状態に遷移させます。\n
     *  started 状態では出力関数 (trace_logger_write 等) が有効になり、
     *  設定関数 (trace_logger_set_name, trace_logger_set_os_level, trace_logger_set_file_level,
     *  trace_logger_set_stderr_level) は使用できなくなります (-1 を返します)。\n
     *  すでに started 状態の場合は何もせず 0 を返します (冪等)。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  内部で排他制御を行います。
     *
     *  @warning        handle が NULL の場合は -1 を返します。
     *
     *  @see            trace_logger_stop
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_start(trace_logger_t *handle);

    /**
     *******************************************************************************
     *  @brief          トレースプロバイダを停止する。
     *
     *  ハンドルを停止中 (stopped) 状態に遷移させます。\n
     *  stopped 状態では出力関数 (trace_logger_write 等) は -1 を返し、
     *  設定関数 (trace_logger_set_name, trace_logger_set_os_level, trace_logger_set_file_level,
     *  trace_logger_set_stderr_level) がスレッド安全に使用できるようになります。\n
     *  すでに stopped 状態の場合は何もせず 0 を返します (冪等)。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  内部で排他制御を行います。
     *
     *  @warning        handle が NULL の場合は -1 を返します。
     *
     *  @see            trace_logger_start
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_stop(trace_logger_t *handle);

    /**
     *******************************************************************************
     *  @brief          トレースメッセージを書き込む。
     *
     *  指定されたトレースレベルでメッセージを書き込みます。\n
     *  内部で trace_level_t を ETW Level または syslog severity に
     *  マッピングして書き込みます。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @param[in]      level    トレースレベル (trace_level_t)。
     *  @param[in]      message  null 終端 UTF-8 文字列。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @note
     *  メッセージが @c TRACE_LOGGER_MESSAGE_MAX_BYTES (1,024) バイト
     *  (null 終端含む) を超える場合、本関数は内部で
     *  1,023 バイト以内に切り詰めて下層 API に渡します。\n
     *  切り詰め時は UTF-8 マルチバイト文字の途中で切断しないよう
     *  バイト境界を調整します。\n
     *  この上限は ETW (約 65,000 バイト) と syslog (RFC 3164:
     *  1,024 バイト) の推奨値のうち小さい方に合わせています。
     *
     *  @pre            ハンドルが started 状態であること (trace_logger_start 呼び出し済み)。
     *                  stopped 状態では -1 を返します。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  started 状態では複数スレッドから並行して呼び出すことができます。
     *
     *  @warning        handle が NULL の場合は何もせず 0 を返します。\n
     *                  message が NULL の場合は何もせず 0 を返します。\n
     *                  stopped 状態では -1 を返します。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_write(trace_logger_t *handle, trace_level_t level, const char *message);

    /**
     *******************************************************************************
     *  @brief          printf 形式でトレースメッセージを書き込む。
     *
     *  フォーマット文字列と可変長引数で構成されたメッセージを書き込みます。\n
     *  内部で @c vsnprintf を使用し、@c TRACE_LOGGER_MESSAGE_MAX_BYTES (1,024) バイトの
     *  バッファにフォーマットします。バッファを超える部分は切り詰められます。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @param[in]      level    トレースレベル (trace_level_t)。
     *  @param[in]      format   printf 形式のフォーマット文字列。
     *  @param[in]      ...      フォーマット文字列に対応する可変長引数。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @par            使用例
     *  @code{.c}
        trace_logger_writef(logger, TRACE_LEVEL_INFO, "user=%s count=%d", username, count);
     *  @endcode
     *
     *  @pre            ハンドルが started 状態であること (trace_logger_start 呼び出し済み)。
     *                  stopped 状態では -1 を返します。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  started 状態では複数スレッドから並行して呼び出すことができます。
     *
     *  @warning        handle が NULL の場合は何もせず 0 を返します。\n
     *                  format が NULL の場合は何もせず 0 を返します。\n
     *                  stopped 状態では -1 を返します。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_writef(trace_logger_t *handle, trace_level_t level, const char *format, ...);

    /**
     *******************************************************************************
     *  @brief          バイナリデータを HEX テキスト形式でトレースに書き込む。
     *
     *  バイナリデータを大文字スペース区切りの HEX 文字列に変換して
     *  トレースに書き込みます。@p message が非 NULL の場合はラベルとして
     *  先頭に付与されます。\n
     *  ラベルなしの場合、最大 @c TRACE_LOGGER_HEX_MAX_DATA_BYTES (341) バイトの
     *  バイナリデータを出力できます。ラベル指定時はラベル長 + セパレータ
     *  (": ", 2 バイト) 分だけ出力可能なデータ量が減少します。\n
     *  データが収まらない場合は切り詰め、末尾に @c "..." を付与します。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @param[in]      level    トレースレベル (trace_level_t)。
     *  @param[in]      data     バイナリデータへのポインタ。
     *  @param[in]      size     バイナリデータのバイト数。
     *  @param[in]      message  HEX データの手前に付与するラベル文字列。
     *                           NULL の場合はラベルなしで HEX のみ出力します。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @par            出力フォーマット
     *  @code
        // message 指定あり (全データ表示):
        "Received data: 48 65 6C 6C 6F"

        // message が NULL (全データ表示):
        "48 65 6C 6C 6F"

        // データが収まらない場合 (切り詰め):
        "Received data: 48 65 6C ..."
     *  @endcode
     *
     *  @pre            ハンドルが started 状態であること (trace_logger_start 呼び出し済み)。
     *                  stopped 状態では -1 を返します。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  started 状態では複数スレッドから並行して呼び出すことができます。
     *
     *  @warning        handle が NULL の場合は何もせず 0 を返します。\n
     *                  data が NULL の場合は何もせず 0 を返します。\n
     *                  size が 0 の場合は何もせず 0 を返します。\n
     *                  stopped 状態では -1 を返します。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_write_hex(trace_logger_t *handle, trace_level_t level,
                               const void *data, size_t size, const char *message);

    /**
     *******************************************************************************
     *  @brief          バイナリデータを HEX テキスト形式でトレースに書き込む (printf 形式ラベル)。
     *
     *  trace_logger_write_hex と同等ですが、ラベル文字列を printf 形式の
     *  フォーマット文字列と可変長引数で指定できます。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @param[in]      level    トレースレベル (trace_level_t)。
     *  @param[in]      data     バイナリデータへのポインタ。
     *  @param[in]      size     バイナリデータのバイト数。
     *  @param[in]      format   printf 形式のフォーマット文字列 (ラベル)。
     *                           NULL の場合はラベルなしで HEX のみ出力します。
     *  @param[in]      ...      フォーマット文字列に対応する可変長引数。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @par            出力フォーマット
     *  @code
        // フォーマット済みラベル付き:
        "packet[3]: 48 65 6C 6C 6F"

        // データが収まらない場合:
        "packet[3]: 48 65 6C ..."
     *  @endcode
     *
     *  @par            使用例
     *  @code{.c}
        trace_logger_write_hexf(logger, TRACE_LEVEL_INFO, data, len, "packet[%d]", index);
     *  @endcode
     *
     *  @pre            ハンドルが started 状態であること (trace_logger_start 呼び出し済み)。
     *                  stopped 状態では -1 を返します。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  started 状態では複数スレッドから並行して呼び出すことができます。
     *
     *  @warning        handle が NULL の場合は何もせず 0 を返します。\n
     *                  data が NULL の場合は何もせず 0 を返します。\n
     *                  size が 0 の場合は何もせず 0 を返します。\n
     *                  stopped 状態では -1 を返します。
     *
     *  @see            trace_logger_write_hex
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_write_hexf(trace_logger_t *handle, trace_level_t level,
                       const void *data, size_t size, const char *format, ...);

    /**
     *******************************************************************************
     *  @brief          トレースプロバイダの識別名と識別番号を設定する。
     *
     *  初期化済みのハンドルを維持したまま、識別名と識別番号を変更します。\n
     *  内部識別名は @p name と @p identifier から以下のルールで生成します。\n
     *  - @p identifier @c == @c 0 : 識別名 = @p name そのもの\n
     *  - @p identifier @c > @c 0 : 識別名 = @c "<name>-<identifier>" (ハイフン区切り)\n
     *  - @p identifier @c < @c 0 : -1 を返す (無効値)\n
     *  Linux 環境では内部で closelog / openlog が呼び出されます。\n
     *  Windows 環境では ETW イベントの "Service" フィールド値のみが変更されます。
     *
     *  @param[in]      handle      trace_logger_create の戻り値。
     *  @param[in]      name        ベース識別名。\n
     *                              NULL の場合は自プロセスの実行ファイル名を使用します
     *                              (trace_logger_create と同じ動作)。
     *  @param[in]      identifier  アプリケーション管理識別番号 (0 以上)。\n
     *                              0 の場合は @p name をそのまま使用します。\n
     *                              正の値の場合は @c "<name>-<identifier>" を識別名とします。\n
     *                              負の値の場合は -1 を返します。
     *  @return         成功 0 / 失敗 (無効な引数・メモリ確保失敗等) -1。
     *
     *  @par            使用例
     *  @code{.c}
        trace_logger_t *logger = trace_logger_create();
        trace_logger_set_name(logger, "worker", 2); // 識別名 = "worker-2"
        trace_logger_start(logger);
        trace_logger_write(logger, TRACE_LEVEL_INFO, "running as worker-2");
        trace_logger_destroy(logger);
     *  @endcode
     *
     *  @pre            ハンドルが stopped 状態であること。
     *                  started 状態では -1 を返します。
     *
     *  @par            スレッド セーフティ
     *  stopped 状態ではスレッドセーフです。\n
     *  内部で排他制御を行います。
     *
     *  @warning        handle が NULL の場合は -1 を返します。\n
     *                  identifier に負の値を指定した場合は -1 を返します。\n
     *                  started 状態では -1 を返します。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_set_name(trace_logger_t *handle, const char *name, int64_t identifier);

    /**
     *******************************************************************************
     *  @brief          OS トレースの現在のスレッショルドレベルを取得する。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @return         現在のスレッショルドレベル。handle が NULL またはロック取得失敗時は
     *                  @c TRACE_LEVEL_NONE を返す。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  started / stopped どちらの状態でも呼び出し可能です。
     *******************************************************************************
     */
    COM_UTIL_EXPORT trace_level_t COM_UTIL_API
        trace_logger_get_os_level(trace_logger_t *handle);

    /**
     *******************************************************************************
     *  @brief          OS トレースのスレッショルドレベルを設定する。
     *
     *  ETW (Windows) または syslog (Linux) に出力するメッセージの
     *  最低重要度レベルを変更します。\n
     *  デフォルト値は @c TRACE_LOGGER_DEFAULT_OS_LEVEL (TRACE_LEVEL_INFO) です。\n
     *  @c TRACE_LEVEL_NONE を指定すると OS トレース出力を完全に抑止します。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @param[in]      level    新しいスレッショルドレベル (trace_level_t)。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが stopped 状態であること。
     *                  started 状態では -1 を返します。
     *
     *  @par            スレッド セーフティ
     *  stopped 状態ではスレッドセーフです。\n
     *  内部で排他制御を行います。
     *
     *  @warning        handle が NULL の場合は -1 を返します。\n
     *                  started 状態では -1 を返します。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_set_os_level(trace_logger_t *handle, trace_level_t level);

    /**
     *******************************************************************************
     *  @brief          ファイルトレースの現在のスレッショルドレベルを取得する。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @return         現在のスレッショルドレベル。handle が NULL またはロック取得失敗時は
     *                  @c TRACE_LEVEL_NONE を返す。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  started / stopped どちらの状態でも呼び出し可能です。
     *******************************************************************************
     */
    COM_UTIL_EXPORT trace_level_t COM_UTIL_API
        trace_logger_get_file_level(trace_logger_t *handle);

    /**
     *******************************************************************************
     *  @brief          ファイルトレースの出力先と設定を変更する。
     *
     *  ファイルトレースを有効化または再構成します。\n
     *  @p path に NULL を指定するとファイルトレースを無効化します
     *  (既存のファイルプロバイダを解放して閉じます)。\n
     *  すでにファイルトレースが有効な場合は既存のプロバイダを解放してから
     *  新しいプロバイダを初期化します。\n
     *  デフォルトのファイルトレースレベルは @c TRACE_LOGGER_DEFAULT_FILE_LEVEL
     *  (TRACE_LEVEL_ERROR) です。\n
     *  ファイルトレースは trace_logger_create の直後は無効 (パス未指定) です。
     *
     *  @param[in]      handle       trace_logger_create の戻り値。
     *  @param[in]      path         出力ファイルパス。NULL の場合はファイルトレースを無効化。
     *  @param[in]      level        ファイルトレースのスレッショルドレベル (trace_level_t)。
     *  @param[in]      max_bytes    1 ファイルあたりの最大バイト数。0 で既定値
     *                               (TRACE_FILE_SINK_DEFAULT_MAX_BYTES = 10 MB) を使用。
     *  @param[in]      generations  保持する旧世代数。0 以下で既定値
     *                               (TRACE_FILE_SINK_DEFAULT_GENERATIONS = 5) を使用。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが stopped 状態であること。
     *                  started 状態では -1 を返します。
     *
     *  @par            スレッド セーフティ
     *  stopped 状態ではスレッドセーフです。\n
     *  内部で排他制御を行います。
     *
     *  @warning        handle が NULL の場合は -1 を返します。\n
     *                  started 状態では -1 を返します。\n
     *                  path に指定したファイルが書き込み不可の場合は -1 を返します。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_set_file_level(trace_logger_t *handle, const char *path,
                             trace_level_t level, size_t max_bytes, int generations);

    /**
     *******************************************************************************
     *  @brief          stderr トレースの現在のスレッショルドレベルを取得する。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @return         現在のスレッショルドレベル。handle が NULL またはロック取得失敗時は
     *                  @c TRACE_LEVEL_NONE を返す。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  started / stopped どちらの状態でも呼び出し可能です。
     *******************************************************************************
     */
    COM_UTIL_EXPORT trace_level_t COM_UTIL_API
        trace_logger_get_stderr_level(trace_logger_t *handle);

    /**
     *******************************************************************************
     *  @brief          stderr トレースのスレッショルドレベルを設定する。
     *
     *  標準エラー出力 (stderr) に出力するメッセージの最低重要度レベルを変更します。\n
     *  出力フォーマットはファイルトレースと同一です。\n
     *  @code
        2026-04-02 12:34:56.789 I メッセージテキスト
     *  @endcode
     *  タイムスタンプは UTC です。\n
     *  デフォルト値は @c TRACE_LOGGER_DEFAULT_STDERR_LEVEL (TRACE_LEVEL_NONE: 無効) です。\n
     *  @c TRACE_LEVEL_NONE を指定すると stderr 出力を完全に抑止します。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *  @param[in]      level    新しいスレッショルドレベル (trace_level_t)。
     *  @return         成功 0 / 失敗 -1。
     *
     *  @pre            ハンドルが stopped 状態であること。
     *                  started 状態では -1 を返します。
     *
     *  @par            スレッド セーフティ
     *  stopped 状態ではスレッドセーフです。\n
     *  内部で排他制御を行います。
     *
     *  @warning        handle が NULL の場合は -1 を返します。\n
     *                  started 状態では -1 を返します。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_logger_set_stderr_level(trace_logger_t *handle, trace_level_t level);
    /**
     *******************************************************************************
     *  @brief          トレースプロバイダを終了し、リソースを解放する。
     *
     *  ハンドルが NULL の場合は何もしません。\n
     *  started 状態のハンドルに対しても呼び出し可能です
     *  (内部で自動的に停止してから解放します)。
     *
     *  @details        通常は本関数で明示解放してください。\n
     *                  shared library アンロード時には、未解放ハンドルは
     *                  trace 内部 registry により自動回収されます。
     *
     *  @param[in]      handle   trace_logger_create の戻り値。
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  本関数を呼び出した時点で進行中の trace_logger_write 等が完了するまで待機してから
     *  リソースを解放します。\n
     *  本関数呼び出し後、handle は無効となります。
     *******************************************************************************
     */
    COM_UTIL_EXPORT void COM_UTIL_API
        trace_logger_destroy(trace_logger_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* ===== ソース位置自動付与マクロ ===== */

/**
 *  @def            _TRACE_LOGGER_BASENAME(f)
 *  @brief          ファイルパスからベースネームを取り出す内部ヘルパーマクロ。
 *                  Windows (\\) と Linux (/) の両パス区切り文字に対応します。
 *  @internal
 */
#define _TRACE_LOGGER_BASENAME(f) \
    (strrchr((f), '/') ? strrchr((f), '/') + 1 : \
     (strrchr((f), '\\') ? strrchr((f), '\\') + 1 : (f)))

/**
 *  @def            TRACE_LOGGER_WRITE(handle, level, message)
 *  @brief          ソースファイル名と行番号を自動付与する trace_logger_write ラッパーマクロ。
 *
 *  呼び出しサイトの @c __FILE__ (バスネーム) と @c __LINE__ を
 *  @c "[filename.c:42] " 形式でメッセージ先頭に付加して trace_logger_writef を呼び出します。
 *
 *  @param[in]      handle   trace_logger_create の戻り値。
 *  @param[in]      level    トレースレベル (trace_level_t)。
 *  @param[in]      message  null 終端 UTF-8 文字列。NULL 不可。
 *  @return         成功 0 / 失敗 -1 (@see trace_logger_writef)。
 *
 *  @par            出力例
 *  @code{.c}
    TRACE_LOGGER_WRITE(logger, TRACE_LEVEL_INFO, "started");
    // → "[add.c:42] started"
 *  @endcode
 */
#define TRACE_LOGGER_WRITE(handle, level, message) \
    trace_logger_writef((handle), (level), "[%s:%d] %s", \
                 _TRACE_LOGGER_BASENAME(__FILE__), __LINE__, (message))

/**
 *  @def            TRACE_LOGGER_WRITEF(handle, level, fmt, ...)
 *  @brief          ソースファイル名と行番号を自動付与する trace_logger_writef ラッパーマクロ。
 *
 *  @c "[filename.c:42] " プレフィックスと @p fmt を文字列リテラル結合して
 *  trace_logger_writef を呼び出します。
 *
 *  @param[in]      handle   trace_logger_create の戻り値。
 *  @param[in]      level    トレースレベル (trace_level_t)。
 *  @param[in]      fmt      printf 形式のフォーマット文字列 (文字列リテラル限定)。
 *  @param[in]      ...      フォーマット文字列に対応する可変長引数。
 *  @return         成功 0 / 失敗 -1 (@see trace_logger_writef)。
 *
 *  @par            出力例
 *  @code{.c}
    TRACE_LOGGER_WRITEF(logger, TRACE_LEVEL_INFO, "user=%s count=%d", username, count);
    // → "[calc.c:88] user=alice count=3"
 *  @endcode
 */
#define TRACE_LOGGER_WRITEF(handle, level, fmt, ...) \
    trace_logger_writef((handle), (level), "[%s:%d] " fmt, \
                 _TRACE_LOGGER_BASENAME(__FILE__), __LINE__, ##__VA_ARGS__)

/**
 *  @def            TRACE_LOGGER_WRITE_HEX(handle, level, data, size, message)
 *  @brief          ソースファイル名と行番号を自動付与する trace_logger_write_hex ラッパーマクロ。
 *
 *  @c "[filename.c:42]" (または @c "[filename.c:42] <message>") をラベルとして
 *  trace_logger_write_hexf を呼び出します。
 *  @p message が NULL の場合も安全に動作します。
 *
 *  @param[in]      handle   trace_logger_create の戻り値。
 *  @param[in]      level    トレースレベル (trace_level_t)。
 *  @param[in]      data     バイナリデータへのポインタ。
 *  @param[in]      size     バイナリデータのバイト数。
 *  @param[in]      message  HEX データの手前に付与するラベル文字列。NULL 可。
 *  @return         成功 0 / 失敗 -1 (@see trace_logger_write_hexf)。
 *
 *  @par            出力例
 *  @code{.c}
    TRACE_LOGGER_WRITE_HEX(logger, TRACE_LEVEL_INFO, buf, len, "Received data");
    // → "[recv.c:77] Received data: 48 65 6C 6C 6F"
    TRACE_LOGGER_WRITE_HEX(logger, TRACE_LEVEL_INFO, buf, len, NULL);
    // → "[recv.c:77]: 48 65 6C 6C 6F"
 *  @endcode
 */
#define TRACE_LOGGER_WRITE_HEX(handle, level, data, size, message) \
    trace_logger_write_hexf((handle), (level), (data), (size), "[%s:%d]%s%s", \
                     _TRACE_LOGGER_BASENAME(__FILE__), __LINE__, \
                     (message) ? " " : "", \
                     (message) ? (message) : "")

/**
 *  @def            TRACE_LOGGER_WRITE_HEXF(handle, level, data, size, fmt, ...)
 *  @brief          ソースファイル名と行番号を自動付与する trace_logger_write_hexf ラッパーマクロ。
 *
 *  @c "[filename.c:42] " プレフィックスと @p fmt を文字列リテラル結合して
 *  trace_logger_write_hexf を呼び出します。
 *
 *  @param[in]      handle   trace_logger_create の戻り値。
 *  @param[in]      level    トレースレベル (trace_level_t)。
 *  @param[in]      data     バイナリデータへのポインタ。
 *  @param[in]      size     バイナリデータのバイト数。
 *  @param[in]      fmt      printf 形式のフォーマット文字列 (文字列リテラル限定)。
 *  @param[in]      ...      フォーマット文字列に対応する可変長引数。
 *  @return         成功 0 / 失敗 -1 (@see trace_logger_write_hexf)。
 *
 *  @par            出力例
 *  @code{.c}
    TRACE_LOGGER_WRITE_HEXF(logger, TRACE_LEVEL_INFO, data, len, "packet[%d]", index);
    // → "[recv.c:99] packet[3]: 48 65 6C 6C 6F"
 *  @endcode
 */
#define TRACE_LOGGER_WRITE_HEXF(handle, level, data, size, fmt, ...) \
    trace_logger_write_hexf((handle), (level), (data), (size), "[%s:%d] " fmt, \
                     _TRACE_LOGGER_BASENAME(__FILE__), __LINE__, ##__VA_ARGS__)

#endif /* TRACE_LOGGER_H */
