/**
 *******************************************************************************
 *  @file           service-sample_windows.c
 *  @brief          クロスプラットフォーム サービスの Windows 向け処理を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  Windows 固有のサービス処理を実装します。
 *  - svc_os_prepare_run : run の入口では何もしない。
 *  - svc_os_run_service : SCM ディスパッチャーへの接続
 *  - svc_os_install     : CreateService による SCM 登録
 *  - svc_os_uninstall   : DeleteService による SCM 解除
 *
 *  共通処理 (svc_run_callbacks / svc_run_lifecycle / main) は service-sample.c に、
 *  コールバック雛形は service-sample-impl.c に実装します。
 *
 *  内部起動パターン (ユーザーは直接使用しない):
 *  - `service-sample run` : SCM から起動されるサービス本体
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <cplat/base/platform.h>

#if defined(PLATFORM_WINDOWS)
    /* Advapi32.lib は windows_sdk.h の #pragma comment(lib, "Advapi32.lib") で指定済みである。 */
    #include <cplat/base/windows_sdk.h>

    #include <assert.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include <cplat/crt/stdio.h>
    #include <cplat/runtime/elevated_process.h>
    #include <cplat/runtime/process.h>
    #include <cplat/win32/win32.h>

    #include "service-sample.h"
    #include "service-sample_internal.h"

/* Doxygen コメントは、ヘッダーに記載 */

/*
 * ============================================================
 *  内部定数
 * ============================================================
 */

    /** PRESHUTDOWN 通知から強制停止までの猶予 [ms]。 */
    #define SVC_PRESHUTDOWN_TIMEOUT_MS 30000U

    /** 異常終了から自動再起動までの遅延 [ms]。Linux 側の RestartSec= と同じ SVC_FAILURE_RESTART_DELAY_SEC から求めます。 */
    #define SVC_FAILURE_RESTART_DELAY_MS (SVC_FAILURE_RESTART_DELAY_SEC * 1000U)

    /** 失敗カウンターをリセットする無失敗期間 [s]。1 日 (24 時間) です。 */
    #define SVC_FAILURE_RESET_PERIOD_SEC (24U * 60U * 60U)

    /**
     *  起動開始の SERVICE_START_PENDING に付ける、次の通知までの見積もり (dwWaitHint) [ms]。
     *
     *  次の通知は、on_start() から戻った後の SERVICE_RUNNING (または失敗時の SERVICE_STOPPED) です。\n
     *  制約: 1 以上とすること。値を大きくすると、on_start() が戻らない場合に SCM が起動の失敗と判断するまでの時間が延びます。
     */
    #define SVC_ON_START_WAIT_HINT_MS 3000U

    /**
     *  制御ハンドラーが停止の要求を受けて通知する SERVICE_STOP_PENDING に付ける、次の通知までの見積もり (dwWaitHint) [ms]。
     *
     *  停止、シャットダウン、シャットダウン前の制御で使います。
     *  次の通知は、停止の要求を受けた on_run() が戻った後の SERVICE_STOP_PENDING です。\n
     *  制約: 1 以上とすること。値を大きくすると、on_run() が戻らない場合に SCM が停止の失敗と判断するまでの時間が延びます。
     */
    #define SVC_ON_RUN_RETURN_WAIT_HINT_MS 5000U

    /**
     *  on_run() の復帰後に通知する SERVICE_STOP_PENDING に付ける、次の通知までの見積もり (dwWaitHint) [ms]。
     *
     *  次の通知は、on_stop() から戻った後の SERVICE_STOPPED です。\n
     *  制約: 1 以上とすること。値を大きくすると、on_stop() が戻らない場合に SCM が停止の失敗と判断するまでの時間が延びます。
     */
    #define SVC_ON_STOP_WAIT_HINT_MS 3000U

    /** セッション ID (DWORD の 10 進表記) を格納する配列の大きさ [byte] (終端 NUL を含む)。 */
    #define SVC_SESSION_ID_TEXT_SIZE 16U

    /** binPath の "\"<パス>\" run" のうち、実行ファイル パス以外の分 (引用符、空白、"run") [byte]。 */
    #define SVC_BIN_PATH_EXTRA_BYTES 16U

