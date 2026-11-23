/**
 *******************************************************************************
 *  @file           service-sample_linux.c
 *  @brief          クロスプラットフォーム サービスの Linux 向け処理を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/06/07
 *  @version        1.0.0
 *
 *  Linux 固有のサービス処理を実装します。
 *  - svc_os_prepare_run : run の入口で、スレッド生成前に SIGHUP をブロックする。
 *  - svc_os_run_service : Type=notify で常駐 (fork 不要)、イベント監視スレッドの起動と停止。
 *                         イベント監視スレッドを起動できない場合は、サービスを開始せずに終了する
 *  - svc_os_install     : systemd ユニット ファイル生成、OOM killer 対策、登録
 *  - svc_os_uninstall   : systemd サービスの解除と削除
 *  - sd_notify 通知     : libsystemd の sd_notify(3) による READY / STOPPING / RELOADING / STATUS 送信
 *
 *  電源・セッション・シャットダウン前イベントの監視 (D-Bus) は
 *  service-sample_linux_events.c に実装します。
 *
 *  共通処理 (svc_run_lifecycle / main) は service-sample.c に、
 *  コールバック雛形は service-sample-impl.c に実装します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <cplat/base/platform.h>

#if defined(PLATFORM_LINUX)

    #include <assert.h>
    #include <errno.h>
    #include <pthread.h>
    #include <signal.h>
    #include <stddef.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include <systemd/sd-daemon.h>

    #include <cplat/crt/stdio.h>
    #include <cplat/runtime/elevated_process.h>
    #include <cplat/runtime/process.h>

    #include "service-sample.h"
    #include "service-sample_internal.h"
    #include "service-sample_linux_events.h"

    /* Doxygen コメントは、ヘッダーに記載 */

    /*
     * ============================================================
     *  内部定数
     * ============================================================
     */

    /** systemd ユニット ファイルのインストール先ディレクトリ。 */
    #define SYSTEMD_UNIT_DIR "/etc/systemd/system"

    /** ManagedOOMPreference= が導入された systemd のバージョン。 */
    #define SYSTEMD_MANAGED_OOM_PREFERENCE_VERSION 248

    /** systemctl --version の 1 行目を読む配列の大きさ [byte] (終端 NUL を含む)。 */
    #define SYSTEMCTL_VERSION_LINE_SIZE 128U

    /** sd_notify の通知文を組み立てる配列の大きさ [byte] (終端 NUL を含む)。 */
    #define SD_NOTIFY_MESSAGE_SIZE 512U

    /** サービス名を複製する配列の大きさ [byte] (終端 NUL を含む)。 */
    #define SVC_NAME_BUFFER_SIZE 256U

    /** ユニット ファイルのパス ("<SYSTEMD_UNIT_DIR>/<サービス名>.service") を格納する配列の大きさ [byte] (終端 NUL を含む)。 */
    #define SYSTEMD_UNIT_PATH_SIZE 512U

    /** ユニット ファイルの内容のうち、実行ファイル パス以外の分の大きさ [byte]。 */
    #define UNIT_CONTENT_EXTRA_BYTES 1024U

    /**
     *  起動の回数を数える期間 [s]。
     *
     *  unit の [Unit] の StartLimitIntervalSec= に出力します。
     *  systemd の既定は 10 [s] の間に 5 回です。
     *  RestartSec=5 では、その回数に到達しません。
     *  そのため、到達できる期間を unit に明示します。
     *  設定は systemd 229 以降の [Unit] に置きます。
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd.unit.html
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd-system.conf.html
     */
    #define SVC_START_LIMIT_INTERVAL_SEC 60U

    /**
     *  期間の中で許可する起動の回数。
     *
     *  unit の [Unit] の StartLimitBurst= に出力します。
     *  手動の起動と Restart= による再起動の両方を数えます。
     *  この回数を超える起動は systemd が拒否します。
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd.unit.html
     */
    #define SVC_START_LIMIT_BURST 5U

static_assert((sizeof(SYSTEMD_UNIT_DIR) - 1U) + 1U + (SVC_NAME_BUFFER_SIZE - 1U) + (sizeof(".service") - 1U) + 1U <=
                  SYSTEMD_UNIT_PATH_SIZE,
              "unit path buffer must hold SYSTEMD_UNIT_DIR and the longest service name");

/* RestartSec= の間隔で StartLimitBurst 回の起動に達する時間が、StartLimitIntervalSec= の期間に収まることを検査する。 */
static_assert(((SVC_START_LIMIT_BURST - 1U) * SVC_FAILURE_RESTART_DELAY_SEC) < SVC_START_LIMIT_INTERVAL_SEC,
              "start limit must be reachable with the restart delay");

