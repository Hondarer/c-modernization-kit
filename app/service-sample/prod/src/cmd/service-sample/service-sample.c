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
 *  - サービス定義・コールバック雛形・エントリ ポイント
 *
 *  プラットフォーム差異は各プラットフォーム ファイルが実装するフック関数
 *  (svc_os_install / svc_os_uninstall / svc_os_run_service) で吸収します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <com_util/console/console.h>
#include <com_util/runtime/shutdown.h>
#include <com_util/sync/sync.h>

#include "service-sample.h"

/* Doxygen コメントは、ヘッダーに記載 */

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

/**
 *  @brief          ライフサイクル コールバックを駆動します。
 *  @param[in]      def サービス定義。
 *  @return         成功時は 0、on_start が失敗した場合はその戻り値を返します。
 *
 *  console モードおよび Linux run モード (Type=simple) の共通実装です。\n
 *  shutdown.h の request callback を登録して SIGINT/SIGTERM を補足し、
 *  on_start → on_run → on_stop の順でライフサイクルを駆動します。
 */
int svc_run_lifecycle(const svc_definition *def)
{
    int rc;

    com_util_console_init();
    com_util_shutdown_request_register(svc_shutdown_request_callback, NULL);

    rc = 0;
    if (def->on_start != NULL)
    {
        rc = def->on_start(def->user_data);
    }

    if (rc == 0)
    {
        def->on_run(def->user_data);

        if (def->on_stop != NULL)
        {
            def->on_stop(def->user_data);
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
    fprintf(stderr, "使い方: %s <コマンド>\n", prog_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "コマンド:\n");
    fprintf(stderr, "  install    OS にサービスを登録します\n");
    fprintf(stderr, "  uninstall  OS からサービスを解除します\n");
    fprintf(stderr, "  run        サービスとして常駐起動します (SCM/systemd から呼ばれます)\n");
    fprintf(stderr, "  console    フォアグラウンドで実行します (デバッグ用)\n");
}

/* ============================================================
 *  エントリ ポイント
 * ============================================================ */

int svc_main(const int argc, char *argv[], const svc_definition *def)
{
    int rc;

    /* 停止イベント抽象の初期化 */
    if (com_util_local_lock_create(&g_stop_lock) != COM_UTIL_SYNC_OK)
    {
        fprintf(stderr, "エラー: ミューテックスの生成に失敗しました。\n");
        return EXIT_FAILURE;
    }
    if (com_util_condvar_create(&g_stop_cv) != COM_UTIL_SYNC_OK)
    {
        fprintf(stderr, "エラー: 条件変数の生成に失敗しました。\n");
        com_util_local_lock_destroy(g_stop_lock);
        g_stop_lock = NULL;
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
        fprintf(stderr, "エラー: 不明なコマンド '%s'\n\n", argv[1]);
        print_usage(argv[0]);
    }

    com_util_condvar_destroy(g_stop_cv);
    g_stop_cv = NULL;
    com_util_local_lock_destroy(g_stop_lock);
    g_stop_lock = NULL;

    return rc;
}

/* ============================================================
 *  サービス コールバック雛形
 * ============================================================ */

/**
 *  @brief          サービス初期化処理の雛形。
 *  @param[in]      user_data 未使用。
 *  @return         常に 0 (成功) を返します。
 */
static int on_start(void *user_data)
{
    (void)user_data;
    printf("[service-sample] 起動しました。\n");
    /* TODO: ここに初期化処理を書く */
    return 0;
}

/**
 *  @brief          サービス メインループの雛形。
 *  @param[in]      user_data 未使用。
 *
 *  1 秒ごとに動作ログを出力し、停止要求を受け取るとループを抜けます。
 */
static void on_run(void *user_data)
{
    (void)user_data;
    printf("[service-sample] 動作中です。Ctrl+C または停止コマンドで終了します。\n");

    while (svc_wait_for_stop(1000) == 0)
    {
        /* TODO: ここに周期処理を書く (現状は何もしない雛形) */
        printf("[service-sample] 動作中...\n");
    }
}

/**
 *  @brief          サービス後処理の雛形。
 *  @param[in]      user_data 未使用。
 */
static void on_stop(void *user_data)
{
    (void)user_data;
    printf("[service-sample] 停止しました。\n");
    /* TODO: ここに後処理を書く */
}

/* ============================================================
 *  サービス定義・エントリ ポイント
 * ============================================================ */

/** サービス定義。 */
static const svc_definition g_service_def = {"service-sample",
                                             "C Modernization Service Sample",
                                             "c-modernization-kit のクロスプラットフォーム サービス サンプルです。",
                                             on_start,
                                             on_run,
                                             on_stop,
                                             NULL};

/**
 *  @brief          メイン エントリ ポイント。
 *  @param[in]      argc コマンド ライン引数の数。
 *  @param[in]      argv コマンド ライン引数の配列。
 *  @return         正常終了時は 0、異常終了時は 0 以外を返します。
 */
int main(int argc, char *argv[])
{
    return svc_main(argc, argv, &g_service_def);
}