static_assert(SVC_ON_START_WAIT_HINT_MS > 0U, "start wait hint must be positive");
static_assert(SVC_ON_RUN_RETURN_WAIT_HINT_MS > 0U, "stop request wait hint must be positive");
static_assert(SVC_ON_STOP_WAIT_HINT_MS > 0U, "stop wait hint must be positive");
static_assert(SVC_SESSION_ID_TEXT_SIZE >= (10U + 1U), "セッション ID の 10 桁と終端 NUL が収まること");

/*
 * ============================================================
 *  内部状態 (SCM ディスパッチャーへの受け渡し)
 * ============================================================
 */

/** ServiceMain からアクセスできるようにファイル static に保持するサービス定義。 */
static const svc_definition *s_def = NULL;

/** サービス ステータス ハンドル。ServiceMain で初期化される。 */
static SERVICE_STATUS_HANDLE s_status_handle = NULL;

/** 現在のサービス ステータス。 */
static SERVICE_STATUS s_status = {0};

/*
 * ============================================================
 *  内部ヘルパー
 * ============================================================
 */

/**
 *  @brief          現在のサービス ステータス (s_status) を SCM に通知します。
 *
 *  通知が失敗した場合は、エラー コードを付けた ERROR のトレースを出して処理を続けます。\n
 *  停止へ進めても、その停止の通知が同じ理由で失敗する可能性が高く、SCM が持つ状態とのずれは解消できないためです。\n
 *  SERVICE_STOPPED の通知は 1 回だけとされているため、再試行はしません。
 *  see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-setservicestatus
 */
