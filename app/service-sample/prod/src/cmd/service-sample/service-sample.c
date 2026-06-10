/**
 *******************************************************************************
 *  @file           service-sample.c
 *  @brief          クロスプラットフォーム サービス サンプル共通実装。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  プラットフォーム共通の処理を実装します。
 *  - 停止イベント抽象 (svc_request_stop / svc_wait_for_stop / svc_stop_requested)
 *  - ライフサイクル駆動 (svc_run_lifecycle)
 *  - 引数ディスパッチ (svc_main)
 *  - エントリ ポイント
 *
 *  プラットフォーム差異は各プラットフォーム ファイルが実装するフック関数
 *  (svc_os_install / svc_os_uninstall / svc_os_run_service / svc_os_notify_*) で吸収します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <com_util/console/console.h>
#include <com_util/crt/path.h>
#include <com_util/runtime/process.h>
#include <com_util/runtime/shutdown.h>
#include <com_util/sync/sync.h>
#include <com_util/trace/trace_file.h>

#include "service-sample.h"

/* Doxygen コメントは、ヘッダーに記載 */

/* ============================================================
 *  tracer (プロセス共通)
 * ============================================================ */

/** プロセス共通の tracer ハンドル。svc_main が open / close する。 */
static com_util_tracer *g_tracer = NULL;

com_util_tracer *svc_get_tracer(void)
{
    return g_tracer;
}

/**
 *  @brief          tracer を生成・設定してプロバイダーを開始します。
 *  @param[in]      def            サービス定義。NULL を渡してはなりません。
 *  @param[in]      enable_stderr  stderr バックエンドを有効にする場合は 0 以外を渡します。
 *  @return         成功時は 0、失敗時は -1 を返します。
 *
 *  失敗時は g_tracer = NULL のまま返します。呼び出し元は失敗時も処理を継続し、
 *  tracer 出力が無効化されるだけの状態として扱ってください。
 */
static int svc_tracer_open(const svc_definition *def, const int enable_stderr)
{
    char exe_path[PLATFORM_PATH_MAX];
    char log_dir[PLATFORM_PATH_MAX];
    char log_path[PLATFORM_PATH_MAX];
    com_util_trace_level_t stderr_level;
    char *sep;

    g_tracer = com_util_tracer_create();
    if (g_tracer == NULL)
    {
        return -1;
    }

    com_util_tracer_set_name(g_tracer, def->name, 0);
    com_util_tracer_set_os_level(g_tracer, COM_UTIL_TRACE_LEVEL_NONE);

    if (enable_stderr != 0)
    {
        stderr_level = COM_UTIL_TRACE_LEVEL_INFO;
    }
    else
    {
        stderr_level = COM_UTIL_TRACE_LEVEL_NONE;
    }
    com_util_tracer_set_stderr_level(g_tracer, stderr_level);

    /* 実行ファイルと同じディレクトリの log/ サブディレクトリにファイル出力する */
    if (com_util_process_get_executable_path(exe_path, sizeof(exe_path)) == 0)
    {
        sep = strrchr(exe_path, PLATFORM_PATH_SEP_CHR);
        if (sep != NULL)
        {
            *sep = '\0'; /* バッファーをディレクトリ部分のみに切り詰める */
            if (com_util_path_concat(log_dir, sizeof(log_dir), NULL, exe_path, PLATFORM_PATH_SEP, "log") == 0)
            {
                /* ディレクトリ生成は com_util_trace_file_sink_create が自動実行する */
                if (com_util_path_concat(log_path, sizeof(log_path), NULL, log_dir, PLATFORM_PATH_SEP, def->name,
                                         ".log") == 0)
                {
                    /* サービスとコンソールの複数プロセスが同一ログへ書き込むため共有モードにする */
                    com_util_tracer_set_file_level(g_tracer, log_path, COM_UTIL_TRACE_LEVEL_VERBOSE, 0, 0,
                                                   COM_UTIL_TRACE_FILE_SINK_SHARED);
                }
            }
        }
    }

    return com_util_tracer_start(g_tracer);
}

/**
 *  @brief  tracer を停止・解放します。
 *
 *  g_tracer が NULL の場合は何もしません。
 */
