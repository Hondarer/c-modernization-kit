/**
 *******************************************************************************
 *  @file           service-sample_linux_events.c
 *  @brief          Linux の OS イベントを監視するスレッドを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/06/10
 *  @version        1.0.0
 *
 *  sd_event のイベント ループを専用スレッドで実行し、以下を担当します。
 *  - systemd-logind (D-Bus) の PrepareForSleep / PrepareForShutdown /
 *    SessionNew / SessionRemoved の監視と svc_dispatch_event() への配送
 *  - サスペンドとシャットダウンの delay inhibitor lock の取得・解放・再取得
 *  - SIGHUP による設定再読込 (svc_dispatch_reload())
 *  - systemd watchdog (WATCHDOG_USEC) への自動応答
 *
 *  SIGHUP は sigaction + eventfd の self-pipe 方式でイベント ループへ
 *  転送します。signalfd 方式 (sd_event_add_signal) は全スレッドでの
 *  シグナル ブロックが前提となりますが、本スレッドの起動時点で tracer の
 *  スレッドが起動済みのため採用していません。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)

    #include <fcntl.h>
    #include <signal.h>
    #include <stdint.h>
    #include <string.h>
    #include <sys/epoll.h>
    #include <sys/eventfd.h>
    #include <unistd.h>

    #include <systemd/sd-bus.h>
    #include <systemd/sd-event.h>

    #include <com_util/sync/sync.h>

    #include "service-sample.h"
    #include "service-sample_linux_events.h"

/* Doxygen コメントは、ヘッダーに記載 */

/* ============================================================
 *  内部定数
 * ============================================================ */

    /** イベント監視スレッドの終了を待機する時間 (ミリ秒)。 */
    #define SVC_EVENTS_JOIN_TIMEOUT_MS 5000

    /** systemd-logind の D-Bus 接続先 (サービス名)。 */
    #define LOGIND_SERVICE "org.freedesktop.login1"
    /** systemd-logind の D-Bus 接続先 (オブジェクト パス)。 */
    #define LOGIND_OBJECT "/org/freedesktop/login1"
    /** systemd-logind の D-Bus 接続先 (インターフェース)。 */
    #define LOGIND_INTERFACE "org.freedesktop.login1.Manager"

/* ============================================================
 *  内部状態
 * ============================================================ */

/**
 *  @brief          イベント監視スレッドの内部状態。
 */
typedef struct svc_linux_events_ctx
{
    const svc_definition *def; /**< サービス定義。svc_linux_events_start() で設定される。 */
    com_util_thread *thread;   /**< イベント監視スレッドのハンドル。未起動時は NULL。 */
    sd_event *event;           /**< sd_event ループ。スレッド内で生成・解放します。 */
    sd_bus *bus;               /**< system bus 接続。接続失敗時は NULL。 */
    int stop_fd;               /**< 停止指示用 eventfd。未生成時は -1。 */
    int reload_fd;             /**< SIGHUP 転送用 eventfd。未生成時は -1。 */
    int inhibit_sleep_fd;      /**< サスペンドの delay inhibitor lock。未取得時は -1。 */
    int inhibit_shutdown_fd;   /**< シャットダウンの delay inhibitor lock。未取得時は -1。 */
} svc_linux_events_ctx;

/** イベント監視スレッドの内部状態 (プロセスで 1 つ)。 */
static svc_linux_events_ctx g_ctx = {NULL, NULL, NULL, NULL, -1, -1, -1, -1};

/** SIGHUP ハンドラー設定前のアクション (svc_linux_events_stop() で復元する)。 */
static struct sigaction g_old_sighup_action;
/** SIGHUP ハンドラーを設定済みかどうか。1 = 設定済み。 */
static int g_sighup_installed = 0;

/* ============================================================
 *  SIGHUP ハンドラー (self-pipe)
 * ============================================================ */

/**
 *  @brief          SIGHUP を reload 用 eventfd に転送します。
 *  @param[in]      signum  シグナル番号 (未使用)。
 *
 *  シグナル ハンドラーのため async-signal-safe な write() のみを使用します。
 */