static void notify_service_status(void)
{
    if (!SetServiceStatus(s_status_handle, &s_status))
    {
        const DWORD err = GetLastError();

        /* 状態通知の失敗を伝える経路はトレースだけであり、その失敗の伝え先はないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "SetServiceStatus が失敗しました (状態: %lu、エラー コード: %lu)。",
                                  s_status.dwCurrentState, err);
    }
}

/**
 *  @brief          サービス ステータスを設定して SCM に通知します。
 *  @param[in]      state       新しいサービス状態 (SERVICE_RUNNING など)。
 *  @param[in]      controls    dwControlsAccepted (受け付けるコントロール)。
 *  @param[in]      checkpoint  進行状況カウンター (起動/停止中に使う)。
 *  @param[in]      wait_hint   次のチェックポイントまでの予想時間 [ms]。
 */
static void set_service_status(const DWORD state, const DWORD controls, const DWORD checkpoint, const DWORD wait_hint)
{
    s_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    s_status.dwCurrentState = state;
    s_status.dwControlsAccepted = controls;
    s_status.dwWin32ExitCode = NO_ERROR;
    s_status.dwServiceSpecificExitCode = 0;
    s_status.dwCheckPoint = checkpoint;
    s_status.dwWaitHint = wait_hint;
    notify_service_status();
}

/**
 *  @brief          停止完了 (SERVICE_STOPPED) を SCM に通知します。
 *  @param[in]      specific_exit_code  サービス固有の終了コード。0 は正常停止、
 *                                      0 以外は失敗として報告します。
 *
 *  set_service_status() は終了コードを NO_ERROR で上書きするため、
 *  失敗を報告する場合は必ずこの関数を使います。\n
 *  0 以外を報告すると SERVICE_CONFIG_FAILURE_ACTIONS_FLAG により
 *  failure actions (自動再起動) の発動条件になります。
 */
static void set_service_stopped(const DWORD specific_exit_code)
{
    s_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    s_status.dwCurrentState = SERVICE_STOPPED;
    s_status.dwControlsAccepted = 0;
    s_status.dwWin32ExitCode = NO_ERROR;
    s_status.dwServiceSpecificExitCode = 0;

    if (specific_exit_code != 0)
    {
        s_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
        s_status.dwServiceSpecificExitCode = specific_exit_code;
    }

    s_status.dwCheckPoint = 0;
    s_status.dwWaitHint = 0;
    notify_service_status();
}

/**
 *  @brief          受け付けるコントロールのビット集合を算出します。
 *  @return         dwControlsAccepted に設定する値。
 *
 *  STOP / SHUTDOWN は常に受け付けます。\n
 *  s_def の on_event が設定されている場合は電源・セッション・
 *  シャットダウン前のコントロールを、on_reload が設定されている場合は
 *  PARAMCHANGE を追加で受け付けます。
 */
static DWORD accepted_controls(void)
{
    DWORD controls;

    controls = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

    if (s_def != NULL && s_def->on_event != NULL)
    {
        controls |= SERVICE_ACCEPT_POWEREVENT | SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_PRESHUTDOWN;
    }

    if (s_def != NULL && s_def->on_reload != NULL)
    {
        controls |= SERVICE_ACCEPT_PARAMCHANGE;
    }

    return controls;
}

/**
 *  @brief          SCM 接続失敗時の診断を出力します。
 *  @param[in]      err 失敗した Windows エラー コード。
 */
static void write_dispatcher_error(const DWORD err)
{
    cplat_tracer *tracer;

    tracer = svc_get_tracer();

    /*
     * トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。
     * 標準エラー出力は診断の出力先がほかにないため、この関数の fprintf の戻り値は明示的に破棄する。
     */
    if (err == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
    {
        if (tracer != NULL)
        {
            (void)cplat_tracer_write(tracer, CPLAT_TRACE_LEVEL_ERROR, NULL, "SCM への接続に失敗しました。");
            (void)cplat_tracer_write(tracer, CPLAT_TRACE_LEVEL_ERROR, NULL,
                                     "サービスとして登録された後、SCM から 'run' 引数で起動してください。");
            (void)cplat_tracer_write(tracer, CPLAT_TRACE_LEVEL_ERROR, NULL,
                                     "コンソールで実行するには 'console' コマンドを使用してください。");
        }
        else
        {
            (void)fprintf(stderr, "SCM への接続に失敗しました。\n");
            (void)fprintf(stderr, "サービスとして登録された後、SCM から 'run' 引数で起動してください。\n");
            (void)fprintf(stderr, "コンソールで実行するには 'console' コマンドを使用してください。\n");
        }

        return;
    }

    if (tracer != NULL)
    {
        (void)cplat_tracer_writef(tracer, CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "StartServiceCtrlDispatcherU が失敗しました (エラー コード: %lu)。", err);
    }
    else
    {
        (void)fprintf(stderr, "StartServiceCtrlDispatcherU が失敗しました (エラー コード: %lu)。\n", err);
    }
}

/**
 *  @brief          管理者権限を保証します。
 *  @param[in]      command         昇格再実行するサブコマンド。
 *  @param[in]      operation_name  操作名 (エラー メッセージ用)。
 *  @param[out]     handled         別プロセスで処理済みの場合は 0 以外を格納します。
 *  @return         継続可能な場合は 0、失敗時または別プロセスの終了コードを返します。
 */
static int ensure_elevated_for_operation(const char *command, const char *operation_name, int *handled)
{
    int exit_code;
    int ret;

    if (handled == NULL)
    {
        return EXIT_FAILURE;
    }

    exit_code = EXIT_FAILURE;
    ret = cplat_elevated_process_run_if_needed(command, &exit_code, handled);
    if (ret != 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "%s には管理者権限が必要です。",
                                  operation_name);
        return EXIT_FAILURE;
    }

    if (*handled != 0)
    {
        return exit_code;
    }

    return EXIT_SUCCESS;
}

/*
 * ============================================================
 *  OS フック実装 (通知)
 * ============================================================
 */

/* Doxygen コメントは、ヘッダーに記載 */

void svc_os_notify_ready(void)
{
    if (s_status_handle == NULL)
    {
        return;
    }

    set_service_status(SERVICE_RUNNING, accepted_controls(), 0, 0);
}

void svc_os_notify_stopping(void)
{
    if (s_status_handle == NULL)
    {
        return;
    }

    /*
     * 停止要求を受けた場合は、制御ハンドラーが checkpoint 1 で SERVICE_STOP_PENDING を通知済みである。
     * 同じ状態を続けて通知するときは checkpoint を増やすため、ここでは 2 を通知する。
     * on_run() が停止要求なしに戻った場合は、この通知が最初の SERVICE_STOP_PENDING になる。
     * see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_status
     */
    set_service_status(SERVICE_STOP_PENDING, 0, 2, SVC_ON_STOP_WAIT_HINT_MS);
}

void svc_os_notify_reloading(void)
{
    /* SCM には reload 開始を通知する仕組みがないため、何もしない。 */
}

void svc_os_notify_status(const char *text)
{
    /*
     * SCM には状態テキストを表示する仕組みがないため、何もしない。
     * 共通層がトレース出力済みである。
     */
    (void)text;
}

/*
 * ============================================================
 *  サービス コントロール ハンドラー
 * ============================================================
 */

/**
 *  @brief          電源イベントを共通イベントに変換して配送します。
 *  @param[in]      ev_type 電源イベント種別 (PBT_APMSUSPEND など)。
 *
 *  サスペンドと復帰のみ配送し、他の PBT_* は無視します。
 */
static void handle_power_event(const DWORD ev_type)
{
    svc_event_info info;

    info.session_id = NULL;

    if (ev_type == PBT_APMSUSPEND)
    {
        info.type = SVC_EVENT_POWER_SUSPEND;
        svc_dispatch_event(s_def, &info);
    }
    else if (ev_type == PBT_APMRESUMEAUTOMATIC || ev_type == PBT_APMRESUMESUSPEND)
    {
        info.type = SVC_EVENT_POWER_RESUME;
        svc_dispatch_event(s_def, &info);
    }
}

/**
 *  @brief          セッション変更イベントを共通イベントに変換して配送します。
 *  @param[in]      ev_type セッション イベント種別 (WTS_SESSION_LOGON など)。
 *  @param[in]      ev_data WTSSESSION_NOTIFICATION へのポインター。NULL の場合は何もしません。
 *
 *  ログオンとログオフのみ配送し、他の WTS_* は無視します。
 */
static void handle_session_change(const DWORD ev_type, const LPVOID ev_data)
{
    const WTSSESSION_NOTIFICATION *notification;
    /* dwSessionId は 32 bit の DWORD であり、最大 10 桁と終端 NUL (11 [byte]) が 16 [byte] に収まる。 */
    char session_id_buf[SVC_SESSION_ID_TEXT_SIZE];
    svc_event_info info;

    if (ev_data == NULL)
    {
        return;
    }

    if (ev_type != WTS_SESSION_LOGON && ev_type != WTS_SESSION_LOGOFF)
    {
        return;
    }

    notification = (const WTSSESSION_NOTIFICATION *)ev_data;

    /*
     * 16 [byte] 配列に最大値が収まり、unsigned long と %lu の型も一致して書式化は失敗しないため、
     * 戻り値は明示的に破棄する。
     */
    (void)cplat_snprintf(session_id_buf, sizeof(session_id_buf), "%lu", (unsigned long)notification->dwSessionId);

    if (ev_type == WTS_SESSION_LOGON)
    {
        info.type = SVC_EVENT_SESSION_LOGON;
    }
    else
    {
        info.type = SVC_EVENT_SESSION_LOGOFF;
    }

    info.session_id = session_id_buf;
    svc_dispatch_event(s_def, &info);
}

/**
 *  @brief          SCM からのコントロール要求を処理します。
 *  @param[in]      ctrl    コントロール コード (SERVICE_CONTROL_STOP など)。
 *  @param[in]      ev_type イベント種別 (POWEREVENT / SESSIONCHANGE のときのみ有効)。
 *  @param[in]      ev_data イベント データ (SESSIONCHANGE のときのみ有効)。
 *  @param[in]      context 未使用。
 *  @return         処理した制御と INTERROGATE は NO_ERROR、未処理の制御は ERROR_CALL_NOT_IMPLEMENTED を返します。
 */
static DWORD WINAPI service_ctrl_handler(DWORD ctrl, DWORD ev_type, LPVOID ev_data, LPVOID context)
{
    svc_event_info info;

    (void)context;

    if (ctrl == SERVICE_CONTROL_STOP || ctrl == SERVICE_CONTROL_SHUTDOWN)
    {
        set_service_status(SERVICE_STOP_PENDING, 0, 1, SVC_ON_RUN_RETURN_WAIT_HINT_MS);

        /* 停止要求を on_run() のメイン ループに伝える。 */
        svc_request_stop();
    }
    else if (ctrl == SERVICE_CONTROL_PRESHUTDOWN)
    {
        /* シャットダウン前の猶予通知を配送してから、停止経路に入る。 */
        info.type = SVC_EVENT_PRESHUTDOWN;
        info.session_id = NULL;
        svc_dispatch_event(s_def, &info);
        set_service_status(SERVICE_STOP_PENDING, 0, 1, SVC_ON_RUN_RETURN_WAIT_HINT_MS);
        svc_request_stop();
    }
    else if (ctrl == SERVICE_CONTROL_POWEREVENT)
    {
        handle_power_event(ev_type);
    }
    else if (ctrl == SERVICE_CONTROL_SESSIONCHANGE)
    {
        handle_session_change(ev_type, ev_data);
    }
    else if (ctrl == SERVICE_CONTROL_PARAMCHANGE)
    {
        svc_dispatch_reload(s_def);
    }
    else if (ctrl == SERVICE_CONTROL_INTERROGATE)
    {
        /* 現在のステータスを再通知する。 */
        notify_service_status();
    }
    else
    {
        /* see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nc-winsvc-lphandler_function_ex */
        return ERROR_CALL_NOT_IMPLEMENTED;
    }

    return NO_ERROR;
}

/*
 * ============================================================
 *  ServiceMain (SCM ディスパッチャーから呼ばれる)
 * ============================================================
 */

/**
 *  @brief          サービス メイン関数 (SCM ディスパッチャーから呼ばれる)。
 *  @param[in]      argc    サービス引数の数。
 *  @param[in]      argv    サービス引数の配列。
 *
 *  制御ハンドラーの登録と SERVICE_START_PENDING の報告の後に、svc_run_callbacks() を呼びます。\n
 *  コールバックの順序と失敗値の採用は svc_run_callbacks() に任せます。\n
 *  戻り値は SERVICE_STOPPED の dwServiceSpecificExitCode として SCM へ報告します。
 */
static VOID WINAPI service_main(DWORD argc, LPWSTR *argv)
{
    int rc;

    (void)argc;
    (void)argv;

    if (s_def == NULL)
    {
        return;
    }

    s_status_handle = RegisterServiceCtrlHandlerExU(s_def->name, service_ctrl_handler, NULL);
    if (s_status_handle == NULL)
    {
        return;
    }

    set_service_status(SERVICE_START_PENDING, 0, 1, SVC_ON_START_WAIT_HINT_MS);

    rc = svc_run_callbacks(s_def);
    set_service_stopped((DWORD)rc);
}

/*
 * ============================================================
 *  OS フック実装
 * ============================================================
 */

/* Doxygen コメントは、ヘッダーに記載 */

int svc_os_prepare_run(void)
{
    /* Windows では、run の入口で行う準備がない。 */
    return EXIT_SUCCESS;
}

int svc_os_run_service(const svc_definition *def)
{
    /*
     * テーブルの最後の要素は、終端を示すため lpServiceName と lpServiceProc を NULL にする。
     * see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-startservicectrldispatcherw
     */
    SERVICE_TABLE_ENTRYU dispatch_table[] = {{def->name, service_main}, {NULL, NULL}};

    /*
     * def をファイル static に退避する。ServiceMain() は固定シグネチャのため、直接渡せない。
     * see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nc-winsvc-lpservice_main_functionw
     */
    s_def = def;

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
    (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "SCM ディスパッチャーに接続します...");

    if (!StartServiceCtrlDispatcherU(dispatch_table))
    {
        DWORD err = GetLastError();
        write_dispatcher_error(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int svc_os_install(const svc_definition *def)
{
    SC_HANDLE scm = NULL;
    SC_HANDLE svc = NULL;
    /* パスに "\"<path>\" run" を格納するための十分なサイズ */
    char bin_path[SVC_EXEC_PATH_MAX + SVC_BIN_PATH_EXTRA_BYTES];
    char exe_path[SVC_EXEC_PATH_MAX];
    int rc;
    int handled;

    rc = ensure_elevated_for_operation("install", "install", &handled);
    if (rc != 0 || handled != 0)
    {
        return rc;
    }

    if (cplat_process_get_executable_path(exe_path, sizeof(exe_path)) != CPLAT_OK)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "実行ファイルのパスを取得できませんでした。");
        return EXIT_FAILURE;
    }

    /*
     * binPath は "\"<パス>\" run" 形式にする (パスにスペースが含まれる場合の対策)。
     * see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-createservicew
     */
    if (cplat_snprintf(bin_path, sizeof(bin_path), "\"%s\" run", exe_path) != CPLAT_OK)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "パスの生成に失敗しました。");
        return EXIT_FAILURE;
    }

    scm = OpenSCManagerU(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (scm == NULL)
    {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED)
        {
            (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                     "SCM へのアクセスが拒否されました。管理者として実行してください。");
        }
        else
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "OpenSCManagerU が失敗しました (エラー コード: %lu)。", err);
        }

        return EXIT_FAILURE;
    }

    rc = EXIT_FAILURE;
    svc = CreateServiceU(scm, def->name, def->display_name, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                         SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, bin_path, NULL, /* ロード オーダー グループなし */
                         NULL,                                                     /* タグ ID なし */
                         NULL,                                                     /* 依存サービスなし */
                         NULL,                                                     /* LocalSystem アカウント */
                         NULL                                                      /* パスワードなし */
    );

    if (svc == NULL)
    {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_EXISTS)
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "サービス '%s' は既に登録されています。", def->name);
        }
        else if (err == ERROR_ACCESS_DENIED)
        {
            (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                     "サービスの登録が拒否されました。管理者として実行してください。");
        }
        else if (err == ERROR_SERVICE_MARKED_FOR_DELETE)
        {
            (void)cplat_tracer_writef(
                svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                "サービス '%s' は削除待ち状態です。Windows を再起動してから再度実行してください。", def->name);
        }
        else
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "CreateServiceU が失敗しました (エラー コード: %lu)。", err);
        }
    }
    else
    {
        SERVICE_DESCRIPTIONA description;
        SERVICE_PRESHUTDOWN_INFO preshutdown_info;

        description.lpDescription = (char *)def->description;
        if (!ChangeServiceConfig2U(svc, SERVICE_CONFIG_DESCRIPTION, &description))
        {
            const DWORD err = GetLastError();

            /*
             * 説明がなくてもサービスは動作するため、警告を残してインストールを続ける。
             * トレースの失敗の伝え先はないため、戻り値は明示的に破棄する。
             */
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                      "サービスの説明の設定に失敗しました (エラー コード: %lu)。", err);
        }

        /* PRESHUTDOWN の猶予を設定する (構造体に文字列を含まないため、W 版を直接使う)。 */
        preshutdown_info.dwPreshutdownTimeout = SVC_PRESHUTDOWN_TIMEOUT_MS;

        if (!ChangeServiceConfig2W(svc, SERVICE_CONFIG_PRESHUTDOWN_INFO, &preshutdown_info))
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                      "PRESHUTDOWN 猶予の設定に失敗しました (エラー コード: %lu)。", GetLastError());
        }

        /*
         * 異常終了時の自動再起動を設定する (Linux の Restart=on-failure / RestartSec=5 と同等)。
         * 文字列メンバー (lpRebootMsg / lpCommand) は NULL のため、W 版を直接使う。
         */
        SC_ACTION restart_action;
        SERVICE_FAILURE_ACTIONSW failure_actions;

        restart_action.Type = SC_ACTION_RESTART;
        restart_action.Delay = SVC_FAILURE_RESTART_DELAY_MS;
        failure_actions.dwResetPeriod = SVC_FAILURE_RESET_PERIOD_SEC;
        failure_actions.lpRebootMsg = NULL;
        failure_actions.lpCommand = NULL;

        /*
         * アクションが 1 件のみの場合、2 回目以降の失敗にも最後のアクションが繰り返される。
         * see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_failure_actionsw
         */
        failure_actions.cActions = 1;
        failure_actions.lpsaActions = &restart_action;

        if (!ChangeServiceConfig2W(svc, SERVICE_CONFIG_FAILURE_ACTIONS, &failure_actions))
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                      "自動再起動の設定に失敗しました (エラー コード: %lu)。", GetLastError());
        }

        /*
         * 既定の SCM は SERVICE_STOPPED 未報告のプロセス消滅のみを失敗とみなすため、
         * NO_ERROR 以外の終了コード報告 (例: on_start() の失敗) も失敗扱いにして、
         * Linux の Restart=on-failure (非ゼロ終了で再起動) と意味を揃える。
         * see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_failure_actions_flag
         */
        SERVICE_FAILURE_ACTIONS_FLAG failure_flag;

        failure_flag.fFailureActionsOnNonCrashFailures = TRUE;

        if (!ChangeServiceConfig2W(svc, SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, &failure_flag))
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                      "自動再起動フラグの設定に失敗しました (エラー コード: %lu)。", GetLastError());
        }

        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "サービス '%s' を登録しました。",
                                  def->name);
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "実行ファイル: %s", bin_path);
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "開始するには: sc start %s",
                                  def->name);

        /* 登録の結果は確定済みで、閉じる失敗で終了コードが変わらないため、戻り値は明示的に破棄する。 */
        (void)CloseServiceHandle(svc);

        rc = EXIT_SUCCESS;
    }

    /* 登録の結果は確定済みで、閉じる失敗で終了コードが変わらないため、戻り値は明示的に破棄する。 */
    (void)CloseServiceHandle(scm);

    return rc;
}

    /** 停止完了を待つ上限 [ms] です。 */
    #define SVC_STOP_TIMEOUT_MS 30000ULL
    /** 停止状態を問い合わせる間隔 [ms] です。 */
    #define SVC_STOP_POLL_INTERVAL_MS 50U

