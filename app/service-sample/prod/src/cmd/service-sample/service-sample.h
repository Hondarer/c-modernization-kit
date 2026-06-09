/**
 *******************************************************************************
 *  @file           service-sample.h
 *  @brief          クロスプラットフォーム サービス サンプルの共通定義。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  Windows サービス (SCM) と Linux systemd の両方に対応した
 *  サービス (デーモン) の雛形です。\n
 *  \n
 *  利用者は svc_definition 構造体にサービス名・表示名・説明と
 *  ライフサイクル コールバック (on_start / on_run / on_stop) を設定し、
 *  svc_main() を呼ぶだけでクロスプラットフォーム サービスを構築できます。\n
 *  \n
 *  サービスの停止要求は 3 経路 (SIGINT/SIGTERM、SetConsoleCtrlHandler、
 *  Windows SCM の SERVICE_CONTROL_STOP) を svc_request_stop() に集約します。
 *  on_run の実装は svc_wait_for_stop() を呼ぶだけで停止を検知できます。
 *
 *  @par            使用方法 (コマンド ライン)
 *  @code{.sh}
    service-sample install    # OS にサービスを登録する
    service-sample uninstall  # OS からサービスを解除する
    service-sample run        # サービスとして起動する (SCM/systemd から呼ばれる)
    service-sample console    # フォアグラウンドで実行する (デバッグ用)
 *  @endcode
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SERVICE_SAMPLE_H
#define SERVICE_SAMPLE_H

#include <com_util/base/platform.h>
#include <com_util/trace/tracer.h>

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

    /* ============================================================
     *  ライフサイクル コールバック型定義
     * ============================================================ */

    /**
     *  @brief          サービス初期化コールバックの型。
     *
     *  サービス起動時に 1 回だけ呼ばれます。\n
     *  0 を返すと on_run() が呼ばれます。0 以外を返すと起動を中断します。
     *
     *  @param[in]      user_data   svc_definition に登録した任意ポインター。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     */
    typedef int (*svc_on_start_fn)(void *user_data);

    /**
     *  @brief          サービス メインループ コールバックの型。
     *
     *  on_start() が成功した後に呼ばれます。\n
     *  停止要求が来るまで戻らないように実装してください。\n
     *  停止要求の検知には svc_wait_for_stop() を使用してください。
     *
     *  @param[in]      user_data   svc_definition に登録した任意ポインター。
     */
    typedef void (*svc_on_run_fn)(void *user_data);

    /**
     *  @brief          サービス停止処理コールバックの型。
     *
     *  on_run() が戻った後に必ず呼ばれます。\n
     *  on_start() が失敗した場合は呼ばれません。
     *
     *  @param[in]      user_data   svc_definition に登録した任意ポインター。
     */
    typedef void (*svc_on_stop_fn)(void *user_data);

    /* ============================================================
     *  サービス定義構造体
     * ============================================================ */

    /**
     *  @brief          サービスの定義。
     *
     *  @par            使用例
     *  @code{.c}
        static const svc_definition g_service_def = {
            "my-service",
            "My Service",
            "サービスの説明",
            on_start,
            on_run,
            on_stop,
            NULL
        };
        int main(int argc, char *argv[]) { return svc_main(argc, argv, &g_service_def); }
     *  @endcode
     */
    typedef struct svc_definition
    {
        const char *name;         /**< 登録名。systemd unit 名 / SCM サービス名に使う (英数・ハイフン推奨)。 */
        const char *display_name; /**< 表示名。Windows SCM のサービス一覧に表示される。 */
        const char *description;  /**< 説明文。Windows SCM / systemd unit の Description に設定される。 */
        svc_on_start_fn on_start; /**< 初期化コールバック。NULL 可。 */
        svc_on_run_fn on_run;     /**< メインループ コールバック。NULL 不可。 */
        svc_on_stop_fn on_stop;   /**< 停止処理コールバック。NULL 可。 */
        void *user_data;          /**< 各コールバックに渡す任意ポインター。 */
    } svc_definition;

    /* ============================================================
     *  停止抽象 API
     * ============================================================ */

    /**
     *  @brief          停止要求が届くまで、または指定時間が経過するまで待機します。
     *  @param[in]      timeout_ms  タイムアウト (ミリ秒)。0 を指定すると即時リターンします。\n
     *                              負値を指定した場合もタイムアウト 0 と同様に即時リターンします。
     *  @return         停止要求が届いた場合は 1、タイムアウトの場合は 0 を返します。
     *
     *  on_run() の周期処理ループでこの関数を使うことで、
     *  停止要求を受け取ったときにメインループを正常に抜けられます。
     *
     *  @par            使用例
     *  @code{.c}
        static void on_run(void *user_data)
        {
            (void)user_data;
            while (svc_wait_for_stop(1000) == 0)
            {
                // TODO: ここに周期処理を書く
            }
        }
     *  @endcode
     */
    int svc_wait_for_stop(int timeout_ms);

    /**
     *  @brief          停止要求が届いているかを即時判定します。
     *  @return         停止要求済みの場合は 1、そうでない場合は 0 を返します。
     */
    int svc_stop_requested(void);

    /**
     *  @brief          停止を要求します。
     *
     *  SIGTERM / SIGINT (Linux console)、SetConsoleCtrlHandler (Windows console)、
     *  ServiceCtrlHandler (Windows SCM) の 3 経路すべてが最終的にこの関数を呼びます。\n
     *  複数回呼んでも安全 (冪等) です。
     */
    void svc_request_stop(void);

    /* ============================================================
     *  エントリ ポイント
     * ============================================================ */

    /**
     *  @brief          サービスのエントリ ポイント。
     *  @param[in]      argc    コマンド ライン引数の数。
     *  @param[in]      argv    コマンド ライン引数の配列。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。
     *  @return         正常終了時は 0、異常終了時は 0 以外を返します。
     *
     *  argv[1] に応じて次の処理を実行します。\n
     *  - install   : OS にサービスを登録します。\n
     *  - uninstall : OS からサービスを解除します。\n
     *  - run       : サービスとして常駐起動します (SCM/systemd から呼ばれる)。\n
     *  - console   : フォアグラウンドで実行します (デバッグ用)。
     */
    int svc_main(int argc, char *argv[], const svc_definition *def);

    /* ============================================================
     *  OS フック (各プラットフォーム ファイルで実装)
     * ============================================================ */

    /**
     *  @brief          OS にサービスを登録します。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     *
     *  - Linux  : /etc/systemd/system/{name}.service を生成し、systemctl enable を実行します。\n
     *             root 権限が必要です。\n
     *  - Windows: SCM に CreateService で登録します。管理者権限が必要です。
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
     */
    int svc_os_uninstall(const svc_definition *def);

    /**
     *  @brief          サービスとして常駐起動します。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。
     *  @return         成功時は 0、失敗時は 0 以外を返します。
     *
     *  SCM/systemd から "run" 引数付きで起動されたときに呼ばれます。\n
     *  - Linux  : Type=notify のため fork せずそのまま常駐し、SIGTERM で停止します。\n
     *  - Windows: StartServiceCtrlDispatcher でサービス ディスパッチャーに接続します。
     */
    int svc_os_run_service(const svc_definition *def);

    /**
     *  @brief          起動完了を OS に通知します (内部共有関数)。
     *
     *  on_start() 成功直後に svc_run_lifecycle() から呼ばれます。\n
     *  - Linux  : NOTIFY_SOCKET へ "READY=1" を送信します (sd_notify 相当)。\n
     *             NOTIFY_SOCKET が設定されていない場合は何もしません。\n
     *  - Windows: SCM に SERVICE_RUNNING を通知します。\n
     *             コンソール モード (SCM 未接続) の場合は何もしません。
     */
    void svc_os_notify_ready(void);

    /**
     *  @brief          停止開始を OS に通知します (内部共有関数)。
     *
     *  on_run() 復帰直後、on_stop() 呼び出し前に svc_run_lifecycle() から呼ばれます。\n
     *  - Linux  : NOTIFY_SOCKET へ "STOPPING=1" を送信します (sd_notify 相当)。\n
     *             NOTIFY_SOCKET が設定されていない場合は何もしません。\n
     *  - Windows: SCM に SERVICE_STOP_PENDING を通知します。\n
     *             コンソール モード (SCM 未接続) の場合は何もしません。
     */
    void svc_os_notify_stopping(void);

    /* ============================================================
     *  内部共有関数 (プラットフォーム ファイルからアクセスするために公開)
     * ============================================================ */

    /**
     *  @brief          ライフサイクル コールバックを駆動します (内部共有関数)。
     *  @param[in]      def     サービス定義。NULL を渡してはなりません。
     *  @return         成功時は 0、on_start が失敗した場合はその戻り値を返します。
     *
     *  console モードおよび Linux run モード (Type=notify) が使用します。\n
     *  shutdown.h の request callback を登録して SIGINT/SIGTERM を補足し、
     *  on_start → on_run → on_stop の順でライフサイクルを駆動します。
     */
    int svc_run_lifecycle(const svc_definition *def);

    /**
     *  @brief          サービス共通の tracer ハンドルを取得します (内部共有関数)。
     *  @return         process-global の tracer ハンドル。\n
     *                  未初期化または初期化失敗時は NULL を返します。
     *
     *  各プラットフォーム ファイル (svc_os_install / svc_os_uninstall 等) が
     *  診断トレースを出力するために使用します。\n
     *  NULL を tracer 出力マクロに渡しても安全 (出力されないだけ) です。
     */
    com_util_tracer *svc_get_tracer(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* SERVICE_SAMPLE_H */
