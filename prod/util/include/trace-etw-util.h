#ifndef TRACE_ETW_UTIL_H
#define TRACE_ETW_UTIL_H

/**
 *  @file           trace-etw-util.h
 *  @brief          ETW (Event Tracing for Windows) ヘルパーライブラリ。
 *  @details        TraceLogging ベースの ETW プロバイダを簡易に操作するための
 *                  ヘルパー関数群を提供します。\n
 *                  Windows 専用ライブラリです。呼び出し元は @c \#ifdef @c _WIN32 の
 *                  中でのみ使用してください。
 */

#ifdef _WIN32

/* ===== DLL エクスポート / インポート制御マクロ ===== */

#ifdef DOXYGEN

/**
 *  @def            TRACE_ETW_UTIL_EXPORT
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                   | 値                       |
 *  | ------------------------------------------------------ | ------------------------ |
 *  | `__INTELLISENSE__` 定義時                              | (空)                     |
 *  | `TRACE_ETW_UTIL_STATIC` 定義時 (静的リンク)                  | (空)                     |
 *  | `TRACE_ETW_UTIL_EXPORTS` 定義時 (DLL ビルド)                 | `__declspec(dllexport)`  |
 *  | `TRACE_ETW_UTIL_EXPORTS` 未定義時 (DLL 利用側)               | `__declspec(dllimport)`  |
 */
#define TRACE_ETW_UTIL_EXPORT

/**
 *  @def            TRACE_ETW_UTIL_API
 *  @brief          呼び出し規約マクロ。
 *  @details        `__stdcall` 呼び出し規約を指定します。\n
 *                  既に定義済みの場合は再定義されません。
 */
#define TRACE_ETW_UTIL_API

#else /* !DOXYGEN */

#ifndef __INTELLISENSE__
    #ifndef TRACE_ETW_UTIL_STATIC
        #ifdef TRACE_ETW_UTIL_EXPORTS
            #define TRACE_ETW_UTIL_EXPORT __declspec(dllexport)
        #else /* !TRACE_ETW_UTIL_EXPORTS */
            #define TRACE_ETW_UTIL_EXPORT __declspec(dllimport)
        #endif /* TRACE_ETW_UTIL_EXPORTS */
    #else      /* TRACE_ETW_UTIL_STATIC */
        #define TRACE_ETW_UTIL_EXPORT
    #endif /* TRACE_ETW_UTIL_STATIC */
#else      /* __INTELLISENSE__ */
    #define TRACE_ETW_UTIL_EXPORT
#endif /* __INTELLISENSE__ */
#ifndef TRACE_ETW_UTIL_API
    #define TRACE_ETW_UTIL_API __stdcall
#endif /* TRACE_ETW_UTIL_API */

#endif /* DOXYGEN */

/* ===== プロバイダ参照型 ===== */

/**
 *  @typedef        etw_provider_ref_t
 *  @brief          プロバイダ参照型。
 *  @details        TraceLoggingHProvider (TraceLoggingProvider.h が定義する型) と同等です。
 */
struct _tlgProvider_t;
typedef struct _tlgProvider_t const *etw_provider_ref_t;

/* ===== プロバイダ定義マクロ ===== */

/**
 *  @def            TRACE_ETW_UTIL_DEFINE_PROVIDER(var, name, guid)
 *  @brief          ETW プロバイダを定義するマクロ。
 *  @details        呼び出し元の .c ファイルのファイルスコープに 1 回だけ記述します。\n
 *                  TRACELOGGING_DEFINE_PROVIDER(var, name, guid) に展開します。\n
 *                  呼び出し元は本マクロ使用前に windows.h と TraceLoggingProvider.h を
 *                  インクルードする必要があります。
 *
 *  @param          var   プロバイダ変数名 (etw_provider_ref_t 型)
 *  @param          name  プロバイダ名 (文字列リテラル)
 *  @param          guid  GUID (TraceLogging 形式の括弧付き定数タプル)
 *
 *  @par            使用例
 *  @code
 *  #include <windows.h>
 *  #include <TraceLoggingProvider.h>
 *  #include <trace-etw-util.h>
 *
 *  TRACE_ETW_UTIL_DEFINE_PROVIDER(
 *      s_my_provider,
 *      "MyProvider",
 *      (0x12345678, 0x1234, 0x1234, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89));
 *  @endcode
 */
#define TRACE_ETW_UTIL_DEFINE_PROVIDER(var, name, guid) \
    TRACELOGGING_DEFINE_PROVIDER(var, name, guid)

/* ===== 不透明ハンドル型 ===== */

/** ETW プロバイダハンドル (不透明型)。 */
typedef struct etw_provider etw_provider_t;