/**
 *  @brief          必要なら停止を要求し、停止状態を上限時間まで確認します。
 *  @param[in]      service  状態照会と停止要求が可能なサービスのハンドル。
 *  @return         停止を確認した場合は 1、照会・停止要求の失敗または時間切れは 0 を返します。
 *
 *  see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-queryservicestatusex
 */
static int wait_for_service_stop(SC_HANDLE service)
{
    SERVICE_STATUS_PROCESS status;
    DWORD bytes_needed;
    SERVICE_STATUS requested_status;
    ULONGLONG started = GetTickCount64();

    if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, (DWORD)sizeof(status), &bytes_needed))
    {
        return 0;
    }

    if ((status.dwCurrentState != SERVICE_STOPPED) && (status.dwCurrentState != SERVICE_STOP_PENDING))
    {
        if (!ControlService(service, SERVICE_CONTROL_STOP, &requested_status))
        {
            return 0;
        }
    }
    while (status.dwCurrentState != SERVICE_STOPPED)
    {
        if (GetTickCount64() - started >= SVC_STOP_TIMEOUT_MS)
        {
            return 0;
        }

        Sleep(SVC_STOP_POLL_INTERVAL_MS);

        if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, (DWORD)sizeof(status),
                                  &bytes_needed))
        {
            return 0;
        }
    }

    return 1;
}

