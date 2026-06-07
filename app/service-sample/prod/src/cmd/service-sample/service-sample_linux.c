/**
 *******************************************************************************
 *  @file           service-sample_linux.c
 *  @brief          クロスプラットフォーム サービス サンプル Linux 実装。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  Linux 固有のサービス処理を実装します。
 *  - svc_os_run_service : Type=simple で常駐 (fork 不要)
 *  - svc_os_install     : systemd ユニット ファイル生成と登録
 *  - svc_os_uninstall   : systemd サービスの解除と削除
 *
 *  共通処理 (svc_run_lifecycle / svc_main / コールバック雛形) は
 *  service-sample.c に実装します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)

    #include <errno.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/wait.h>
    #include <unistd.h>

    #include <com_util/runtime/process.h>

    #include "service-sample.h"

    /* Doxygen コメントは、ヘッダーに記載 */

    /* ============================================================
     *  内部定数
     * ============================================================ */

    /** systemd ユニット ファイルのインストール先ディレクトリ。 */
    #define SYSTEMD_UNIT_DIR "/etc/systemd/system"

    /** readlink で取得するバイナリ パスの最大長。 */
    #define EXEC_PATH_MAX 4096

/* ============================================================
 *  内部ヘルパー
 * ============================================================ */

/**
 *  @brief          外部コマンドを fork + execvp で実行します。
 *  @param[in]      argv    コマンドと引数の配列 (NULL 終端)。
 *  @return         コマンドの終了コード。実行失敗時は -1 を返します。
 */
static int run_command(char *const argv[])
{
    pid_t pid;
    int status;
    int exit_code;

    pid = fork();
    if (pid < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "fork() が失敗しました: %s",
                               strerror(errno));
        return -1;
    }

    if (pid == 0)
    {
        /* 子プロセス: fork 後のため tracer は使用せず stderr へ直接書く */
        execvp(argv[0], argv);
        fprintf(stderr, "エラー: execvp(\"%s\") が失敗しました: %s\n", argv[0], strerror(errno));
        _exit(127);
    }

    /* 親プロセス: 子の終了を待つ */
    if (waitpid(pid, &status, 0) < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "waitpid() が失敗しました: %s",
                               strerror(errno));
        return -1;
    }

    exit_code = -1;
    if (WIFEXITED(status))
    {
        exit_code = WEXITSTATUS(status);
    }
    return exit_code;
}

/**
 *  @brief          root 権限を確認します。
 *  @param[in]      operation_name  操作名 (エラーメッセージ用)。
 *  @return         root の場合は 0、そうでない場合は -1 を返します。
 */
static int check_root(const char *operation_name)
{
    if (geteuid() != 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                               "%s には root 権限 (sudo) が必要です。", operation_name);
        return -1;
    }
    return 0;
}

/* ============================================================
 *  OS フック実装
 * ============================================================ */

int svc_os_run_service(const svc_definition *def)
{
    /* Type=simple のため fork 不要。フォアグラウンドのまま常駐する。
       shutdown.h が SIGTERM / SIGINT を補足して svc_request_stop() を呼ぶ。 */
    return svc_run_lifecycle(def);
}

