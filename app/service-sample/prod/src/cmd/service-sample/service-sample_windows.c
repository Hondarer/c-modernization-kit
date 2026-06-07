/**
 *******************************************************************************
 *  @file           service-sample_windows.c
 *  @brief          クロスプラットフォーム サービス サンプル Windows 実装。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  Windows 固有のサービス処理を実装します。
 *  - svc_os_run_service : SCM ディスパッチャーへの接続
 *  - svc_os_install     : CreateService による SCM 登録
 *  - svc_os_uninstall   : DeleteService による SCM 解除
 *
 *  共通処理 (svc_run_lifecycle / svc_main / コールバック雛形) は
 *  service-sample.c に実装します。
 *
 *  内部起動パターン (ユーザーは直接使わない):
 *  - `service-sample run` : SCM から起動されるサービス本体
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_WINDOWS)

    #include <com_util/base/windows_sdk.h>
/* Advapi32.lib は windows_sdk.h の #pragma comment(lib, "Advapi32.lib") で指定済み */

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include <com_util/runtime/process.h>

    #include "service-sample.h"

/* Doxygen コメントは、ヘッダーに記載 */

/* ============================================================
 *  内部状態 (SCM ディスパッチャーへの受け渡し)
 * ============================================================ */

/** ServiceMain からアクセスできるようにファイル static に保持するサービス定義。 */
static const svc_definition *g_def = NULL;
/** サービス ステータス ハンドル。ServiceMain で初期化される。 */
static SERVICE_STATUS_HANDLE g_status_handle = NULL;
/** 現在のサービス ステータス。 */
static SERVICE_STATUS g_status = {0};

/* ============================================================
 *  内部ヘルパー
 * ============================================================ */

/**
 *  @brief          サービス ステータスを設定して SCM に通知します。
 *  @param[in]      state       新しいサービス状態 (SERVICE_RUNNING など)。
 *  @param[in]      controls    dwControlsAccepted (受け付けるコントロール)。
 *  @param[in]      checkpoint  進行状況カウンター (起動/停止中に使う)。
 *  @param[in]      wait_hint   次のチェックポイントまでの予想時間 (ミリ秒)。
 */
static void set_service_status(const DWORD state, const DWORD controls, const DWORD checkpoint, const DWORD wait_hint)
{
    g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_status.dwCurrentState = state;
    g_status.dwControlsAccepted = controls;
    g_status.dwWin32ExitCode = NO_ERROR;
    g_status.dwServiceSpecificExitCode = 0;
    g_status.dwCheckPoint = checkpoint;
    g_status.dwWaitHint = wait_hint;
    SetServiceStatus(g_status_handle, &g_status);
}

/* ============================================================
 *  サービス コントロール ハンドラー
 * ============================================================ */

/**
 *  @brief          SCM からのコントロール要求を処理します。
 *  @param[in]      ctrl    コントロール コード (SERVICE_CONTROL_STOP など)。
 *  @param[in]      ev_type イベント種別 (通常は 0)。
 *  @param[in]      ev_data イベント データ (通常は NULL)。
 *  @param[in]      context 未使用。
 *  @return         常に NO_ERROR を返します。
 */
static DWORD WINAPI service_ctrl_handler(DWORD ctrl, DWORD ev_type, LPVOID ev_data, LPVOID context)
{
    (void)ev_type;
    (void)ev_data;
    (void)context;

    if (ctrl == SERVICE_CONTROL_STOP || ctrl == SERVICE_CONTROL_SHUTDOWN)
    {
        set_service_status(SERVICE_STOP_PENDING, 0, 1, 5000);
        /* 停止要求を on_run のメインループに伝える */
        svc_request_stop();
    }
    else if (ctrl == SERVICE_CONTROL_INTERROGATE)
    {
        /* 現在のステータスを再通知する */
        SetServiceStatus(g_status_handle, &g_status);
    }

    return NO_ERROR;
}

/* ============================================================
 *  ServiceMain (SCM ディスパッチャーから呼ばれる)
 * ============================================================ */

/**
 *  @brief          サービス メイン関数 (SCM ディスパッチャーから呼ばれる)。
 *  @param[in]      argc    サービス引数の数。
 *  @param[in]      argv    サービス引数の配列。
 */
static VOID WINAPI service_main(DWORD argc, LPTSTR *argv)
{
    (void)argc;
    (void)argv;

    if (g_def == NULL)
    {
        return;
    }

    /* コントロール ハンドラーを登録する */
    g_status_handle = RegisterServiceCtrlHandlerEx(g_def->name, service_ctrl_handler, NULL);
    if (g_status_handle == NULL)
    {
        return;
    }

    /* 起動中を通知する */
    set_service_status(SERVICE_START_PENDING, 0, 1, 3000);

    /* on_start を呼ぶ */
    if (g_def->on_start != NULL)
    {
        if (g_def->on_start(g_def->user_data) != 0)
        {
            g_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
            g_status.dwServiceSpecificExitCode = 1;
            set_service_status(SERVICE_STOPPED, 0, 0, 0);
            return;
        }
    }

    /* 起動完了を通知する */
    set_service_status(SERVICE_RUNNING, SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN, 0, 0);

    /* on_run を呼ぶ (停止要求まで戻らない) */
    g_def->on_run(g_def->user_data);

    /* 停止中を通知する */
    set_service_status(SERVICE_STOP_PENDING, 0, 1, 3000);

    /* on_stop を呼ぶ */
    if (g_def->on_stop != NULL)
    {
        g_def->on_stop(g_def->user_data);
    }

    /* 停止完了を通知する */
    set_service_status(SERVICE_STOPPED, 0, 0, 0);
}