/*
 * Description= の本文と実行ファイルのパスを除いた unit の固定部分が、UNIT_CONTENT_EXTRA_BYTES に収まることを検査する。
 * 書式を変えたときは、この文字列も svc_os_install() の cplat_snprintf() に合わせる。
 * %u の桁数は、unsigned int が 64 [bit] の場合の 20 桁で見積もる。対象は 4 つである。
 * WatchdogSec= は SVC_UNIT_WATCHDOG_SEC を %u で出すため、桁の見積もりに含める。
 */
static_assert(sizeof(unsigned int) <= 8U, "percent-u digit estimate assumes unsigned int is at most 64-bit");
static_assert((sizeof("[Unit]\n"
                      "Description=\n"
                      "After=network.target\n"
                      "StartLimitIntervalSec=\n"
                      "StartLimitBurst=\n"
                      "\n"
                      "[Service]\n"
                      "Type=notify\n"
                      "ExecStart= run\n"
                      "ExecReload=/bin/kill -HUP $MAINPID\n"
                      "Restart=on-failure\n"
                      "RestartSec=\n"
                      "WatchdogSec=\n"
                      "OOMScoreAdjust=-1000\n"
                      "ManagedOOMPreference=omit\n"
                      "\n"
                      "[Install]\n"
                      "WantedBy=multi-user.target\n") -
               1U) +
                      (20U * 4U) <=
                  UNIT_CONTENT_EXTRA_BYTES,
              "unit content fixed part must fit in UNIT_CONTENT_EXTRA_BYTES");

/*
 * ============================================================
 *  内部ヘルパー
 * ============================================================
 */

/**
 *  @brief          外部コマンドを fork + execvp で実行します。
 *  @param[in]      argv    コマンドと引数の配列 (NULL 終端)。
 *  @return         コマンドの終了コード。実行失敗時は -1 を返します。
 */
static int run_command(char *const argv[])
{
    int exit_code;
    cplat_process_options options = {0};
    int result;

    options.argv = argv;

    result = cplat_process_run_sync(&options, &exit_code);
    if (result != CPLAT_OK)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
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

    /* 接頭辞 "systemd " と一致するか判定する。 */
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

    /* バージョン番号の数字列を整数に変換する。 */
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
    char line[SYSTEMCTL_VERSION_LINE_SIZE];
    FILE *pipe;
    int version;

    pipe = popen("systemctl --version", "r");
    if (pipe == NULL)
    {
        return -1;
    }

    if (fgets(line, sizeof(line), pipe) == NULL)
    {
        (void)pclose(pipe); /* fgets の失敗で -1 を返す経路のため、戻り値は明示的に破棄する */
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
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "%s には root 権限 (sudo) が必要です。", operation_name);
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
 *  sd_notify ヘルパー
 * ============================================================
 */

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
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "sd_notify: 送信に失敗しました: %s", strerror(-rc));
    }

    /* rc == 0 は NOTIFY_SOCKET 未設定 (コンソール モードなどでは通常の状態)。 */
}

/*
 * ============================================================
 *  OS フック実装 (通知)
 * ============================================================
 */

/* Doxygen コメントは、ヘッダーに記載 */

