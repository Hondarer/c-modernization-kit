#ifndef LOG_UTIL_H
#define LOG_UTIL_H

/**
 *  @file           log-util.h
 *  @brief          クロスプラットフォーム ロギング API。
 *  @details        Windows (ETW) と Linux (syslog) の差異を抽象化し、
 *                  共通のログレベルとインターフェースを提供します。\n
 *                  内部で @c etw-util.h (Windows) または @c syslog-util.h (Linux) を
 *                  使用します。
 *
 *  @par            アーキテクチャ
 *  @code
 *  Application
 *        |
 *        v
 *  log-util.h (共通 API)
 *        |
 *  +-----+-----+
 *  |           |
 *  ETW        syslog
 *  (Windows)  (Linux)
 *  @endcode
 *
 *  @par            使用例 (Linux)
 *  @code
 *  #include <log-util.h>
 *
 *  log_provider_t *logger = log_init_linux("myapp");
 *  log_write(logger, LOG_INFO, "application started");
 *  log_dispose(logger);
 *  @endcode
 *
 *  @par            使用例 (Windows)
 *  @code
 *  #include <windows.h>
 *  #include <TraceLoggingProvider.h>
 *  #include <log-util.h>
 *
 *  ETW_UTIL_DEFINE_PROVIDER(
 *      s_provider, "MyApp",
 *      (0x12345678, 0x1234, 0x1234, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89));
 *
 *  log_provider_t *logger = log_init_windows(s_provider);
 *  log_write(logger, LOG_INFO, "application started");
 *  log_dispose(logger);
 *  @endcode
 */

/* 内部で使用するプラットフォーム固有ヘッダー */
#ifdef _WIN32
#include <etw-util.h>
#else /* !_WIN32 */
#include <syslog-util.h>
#endif /* _WIN32 */

/* ===== DLL エクスポート / インポート制御マクロ ===== */

#ifdef DOXYGEN

/**
 *  @def            LOG_UTIL_EXPORT
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                   | 値                       |
 *  | ------------------------------------------------------ | ------------------------ |
 *  | Linux (非 Windows)                                     | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                    | (空)                     |
 *  | Windows / `LOG_UTIL_STATIC` 定義時 (静的リンク)        | (空)                     |
 *  | Windows / `LOG_UTIL_EXPORTS` 定義時 (DLL ビルド)       | `__declspec(dllexport)`  |
 *  | Windows / `LOG_UTIL_EXPORTS` 未定義時 (DLL 利用側)     | `__declspec(dllimport)`  |
 */
#define LOG_UTIL_EXPORT

/**
 *  @def            LOG_UTIL_API
 *  @brief          呼び出し規約マクロ。
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#define LOG_UTIL_API

#else /* !DOXYGEN */

#ifndef _WIN32
    #define LOG_UTIL_EXPORT
    #define LOG_UTIL_API
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef LOG_UTIL_STATIC
            #ifdef LOG_UTIL_EXPORTS
                #define LOG_UTIL_EXPORT __declspec(dllexport)
            #else /* !LOG_UTIL_EXPORTS */
                #define LOG_UTIL_EXPORT __declspec(dllimport)
            #endif /* LOG_UTIL_EXPORTS */
        #else      /* LOG_UTIL_STATIC */
            #define LOG_UTIL_EXPORT
        #endif /* LOG_UTIL_STATIC */
    #else      /* __INTELLISENSE__ */
        #define LOG_UTIL_EXPORT
    #endif /* __INTELLISENSE__ */
    #ifndef LOG_UTIL_API
        #define LOG_UTIL_API __stdcall
    #endif /* LOG_UTIL_API */
#endif     /* _WIN32 */

#endif /* DOXYGEN */

/* ===== メッセージ長制限 ===== */

/**
 *  @def            LOG_MESSAGE_MAX_BYTES
 *  @brief          log_write が受け付けるメッセージの最大バイト数 (null 終端含む)。
 *  @details        ETW (約 65,000 バイト) と syslog (RFC 3164: 1,024 バイト) の
 *                  推奨上限のうち小さい方を採用し、クロスプラットフォームでの
 *                  安全な転送を保証します。\n
 *                  本文の最大長は @c LOG_MESSAGE_MAX_BYTES @c - @c 1 (= 1,023) バイトです。
 */
#define LOG_MESSAGE_MAX_BYTES 1024