/* ============================================================
 *  OS フック実装
 * ============================================================ */

int svc_os_run_service(const svc_definition *def)
{
    SERVICE_TABLE_ENTRY dispatch_table[2];

    /* def をファイル static に退避する (ServiceMain は固定シグネチャのため直接渡せない) */
    g_def = def;

    dispatch_table[0].lpServiceName = (LPTSTR)def->name;
    dispatch_table[0].lpServiceProc = service_main;
    dispatch_table[1].lpServiceName = NULL;
    dispatch_table[1].lpServiceProc = NULL;

    if (!StartServiceCtrlDispatcher(dispatch_table))
    {
        DWORD err = GetLastError();
        if (err == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
        {
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "SCM への接続に失敗しました。");
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                  "サービスとして登録された後、SCM から 'run' 引数で起動してください。");
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                  "コンソールで実行するには 'console' コマンドを使用してください。");
        }
        else
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "StartServiceCtrlDispatcher が失敗しました (エラー コード: %lu)。", err);
        }
        return EXIT_FAILURE;
    }
    return 0;
}

int svc_os_install(const svc_definition *def)
{
    SC_HANDLE scm = NULL;
    SC_HANDLE svc = NULL;
    char bin_path[4096 + 16]; /* パスに "\"<path>\" run" を格納するための十分なサイズ */
    char exe_path[4096];
    SERVICE_DESCRIPTION desc;
    int rc;

    /* 実行ファイルの絶対パスを取得する */
    if (com_util_process_get_executable_path(exe_path, sizeof(exe_path)) != 0)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                              "実行ファイルのパスを取得できませんでした。");
        return EXIT_FAILURE;
    }

    /* binPath は "\"<パス>\" run" 形式にする (パスにスペースが含まれる場合の対策) */
    if (snprintf(bin_path, sizeof(bin_path), "\"%s\" run", exe_path) < 0)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "パスの生成に失敗しました。");
        return EXIT_FAILURE;
    }

    /* SCM に接続する */
    scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (scm == NULL)
    {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED)
        {
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                  "SCM へのアクセスが拒否されました。管理者として実行してください。");
        }
        else
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "OpenSCManager が失敗しました (エラー コード: %lu)。", err);
        }
        return EXIT_FAILURE;
    }

    rc = EXIT_FAILURE;

    /* サービスを登録する */
    svc = CreateServiceA(scm, def->name, def->display_name, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
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
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "サービス '%s' は既に登録されています。", def->name);
        }
        else if (err == ERROR_ACCESS_DENIED)
        {
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                  "サービスの登録が拒否されました。管理者として実行してください。");
        }
        else
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "CreateService が失敗しました (エラー コード: %lu)。", err);
        }
    }
    else
    {
        /* 説明文を設定する */
        desc.lpDescription = (LPSTR)def->description;
        ChangeServiceConfig2(svc, SERVICE_CONFIG_DESCRIPTION, &desc);

        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サービス '%s' を登録しました。",
                               def->name);
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "実行ファイル: %s", bin_path);
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "開始するには: sc start %s",
                               def->name);
        CloseServiceHandle(svc);
        rc = 0;
    }

    CloseServiceHandle(scm);
    return rc;
}

int svc_os_uninstall(const svc_definition *def)
{
    SC_HANDLE scm = NULL;
    SC_HANDLE svc = NULL;
    SERVICE_STATUS status;
    int rc;

    /* SCM に接続する */
    scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scm == NULL)
    {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED)
        {
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                  "SCM へのアクセスが拒否されました。管理者として実行してください。");
        }
        else
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "OpenSCManager が失敗しました (エラー コード: %lu)。", err);
        }
        return EXIT_FAILURE;
    }

    rc = EXIT_FAILURE;

    svc = OpenServiceA(scm, def->name, SERVICE_STOP | DELETE | SERVICE_QUERY_STATUS);
    if (svc == NULL)
    {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_DOES_NOT_EXIST)
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "サービス '%s' は登録されていません。", def->name);
        }
        else if (err == ERROR_ACCESS_DENIED)
        {
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                  "サービスへのアクセスが拒否されました。管理者として実行してください。");
        }
        else
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "OpenService が失敗しました (エラー コード: %lu)。", err);
        }
    }
    else
    {
        /* 動作中の場合は停止する */
        if (ControlService(svc, SERVICE_CONTROL_STOP, &status))
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                                   "サービス '%s' を停止しています...", def->name);
            Sleep(1000);
        }

        /* サービスを削除する */
        if (!DeleteService(svc))
        {
            DWORD err = GetLastError();
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "DeleteService が失敗しました (エラー コード: %lu)。", err);
        }
        else
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サービス '%s' を解除しました。",
                                   def->name);
            rc = 0;
        }

        CloseServiceHandle(svc);
    }

    CloseServiceHandle(scm);
    return rc;
}

#endif /* PLATFORM_WINDOWS */