int svc_os_uninstall(const svc_definition *def)
{
    SC_HANDLE scm = NULL;
    SC_HANDLE svc = NULL;
    int rc;
    int handled;

    rc = ensure_elevated_for_operation("uninstall", "uninstall", &handled);
    if (rc != 0 || handled != 0)
    {
        return rc;
    }

    scm = OpenSCManagerU(NULL, NULL, SC_MANAGER_CONNECT);
    if (scm == NULL)
    {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED)
        {
            /*
             * トレースは補助の記録であり、出力の失敗で処理を変えないため、
             * この関数のトレースの戻り値は明示的に破棄する。
             */
            (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                     "SCM へのアクセスが拒否されました。管理者として実行してください。");
        }
        else
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "OpenSCManagerU が失敗しました (エラー コード: %lu)。", err);
        }

        return EXIT_FAILURE;
    }

    rc = EXIT_FAILURE;

    svc = OpenServiceU(scm, def->name, SERVICE_STOP | DELETE | SERVICE_QUERY_STATUS);
    if (svc == NULL)
    {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_DOES_NOT_EXIST)
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "サービス '%s' は登録されていません。", def->name);
        }
        else if (err == ERROR_ACCESS_DENIED)
        {
            (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                     "サービスへのアクセスが拒否されました。管理者として実行してください。");
        }
        else
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "OpenServiceU が失敗しました (エラー コード: %lu)。", err);
        }
    }
    else
    {
        /*
         * 削除は停止確認後に要求する。ほかのハンドルが開いている間は、削除完了とは限らない。
         * see: https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-deleteservice
         */
        if (!wait_for_service_stop(svc))
        {
            (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                     "サービスの停止を確認できないため、削除しません。");

            /* 停止を確認できない失敗を返す経路のため、この 2 つの CloseServiceHandle の戻り値は明示的に破棄する。 */
            (void)CloseServiceHandle(svc);
            (void)CloseServiceHandle(scm);
            return EXIT_FAILURE;
        }

        if (!DeleteService(svc))
        {
            DWORD err = GetLastError();
            if (err == ERROR_SERVICE_MARKED_FOR_DELETE)
            {
                (void)cplat_tracer_writef(
                    svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                    "サービス '%s' は削除待ち状態です。Windows を再起動してから再度実行してください。", def->name);
            }
            else
            {
                (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                          "DeleteService が失敗しました (エラー コード: %lu)。", err);
            }
        }
        else
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                                      "サービス '%s' の削除を要求しました。", def->name);
            rc = EXIT_SUCCESS;
        }

        /* 削除の要求の結果は確定済みで、閉じる失敗で終了コードが変わらないため、戻り値は明示的に破棄する。 */
        (void)CloseServiceHandle(svc);
    }

    /* 終了コードは確定済みで、閉じる失敗で変わらないため、戻り値は明示的に破棄する。 */
    (void)CloseServiceHandle(scm);

    return rc;
}

#endif /* PLATFORM_WINDOWS */