static void sighup_signal_handler(int signum)
{
    uint64_t value;
    ssize_t written;

    (void)signum;
    if (g_ctx.reload_fd >= 0)
    {
        value = 1;
        written = write(g_ctx.reload_fd, &value, sizeof(value));
        (void)written;
    }
}

/* ============================================================
 *  inhibitor lock
 * ============================================================ */

/**
 *  @brief          logind の delay inhibitor lock を取得します。
 *  @param[in]      what    対象 ("sleep" または "shutdown")。
 *  @return         取得した lock の fd。失敗時は -1 を返します。
 *
 *  lock は fd の close で解放されます。\n
 *  delay lock のため、イベント発生から logind の InhibitDelayMaxSec
 *  (既定 5 秒) が経過すると lock の解放を待たずに処理が進みます。
 */
static int acquire_inhibit_lock(const char *what)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = NULL;
    int fd;
    int lock_fd;
    int rc;

    lock_fd = -1;
    rc = sd_bus_call_method(g_ctx.bus, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "Inhibit", &error, &reply,
                            "ssss", what, g_ctx.def->name, "イベント コールバックの実行猶予を確保するため", "delay");
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "inhibitor lock (%s) の取得に失敗しました: %s", what, strerror(-rc));
    }
    else
    {
        rc = sd_bus_message_read(reply, "h", &fd);
        if (rc < 0)
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                                   "inhibitor lock (%s) の fd を取得できませんでした: %s", what, strerror(-rc));
        }
        else
        {
            /* fd は reply メッセージが所有するため、複製して保持する */
            lock_fd = fcntl(fd, F_DUPFD_CLOEXEC, 3);
            if (lock_fd < 0)
            {
                com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                                       "inhibitor lock (%s) の fd を複製できませんでした。", what);
            }
        }
        sd_bus_message_unref(reply);
    }
    sd_bus_error_free(&error);
    return lock_fd;
}

/**
 *  @brief          保持しているすべての inhibitor lock を解放します。
 */
static void release_inhibit_locks(void)
{
    if (g_ctx.inhibit_sleep_fd >= 0)
    {
        close(g_ctx.inhibit_sleep_fd);
        g_ctx.inhibit_sleep_fd = -1;
    }
    if (g_ctx.inhibit_shutdown_fd >= 0)
    {
        close(g_ctx.inhibit_shutdown_fd);
        g_ctx.inhibit_shutdown_fd = -1;
    }
}

/* ============================================================
 *  D-Bus シグナル ハンドラー
 * ============================================================ */

/**
 *  @brief          logind の PrepareForSleep シグナルを処理します。
 *  @param[in]      m           受信したシグナル メッセージ。
 *  @param[in]      userdata    未使用。
 *  @param[in]      ret_error   未使用。
 *  @return         常に 0 を返します。
 *
 *  サスペンド開始時はイベント配送後に sleep lock を解放してサスペンドを
 *  許可し、復帰時はイベント配送後に次のサスペンドに備えて再取得します。
 */
static int on_prepare_for_sleep(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    int starting;
    svc_event_info info;

    (void)userdata;
    (void)ret_error;

    if (sd_bus_message_read(m, "b", &starting) < 0)
    {
        return 0;
    }

    info.session_id = NULL;
    if (starting != 0)
    {
        info.type = SVC_EVENT_POWER_SUSPEND;
        svc_dispatch_event(g_ctx.def, &info);
        /* lock を解放してサスペンドを許可する */
        if (g_ctx.inhibit_sleep_fd >= 0)
        {
            close(g_ctx.inhibit_sleep_fd);
            g_ctx.inhibit_sleep_fd = -1;
        }
    }
    else
    {
        info.type = SVC_EVENT_POWER_RESUME;
        svc_dispatch_event(g_ctx.def, &info);
        /* 次のサスペンドに備えて lock を再取得する */
        g_ctx.inhibit_sleep_fd = acquire_inhibit_lock("sleep");
    }
    return 0;
}

