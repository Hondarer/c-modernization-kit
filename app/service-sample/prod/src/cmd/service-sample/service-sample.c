/**
 *******************************************************************************
 *  @file           service-sample.c
 *  @brief          クロスプラットフォーム サービスの共通処理を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  プラットフォーム共通の処理を実装します。
 *  - 停止イベント抽象 (svc_request_stop / svc_wait_for_stop / svc_stop_requested)
 *  - ライフサイクル駆動 (svc_run_lifecycle)
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

#include <cplat/console/console.h>
#include <cplat/runtime/shutdown.h>
#include <cplat/sync/sync.h>
#include <cplat/trace/trace_file.h>
#include <cplat/argparser/argparser.h>

#include "service-sample.h"

/* Doxygen コメントは、ヘッダーに記載 */

/*
 * ============================================================
 *  tracer (プロセス共通)
 * ============================================================
 */

/** プロセス共通の tracer ハンドル。main() が open / close します。 */
static cplat_tracer *s_tracer = NULL;

cplat_tracer *svc_get_tracer(void)
{
    return s_tracer;
}

/**
 *  @brief          tracer を生成・設定してプロバイダーを開始します。
 *  @param[in]      def            サービス定義。NULL を渡してはなりません。
 *  @param[in]      enable_stderr  stderr バックエンドを有効にする場合は 0 以外を渡します。
 *  @return         成功時は 0、失敗時は -1 を返します。
 *
 *  失敗時は s_tracer = NULL のまま返します。呼び出し元は失敗時も処理を継続し、
 *  tracer 出力が無効化されるだけの状態として扱ってください。
 */
static int tracer_open(const svc_definition *def, const int enable_stderr)
{
    cplat_trace_level stderr_level;
    int file_result;

    s_tracer =cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    if (s_tracer == NULL)
    {
        return -1;
    }

    /*
     * 名前は OS トレースと ETW の識別にだけ使い、ファイルと標準エラー出力のトレースには影響しない (tracer.h) ため、
     * 戻り値は明示的に破棄する。
     */
    (void)cplat_tracer_set_name(s_tracer, def->name, 0);

    /* 生成済みのハンドルと範囲内のレベルを渡し、tracer.h の失敗の条件に当たらないため、戻り値は明示的に破棄する。 */
    (void)cplat_tracer_set_os_level(s_tracer, CPLAT_TRACE_LEVEL_NONE);

    if (enable_stderr != 0)
    {
        stderr_level = CPLAT_TRACE_LEVEL_INFO;
    }
    else
    {
        stderr_level = CPLAT_TRACE_LEVEL_NONE;
    }

    /* 生成済みのハンドルと範囲内のレベルを渡し、tracer.h の失敗の条件に当たらないため、戻り値は明示的に破棄する。 */
    (void)cplat_tracer_set_stderr_level(s_tracer, stderr_level);

    /*
     * Windows で一般ユーザーによる実行の場合、昇格が発生し同じファイルに複数プロセスが
     * 出力するため、共有モードを指定する。パスとファイル名は cplat tracer の既定の解決に任せる。
     * 設定できない場合はファイルへのトレースだけが無効になるため、診断を出して開始へ進む。
     */
    file_result =
        cplat_tracer_set_file_level(s_tracer, NULL, CPLAT_TRACE_LEVEL_VERBOSE, 0, 0, CPLAT_TRACE_FILE_SINK_SHARED);
    if (file_result != CPLAT_OK)
    {
        /* 診断の出力先がほかにないため、戻り値は明示的に破棄する。 */
        (void)fprintf(stderr, "ファイルへのトレースを設定できませんでした (結果コード: %d)。\n", file_result);
    }

    return cplat_tracer_start(s_tracer);
}

/**
 *  @brief  tracer を停止・解放します。
 *
 *  s_tracer が NULL の場合は何もしません。
 */
