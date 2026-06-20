/**
 *******************************************************************************
 *  @file           service-sample_linux.c
 *  @brief          クロスプラットフォーム サービス サンプル Linux 実装。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  Linux 固有のサービス処理を実装します。
 *  - svc_os_run_service : Type=notify で常駐 (fork 不要)、イベント監視スレッドの起動と停止
 *  - svc_os_install     : systemd ユニット ファイル生成、OOM killer 対策、登録
 *  - svc_os_uninstall   : systemd サービスの解除と削除
 *  - sd_notify 通知     : libsystemd の sd_notify(3) による READY / STOPPING / RELOADING / STATUS 送信
 *
 *  電源・セッション・シャットダウン前イベントの監視 (D-Bus) は
 *  service-sample_linux_events.c に実装します。
 *
 *  共通処理 (svc_run_lifecycle / svc_main / main) は service-sample.c に、
 *  コールバック雛形は service-sample-impl.c に実装します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)

    #include <errno.h>
    #include <stddef.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include <systemd/sd-daemon.h>

    #include <com_util/runtime/elevated_process.h>
    #include <com_util/runtime/process.h>

    #include "service-sample.h"
    #include "service-sample_linux_events.h"

    /* Doxygen コメントは、ヘッダーに記載 */

    /* ============================================================
     *  内部定数
     * ============================================================ */

    /** systemd ユニット ファイルのインストール先ディレクトリ。 */
    #define SYSTEMD_UNIT_DIR "/etc/systemd/system"

    /** readlink で取得するバイナリ パスの最大長。 */
    #define EXEC_PATH_MAX 4096

    /** ManagedOOMPreference= が導入された systemd のバージョン。 */
    #define SYSTEMD_MANAGED_OOM_PREFERENCE_VERSION 248

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
    int exit_code;
    com_util_process_options_t options;
    com_util_process_result_t result;

    memset(&options, 0, sizeof(options));
    options.argv = argv;

    result = com_util_process_run_sync(&options, COM_UTIL_PROCESS_WAIT_FOREVER, &exit_code);
    if (result != COM_UTIL_PROCESS_OK)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                               "外部コマンドの実行に失敗しました: %s", argv[0]);
        return -1;
    }
    return exit_code;
}

/**
 *  @brief          "systemd 248" 形式のバージョン行から major version を取得します。
 *  @param[in]      version_line  systemctl --version の 1 行目。NULL を渡してはなりません。
 *  @return         取得できた major version。失敗時は -1 を返します。
 */
static int parse_systemd_major_version(const char *version_line)
{
    const char prefix[] = "systemd ";
    size_t index;
    int version;
    int has_digit;

    index = 0;
    while (prefix[index] != '\0')
    {
        if (version_line[index] != prefix[index])
        {
            return -1;
        }
        index++;
    }

    version = 0;
    has_digit = 0;
    while (version_line[index] >= '0' && version_line[index] <= '9')
    {
        has_digit = 1;
        version = (version * 10) + (version_line[index] - '0');
        index++;
    }

    if (has_digit == 0)
    {
        return -1;
    }
    return version;
}

/**
 *  @brief          systemd の major version を取得します。
 *  @return         取得できた major version。失敗時は -1 を返します。
 */
static int get_systemd_major_version(void)
{
    char line[128];
    FILE *pipe;
    int version;

    pipe = popen("systemctl --version", "r");
    if (pipe == NULL)
    {
        return -1;
    }

    if (fgets(line, sizeof(line), pipe) == NULL)
    {
        pclose(pipe);
        return -1;
    }

    if (pclose(pipe) == -1)
    {
        return -1;
    }

    version = parse_systemd_major_version(line);
    if (version < 0)
    {
        return -1;
    }
    return version;
}

/**
 *  @brief          root 権限を保証します。
 *  @param[in]      command         昇格再実行するサブコマンド。
 *  @param[in]      operation_name  操作名 (エラーメッセージ用)。
 *  @param[out]     handled         別プロセスで処理済みの場合は 0 以外を格納します。
 *  @return         継続可能な場合は 0、失敗時または別プロセスの終了コードを返します。
 */
static int ensure_elevated_for_operation(const char *command, const char *operation_name, int *handled)
{
    int exit_code;
    int rc;

    if (handled == NULL)
    {
        return EXIT_FAILURE;
    }

    exit_code = EXIT_FAILURE;
    rc = com_util_elevated_process_run_if_needed(command, &exit_code, handled);
    if (rc != 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                               "%s には root 権限 (sudo) が必要です。", operation_name);
        return EXIT_FAILURE;
    }

    if (*handled != 0)
    {
        return exit_code;
    }
    return 0;
}

/* ============================================================
 *  sd_notify ヘルパー
 * ============================================================ */