/**
 *  @brief          logind の PrepareForShutdown シグナルを処理します。
 *  @param[in]      m           受信したシグナル メッセージ。
 *  @param[in]      userdata    未使用。
 *  @param[in]      ret_error   未使用。
 *  @return         常に 0 を返します。
 *
 *  シャットダウン開始時はイベント配送後に shutdown lock を解放します。\n
 *  実際のサービス停止は、この後 systemd が送る SIGTERM (既存の停止経路)
 *  で処理されます。
 */
static int on_prepare_for_shutdown(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    int starting;
    svc_event_info info;

    (void)userdata;
    (void)ret_error;

    if (sd_bus_message_read(m, "b", &starting) < 0)
    {
        return 0;
    }

    if (starting != 0)
    {
        info.type = SVC_EVENT_PRESHUTDOWN;
        info.session_id = NULL;
        svc_dispatch_event(g_ctx.def, &info);
        /* lock を解放してシャットダウンを許可する */
        if (g_ctx.inhibit_shutdown_fd >= 0)
        {
            close(g_ctx.inhibit_shutdown_fd);
            g_ctx.inhibit_shutdown_fd = -1;
        }
    }
    else
    {
        /* シャットダウンが取り消された場合に備えて lock を再取得する */
        if (g_ctx.inhibit_shutdown_fd < 0)
        {
            g_ctx.inhibit_shutdown_fd = acquire_inhibit_lock("shutdown");
        }
    }
    return 0;
}

/**
 *  @brief          logind のセッション シグナルを共通イベントに変換して配送します。
 *  @param[in]      m       受信したシグナル メッセージ (引数は "so")。
 *  @param[in]      type    配送するイベント種別。
 *  @return         常に 0 を返します。
 */
static int dispatch_session_signal(sd_bus_message *m, const svc_event_type type)
{
    const char *session_id = NULL;
    const char *object_path = NULL;
    svc_event_info info;

    if (sd_bus_message_read(m, "so", &session_id, &object_path) < 0)
    {
        return 0;
    }

    info.type = type;
    info.session_id = session_id;
    svc_dispatch_event(g_ctx.def, &info);
    return 0;
}

/**
 *  @brief          logind の SessionNew シグナルを処理します。
 *  @param[in]      m           受信したシグナル メッセージ。
 *  @param[in]      userdata    未使用。
 *  @param[in]      ret_error   未使用。
 *  @return         常に 0 を返します。
 */
static int on_session_new(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    (void)userdata;
    (void)ret_error;
    return dispatch_session_signal(m, SVC_EVENT_SESSION_LOGON);
}

/**
 *  @brief          logind の SessionRemoved シグナルを処理します。
 *  @param[in]      m           受信したシグナル メッセージ。
 *  @param[in]      userdata    未使用。
 *  @param[in]      ret_error   未使用。
 *  @return         常に 0 を返します。
 */
static int on_session_removed(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    (void)userdata;
    (void)ret_error;
    return dispatch_session_signal(m, SVC_EVENT_SESSION_LOGOFF);
}

/* ============================================================
 *  eventfd ハンドラー
 * ============================================================ */

/**
 *  @brief          停止指示 (eventfd) を受けてイベント ループを終了します。
 *  @param[in]      source      イベント ソース。
 *  @param[in]      fd          停止指示用 eventfd。
 *  @param[in]      revents     発生したイベント (未使用)。
 *  @param[in]      userdata    未使用。
 *  @return         常に 0 を返します。
 */
static int on_stop_requested(sd_event_source *source, int fd, uint32_t revents, void *userdata)
{
    uint64_t value;
    ssize_t bytes;

    (void)revents;
    (void)userdata;

    bytes = read(fd, &value, sizeof(value));
    (void)bytes;
    sd_event_exit(sd_event_source_get_event(source), 0);
    return 0;
}

/**
 *  @brief          SIGHUP 転送 (eventfd) を受けて設定再読込を配送します。
 *  @param[in]      source      イベント ソース (未使用)。
 *  @param[in]      fd          reload 用 eventfd。
 *  @param[in]      revents     発生したイベント (未使用)。
 *  @param[in]      userdata    未使用。
 *  @return         常に 0 を返します。
 */