static void svc_tracer_close(void)
{
    if (g_tracer != NULL)
    {
        com_util_tracer_stop(g_tracer);
        com_util_tracer_dispose(g_tracer);
        g_tracer = NULL;
    }
}

/* ============================================================
 *  停止イベント抽象 (内部状態)
 * ============================================================ */

/** 停止要求の有無。1 = 停止要求済み。 */
static volatile int g_stop_requested = 0;
/** 停止フラグ・condvar を保護するミューテックス。 */
static com_util_local_lock *g_stop_lock = NULL;
/** 停止要求を on_run に通知するための条件変数。 */
static com_util_condvar *g_stop_cv = NULL;

void svc_request_stop(void)
{
    if (g_stop_lock == NULL || g_stop_cv == NULL)
    {
        return;
    }
    com_util_local_lock_lock(g_stop_lock, COM_UTIL_SYNC_WAIT_FOREVER);
    g_stop_requested = 1;
    com_util_local_lock_unlock(g_stop_lock);
    com_util_condvar_broadcast(g_stop_cv);
}

int svc_stop_requested(void)
{
    int requested;

    if (g_stop_lock == NULL)
    {
        return 0;
    }
    com_util_local_lock_lock(g_stop_lock, COM_UTIL_SYNC_WAIT_FOREVER);
    requested = g_stop_requested;
    com_util_local_lock_unlock(g_stop_lock);
    return requested;
}

int svc_wait_for_stop(const int timeout_ms)
{
    int requested;

    if (g_stop_lock == NULL || g_stop_cv == NULL)
    {
        return 1;
    }
    com_util_local_lock_lock(g_stop_lock, COM_UTIL_SYNC_WAIT_FOREVER);
    if (g_stop_requested == 0)
    {
        /* spurious wakeup 対策のため待機後に再確認する */
        com_util_condvar_wait(g_stop_cv, g_stop_lock, timeout_ms);
    }
    requested = g_stop_requested;
    com_util_local_lock_unlock(g_stop_lock);
    return requested;
}

/* ============================================================
 *  状態通知 API
 * ============================================================ */

/* Doxygen コメントは、ヘッダーに記載 */

void svc_set_status_text(const char *text)
{
    if (text == NULL)
    {
        return;
    }
    com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_VERBOSE, NULL, "状態テキスト: %s", text);
    svc_os_notify_status(text);
}

/* ============================================================
 *  OS イベント配送 (プラットフォーム ファイルから呼ばれる)
 * ============================================================ */

/* Doxygen コメントは、ヘッダーに記載 */

void svc_dispatch_event(const svc_definition *def, const svc_event_info *info)
{
    if (def == NULL || info == NULL || def->on_event == NULL)
    {
        return;
    }
    com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "OS イベントを配送します (種別: %d)。",
                           (int)info->type);
    def->on_event(info, def->user_data);
}

void svc_dispatch_reload(const svc_definition *def)
{
    if (def == NULL || def->on_reload == NULL)
    {
        return;
    }
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "設定再読込要求を配送します。");
    svc_os_notify_reloading();
    def->on_reload(def->user_data);
    svc_os_notify_ready();
}

/* ============================================================
 *  停止要求 callback (shutdown.h 経由で呼ばれる)
 * ============================================================ */

/**
 *  @brief          終了要求 callback。
 *  @param[in]      event   終了イベント情報。
 *  @param[in]      context 未使用。
 *
 *  SIGINT / SIGTERM (Linux) または SetConsoleCtrlHandler (Windows) の
 *  いずれかが補足されると shutdown.h 経由でこの callback が呼ばれます。
 *  svc_request_stop() を呼んで on_run のメインループを停止させます。
 */
static void svc_shutdown_request_callback(const com_util_shutdown_event *event, void *context)
{
    (void)event;
    (void)context;
    svc_request_stop();
}

/* ============================================================
 *  ライフサイクル駆動 (console モードと Linux run モードが共用)
 * ============================================================ */

/* Doxygen コメントは、ヘッダーに記載 */

