/**
 *******************************************************************************
 *  @file           service-sample.h
 *  @brief          クロスプラットフォーム サービスの共通インターフェイスを宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  Windows サービス (SCM) と Linux systemd の両方に対応した
 *  サービス (デーモン) の雛形です。\n
 *  \n
 *  利用者は svc_definition 構造体にサービス名・表示名・説明と
 *  ライフサイクル コールバック (on_start / on_run / on_stop) を設定するだけで
 *  クロスプラットフォーム サービスを構築できます。\n
 *  エントリ ポイント main() は共通実装 (service-sample.c) が提供しており、
 *  実装側 (service-sample-impl.c) は g_service_def の定義のみを行います。\n
 *  \n
 *  サービスの停止要求は 3 経路 (SIGINT/SIGTERM、SetConsoleCtrlHandler、
 *  Windows SCM の SERVICE_CONTROL_STOP) を svc_request_stop() に集約します。
 *  on_run の実装は svc_wait_for_stop() を呼び出すだけで停止を検知できます。\n
 *  \n
 *  任意で on_event (電源・セッション・シャットダウン前イベント) と
 *  on_reload (設定再読込) のコールバックを設定できます。\n
 *  Windows は SCM のコントロール通知、Linux は systemd-logind (D-Bus) と
 *  SIGHUP を共通のイベントに対応付けます。
 *
 *  @par            Windows の SCM へ通知する所要時間の見積もり
 *  Windows のサービスでは、起動中と停止中の状態を SCM へ通知するときに、
 *  次の通知までの所要時間の見積もり (SERVICE_STATUS の dwWaitHint) を固定値で渡します。\n
 *  dwWaitHint は見積もりであり、on_start()、停止要求の後の on_run()、on_stop() が
 *  これより長くかかる場合の進捗の通知 (checkpoint の更新) には対応していません。\n
 *  see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_status
 *
 *  @par            使用方法 (コマンド ライン)
    @code{.sh}
    service-sample install    # OS にサービスを登録する
    service-sample uninstall  # OS からサービスを解除する
    service-sample run        # サービスとして起動する (SCM/systemd から呼ばれる)
    service-sample console    # フォアグラウンドで実行する (デバッグ用)
    @endcode
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SERVICE_SAMPLE_PRIVATE_H
#define SERVICE_SAMPLE_PRIVATE_H

#include <cplat/base/platform.h>
#include <cplat/trace/tracer.h>

#include <stdint.h>

/**
 *  @defgroup       SERVICE_SAMPLE_PUBLIC_API 公開 API (service-sample)
 *  @brief          service-sample のサービス ライフサイクル API です。
 */