static int on_reload_requested(sd_event_source *source, int fd, uint32_t revents, void *userdata)
{
    uint64_t value;
    ssize_t bytes;

    (void)source;
    (void)revents;
    (void)userdata;

    bytes = read(fd, &value, sizeof(value));
    (void)bytes;
    svc_dispatch_reload(g_ctx.def);
    return 0;
}

/* ============================================================
 *  D-Bus 監視の構築
 * ============================================================ */

/**
 *  @brief          system bus に接続して logind のシグナル監視を構築します。
 *
 *  接続や購読に失敗した場合は WARNING を出力して該当機能のみ無効化します
 *  (コンテナーなど D-Bus が存在しない環境への対応)。
 */
static void setup_bus_monitoring(void)
{
    int rc;

    rc = sd_bus_open_system(&g_ctx.bus);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "D-Bus (system bus) に接続できないため電源・セッション イベントは無効です: %s",
                               strerror(-rc));
        g_ctx.bus = NULL;
        return;
    }

    rc = sd_bus_match_signal(g_ctx.bus, NULL, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "PrepareForSleep",
                             on_prepare_for_sleep, NULL);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "PrepareForSleep の購読に失敗しました: %s", strerror(-rc));
    }
    rc = sd_bus_match_signal(g_ctx.bus, NULL, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "PrepareForShutdown",
                             on_prepare_for_shutdown, NULL);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "PrepareForShutdown の購読に失敗しました: %s", strerror(-rc));
    }
    rc = sd_bus_match_signal(g_ctx.bus, NULL, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "SessionNew",
                             on_session_new, NULL);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "SessionNew の購読に失敗しました: %s", strerror(-rc));
    }
    rc = sd_bus_match_signal(g_ctx.bus, NULL, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "SessionRemoved",
                             on_session_removed, NULL);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "SessionRemoved の購読に失敗しました: %s", strerror(-rc));
    }

    /* イベント発生時にコールバックを実行する猶予を確保するための delay lock */
    g_ctx.inhibit_sleep_fd = acquire_inhibit_lock("sleep");
    g_ctx.inhibit_shutdown_fd = acquire_inhibit_lock("shutdown");

    rc = sd_bus_attach_event(g_ctx.bus, g_ctx.event, SD_EVENT_PRIORITY_NORMAL);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "D-Bus をイベント ループに接続できないため電源・セッション イベントは無効です: %s",
                               strerror(-rc));
        release_inhibit_locks();
        sd_bus_flush_close_unref(g_ctx.bus);
        g_ctx.bus = NULL;
    }
}

/* ============================================================
 *  イベント監視スレッド
 * ============================================================ */

/**
 *  @brief          イベント監視スレッドの本体。
 *  @param[in]      arg 未使用。
 *
 *  sd_event ループを構築して実行し、終了時に資源を解放します。
 */
static void events_thread_func(void *arg)
{
    int rc;

    (void)arg;

    rc = sd_event_new(&g_ctx.event);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "sd_event の生成に失敗したため OS イベント監視は無効です: %s", strerror(-rc));
        return;
    }

    /* WATCHDOG_USEC が設定されている場合のみ有効になる (未設定なら何もしない) */
    rc = sd_event_set_watchdog(g_ctx.event, 1);
    if (rc < 0)
    {
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "watchdog 応答の設定に失敗しました: %s", strerror(-rc));
    }

    rc = sd_event_add_io(g_ctx.event, NULL, g_ctx.stop_fd, EPOLLIN, on_stop_requested, NULL);
    if (rc < 0)
    {
        /* 停止指示を監視できないとスレッドを終了させられないため、ループに入らない */
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                               "停止監視の登録に失敗したため OS イベント監視は無効です: %s", strerror(-rc));
    }
    else
    {
        if (g_ctx.reload_fd >= 0)
        {
            rc = sd_event_add_io(g_ctx.event, NULL, g_ctx.reload_fd, EPOLLIN, on_reload_requested, NULL);
            if (rc < 0)
            {
                com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                                       "reload 監視の登録に失敗したため設定再読込は無効です: %s", strerror(-rc));
            }
        }

        if (g_ctx.def->on_event != NULL)
        {
            setup_bus_monitoring();
        }

        rc = sd_event_loop(g_ctx.event);
        if (rc < 0)
        {
            com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                                   "イベント ループがエラーで終了しました: %s", strerror(-rc));
        }
    }

    release_inhibit_locks();
    if (g_ctx.bus != NULL)
    {
        sd_bus_flush_close_unref(g_ctx.bus);
        g_ctx.bus = NULL;
    }
    sd_event_unref(g_ctx.event);
    g_ctx.event = NULL;
}