int svc_run_lifecycle(const svc_definition *def)
{
    int rc;

    com_util_console_init();
    com_util_shutdown_request_register(svc_shutdown_request_callback, NULL);

    rc = 0;
    if (def->on_start != NULL)
    {
        rc = def->on_start(def->user_data);
        if (rc != 0)
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "on_start が失敗しました (戻り値: %d)。", rc);
        }
    }

    if (rc == 0)
    {
        int run_rc;
        int stop_rc;

        svc_os_notify_ready();

        run_rc = def->on_run(def->user_data);
        if (run_rc != 0)
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                   "on_run が失敗しました (戻り値: %d)。", run_rc);
        }

        svc_os_notify_stopping();

        /* on_run が失敗しても後始末のため on_stop は実行する */
        stop_rc = 0;
        if (def->on_stop != NULL)
        {
            stop_rc = def->on_stop(def->user_data);
            if (stop_rc != 0)
            {
                com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                                       "on_stop が失敗しました (戻り値: %d)。", stop_rc);
            }
        }

        /* 最初に失敗したコールバックの戻り値を採用する */
        if (run_rc != 0)
        {
            rc = run_rc;
        }
        else
        {
            rc = stop_rc;
        }
    }

    return rc;
}

/* ============================================================
 *  使い方の表示
 * ============================================================ */

/**
 *  @brief          使い方を表示します。
 *  @param[in]      prog_name プログラム名。
 */
static void print_usage(const char *prog_name)
{
    com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "使い方: %s <コマンド>", prog_name);
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "コマンド:");
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "  install    OS にサービスを登録します");
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                          "  uninstall  OS からサービスを解除します");
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                          "  run        サービスとして常駐起動します (SCM/systemd から呼ばれます)");
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                          "  console    フォアグラウンドで実行します (デバッグ用)");
}

/* ============================================================
 *  エントリ ポイント
 * ============================================================ */

int svc_main(const int argc, char *argv[], const svc_definition *def)
{
    int rc;
    int is_service_mode;

    /* run コマンドのみサービス モード: SCM/systemd 起動のため stderr は無効化する */
    is_service_mode = (argc >= 2 && strcmp(argv[1], "run") == 0);
    svc_tracer_open(def, !is_service_mode); /* 失敗しても g_tracer=NULL で継続 */

    /* 停止イベント抽象の初期化 */
    if (com_util_local_lock_create(&g_stop_lock) != COM_UTIL_SYNC_OK)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                              "ミューテックスの生成に失敗しました。");
        svc_tracer_close();
        return EXIT_FAILURE;
    }
    if (com_util_condvar_create(&g_stop_cv) != COM_UTIL_SYNC_OK)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "条件変数の生成に失敗しました。");
        com_util_local_lock_destroy(g_stop_lock);
        g_stop_lock = NULL;
        svc_tracer_close();
        return EXIT_FAILURE;
    }

    rc = EXIT_FAILURE;

    if (argc < 2)
    {
        print_usage(argv[0]);
    }
    else if (strcmp(argv[1], "install") == 0)
    {
        rc = svc_os_install(def);
    }
    else if (strcmp(argv[1], "uninstall") == 0)
    {
        rc = svc_os_uninstall(def);
    }
    else if (strcmp(argv[1], "run") == 0)
    {
        rc = svc_os_run_service(def);
    }
    else if (strcmp(argv[1], "console") == 0)
    {
        rc = svc_run_lifecycle(def);
    }
    else
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "不明なコマンド '%s'", argv[1]);
        print_usage(argv[0]);
    }

    com_util_condvar_destroy(g_stop_cv);
    g_stop_cv = NULL;
    com_util_local_lock_destroy(g_stop_lock);
    g_stop_lock = NULL;

    svc_tracer_close();
    return rc;
}

/* ============================================================
 *  エントリ ポイント
 * ============================================================ */

/** サービス定義。実装側 (service-sample-impl.c) が定義する。 */
extern const svc_definition g_service_def;

/**
 *  @brief          メイン エントリ ポイント。
 *  @param[in]      argc コマンド ライン引数の数。
 *  @param[in]      argv コマンド ライン引数の配列。
 *  @return         正常終了時は 0、異常終了時は 0 以外を返します。
 */
int main(int argc, char *argv[])
{
    com_util_console_init();

    return svc_main(argc, argv, &g_service_def);
}