static void tracer_close(void)
{
    if (s_tracer != NULL)
    {
        /* NULL でないことを確認済みであり、tracer.h の失敗の条件 (NULL) に当たらないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_stop(s_tracer);
        cplat_tracer_dispose(&s_tracer);
        s_tracer = NULL;
    }
}

/*
 * ============================================================
 *  停止イベント抽象 (内部状態)
 * ============================================================
 */

/** 停止要求の有無。1 = 停止要求済み。 */
static int s_stop_requested = 0;

/** 停止フラグ・condvar を保護するミューテックス。 */
static cplat_local_lock *s_stop_lock = NULL;

/** 停止要求を on_run に通知するための条件変数。 */
static cplat_condvar *s_stop_cv = NULL;

void svc_request_stop(void)
{
    if (s_stop_lock == NULL || s_stop_cv == NULL)
    {
        return;
    }

    cplat_local_lock_lock(s_stop_lock);
    s_stop_requested = 1;
    cplat_local_lock_unlock(s_stop_lock);
    cplat_condvar_broadcast(s_stop_cv);
}

int svc_stop_requested(void)
{
    int requested;

    if (s_stop_lock == NULL)
    {
        return 0;
    }

    cplat_local_lock_lock(s_stop_lock);
    requested = s_stop_requested;
    cplat_local_lock_unlock(s_stop_lock);

    return requested;
}

int svc_wait_for_stop(const int timeout_ms)
{
    int requested;
    int wait_ms;
    int wait_result;

    if (s_stop_lock == NULL || s_stop_cv == NULL)
    {
        return 1;
    }

    /* 負の時間は待機の関数が失敗として扱うため、ヘッダーの記載どおりタイムアウト 0 として扱う。 */
    wait_ms = timeout_ms;
    if (wait_ms < 0)
    {
        wait_ms = 0;
    }

    wait_result = CPLAT_OK;
    cplat_local_lock_lock(s_stop_lock);

    if (s_stop_requested == 0)
    {
        /* spurious wakeup 対策のため待機後に再確認する。 */
        wait_result = cplat_condvar_timed_wait(s_stop_cv, s_stop_lock, wait_ms);
    }

    requested = s_stop_requested;
    cplat_local_lock_unlock(s_stop_lock);

    /*
     * 待機に失敗した場合は、待たずに戻る呼び出しが周期処理を間隔なしで回し続けないよう、
     * 停止の要求の状態は変えずに 1 を返して周期処理を抜けさせる。
     */
    if (wait_result != CPLAT_OK && wait_result != CPLAT_ERR_TIMEOUT)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "停止要求の待機に失敗しました (結果コード: %d)。", wait_result);
        return 1;
    }

    return requested;
}

/*
 * ============================================================
 *  状態通知 API
 * ============================================================
 */

/* Doxygen コメントは、ヘッダーに記載 */

void svc_set_status_text(const char *text)
{
    if (text == NULL)
    {
        return;
    }

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_VERBOSE, NULL, "状態テキスト: %s", text);
    svc_os_notify_status(text);
}

/*
 * ============================================================
 *  OS イベント配送 (プラットフォーム ファイルから呼ばれる)
 * ============================================================
 */

/* Doxygen コメントは、ヘッダーに記載 */

void svc_dispatch_event(const svc_definition *def, const svc_event_info *info)
{
    if (def == NULL || info == NULL || def->on_event == NULL)
    {
        return;
    }

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "OS イベントを配送します (種別: %d)。",
                              (int)info->type);
    def->on_event(info, def->user_data);
}

void svc_dispatch_reload(const svc_definition *def)
{
    if (def == NULL || def->on_reload == NULL)
    {
        return;
    }

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "設定再読込要求を配送します。");
    svc_os_notify_reloading();
    def->on_reload(def->user_data);
    svc_os_notify_ready();
}

/*
 * ============================================================
 *  停止要求 callback (shutdown.h 経由で呼ばれる)
 * ============================================================
 */

/**
 *  @brief          終了要求 callback。
 *  @param[in]      event   終了イベント情報。
 *  @param[in]      context 未使用。
 *
 *  SIGINT / SIGTERM (Linux) または SetConsoleCtrlHandler (Windows) の
 *  いずれかが補足されると shutdown.h 経由でこの callback が呼ばれます。
 *  svc_request_stop() を呼んで on_run のメイン ループを停止させます。
 */
static void shutdown_request_callback(const cplat_shutdown_event *event, void *context)
{
    (void)event;
    (void)context;
    svc_request_stop();
}

/*
 * ============================================================
 *  ライフサイクル駆動 (console モードと Linux run モードが共用)
 * ============================================================
 */

/* Doxygen コメントは、ヘッダーに記載 */

int svc_run_lifecycle(const svc_definition *def)
{
    int rc;

    cplat_console_init();

    /* 停止の要求を受け取れないとサービスを停止できないため、登録に失敗した場合は開始しない。 */
    rc = cplat_shutdown_request_register(shutdown_request_callback, NULL);
    if (rc != CPLAT_OK)
    {
        /*
         * トレースは補助の記録であり、出力の失敗で処理を変えないため、
         * この関数のトレースの戻り値は明示的に破棄する。
         */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "停止要求の登録に失敗しました (結果コード: %d)。", rc);
        return EXIT_FAILURE;
    }

    rc = EXIT_SUCCESS;

    /* 初期化コールバックが設定されている場合は実行する。 */
    if (def->on_start != NULL)
    {
        rc = def->on_start(def->user_data);
        if (rc != EXIT_SUCCESS)
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "on_start が失敗しました (戻り値: %d)。", rc);
        }
    }

    /* 初期化に成功した場合はメイン ループと停止処理を実行する。 */
    if (rc == EXIT_SUCCESS)
    {
        int run_rc;
        int stop_rc;

        svc_os_notify_ready();

        run_rc = def->on_run(def->user_data);
        if (run_rc != EXIT_SUCCESS)
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "on_run が失敗しました (戻り値: %d)。", run_rc);
        }

        svc_os_notify_stopping();

        /* on_run が失敗しても後始末のため on_stop は実行する。 */
        stop_rc = EXIT_SUCCESS;

        if (def->on_stop != NULL)
        {
            stop_rc = def->on_stop(def->user_data);
            if (stop_rc != EXIT_SUCCESS)
            {
                (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                          "on_stop が失敗しました (戻り値: %d)。", stop_rc);
            }
        }

        /* 最初に失敗したコールバックの戻り値を採用する。 */
        if (run_rc != EXIT_SUCCESS)
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