/**
 *  @brief          NOTIFY_SOCKET へメッセージを送信します。
 *  @param[in]      message  送信するメッセージ文字列 (例: "READY=1")。
 *
 *  libsystemd の sd_notify(3) の薄いラッパーです。\n
 *  環境変数 NOTIFY_SOCKET が設定されていない場合は何もしません。\n
 *  送信失敗時はトレースに WARNING を出力するだけで処理を継続します。
 */
static void sd_notify_send(const char *message)
{
    int rc;

    rc = sd_notify(0, message);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "sd_notify: 送信に失敗しました: %s", strerror(-rc));
    }
    /* rc == 0 は NOTIFY_SOCKET 未設定 (コンソール モードなどでは通常の状態) */
}

/* ============================================================
 *  OS フック実装 (通知)
 * ============================================================ */

/* Doxygen コメントは、ヘッダーに記載 */

void svc_os_notify_ready(void)
{
    sd_notify_send("READY=1");
}

void svc_os_notify_stopping(void)
{
    sd_notify_send("STOPPING=1");
}

void svc_os_notify_reloading(void)
{
    sd_notify_send("RELOADING=1");
}

void svc_os_notify_status(const char *text)
{
    char message[512];

    if (text == NULL)
    {
        return;
    }
    /* 長すぎる場合は切り詰めを許容する */
    snprintf(message, sizeof(message), "STATUS=%s", text);
    sd_notify_send(message);
}

/* ============================================================
 *  OS フック実装
 * ============================================================ */

int svc_os_run_service(const svc_definition *def)
{
    int rc;

    /* 電源・セッション イベント (D-Bus)、SIGHUP reload、watchdog 応答を
       担当するイベント監視スレッドを起動する。失敗しても該当機能が
       無効になるだけで、サービス本体は継続する。 */
    svc_linux_events_start(def);

    /* Type=notify のため fork せず、フォアグラウンドのまま常駐する。
       shutdown.h が SIGTERM / SIGINT を補足して svc_request_stop() を呼ぶ。 */
    rc = svc_run_lifecycle(def);

    svc_linux_events_stop();
    return rc;
}

int svc_os_install(const svc_definition *def)
{
    char exec_path[EXEC_PATH_MAX];
    char unit_path[EXEC_PATH_MAX + 64];
    char unit_content[EXEC_PATH_MAX + 1024];
    char svc_name_buf[256];
    const char *managed_oom_preference_line;
    FILE *fp;
    int written;
    /* execvp は char * を期待するため、const char * を持つローカル バッファーを使う */
    char *argv_daemon_reload[] = {"systemctl", "daemon-reload", NULL};
    char *argv_enable[4];
    int rc;
    int handled;
    int systemd_major_version;

    snprintf(svc_name_buf, sizeof(svc_name_buf), "%s", def->name);
    argv_enable[0] = "systemctl";
    argv_enable[1] = "enable";
    argv_enable[2] = svc_name_buf;
    argv_enable[3] = NULL;

    rc = ensure_elevated_for_operation("install", "install", &handled);
    if (rc != 0 || handled != 0)
    {
        return rc;
    }

    if (com_util_process_get_executable_path(exec_path, sizeof(exec_path)) != 0)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_ERROR, NULL,
                              "実行ファイルのパスを取得できませんでした。");
        return EXIT_FAILURE;
    }

    managed_oom_preference_line = "";
    systemd_major_version = get_systemd_major_version();
    if (systemd_major_version >= SYSTEMD_MANAGED_OOM_PREFERENCE_VERSION)
    {
        managed_oom_preference_line = "ManagedOOMPreference=omit\n";
    }
    else if (systemd_major_version < 0)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                              "systemd のバージョンを取得できないため ManagedOOMPreference=omit は設定しません。");
    }
    else
    {
        com_util_tracer_writef(
            svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
            "systemd %d は ManagedOOMPreference= に未対応のため ManagedOOMPreference=omit は設定しません。",
            systemd_major_version);
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
                       "Type=notify\n"
                       "ExecStart=%s run\n"
                       "ExecReload=/bin/kill -HUP $MAINPID\n"
                       "Restart=on-failure\n"
                       "RestartSec=5\n"
                       "WatchdogSec=30\n"
                       "OOMScoreAdjust=-1000\n"
                       "%s"
                       "\n"
                       "[Install]\n"
                       "WantedBy=multi-user.target\n",
                       def->description, exec_path, managed_oom_preference_line);
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
    int handled;

    snprintf(svc_name_buf, sizeof(svc_name_buf), "%s", def->name);
    argv_stop[0] = "systemctl";
    argv_stop[1] = "stop";
    argv_stop[2] = svc_name_buf;
    argv_stop[3] = NULL;
    argv_disable[0] = "systemctl";
    argv_disable[1] = "disable";
    argv_disable[2] = svc_name_buf;
    argv_disable[3] = NULL;

    rc = ensure_elevated_for_operation("uninstall", "uninstall", &handled);
    if (rc != 0 || handled != 0)
    {
        return rc;
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
