#ifndef TRACE_SYSLOG_UTIL_H
#define TRACE_SYSLOG_UTIL_H

/**
 *  @file           trace-syslog-util.h
 *  @brief          syslog ヘルパーライブラリ。
 *  @details        Linux syslog (RFC5424 系実装) のラッパー関数群を提供します。\n
 *                  Linux 専用ライブラリです。呼び出し元は @c \#ifndef @c _WIN32 の
 *                  中でのみ使用してください。
 */

#ifndef _WIN32

/* ===== エクスポート / 呼び出し規約マクロ ===== */

#ifdef DOXYGEN

/**
 *  @def            TRACE_SYSLOG_UTIL_EXPORT
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *  @details        Linux 環境では常に空に展開されます。
 */
#define TRACE_SYSLOG_UTIL_EXPORT

/**
 *  @def            TRACE_SYSLOG_UTIL_API
 *  @brief          呼び出し規約マクロ。
 *  @details        Linux 環境では常に空に展開されます。
 */
#define TRACE_SYSLOG_UTIL_API

#else /* !DOXYGEN */

#define TRACE_SYSLOG_UTIL_EXPORT
#define TRACE_SYSLOG_UTIL_API

#endif /* DOXYGEN */

/* ===== 不透明ハンドル型 ===== */

/** syslog プロバイダハンドル (不透明型)。 */
typedef struct syslog_provider syslog_provider_t;

/* ===== API 関数 ===== */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          syslog プロバイダを初期化する。
     *  @details        openlog を呼び出し、指定された識別子と facility で
     *                  syslog 接続を開きます。
     *
     *  @param[in]      ident     syslog メッセージに付与される識別子文字列。
     *                            NULL の場合は何もせず NULL を返します。
     *  @param[in]      facility  syslog facility 値
     *                            (例: LOG_USER, LOG_LOCAL0〜LOG_LOCAL7)。
     *  @return         成功時: ハンドル。失敗時: NULL。
     *
     *  @par            使用例
     *  @code
     *  #include <syslog.h>
     *  #include <trace-syslog-util.h>
     *
     *  syslog_provider_t *h = syslog_provider_init("myapp", LOG_USER);
     *  @endcode
     */
    TRACE_SYSLOG_UTIL_EXPORT syslog_provider_t *TRACE_SYSLOG_UTIL_API
        syslog_provider_init(const char *ident, int facility);

    /**
     *  @brief          syslog へ UTF-8 メッセージを書き込む。
     *
     *  @param[in]      handle   syslog_provider_init の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    syslog severity 値。
     *                           LOG_CRIT / LOG_ERR / LOG_WARNING / LOG_INFO / LOG_DEBUG
     *  @param[in]      message  null 終端 UTF-8 文字列。NULL の場合は何もせず 0 を返します。
     *
     *                           @par    長さ制限
     *                           RFC 5424 ではメッセージ全体 (ヘッダー + 構造化データ + MSG) の
     *                           推奨最大長は **2,048 バイト**です。多くの syslog 実装
     *                           (rsyslog, syslog-ng 等) はこれより大きなメッセージも受け付けますが、
     *                           中継経路上のリレーやコレクタが切り詰める可能性があります。\n
     *                           本関数は内部で @c syslog() を呼び出しており、glibc 実装では
     *                           メッセージ長に明示的な上限はありません (動的にバッファを確保します)。
     *                           ただし、ネットワーク転送時は UDP の場合 **65,535 バイト**
     *                           (IP ペイロード上限) を超えるメッセージは送信できません。\n
     *                           相互運用性を重視する場合は **1,024 バイト**以下
     *                           (RFC 3164 の従来上限) を目安にしてください。本関数側での切り詰めは
     *                           行わないため、呼び出し元で長大なメッセージを分割するか、
     *                           上限内に収まることを保証してください。
     *
     *  @return         成功 0 / 失敗 -1。
     */
    TRACE_SYSLOG_UTIL_EXPORT int TRACE_SYSLOG_UTIL_API
        syslog_provider_write(syslog_provider_t *handle, int level, const char *message);

    /**
     *  @brief          syslog プロバイダを終了する。
     *  @details        closelog を呼び出し、リソースを解放します。\n
     *                  ハンドルが NULL の場合は何もしません。
     *
     *  @param[in]      handle   syslog_provider_init の戻り値。
     */
    TRACE_SYSLOG_UTIL_EXPORT void TRACE_SYSLOG_UTIL_API
        syslog_provider_dispose(syslog_provider_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_WIN32 */

#endif /* TRACE_SYSLOG_UTIL_H */