int svc_os_prepare_run(void)
{
    sigset_t set;
    int rc;

    /*
     * 有効なシグナル番号 SIGHUP を渡すため、sigemptyset と sigaddset は失敗の条件に当たらず、
     * 戻り値は明示的に破棄する。
     * see: https://man7.org/linux/man-pages/man3/sigemptyset.3p.html
     * see: https://man7.org/linux/man-pages/man3/sigaddset.3p.html
     */
    (void)sigemptyset(&set);
    (void)sigaddset(&set, SIGHUP);

    /*
     * 失敗時は errno ではなくエラー番号を返す。0 以外なら起動の準備を失敗とする。
     * ブロックしたマスクは、プロセスが終わるまで戻さない。fork と execve を越えて子へ引き継がれる。
     * see: https://man7.org/linux/man-pages/man3/pthread_sigmask.3.html
     * see: https://man7.org/linux/man-pages/man7/signal.7.html
     */
    rc = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (rc != 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* Doxygen コメントは、ヘッダーに記載 */

void svc_os_notify_ready(void)
{
    /* on_start() の実行中にイベント ループが異常終了した場合は、停止を要求済みであり、READY=1 を送らない。 */
    if (svc_linux_events_loop_failed() != 0)
    {
        return;
    }

    sd_notify_send("READY=1");
    svc_linux_events_enter_running();
}

void svc_os_notify_stopping(void)
{
    svc_linux_events_enter_stopping();
    sd_notify_send("STOPPING=1");
}

void svc_os_notify_reloading(void)
{
    sd_notify_send("RELOADING=1");
}

void svc_os_notify_status(const char *text)
{
    char message[SD_NOTIFY_MESSAGE_SIZE];

    if (text == NULL)
    {
        return;
    }

    /*
     * cplat_snprintf は切り詰めで空文字にするため使わない。
     * 状態の通知文は長すぎる場合の切り詰めを許容するため、戻り値は明示的に破棄する。
     */
    (void)snprintf(message, sizeof(message), "STATUS=%s", text); /* 置換対象外: 通知文の意図的な切り詰め */
    sd_notify_send(message);
}

/*
 * ============================================================
 *  OS フック実装
 * ============================================================
 */

int svc_os_run_service(const svc_definition *def)
{
    int rc;

    /*
     * 電源・セッション イベント (D-Bus)、SIGHUP reload、watchdog 応答を
     * 担当するイベント監視スレッドを起動する。
     * イベント監視スレッドの必須の初期化に失敗した場合は、サービスを開始しない。
     * on_start() を呼ばず、READY=1 も送らずに失敗で終了し、systemd が起動の失敗として扱う。
     */
    if (svc_linux_events_start(def) != 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "イベント監視スレッドを起動できないため、サービスを開始しません。");
        return EXIT_FAILURE;
    }

    /*
     * Type=notify のため fork せず、フォアグラウンドのまま常駐する。
     * shutdown.h が SIGTERM / SIGINT を捕捉して svc_request_stop() を呼び出す。
     */
    rc = svc_run_lifecycle(def);

    /* 実行中にイベント ループが異常終了した場合は、コールバックが成功していても失敗として終了する。 */
    if ((svc_linux_events_stop() != 0) && (rc == EXIT_SUCCESS))
    {
        rc = EXIT_FAILURE;
    }

    return rc;
}

int svc_os_install(const svc_definition *def)
{
    char exec_path[SVC_EXEC_PATH_MAX];
    char unit_path[SYSTEMD_UNIT_PATH_SIZE];
    char unit_content[SVC_EXEC_PATH_MAX + UNIT_CONTENT_EXTRA_BYTES];
    char svc_name_buf[SVC_NAME_BUFFER_SIZE];
    const char *managed_oom_preference_line;
    const char *exec_reload_line;
    FILE *fp;

    /* execvp は char * を期待するため、const char * を持つローカル バッファーを使う。 */
    char *argv_daemon_reload[] = {"systemctl", "daemon-reload", NULL};
    char *argv_enable[] = {"systemctl", "enable", svc_name_buf, NULL};
    int rc;
    int handled;
    int systemd_major_version;

    if (cplat_snprintf(svc_name_buf, sizeof(svc_name_buf), "%s", def->name) != CPLAT_OK)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "サービス名が長すぎます。");
        return EXIT_FAILURE;
    }

    rc = ensure_elevated_for_operation("install", "install", &handled);
    if (rc != 0 || handled != 0)
    {
        return rc;
    }

    if (cplat_process_get_executable_path(exec_path, sizeof(exec_path)) != CPLAT_OK)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "実行ファイルのパスを取得できませんでした。");
        return EXIT_FAILURE;
    }

    managed_oom_preference_line = "";
    systemd_major_version = get_systemd_major_version();

    /*
     * ManagedOOMPreference=omit を設定できる systemd のバージョンかを判定する。
     * 正本は app/service-sample/docs/systemd-oomkiller-advisory.md の「推奨 unit 設定」節である。
     */
    if (systemd_major_version >= SYSTEMD_MANAGED_OOM_PREFERENCE_VERSION)
    {
        managed_oom_preference_line = "ManagedOOMPreference=omit\n";
    }
    else if (systemd_major_version < 0)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                 "systemd のバージョンを取得できないため ManagedOOMPreference=omit は設定しません。");
    }
    else
    {
        (void)cplat_tracer_writef(
            svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
            "systemd %d は ManagedOOMPreference= に未対応のため ManagedOOMPreference=omit は設定しません。",
            systemd_major_version);
    }

    /*
     * on_reload がない場合は ExecReload= を書き込まない。systemctl reload を systemd が拒否し、
     * 受け付けない再読込要求をサービスへ送らないためである。
     */
    exec_reload_line = "";
    if (def->on_reload != NULL)
    {
        exec_reload_line = "ExecReload=/bin/kill -HUP $MAINPID\n";
    }

    /* ユニット ファイルを生成して書き込む。 */
    if (cplat_snprintf(unit_path, sizeof(unit_path), "%s/%s.service", SYSTEMD_UNIT_DIR, def->name) != CPLAT_OK)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "ユニット ファイルのパスが長すぎます。");
        return EXIT_FAILURE;
    }

    if (cplat_snprintf(unit_content, sizeof(unit_content),
                       "[Unit]\n"
                       "Description=%s\n"
                       "After=network.target\n"
                       "StartLimitIntervalSec=%u\n"
                       "StartLimitBurst=%u\n"
                       "\n"
                       "[Service]\n"
                       "Type=notify\n"
                       "ExecStart=%s run\n"
                       "%s"
                       "Restart=on-failure\n"
                       "RestartSec=%u\n"
                       "WatchdogSec=%u\n"
                       "OOMScoreAdjust=-1000\n"
                       "%s"
                       "\n"
                       "[Install]\n"
                       "WantedBy=multi-user.target\n",
                       def->description, (unsigned int)SVC_START_LIMIT_INTERVAL_SEC,
                       (unsigned int)SVC_START_LIMIT_BURST, exec_path, exec_reload_line,
                       (unsigned int)SVC_FAILURE_RESTART_DELAY_SEC, (unsigned int)SVC_UNIT_WATCHDOG_SEC,
                       managed_oom_preference_line) != CPLAT_OK)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "ユニット ファイルの内容が長すぎます。");
        return EXIT_FAILURE;
    }

    fp = fopen(unit_path, "w");
    if (fp == NULL)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "%s を開けませんでした: %s",
                                  unit_path, strerror(errno));
        return EXIT_FAILURE;
    }

    if (fputs(unit_content, fp) == EOF)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "%s への書き込みに失敗しました: %s",
                                  unit_path, strerror(errno));
        (void)fclose(fp); /* 書き込みの失敗を返す経路のため、戻り値は明示的に破棄する */
        return EXIT_FAILURE;
    }

    /* 閉じる処理は残りの書き出しを含むため、失敗した場合はユニット ファイルの書き込みの失敗として扱う。 */
    if (fclose(fp) == EOF)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "%s への書き込みに失敗しました: %s",
                                  unit_path, strerror(errno));
        return EXIT_FAILURE;
    }

    (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "ユニット ファイルを書き込みました: %s",
                              unit_path);

    /* 新しいユニット ファイルを systemd に反映するため、daemon-reload を実行する。 */
    rc = run_command(argv_daemon_reload);
    if (rc != 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "systemctl daemon-reload が失敗しました (終了コード %d)。", rc);
        return EXIT_FAILURE;
    }

    /* サービスが OS 起動時に自動開始されるよう、systemctl enable を実行する。 */
    rc = run_command(argv_enable);
    if (rc != 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "systemctl enable %s が失敗しました (終了コード %d)。", def->name, rc);
        return EXIT_FAILURE;
    }

    (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "サービス '%s' を登録しました。",
                              def->name);
    (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "開始するには: sudo systemctl start %s",
                              def->name);

    return EXIT_SUCCESS;
}

