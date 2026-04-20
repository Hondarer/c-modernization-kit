#ifndef TRACE_ETW_H
#define TRACE_ETW_H

#include <com_util/base/platform.h>
#include <com_util_export.h>

/**
 *  @file           trace_etw.h
 *  @brief          ETW (Event Tracing for Windows) ヘルパーライブラリ。
 *  @details        TraceLogging ベースの ETW プロバイダを簡易に操作するための
 *                  ヘルパー関数群を提供します。\n
 *                  Windows 専用ライブラリです。呼び出し元は @c \#if defined(PLATFORM_WINDOWS) の
 *                  中でのみ使用してください。
 */

#if defined(PLATFORM_WINDOWS)

#include <com_util/base/windows_sdk.h>

/* ===== プロバイダ参照型 ===== */

/**
 *  @typedef        trace_etw_provider_ref_t
 *  @brief          プロバイダ参照型。
 *  @details        TraceLoggingHProvider (TraceLoggingProvider.h が定義する型) と同等です。
 */
struct _tlgProvider_t;
typedef struct _tlgProvider_t const *trace_etw_provider_ref_t;

/* ===== プロバイダ定義マクロ ===== */

/**
 *  @def            TRACE_ETW_DEFINE_PROVIDER(var, name, guid)
 *  @brief          ETW プロバイダを定義するマクロ。
 *  @details        呼び出し元の .c ファイルのファイルスコープに 1 回だけ記述します。\n
 *                  TRACELOGGING_DEFINE_PROVIDER(var, name, guid) に展開します。\n
 *                  呼び出し元は本マクロ使用前に windows_sdk.h と TraceLoggingProvider.h を
 *                  インクルードする必要があります。
 *
 *  @param          var   プロバイダ変数名 (trace_etw_provider_ref_t 型)
 *  @param          name  プロバイダ名 (文字列リテラル)
 *  @param          guid  GUID (TraceLogging 形式の括弧付き定数タプル)
 *
 *  @par            使用例
 *  @code{.c}
    #include <com_util/base/windows_sdk.h>
    #include <TraceLoggingProvider.h>
    #include <com_util/trace/trace_etw.h>

    TRACE_ETW_DEFINE_PROVIDER(
        s_my_provider,
        "MyProvider",
        (0x12345678, 0x1234, 0x1234, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89));
 *  @endcode
 */
#define TRACE_ETW_DEFINE_PROVIDER(var, name, guid) \
    TRACELOGGING_DEFINE_PROVIDER(var, name, guid)

/* ===== 不透明ハンドル型 ===== */

/** ETW プロバイダハンドル (不透明型)。 */
typedef struct trace_etw_provider trace_etw_provider_t;