/**
 *  @ingroup        SERVICE_SAMPLE_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /*
     * ============================================================
     *  ライフサイクル コールバック型定義
     * ============================================================
     */

    /**
     *  @brief          サービス初期化コールバックの型。
     *
     *  サービス起動時に 1 回だけ呼ばれます。\n
     *  0 を返すと on_run() が呼ばれます。0 以外を返すと起動を中断します
     *  (on_run() と on_stop() は呼ばれません)。\n
     *  失敗を返すとサービスは失敗として終了し、自動再起動の対象になります。\n
     *  失敗値は、プロセスの終了コードにもなるため、1 から 125 の範囲 (3 は abort() の終了コードのため除く) を推奨します。\n
     *  範囲の根拠は、共通のコーディング規範の「プロセスの終了コード」です。\n
     *  Linux では、systemd の起動タイムアウト (TimeoutStartSec=。unit で指定していないため、既定の 90 [s]) より
     *  短い時間で戻る必要があります。超えた場合は、systemd がサービスを起動の失敗として終了させます。
     *
     *  @param[in]      user_data   svc_definition に登録した任意ポインター。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     */
    typedef int (*svc_on_start_fn)(void *user_data);

    /**
     *  @brief          サービス メイン ループ コールバックの型。
     *
     *  on_start() が成功した後に呼ばれます。\n
     *  停止要求が来るまで戻らないように実装してください。\n
     *  停止要求の検知には svc_wait_for_stop() を使用してください。\n
     *  失敗 (0 以外) を返してもその後の on_stop() は呼ばれます。\n
     *  失敗を返すとサービスは失敗として終了し、自動再起動の対象になります。\n
     *  失敗値は、プロセスの終了コードにもなるため、1 から 125 の範囲 (3 は abort() の終了コードのため除く) を推奨します。\n
     *  範囲の根拠は、共通のコーディング規範の「プロセスの終了コード」です。
     *
     *  @par            Linux の watchdog との関係
     *  systemd が watchdog を要求している場合 (unit の WatchdogSec=) は、この関数の進行を watchdog の通知に結び付けます。\n
     *  フレームワークは、この関数が svc_wait_for_stop() で待機している間と、
     *  svc_wait_for_stop() の呼び出しを続けている間だけ、WATCHDOG=1 を送ります。\n
     *  svc_wait_for_stop() から戻った後は、watchdog の期限の半分より短い時間で、次の svc_wait_for_stop() を
     *  呼んでください (install が生成する unit は WatchdogSec=30 のため、15 [s] 未満)。\n
     *  この関数の開始から最初の呼び出しまでも同じです。\n
     *  svc_wait_for_stop() は、この関数を実行しているスレッドから呼んでください。\n
     *  期限内に呼ばない場合は、systemd がサービスを失敗として SIGABRT で終了させ、Restart=on-failure により再起動します。\n
     *  Windows の SCM には、動作中のサービスの生存を確かめる仕組みがないため、この条件は Linux だけに適用します。\n
     *  watchdog が要求されていない場合 (WatchdogSec= がない、systemd の外で実行した場合) も適用しません。
     *
     *  @param[in]      user_data   svc_definition に登録した任意ポインター。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     */
    typedef int (*svc_on_run_fn)(void *user_data);

    /**
     *  @brief          サービス停止処理コールバックの型。
     *
     *  on_run() が戻った後に必ず呼ばれます (on_run() が失敗を返した場合も
     *  後始末のために呼ばれます)。\n
     *  on_start() が失敗した場合は呼ばれません。\n
     *  失敗を返すとサービスは失敗として終了し、自動再起動の対象になります。\n
     *  失敗値は、プロセスの終了コードにもなるため、1 から 125 の範囲 (3 は abort() の終了コードのため除く) を推奨します。\n
     *  範囲の根拠は、共通のコーディング規範の「プロセスの終了コード」です。\n
     *  Linux では、停止の要求から on_run() の復帰と on_stop() の完了までの合計を 60 [s] 未満にしてください
     *  (systemd の停止タイムアウト 90 [s] から、イベント監視スレッドの終了を待つ上限 30 [s] を除いた時間)。\n
     *  停止の開始の直前に WATCHDOG=1 が届き、systemd の watchdog が再び有効になった場合は、
     *  合計を WatchdogSec= 未満にしてください。
     *
     *  @param[in]      user_data   svc_definition に登録した任意ポインター。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     */
    typedef int (*svc_on_stop_fn)(void *user_data);

    /*
     * ============================================================
     *  OS イベント定義
     * ============================================================
     */

    /**
     *  @brief          OS イベントの種別。
     *
     *  Windows SCM のコントロール通知と Linux systemd-logind のシグナルを
     *  共通のイベント種別に対応付けます。
     */
    typedef enum svc_event_type
    {
        SVC_EVENT_POWER_SUSPEND = 1, /**< サスペンド開始。Windows: PBT_APMSUSPEND / Linux: PrepareForSleep(true)。 */
        SVC_EVENT_POWER_RESUME = 2,  /**< サスペンドからの復帰。Windows: PBT_APMRESUMEAUTOMATIC,
                                          PBT_APMRESUMESUSPEND / Linux: PrepareForSleep(false)。 */
        SVC_EVENT_SESSION_LOGON = 3, /**< セッションのログオン。Windows: WTS_SESSION_LOGON / Linux: SessionNew。 */
        SVC_EVENT_SESSION_LOGOFF =
            4,                    /**< セッションのログオフ。Windows: WTS_SESSION_LOGOFF / Linux: SessionRemoved。 */
        SVC_EVENT_PRESHUTDOWN = 5 /**< システム シャットダウン前の猶予通知。Windows: SERVICE_CONTROL_PRESHUTDOWN /
                                       Linux: PrepareForShutdown(true)。 */
    } svc_event_type;

    /**
     *  @brief          OS イベントの付加情報。
     */
    typedef struct svc_event_info
    {
        svc_event_type type; /**< イベント種別。 */
        unsigned int pad;    /**< x64 環境で後続の session_id のアラインメントを明示するパディング。未使用。 */
        const char *session_id; /**< セッション ID。SVC_EVENT_SESSION_LOGON / SVC_EVENT_SESSION_LOGOFF のときのみ
                                     有効で、それ以外のイベントでは NULL です。\n
                                     Windows: dwSessionId の 10 進文字列 / Linux: logind のセッション ID。\n
                                     コールバックから戻った後は無効になるため、保持する場合は複製してください。 */
    } svc_event_info;

    /**
     *  @brief          OS イベント コールバックの型。
     *
     *  電源・セッション・シャットダウン前のイベント発生時に呼ばれます。\n
     *  on_run() とは別のスレッド (Windows: SCM ハンドラー スレッド、
     *  Linux: イベント監視スレッド) から呼ばれるため、共有データへの
     *  アクセスには同期が必要です。\n
     *  OS 側の応答期限 (Linux のサスペンド猶予は logind の InhibitDelayMaxSec、
     *  既定 5 [s]) があるため、短時間で戻るように実装してください。\n
     *  on_start() の成功後から on_run() が戻るまでの間に呼び出しを始めます。
     *  開始済みの呼び出しは、on_stop() と並行して実行中のことがあります。
     *
     *  @param[in]      info        イベントの付加情報。NULL は渡されません。
     *  @param[in]      user_data   svc_definition に登録した任意ポインター。
     */
    typedef void (*svc_on_event_fn)(const svc_event_info *info, void *user_data);

    /**
     *  @brief          設定再読込コールバックの型。
     *
     *  Windows: SERVICE_CONTROL_PARAMCHANGE、Linux: SIGHUP (systemctl reload)
     *  を受けると呼ばれます。\n
     *  Linux では呼び出しの前後で RELOADING=1 / READY=1 の通知を
     *  フレームワーク側が自動で行います。\n
     *  on_run() とは別のスレッドから呼ばれるため、共有データへの
     *  アクセスには同期が必要です。短時間で戻るように実装してください。\n
     *  on_start() の成功後から on_run() が戻るまでの間に呼び出しを始めます。
     *  開始済みの呼び出しは、on_stop() と並行して実行中のことがあります。
     *
     *  @param[in]      user_data   svc_definition に登録した任意ポインター。
     */
    typedef void (*svc_on_reload_fn)(void *user_data);

    /*
     * ============================================================
     *  サービス定義構造体
     * ============================================================
     */

    /**
     *  @brief          サービスの定義。
     *
     *  @par            使用例
        @code{.c}
        // 実装側で g_service_def を定義する (外部結合が必要)。
        // main() は共通実装 (service-sample.c) が提供し、g_service_def を参照する。
        const svc_definition g_service_def = {
            "my-service",
            "My Service",
            "サービスの説明",
            on_start,
            on_run,
            on_stop,
            NULL,
            on_event,
            on_reload
        };
        @endcode
     */
    typedef struct svc_definition
    {
        const char *name;         /**< 登録名。systemd unit 名 / SCM サービス名に使う (英数・ハイフン推奨)。 */
        const char *display_name; /**< 表示名。Windows SCM のサービス一覧に表示される。 */
        const char *description;  /**< 説明文。Windows SCM / systemd unit の Description に設定される。 */
        svc_on_start_fn on_start; /**< 初期化コールバック。NULL 可。失敗 (0 以外) を返すと起動を中断します。 */
        svc_on_run_fn on_run;     /**< メイン ループ コールバック。NULL 不可。失敗 (0 以外) で失敗終了します。 */
        svc_on_stop_fn on_stop;   /**< 停止処理コールバック。NULL 可。失敗 (0 以外) で失敗終了します。 */
        void *user_data;          /**< 各コールバックに渡す任意ポインター。 */
        svc_on_event_fn on_event; /**< OS イベント コールバック。NULL 可 (NULL の場合は電源・セッション・
                                       シャットダウン前イベントの監視を行わない)。 */
        svc_on_reload_fn on_reload; /**< 設定再読込コールバック。NULL 可 (NULL の場合は再読込要求を受け付けない。
                                         Linux では unit へ ExecReload= を書き込まず、SIGHUP を受信しても
                                         トレースへ記録して無視する)。 */
    } svc_definition;

    /*
     * ============================================================
     *  停止抽象 API
     * ============================================================
     */

    /**
     *  @brief          停止要求が届くまで、または指定時間が経過するまで待機します。
     *  @param[in]      timeout_ms  タイムアウト [ms]。0 を指定すると即時リターンします。\n
     *                              負値を指定した場合もタイムアウト 0 と同様に即時リターンします。
     *  @return         停止要求が届いた場合は 1、タイムアウトの場合は 0 を返します。\n
     *                  待機に失敗した場合も 1 を返します。停止要求の状態は変えません。
     *
     *  on_run() の周期処理ループでこの関数を使うことで、
     *  停止要求を受け取ったときにメイン ループを正常に抜けられます。\n
     *  呼び出しの入口と出口で on_run() の進行を記録します。Linux では、この記録を watchdog の通知の判定に使います
     *  (svc_on_run_fn の「Linux の watchdog との関係」を参照)。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     *
     *  @par            使用例
     *  @code{.c}
     *  static int on_run(void *user_data)
     *  {
     *      (void)user_data;
     *      while (svc_wait_for_stop(1000) == 0)
     *      {
     *          // TODO: ここに周期処理を書く
     *      }
     *      return 0;
     *  }
     *  @endcode
     */
    int svc_wait_for_stop(int timeout_ms);

    /**
     *  @brief          前回の確認の後に on_run() が進行したかを判定します (内部共有関数)。
     *  @param[in,out]  seen_count  前回の確認で記録した svc_wait_for_stop() の呼び出しの計数。NULL を渡してはなりません。\n
     *                              1 を返す場合は、現在の計数で更新します。初回は 0 を指定してください。
     *  @return         svc_wait_for_stop() で待機中の場合、または前回の確認の後に svc_wait_for_stop() を
     *                  呼んでいた場合は 1、それ以外は 0 を返します。
     *
     *  Linux のイベント監視スレッドが、WATCHDOG=1 を送るかどうかの判定に使います。\n
     *  Windows でもコンパイルされますが、通知の判定には使いません。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  svc_wait_for_stop() と同時に呼び出せます。同じ @p seen_count を複数のスレッドから同時に渡してはなりません。
     */
    int svc_on_run_made_progress(uint32_t *seen_count);

    /**
     *  @brief          停止要求が届いているかを即時判定します。
     *  @return         停止要求済みの場合は 1、そうでない場合は 0 を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    int svc_stop_requested(void);

    /**
     *  @brief          停止を要求します。
     *
     *  SIGTERM / SIGINT (Linux console)、SetConsoleCtrlHandler (Windows console)、
     *  ServiceCtrlHandler (Windows SCM) の 3 経路すべてが最終的にこの関数を呼びます。\n
     *  複数回呼んでも安全 (べき等) です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    void svc_request_stop(void);

    /*
     * ============================================================
     *  状態通知 API
     * ============================================================
     */

    /**
     *  @brief          サービスの状態テキストを OS に通知します。
     *  @param[in]      text    状態テキスト (例: "処理中 (10 件完了)")。\n
     *                          NULL を指定した場合は何もしません。
     *
     *  - Linux  : sd_notify の "STATUS=" として送信し、systemctl status に表示されます。\n
     *             NOTIFY_SOCKET が設定されていない場合は何もしません。\n
     *  - Windows: SCM に等価な機能がないため、トレース出力のみ行います。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  同一プロセスからの並行呼び出しを呼び出し側で防止してください。
     */
    void svc_set_status_text(const char *text);

    /*
     * ============================================================
     *  OS フック (各プラットフォーム ファイルで実装)
     * ============================================================
     */

    /**
     *  @brief          OS にサービスを登録します。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     *
     *  - Linux  : /etc/systemd/system/{name}.service を生成し、systemctl enable を実行します。\n
     *             root 権限が必要です。\n
     *  - Windows: SCM に CreateService で登録します。管理者権限が必要です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。
     */
    int svc_os_install(const svc_definition *def);

    /**
     *  @brief          OS からサービスを解除します。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     *
     *  - Linux  : systemctl disable / stop を実行し、unit ファイルを削除します。\n
     *             root 権限が必要です。\n
     *  - Windows: 動作中のサービスを停止してから DeleteService で削除します。\n
     *             管理者権限が必要です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。
     *  @note Windows は最大 30 秒の停止待ち後に削除を要求します。停止を確認できなければ削除しません。
 */
    int svc_os_uninstall(const svc_definition *def);

    /**
     *  @brief          run コマンドの実行の準備を行います (内部共有関数)。
     *  @return         成功時は EXIT_SUCCESS、失敗時は EXIT_FAILURE を返します。
     *
     *  main() が、run コマンドで、トレーサーの開始とスレッドの生成より前に呼びます。\n
     *  - Linux  : SIGHUP をブロックします。以後に生成するスレッドは、このマスクを引き継ぎます。
     *             ブロックしたシグナルは、プロセスが終わるまで戻しません。\n
     *  - Windows: 何もしません。
     *
     *  @attention      Linux では、ブロックしたマスクが fork と execve を越えて子プロセスへ引き継がれます。
     *                  run の中で起動した子プロセスで SIGHUP を使う場合は、子の側でマスクを戻してください。
     *                  cplat_process_start() は、子のシグナル マスクを変えません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。
     */
    int svc_os_prepare_run(void);

    /**
     *  @brief          サービスとして常駐起動します。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     *
     *  SCM/systemd から "run" 引数付きで起動されたときに呼ばれます。\n
     *  - Linux  : Type=notify のため fork せずそのまま常駐し、SIGTERM で停止します。\n
     *             常駐の前後でイベント監視スレッド (電源・セッション イベント、
     *             SIGHUP、watchdog 応答) を起動・停止します。\n
     *  - Windows: StartServiceCtrlDispatcher でサービス ディスパッチャーに接続します。\n
     *  \n
     *  Linux では、イベント監視スレッドの必須の初期化 (イベント ループの生成、停止監視、
     *  watchdog のタイマー、SIGHUP の受信の登録) に失敗した場合は、on_start() を呼ばず、READY=1 も送らずに
     *  失敗を返します。\n
     *  実行中にイベント ループが異常終了した場合は停止を要求し、ライフサイクル コールバックが成功していても失敗を返します。
     *
     *  @attention      Linux では、停止の時にイベント監視スレッドが 30 [s] 以内に終了しない場合、
     *                  資源を解放せずにプロセスを異常終了させます。
     *                  スレッドが動いたまま終了を続けると、解放済みのサービス定義とトレースを参照し、
     *                  プロセスの継続が危険になるためです。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。
     */
    int svc_os_run_service(const svc_definition *def);

    /**
     *  @brief          起動完了を OS に通知します (内部共有関数)。
     *
     *  on_start() 成功直後に svc_run_callbacks() から呼ばれます。\n
     *  - Linux  : sd_notify(3) で "READY=1" を送信します。\n
     *             NOTIFY_SOCKET が設定されていない場合は何もしません。\n
     *  - Windows: SCM に SERVICE_RUNNING を通知します。\n
     *             コンソール モード (SCM 未接続) の場合は何もしません。
     */
    void svc_os_notify_ready(void);

    /**
     *  @brief          停止開始を OS に通知します (内部共有関数)。
     *
     *  on_run() 復帰直後、on_stop() 呼び出し前に svc_run_callbacks() から呼ばれます。\n
     *  - Linux  : sd_notify(3) で "STOPPING=1" を送信します。\n
     *             NOTIFY_SOCKET が設定されていない場合は何もしません。\n
     *  - Windows: SCM に SERVICE_STOP_PENDING を通知します。\n
     *             コンソール モード (SCM 未接続) の場合は何もしません。
     */
    void svc_os_notify_stopping(void);

    /**
     *  @brief          設定再読込の開始を OS に通知します (内部共有関数)。
     *
     *  svc_dispatch_reload() が on_reload() 呼び出し前に呼びます。\n
     *  - Linux  : sd_notify(3) で "RELOADING=1" を送信します。\n
     *             NOTIFY_SOCKET が設定されていない場合は何もしません。\n
     *  - Windows: SCM に等価な通知がないため何もしません。
     */
    void svc_os_notify_reloading(void);

    /**
     *  @brief          状態テキストを OS に通知します (内部共有関数)。
     *  @param[in]      text    状態テキスト。NULL を渡してはなりません。
     *
     *  svc_set_status_text() から呼ばれます。\n
     *  - Linux  : sd_notify(3) で "STATUS=<text>" を送信します。\n
     *             NOTIFY_SOCKET が設定されていない場合は何もしません。\n
     *  - Windows: SCM に等価な通知がないため何もしません。
     */
    void svc_os_notify_status(const char *text);

    /*
     * ============================================================
     *  内部共有関数 (プラットフォーム ファイルからアクセスするために公開)
     * ============================================================
     */

    /**
     *  @brief          ライフサイクル コールバックを駆動します (内部共有関数)。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。on_run は NULL であってはなりません。
     *  @return         成功時は 0、失敗した場合は、最初に失敗したコールバックの戻り値を返します。\n
     *                  失敗値の範囲は検査しません。\n
     *                  範囲の根拠は、共通のコーディング規範の「プロセスの終了コード」です。
     *
     *  on_start (NULL でなければ)、svc_os_notify_ready()、on_run、svc_os_notify_stopping()、on_stop (NULL でなければ) の順に呼びます。\n
     *  on_start が失敗した場合は、後続を呼ばずに、その戻り値を返します。\n
     *  on_run が失敗しても、on_stop は呼びます。on_run と on_stop の両方が失敗した場合は、on_run の戻り値を返します。\n
     *  失敗したコールバックごとに、ERROR のトレース「on_start が失敗しました (戻り値: %d)。」の形
     *  (on_run と on_stop も同じ形) を出します。\n
     *  OS ごとの入口の処理 (コンソールの初期化、停止要求の登録、SCM への状態の報告) と、結果の渡し方は、呼び出し元が行います。
     */
    int svc_run_callbacks(const svc_definition *def);

    /**
     *  @brief          コンソールを初期化し、ライフサイクル コールバックを駆動します (内部共有関数)。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。
     *  @return         成功時は 0、いずれかのコールバックが失敗した場合は
     *                  最初に失敗したコールバックの戻り値を返します。\n
     *                  停止要求の登録に失敗した場合は、コールバックを呼ばずに EXIT_FAILURE を返します。\n
     *                  失敗値の範囲は検査しません。\n
     *                  範囲の根拠は、共通のコーディング規範の「プロセスの終了コード」です。
     *
     *  console モードおよび Linux run モード (Type=notify) が使用します。\n
     *  コンソールを初期化し、shutdown.h の request callback を登録して SIGINT/SIGTERM を補足します。\n
     *  登録に成功した場合は、コールバックの順序と失敗値の採用を svc_run_callbacks() に任せます。\n
     *  戻り値はプロセス終了コードとして OS に伝わり、失敗時は自動再起動の
     *  発動条件になります。
     */
    int svc_run_lifecycle(const svc_definition *def);

    /**
     *  @brief          OS イベントを on_event コールバックに配送します (内部共有関数)。
     *  @param[in]      def     サービス定義。NULL の場合は何もしません。
     *  @param[in]      info    イベントの付加情報。NULL の場合は何もしません。
     *
     *  各プラットフォーム ファイルが OS イベントを検出したときに呼びます。\n
     *  def->on_event が NULL の場合は何もしません。
     */
    void svc_dispatch_event(const svc_definition *def, const svc_event_info *info);

    /**
     *  @brief          設定再読込要求を on_reload コールバックに配送します (内部共有関数)。
     *  @param[in]      def     サービス定義。NULL の場合は何もしません。
     *
     *  各プラットフォーム ファイルが再読込要求を検出したときに呼びます。\n
     *  def->on_reload が NULL の場合は何もしません。\n
     *  svc_os_notify_reloading() → on_reload() → svc_os_notify_ready() の順に
     *  呼び出し、Type=notify の reload 契約 (RELOADING=1 → READY=1) を満たします。
     */
    void svc_dispatch_reload(const svc_definition *def);

    /**
     *  @brief          サービス共通の tracer ハンドルを取得します (内部共有関数)。
     *  @return         process-global の tracer ハンドル。\n
     *                  未初期化または初期化失敗時は NULL を返します。
     *
     *  各プラットフォーム ファイル (svc_os_install / svc_os_uninstall 等) が
     *  診断トレースを出力するために使用します。\n
     *  NULL を tracer 出力マクロに渡しても安全 (出力されないだけ) です。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  起動完了後の参照は安全です。生成と破棄は main スレッドのみが行います。
     */
    cplat_tracer *svc_get_tracer(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* SERVICE_SAMPLE_PRIVATE_H */