int svc_os_uninstall(const svc_definition *def)
{
    char unit_path[SYSTEMD_UNIT_PATH_SIZE];
    char svc_name_buf[SVC_NAME_BUFFER_SIZE];

    /* execvp は char * を期待するため、const char * を持つローカル バッファーを使う。 */
    char *argv_stop[] = {"systemctl", "stop", svc_name_buf, NULL};
    char *argv_disable[] = {"systemctl", "disable", svc_name_buf, NULL};
    char *argv_daemon_reload[] = {"systemctl", "daemon-reload", NULL};
    int rc;
    int handled;

    if (cplat_snprintf(svc_name_buf, sizeof(svc_name_buf), "%s", def->name) != CPLAT_OK)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "サービス名が長すぎます。");
        return EXIT_FAILURE;
    }

    rc = ensure_elevated_for_operation("uninstall", "uninstall", &handled);
    if (rc != 0 || handled != 0)
    {
        return rc;
    }

    if (cplat_snprintf(unit_path, sizeof(unit_path), "%s/%s.service", SYSTEMD_UNIT_DIR, def->name) != CPLAT_OK)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "ユニット ファイルのパスが長すぎます。");
        return EXIT_FAILURE;
    }

    /* サービスが動作中でない場合もあり、停止の失敗をエラーとせず継続するため、戻り値は明示的に破棄する。 */
    (void)run_command(argv_stop);

    /* サービスが OS 起動時に自動開始されないよう、systemctl disable を実行する。 */
    rc = run_command(argv_disable);
    if (rc != 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "systemctl disable %s が失敗しました (終了コード %d)。", def->name, rc);
    }

    /* ユニット ファイルを削除する。ファイルが存在しない場合はエラーとしない。 */
    if (remove(unit_path) != 0 && errno != ENOENT)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "%s の削除に失敗しました: %s",
                                  unit_path, strerror(errno));
        return EXIT_FAILURE;
    }

    (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "ユニット ファイルを削除しました: %s",
                              unit_path);

    /* ユニット ファイルの削除を systemd に反映するため、daemon-reload を実行する。 */
    rc = run_command(argv_daemon_reload);
    if (rc != 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "systemctl daemon-reload が失敗しました (終了コード %d)。", rc);
    }

    (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "サービス '%s' を解除しました。",
                              def->name);

    return EXIT_SUCCESS;
}

#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)
    #pragma warning(disable : 4206)
#endif