/* ===== API 関数 ===== */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          ETW プロバイダを登録する。
     *  @details        呼び出し元が TRACE_ETW_DEFINE_PROVIDER で定義したプロバイダ変数を渡します。
     *                  これにより、呼び出し元ごとに異なる GUID/プロバイダ名を使用できます。
     *
     *  @param[in]      provider_ref  TRACE_ETW_DEFINE_PROVIDER で定義した変数。
     *  @return         成功時: ハンドル。失敗時: NULL。
     *
     *  @par            使用例
     *  @code{.c}
        trace_etw_provider_t *h = trace_etw_provider_create(s_my_provider);
     *  @endcode
     */
    COM_UTIL_EXPORT trace_etw_provider_t *COM_UTIL_API
        trace_etw_provider_create(trace_etw_provider_ref_t provider_ref);

    /**
     *  @brief          ETW プロバイダへ UTF-8 メッセージを書き込む。
     *
     *  @param[in]      handle   trace_etw_provider_create の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    イベントレベル。
     *                           1=CRITICAL / 2=ERROR / 3=WARNING / 4=INFO / 5=VERBOSE
     *  @param[in]      service  サービス名 (ETW "Trace" イベントの "Service" フィールド)。
     *                           NULL の場合は Service フィールドなしで書き込みます。
     *  @param[in]      message  null 終端 UTF-8 文字列。NULL の場合は何もせず 0 を返します。
     *
     *  @par    長さ制限
     *  ETW の単一イベント最大サイズは **65,535 バイト (64 KB)** です
     *  (イベントヘッダー・メタデータを含む)。ヘッダーやフィールド名等の
     *  オーバーヘッドを差し引くと、message に使用できる実質的な上限は
     *  約 **65,000 バイト**程度です。この制限はバイト長に対するものであり、
     *  UTF-8 マルチバイト文字を含む場合は文字数ではなくバイト数で判定されます。
     *
     *  上限を超えるイベントは EventWrite が
     *  @c ERROR_ARITHMETIC_OVERFLOW を返し、**何も記録されずに
     *  無視されます** (クラッシュはしません)。本関数側での切り詰めは
     *  行わないため、呼び出し元で長大なメッセージを分割するか、
     *  上限内に収まることを保証してください。
     *
     *  @return         成功 0 / 失敗 -1。
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_etw_provider_write(trace_etw_provider_t *handle, int level,
                           const char *service, const char *message);

    /**
     *  @brief          ETW プロバイダの登録を解除する。
     *  @details        ハンドルが NULL の場合は何もしません。
     *
     *  @param[in]      handle   trace_etw_provider_create の戻り値。
     */
    COM_UTIL_EXPORT void COM_UTIL_API
        trace_etw_provider_destroy(trace_etw_provider_t *handle);

    /* ===== セッション (Consumer) API ===== */

    /** @name ETW セッションステータスコード */
    /** @{ */

    /** 成功。 */
    #define TRACE_ETW_SESSION_OK          0
    /** パラメータエラー (NULL または不正な GUID)。 */
    #define TRACE_ETW_SESSION_ERR_PARAM  -1
    /** 権限不足 (Administrators または Performance Log Users が必要)。 */
    #define TRACE_ETW_SESSION_ERR_ACCESS -2
    /** その他のシステムエラー。 */
    #define TRACE_ETW_SESSION_ERR_SYSTEM -3

    /** @} */

    /**
     *  @typedef        trace_etw_event_callback_t
     *  @brief          ETW イベント受信コールバック型。
     *  @details        セッションがイベントを受信するたびに呼び出されます。\n
     *                  コールバックは trace_etw_session_start 内部のワーカースレッドから呼ばれます。\n
     *                  呼び出し元が共有データにアクセスする場合はスレッド安全に保護してください。
     *
     *  @param[in]      level    イベントレベル (1-5)。
     *  @param[in]      message  null 終端 UTF-8 文字列。NULL の場合があります。
     *  @param[in]      context  trace_etw_session_start に渡したユーザーデータ。
     */
    typedef void (*trace_etw_event_callback_t)(int level, const char *message, void *context);

    /** ETW セッションハンドル (不透明型)。 */
    typedef struct trace_etw_session trace_etw_session_t;

    /**
     *  @brief          ETW セッション開始に必要な権限があるか検査する。
     *  @details        内部でダミーセッションの開始・停止を行い、権限の有無を判定します。\n
     *                  アプリケーションの起動時やテストの SetUp で呼び出すことを想定しています。
     *
     *  @return         @c TRACE_ETW_SESSION_OK     権限あり。\n
     *                  @c TRACE_ETW_SESSION_ERR_ACCESS 権限不足。\n
     *                  @c TRACE_ETW_SESSION_ERR_SYSTEM その他のシステムエラー。
     *
     *  @par            使用例
     *  @code{.c}
        int status = trace_etw_session_check_access();
        if (status == TRACE_ETW_SESSION_ERR_ACCESS) {
            fprintf(stderr, "Run: net localgroup \"Performance Log Users\" %%USERNAME%% /add\n");
        }
     *  @endcode
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        trace_etw_session_check_access(void);

    /**
     *  @brief          リアルタイム ETW セッションを開始し、指定プロバイダを購読する。
     *  @details        内部でワーカースレッドを起動し ProcessTrace をブロッキング実行します。\n
     *                  セッション開始には Administrators または Performance Log Users グループの
     *                  メンバーシップが必要です。権限不足の場合、@p out_status に
     *                  @c TRACE_ETW_SESSION_ERR_ACCESS が設定されます。
     *
     *  @param[in]      session_name       セッション名 (システム全体で一意にすること)。
     *  @param[in]      provider_guid_str  GUID 文字列 "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"。
     *  @param[in]      callback           イベント受信時に呼ばれるコールバック。
     *  @param[in]      context            コールバックに渡すユーザーデータ。
     *  @param[out]     out_status         エラーコード出力先 (NULL 可)。\n
     *                                     @c TRACE_ETW_SESSION_OK / @c TRACE_ETW_SESSION_ERR_PARAM /
     *                                     @c TRACE_ETW_SESSION_ERR_ACCESS / @c TRACE_ETW_SESSION_ERR_SYSTEM。
     *  @return         成功: セッションハンドル / 失敗: NULL。
     *
     *  @par            使用例
     *  @code{.c}
        int status;
        trace_etw_session_t *s = trace_etw_session_start(
            "MySession",
            "12345678-1234-1234-abcd-ef0123456789",
            my_callback, my_context, &status);
        if (s == NULL && status == TRACE_ETW_SESSION_ERR_ACCESS) {
            fprintf(stderr, "Run: net localgroup \"Performance Log Users\" %%USERNAME%% /add\n");
        }
     *  @endcode
     */
    COM_UTIL_EXPORT trace_etw_session_t *COM_UTIL_API
        trace_etw_session_start(const char *session_name,
                          const char *provider_guid_str,
                          trace_etw_event_callback_t callback,
                          void *context,
                          int *out_status);

    /**
     *  @brief          ETW セッションを停止し、リソースを解放する。
     *  @details        セッションを停止しバッファをフラッシュした後、
     *                  ワーカースレッドを join してリソースを解放します。\n
     *                  ハンドルが NULL の場合は何もしません。
     *
     *  @param[in]      session  trace_etw_session_start の戻り値。
     */
    COM_UTIL_EXPORT void COM_UTIL_API
        trace_etw_session_stop(trace_etw_session_t *session);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PLATFORM_WINDOWS */

#endif /* TRACE_ETW_H */