/* ===== API 関数 ===== */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          ETW プロバイダを登録する。
     *  @details        呼び出し元が TRACE_ETW_UTIL_DEFINE_PROVIDER で定義したプロバイダ変数を渡します。
     *                  これにより、呼び出し元ごとに異なる GUID/プロバイダ名を使用できます。
     *
     *  @param[in]      provider_ref  TRACE_ETW_UTIL_DEFINE_PROVIDER で定義した変数。
     *  @return         成功時: ハンドル。失敗時: NULL。
     *
     *  @par            使用例
     *  @code
     *  etw_provider_t *h = etw_provider_init(s_my_provider);
     *  @endcode
     */
    TRACE_ETW_UTIL_EXPORT etw_provider_t *TRACE_ETW_UTIL_API
        etw_provider_init(etw_provider_ref_t provider_ref);

    /**
     *  @brief          ETW プロバイダへ UTF-8 メッセージを書き込む。
     *
     *  @param[in]      handle   etw_provider_init の戻り値。
     *                           NULL の場合は何もせず 0 を返します。
     *  @param[in]      level    イベントレベル。
     *                           1=CRITICAL / 2=ERROR / 3=WARNING / 4=INFO / 5=VERBOSE
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
    TRACE_ETW_UTIL_EXPORT int TRACE_ETW_UTIL_API
        etw_provider_write(etw_provider_t *handle, int level, const char *message);

    /**
     *  @brief          ETW プロバイダの登録を解除する。
     *  @details        ハンドルが NULL の場合は何もしません。
     *
     *  @param[in]      handle   etw_provider_init の戻り値。
     */
    TRACE_ETW_UTIL_EXPORT void TRACE_ETW_UTIL_API
        etw_provider_dispose(etw_provider_t *handle);

    /* ===== セッション (Consumer) API ===== */

    /** @name ETW セッションステータスコード */
    /** @{ */

    /** 成功。 */
    #define ETW_SESSION_OK          0
    /** パラメータエラー (NULL または不正な GUID)。 */
    #define ETW_SESSION_ERR_PARAM  -1
    /** 権限不足 (Administrators または Performance Log Users が必要)。 */
    #define ETW_SESSION_ERR_ACCESS -2
    /** その他のシステムエラー。 */
    #define ETW_SESSION_ERR_SYSTEM -3

    /** @} */

    /**
     *  @typedef        etw_event_callback_t
     *  @brief          ETW イベント受信コールバック型。
     *  @details        セッションがイベントを受信するたびに呼び出されます。\n
     *                  コールバックは etw_session_start 内部のワーカースレッドから呼ばれます。\n
     *                  呼び出し元が共有データにアクセスする場合はスレッド安全に保護してください。
     *
     *  @param[in]      level    イベントレベル (1-5)。
     *  @param[in]      message  null 終端 UTF-8 文字列。NULL の場合があります。
     *  @param[in]      context  etw_session_start に渡したユーザーデータ。
     */
    typedef void (*etw_event_callback_t)(int level, const char *message, void *context);

    /** ETW セッションハンドル (不透明型)。 */
    typedef struct etw_session etw_session_t;

    /**
     *  @brief          ETW セッション開始に必要な権限があるか検査する。
     *  @details        内部でダミーセッションの開始・停止を行い、権限の有無を判定します。\n
     *                  アプリケーションの起動時やテストの SetUp で呼び出すことを想定しています。
     *
     *  @return         @c ETW_SESSION_OK     権限あり。\n
     *                  @c ETW_SESSION_ERR_ACCESS 権限不足。\n
     *                  @c ETW_SESSION_ERR_SYSTEM その他のシステムエラー。
     *
     *  @par            使用例
     *  @code
     *  int status = etw_session_check_access();
     *  if (status == ETW_SESSION_ERR_ACCESS) {
     *      fprintf(stderr, "Run: net localgroup \"Performance Log Users\" %%USERNAME%% /add\n");
     *  }
     *  @endcode
     */
    TRACE_ETW_UTIL_EXPORT int TRACE_ETW_UTIL_API
        etw_session_check_access(void);

    /**
     *  @brief          リアルタイム ETW セッションを開始し、指定プロバイダを購読する。
     *  @details        内部でワーカースレッドを起動し ProcessTrace をブロッキング実行します。\n
     *                  セッション開始には Administrators または Performance Log Users グループの
     *                  メンバーシップが必要です。権限不足の場合、@p out_status に
     *                  @c ETW_SESSION_ERR_ACCESS が設定されます。
     *
     *  @param[in]      session_name       セッション名 (システム全体で一意にすること)。
     *  @param[in]      provider_guid_str  GUID 文字列 "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"。
     *  @param[in]      callback           イベント受信時に呼ばれるコールバック。
     *  @param[in]      context            コールバックに渡すユーザーデータ。
     *  @param[out]     out_status         エラーコード出力先 (NULL 可)。\n
     *                                     @c ETW_SESSION_OK / @c ETW_SESSION_ERR_PARAM /
     *                                     @c ETW_SESSION_ERR_ACCESS / @c ETW_SESSION_ERR_SYSTEM。
     *  @return         成功: セッションハンドル / 失敗: NULL。
     *
     *  @par            使用例
     *  @code
     *  int status;
     *  etw_session_t *s = etw_session_start(
     *      "MySession",
     *      "12345678-1234-1234-abcd-ef0123456789",
     *      my_callback, my_context, &status);
     *  if (s == NULL && status == ETW_SESSION_ERR_ACCESS) {
     *      fprintf(stderr, "Run: net localgroup \"Performance Log Users\" %%USERNAME%% /add\n");
     *  }
     *  @endcode
     */
    TRACE_ETW_UTIL_EXPORT etw_session_t *TRACE_ETW_UTIL_API
        etw_session_start(const char *session_name,
                          const char *provider_guid_str,
                          etw_event_callback_t callback,
                          void *context,
                          int *out_status);

    /**
     *  @brief          ETW セッションを停止し、リソースを解放する。
     *  @details        セッションを停止しバッファをフラッシュした後、
     *                  ワーカースレッドを join してリソースを解放します。\n
     *                  ハンドルが NULL の場合は何もしません。
     *
     *  @param[in]      session  etw_session_start の戻り値。
     */
    TRACE_ETW_UTIL_EXPORT void TRACE_ETW_UTIL_API
        etw_session_stop(etw_session_t *session);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WIN32 */

#endif /* TRACE_ETW_UTIL_H */
