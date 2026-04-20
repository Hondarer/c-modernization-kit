#ifndef TRACE_SYSLOG_H
#define TRACE_SYSLOG_H

#include <com_util/base/platform.h>
#include <com_util_export.h>

/**
 *  @file           trace_syslog.h
 *  @brief          syslog ヘルパーライブラリ。
 *  @details        Linux syslog (RFC5424 系実装) のラッパー関数群を提供します。\n
 *                  Linux 専用ライブラリです。呼び出し元は @c \#if defined(PLATFORM_LINUX) の
 *                  中でのみ使用してください。
 */

#if defined(PLATFORM_LINUX)

/* ===== 不透明ハンドル型 ===== */

/** syslog プロバイダハンドル (不透明型)。 */
typedef struct trace_syslog_sink trace_syslog_sink_t;

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
     *  @code{.c}
        #include <syslog.h>
        #include <com_util/trace/trace_syslog.h>

        trace_syslog_sink_t *h = trace_syslog_sink_create("myapp", LOG_USER);
     *  @endcode
     */
    COM_UTIL_EXPORT trace_syslog_sink_t *COM_UTIL_API
        trace_syslog_sink_create(const char *ident, int facility);

    /**
     *  @brief          syslog へ UTF-8 メッセージを書き込む。
     *
     *  @param[in]      handle   trace_syslog_sink_create の戻り値。
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
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_syslog_sink_write(trace_syslog_sink_t *handle, int level, const char *message);

    /**
     *  @brief          syslog プロバイダの識別子を変更する。
     *  @details        closelog を呼び出した後、新しい識別子で openlog を
     *                  再度呼び出します。ハンドル自体は再利用されます。
     *
     *  @param[in]      handle     trace_syslog_sink_create の戻り値。
     *                             NULL の場合は何もせず -1 を返します。
     *  @param[in]      new_ident  新しい識別子文字列。
     *                             NULL の場合は何もせず -1 を返します。
     *  @return         成功 0 / 失敗 -1。
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_syslog_sink_rename(trace_syslog_sink_t *handle, const char *new_ident);

    /**
     *  @brief          syslog プロバイダを終了する。
     *  @details        closelog を呼び出し、リソースを解放します。\n
     *                  ハンドルが NULL の場合は何もしません。
     *
     *  @param[in]      handle   trace_syslog_sink_create の戻り値。
     */
    COM_UTIL_EXPORT void COM_UTIL_API
        trace_syslog_sink_destroy(trace_syslog_sink_t *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PLATFORM_LINUX */

#endif /* TRACE_SYSLOG_H */