/**
 *  @def            LOG_HEX_MAX_DATA_BYTES
 *  @brief          log_hex_write がラベルなしで HEX 出力できるバイナリデータの最大バイト数。
 *  @details        1 バイトあたり 3 文字 (HH + スペース) を消費し、最終バイトは 2 文字です。\n
 *                  ラベル (@p message) を指定した場合はラベル長 + セパレータ (": ") 分だけ
 *                  出力可能なバイナリデータ量が減少します。\n
 *                  データがこの上限を超える場合は切り詰めが行われ、
 *                  末尾に @c "..." が付与されます。
 */
#define LOG_HEX_MAX_DATA_BYTES 341

/* ===== 共通ログレベル ===== */

/**
 *  @enum           log_level
 *  @brief          アプリケーション共通ログレベル。
 *  @details        OS 非依存のログレベルを定義します。重大度は上から下へ低下します。\n
 *                  内部で ETW Level (1-5) および syslog severity へマッピングされます。
 *
 *  | log_level    | ETW Level        | syslog severity |
 *  | ------------ | ---------------- | --------------- |
 *  | LOG_CRITICAL | Critical (1)     | LOG_CRIT (2)    |
 *  | LOG_ERROR    | Error (2)        | LOG_ERR (3)     |
 *  | LOG_WARNING  | Warning (3)      | LOG_WARNING (4) |
 *  | LOG_INFO     | Informational (4)| LOG_INFO (6)    |
 *  | LOG_VERBOSE  | Verbose (5)      | LOG_DEBUG (7)   |
 */
enum log_level
{
    LOG_LV_CRITICAL = 0, /**< 致命的エラー。 */
    LOG_LV_ERROR    = 1, /**< エラー。 */
    LOG_LV_WARNING  = 2, /**< 警告。 */
    LOG_LV_INFO     = 3, /**< 情報。 */
    LOG_LV_VERBOSE  = 4  /**< 詳細 (デバッグ)。 */
};

/* ===== 不透明ハンドル型 ===== */

/** ログプロバイダハンドル (不透明型)。 */
typedef struct log_provider log_provider_t;

/* ===== API 関数 ===== */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _WIN32
    /**
     *  @brief          ログプロバイダを初期化する (Linux 用)。
     *  @details        syslog を @p name で初期化します (facility は LOG_USER)。\n
     *                  本関数は Linux 環境でのみ使用可能です。
     *
     *  @param[in]      name  アプリケーション名 (syslog の ident に使用)。
     *                        NULL の場合は自プロセスの実行ファイル名を使用します
     *                        (例: @c /usr/bin/myapp → @c "myapp")。\n
     *                        プロセス名の取得に失敗した場合は @c "unknown" を使用します。
     *  @return         成功時: ハンドル。失敗時: NULL。
     *
     *  @see            log_init_windows
     */
    LOG_UTIL_EXPORT log_provider_t *LOG_UTIL_API
        log_init_linux(const char *name);
#else /* _WIN32 */
    /**
     *  @brief          ログプロバイダを ETW で初期化する (Windows 専用)。
     *  @details        呼び出し元が ETW_UTIL_DEFINE_PROVIDER で定義した
     *                  プロバイダ変数を渡します。\n
     *                  本関数は Windows 環境でのみ使用可能です。
     *
     *  @param[in]      provider_ref  ETW_UTIL_DEFINE_PROVIDER で定義した変数。
     *  @return         成功時: ハンドル。失敗時: NULL。
     *
     *  @par            使用例
     *  @code
     *  #include <windows.h>
     *  #include <TraceLoggingProvider.h>
     *  #include <log-util.h>
     *
     *  ETW_UTIL_DEFINE_PROVIDER(
     *      s_provider, "MyApp",
     *      (0x12345678, 0x1234, 0x1234, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89));
     *
     *  log_provider_t *logger = log_init_windows(s_provider);
     *  @endcode
     *
     *  @see            log_init_linux
     */
    LOG_UTIL_EXPORT log_provider_t *LOG_UTIL_API
        log_init_windows(etw_provider_ref_t provider_ref);