int svc_os_install(const svc_definition *def)
{
    char exec_path[EXEC_PATH_MAX];
    char unit_path[EXEC_PATH_MAX + 64];
    char unit_content[EXEC_PATH_MAX + 512];
    char svc_name_buf[256];
    FILE *fp;
    int written;
    /* execvp は char * を期待するため、const char * を持つローカル バッファーを使う */
    char *argv_daemon_reload[] = {"systemctl", "daemon-reload", NULL};
    char *argv_enable[4];
    int rc;

    snprintf(svc_name_buf, sizeof(svc_name_buf), "%s", def->name);
    argv_enable[0] = "systemctl";
    argv_enable[1] = "enable";
    argv_enable[2] = svc_name_buf;
    argv_enable[3] = NULL;

    if (check_root("install") != 0)
    {
        return EXIT_FAILURE;
    }

    if (com_util_process_get_executable_path(exec_path, sizeof(exec_path)) != 0)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                              "実行ファイルのパスを取得できませんでした。");
        return EXIT_FAILURE;
    }

    /* ユニット ファイルのパスを生成する */
    written = snprintf(unit_path, sizeof(unit_path), "%s/%s.service", SYSTEMD_UNIT_DIR, def->name);
    if (written < 0 || (size_t)written >= sizeof(unit_path))
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                              "ユニット ファイルのパスが長すぎます。");
        return EXIT_FAILURE;
    }

    /* ユニット ファイルの内容を生成する */
    written = snprintf(unit_content, sizeof(unit_content),
                       "[Unit]\n"
                       "Description=%s\n"
                       "After=network.target\n"
                       "\n"
                       "[Service]\n"
                       "Type=simple\n"
                       "ExecStart=%s run\n"
                       "Restart=on-failure\n"
                       "RestartSec=5\n"
                       "\n"
                       "[Install]\n"
                       "WantedBy=multi-user.target\n",
                       def->description, exec_path);
    if (written < 0 || (size_t)written >= sizeof(unit_content))
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                              "ユニット ファイルの内容が長すぎます。");
        return EXIT_FAILURE;
    }

    /* ユニット ファイルに書き込む */
    fp = fopen(unit_path, "w");
    if (fp == NULL)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "%s を開けませんでした: %s",
                               unit_path, strerror(errno));
        return EXIT_FAILURE;
    }
    if (fputs(unit_content, fp) == EOF)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "%s への書き込みに失敗しました: %s",
                               unit_path, strerror(errno));
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);
    com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "ユニット ファイルを書き込みました: %s",
                           unit_path);

    /* systemctl daemon-reload を実行する */
    rc = run_command(argv_daemon_reload);
    if (rc != 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                               "systemctl daemon-reload が失敗しました (終了コード %d)。", rc);
        return EXIT_FAILURE;
    }

    /* systemctl enable を実行する */
    rc = run_command(argv_enable);
    if (rc != 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                               "systemctl enable %s が失敗しました (終了コード %d)。", def->name, rc);
        return EXIT_FAILURE;
    }

    com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サービス '%s' を登録しました。",
                           def->name);
    com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "開始するには: sudo systemctl start %s",
                           def->name);
    return 0;
}

int svc_os_uninstall(const svc_definition *def)
{
    char unit_path[512];
    char svc_name_buf[256];
    int written;
    /* execvp は char * を期待するため、const char * を持つローカル バッファーを使う */
    char *argv_stop[4];
    char *argv_disable[4];
    char *argv_daemon_reload[] = {"systemctl", "daemon-reload", NULL};
    int rc;

    snprintf(svc_name_buf, sizeof(svc_name_buf), "%s", def->name);
    argv_stop[0] = "systemctl";
    argv_stop[1] = "stop";
    argv_stop[2] = svc_name_buf;
    argv_stop[3] = NULL;
    argv_disable[0] = "systemctl";
    argv_disable[1] = "disable";
    argv_disable[2] = svc_name_buf;
    argv_disable[3] = NULL;

    if (check_root("uninstall") != 0)
    {
        return EXIT_FAILURE;
    }

    written = snprintf(unit_path, sizeof(unit_path), "%s/%s.service", SYSTEMD_UNIT_DIR, def->name);
    if (written < 0 || (size_t)written >= sizeof(unit_path))
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                              "ユニット ファイルのパスが長すぎます。");
        return EXIT_FAILURE;
    }

    /* systemctl stop を実行する (実行中でなくてもエラーにしない) */
    run_command(argv_stop);

    /* systemctl disable を実行する */
    rc = run_command(argv_disable);
    if (rc != 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "systemctl disable %s が失敗しました (終了コード %d)。", def->name, rc);
    }

    /* ユニット ファイルを削除する */
    if (remove(unit_path) != 0 && errno != ENOENT)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL, "%s の削除に失敗しました: %s",
                               unit_path, strerror(errno));
        return EXIT_FAILURE;
    }
    com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "ユニット ファイルを削除しました: %s",
                           unit_path);

    /* systemctl daemon-reload を実行する */
    rc = run_command(argv_daemon_reload);
    if (rc != 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "systemctl daemon-reload が失敗しました (終了コード %d)。", rc);
    }

    com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サービス '%s' を解除しました。",
                           def->name);
    return 0;
}

#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)
    #pragma warning(disable : 4206)
#endif /* PLATFORM_LINUX */