/** サービス定義。実装側 (service-sample-impl.c) が定義します。 */
extern const svc_definition g_service_def;

/**
 *  @brief          メイン エントリ ポイント。
 *  @param[in]      argc コマンド ライン引数の数。
 *  @param[in]      argv コマンド ライン引数の配列。
 *  @return         正常終了時は 0、異常終了時は 0 以外を返します。
 *
 *  command に応じて次の処理を実行します。\n
 *  - install   : OS にサービスを登録します。\n
 *  - uninstall : OS からサービスを解除します。\n
 *  - run       : サービスとして常駐起動します (SCM/systemd から呼ばれる)。\n
 *  - console   : フォアグラウンドで実行します (デバッグ用)。
 */
int main(int argc, char *argv[])
{
    /*
     * 昇格起動された場合、親コンソールへ再接続して出力を元のコンソールへ戻す。
     * 引き継ぎフラグを argv から取り除く必要があるため、引数解析より前に呼び出す。
     * 再接続できない場合もフラグは取り除かれ、出力が元のコンソールへ戻らないだけで処理を続けられるため、
     * 戻り値は明示的に破棄する。
     */
    (void)cplat_console_attach_parent(&argc, argv, NULL);

    cplat_console_init();

    int need_help = 0;
    const char *command = NULL;

    cplat_argparser_init(argc, argv, "サービスの登録、削除、起動を行います。");

    /* 登録の失敗は cplat_argparser_get_register_error_count() でまとめて検査するため、戻り値は明示的に破棄する。 */
    (void)cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します。", &need_help);
    (void)cplat_argparser_register_positional_string("command", "install、uninstall、run、console のいずれか。",
                                                     CPLAT_ARGPARSER_REQUIRED, &command);

    if (cplat_argparser_get_register_error_count() > 0)
    {
        /* 終了コードはすでに失敗に決まっているため、戻り値は明示的に破棄する。 */
        (void)cplat_argparser_print_register_error_messages(stderr);
        return EXIT_FAILURE;
    }

    int parse_result = cplat_argparser_parse();

    if (need_help != 0)
    {
        if (cplat_argparser_print_usage(stdout) != CPLAT_OK || fflush(stdout) != 0 || ferror(stdout) != 0)
        {
            /* 診断の出力先がほかにないため、戻り値は明示的に破棄する。 */
            (void)fprintf(stderr, "エラー: 使い方を標準出力へ書き込めませんでした。\n");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    if (parse_result != CPLAT_OK)
    {
        /* 終了コードはすでに失敗に決まっているため、この 2 つの表示の関数の戻り値は明示的に破棄する。 */
        (void)cplat_argparser_print_error_messages(stderr);
        (void)cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    if (strcmp(command, "install") != 0 && strcmp(command, "uninstall") != 0 && strcmp(command, "run") != 0 &&
        strcmp(command, "console") != 0)
    {
        /* 診断の出力であり、終了コードはすでに失敗に決まっているため、この 2 つの出力の戻り値は明示的に破棄する。 */
        (void)fprintf(stderr, "不明なコマンド '%s'\n\n", command);
        (void)cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    /* run コマンドのみサービス モード: SCM/systemd 起動のため stderr は無効化する。 */
    int is_service_mode = (strcmp(command, "run") == 0);

    /* 失敗しても s_tracer=NULL でトレースが無効になるだけで処理を続けられるため、戻り値は明示的に破棄する。 */
    (void)tracer_open(&g_service_def, !is_service_mode);

    /* 停止イベント抽象を初期化する。 */
    if (cplat_local_lock_create(&s_stop_lock) != CPLAT_OK)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "ミューテックスの生成に失敗しました。");
        tracer_close();
        return EXIT_FAILURE;
    }

    if (cplat_condvar_create(&s_stop_cv) != CPLAT_OK)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "条件変数の生成に失敗しました。");
        cplat_local_lock_dispose(s_stop_lock);
        s_stop_lock = NULL;
        tracer_close();
        return EXIT_FAILURE;
    }

    int rc = EXIT_FAILURE;

    /* 指定されたコマンドに応じた処理を実行する。 */
    if (strcmp(command, "install") == 0)
    {
        rc = svc_os_install(&g_service_def);
    }
    else if (strcmp(command, "uninstall") == 0)
    {
        rc = svc_os_uninstall(&g_service_def);
    }
    else if (strcmp(command, "run") == 0)
    {
        rc = svc_os_run_service(&g_service_def);
    }
    else if (strcmp(command, "console") == 0)
    {
        rc = svc_run_lifecycle(&g_service_def);
    }

    cplat_condvar_dispose(s_stop_cv);
    s_stop_cv = NULL;
    cplat_local_lock_dispose(s_stop_lock);
    s_stop_lock = NULL;

    tracer_close();

    return rc;
}