#endif /* _WIN32 */

    /**
     *  @brief          ログメッセージを書き込む。
     *  @details        指定されたログレベルでメッセージを書き込みます。\n
     *                  内部で enum log_level を ETW Level または syslog severity に
     *                  マッピングして書き込みます。
     *
     *  @param[in]      handle   log_init_linux または log_init_windows の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    ログレベル (enum log_level)。
     *  @param[in]      message  null 終端 UTF-8 文字列。NULL の場合は何もせず 0 を返します。
     *
     *  メッセージが @c LOG_MESSAGE_MAX_BYTES (1,024) バイト
     *  (null 終端含む) を超える場合、本関数は内部で
     *  1,023 バイト以内に切り詰めて下層 API に渡します。\n
     *  切り詰め時は UTF-8 マルチバイト文字の途中で切断しないよう
     *  バイト境界を調整します。\n
     *  この上限は ETW (約 65,000 バイト) と syslog (RFC 3164:
     *  1,024 バイト) の推奨値のうち小さい方に合わせています。
     *
     *  @return         成功 0 / 失敗 -1。
     */
    LOG_UTIL_EXPORT int LOG_UTIL_API
        log_write(log_provider_t *handle, enum log_level level, const char *message);

    /**
     *  @brief          printf 形式でログメッセージを書き込む。
     *  @details        フォーマット文字列と可変長引数で構成されたメッセージを書き込みます。\n
     *                  内部で @c vsnprintf を使用し、@c LOG_MESSAGE_MAX_BYTES (1,024) バイトの
     *                  バッファにフォーマットします。バッファを超える部分は切り詰められます。
     *
     *  @param[in]      handle   log_init_linux または log_init_windows の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    ログレベル (enum log_level)。
     *  @param[in]      format   printf 形式のフォーマット文字列。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      ...      フォーマット文字列に対応する可変長引数。
     *
     *  @par            使用例
     *  @code
     *  log_writef(logger, LOG_LV_INFO, "user=%s count=%d", username, count);
     *  @endcode
     *
     *  @return         成功 0 / 失敗 -1。
     */
    LOG_UTIL_EXPORT int LOG_UTIL_API
        log_writef(log_provider_t *handle, enum log_level level, const char *format, ...);

    /**
     *  @brief          バイナリデータを HEX テキスト形式でログに書き込む。
     *  @details        バイナリデータを大文字スペース区切りの HEX 文字列に変換して
     *                  ログに書き込みます。@p message が非 NULL の場合はラベルとして
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
     *  @param[in]      handle   log_init_linux または log_init_windows の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    ログレベル (enum log_level)。
     *  @param[in]      data     バイナリデータへのポインタ。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      size     バイナリデータのバイト数。
     *                           0 の場合は何もせず 0 を返します。
     *  @param[in]      message  HEX データの手前に付与するラベル文字列。
     *                           NULL の場合はラベルなしで HEX のみ出力します。
     *
     *  ラベルなしの場合、最大 @c LOG_HEX_MAX_DATA_BYTES (341) バイトの
     *  バイナリデータを出力できます。ラベル指定時はラベル長 + セパレータ
     *  (": ", 2 バイト) 分だけ出力可能なデータ量が減少します。\n
     *  データが収まらない場合は切り詰め、末尾に @c "..." を付与します。
     *
     *  @return         成功 0 / 失敗 -1。
     */
    LOG_UTIL_EXPORT int LOG_UTIL_API
        log_hex_write(log_provider_t *handle, enum log_level level,
                      const void *data, size_t size, const char *message);

    /**
     *  @brief          バイナリデータを HEX テキスト形式でログに書き込む (printf 形式ラベル)。
     *  @details        log_hex_write と同等ですが、ラベル文字列を printf 形式の
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
     *  @param[in]      handle   log_init_linux または log_init_windows の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    ログレベル (enum log_level)。
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
     *  log_hex_writef(logger, LOG_LV_INFO, data, len, "packet[%d]", index);
     *  @endcode
     *
     *  @return         成功 0 / 失敗 -1。
     *
     *  @see            log_hex_write
     */
    LOG_UTIL_EXPORT int LOG_UTIL_API
        log_hex_writef(log_provider_t *handle, enum log_level level,
                       const void *data, size_t size, const char *format, ...);

    /**
     *  @brief          ログプロバイダを終了し、リソースを解放する。
     *  @details        ハンドルが NULL の場合は何もしません。
     *
     *  @param[in]      handle   log_init_linux または log_init_windows の戻り値。
     */
    LOG_UTIL_EXPORT void LOG_UTIL_API
        log_dispose(log_provider_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOG_UTIL_H */