/* ============================================================
 *  起動・停止 (svc_os_run_service から呼ばれる)
 * ============================================================ */

/**
 *  @brief          start 失敗時・停止時にスレッド外の資源を解放します。
 *
 *  SIGHUP ハンドラーの復元 → eventfd の close の順に行います
 *  (close を先にするとハンドラーが解放済み fd へ書き込む可能性があるため)。
 */
static void release_local_resources(void)
{
    if (g_sighup_installed != 0)
    {
        sigaction(SIGHUP, &g_old_sighup_action, NULL);
        g_sighup_installed = 0;
    }
    if (g_ctx.reload_fd >= 0)
    {
        close(g_ctx.reload_fd);
        g_ctx.reload_fd = -1;
    }
    if (g_ctx.stop_fd >= 0)
    {
        close(g_ctx.stop_fd);
        g_ctx.stop_fd = -1;
    }
    g_ctx.def = NULL;
}

/* Doxygen コメントは、ヘッダーに記載 */

int svc_linux_events_start(const svc_definition *def)
{
    com_util_sync_result_t result;
    struct sigaction action;

    if (def == NULL)
    {
        return -1;
    }
    if (g_ctx.thread != NULL)
    {
        /* 既に起動済み */
        return 0;
    }

    g_ctx.def = def;

    g_ctx.stop_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (g_ctx.stop_fd < 0)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                              "停止指示用 eventfd の生成に失敗したため OS イベント監視は無効です。");
        release_local_resources();
        return -1;
    }

    if (def->on_reload != NULL)
    {
        g_ctx.reload_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (g_ctx.reload_fd < 0)
        {
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                                  "reload 用 eventfd の生成に失敗したため設定再読込は無効です。");
        }
        else
        {
            memset(&action, 0, sizeof(action));
            action.sa_handler = sighup_signal_handler;
            sigemptyset(&action.sa_mask);
            action.sa_flags = SA_RESTART;
            if (sigaction(SIGHUP, &action, &g_old_sighup_action) != 0)
            {
                com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                                      "SIGHUP ハンドラーの設定に失敗したため設定再読込は無効です。");
                close(g_ctx.reload_fd);
                g_ctx.reload_fd = -1;
            }
            else
            {
                g_sighup_installed = 1;
            }
        }
    }

    result = com_util_thread_create(&g_ctx.thread, events_thread_func, NULL);
    if (result != COM_UTIL_SYNC_OK)
    {
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                              "イベント監視スレッドの起動に失敗したため OS イベント監視は無効です。");
        g_ctx.thread = NULL;
        release_local_resources();
        return -1;
    }

    return 0;
}

void svc_linux_events_stop(void)
{
    com_util_sync_result_t result;
    uint64_t value;
    ssize_t written;

    if (g_ctx.thread != NULL)
    {
        value = 1;
        written = write(g_ctx.stop_fd, &value, sizeof(value));
        (void)written;

        result = com_util_thread_join(g_ctx.thread, SVC_EVENTS_JOIN_TIMEOUT_MS);
        if (result != COM_UTIL_SYNC_OK)
        {
            com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL,
                                  "イベント監視スレッドが時間内に終了しないため切り離します。");
            com_util_thread_detach(g_ctx.thread);
        }
        g_ctx.thread = NULL;
    }

    release_local_resources();
}

#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)
    #pragma warning(disable : 4206)
#endif /* PLATFORM_LINUX */
