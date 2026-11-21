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
 *  - systemd watchdog (WATCHDOG_USEC) への応答。on_run() の進行を確かめた場合だけ WATCHDOG=1 を送ります。
 *
 *  OS イベントと設定再読込の配送は、動作中の段階 (READY=1 の通知後から STOPPING=1 の通知前まで) に限ります。
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

#include <cplat/base/platform.h>

#if defined(PLATFORM_LINUX)

    #include <assert.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <stdint.h>
    #include <string.h>
    #include <sys/epoll.h>
    #include <sys/eventfd.h>
    #include <time.h>
    #include <unistd.h>

    #include <cplat/crt/unistd.h>

    #include <systemd/sd-bus.h>
    #include <systemd/sd-daemon.h>
    #include <systemd/sd-event.h>

    #include <cplat/runtime/abort.h>
    #include <cplat/sync/atomic.h>
    #include <cplat/sync/sync.h>

    #include "service-sample.h"
    #include "service-sample_linux_events.h"

/* Doxygen コメントは、ヘッダーに記載 */

/*
 * ============================================================
 *  内部定数
 * ============================================================
 */

    /**
     *  sd-bus の同期呼び出し (sd_bus_call_method) の既定のタイムアウト [ms] です。
     *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_call.html
     */
    #define SVC_SD_BUS_DEFAULT_CALL_TIMEOUT_MS 25000

    /**
     *  systemd の既定の停止タイムアウト (DefaultTimeoutStopSec=) [ms] です。
     *  unit に TimeoutStopSec= を指定していないため、この既定値が適用されます。
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd-system.conf.html
     */
    #define SVC_SYSTEMD_DEFAULT_TIMEOUT_STOP_MS 90000

    /**
     *  イベント監視スレッドの終了を待機する時間 [ms]。
     *
     *  スレッドの中の D-Bus の同期呼び出しが既定のタイムアウト (25 [s]) で戻るのを待てるよう、25 [s] より大きくします。\n
     *  systemd の停止タイムアウト (TimeoutStopSec=。unit で指定していないため、既定の 90 [s]) より小さくします。\n
     *  この時間内に終了しない場合は、資源を解放せずにプロセスを異常終了させます。\n
     *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_set_method_call_timeout.html \n
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html \n
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd-system.conf.html
     */
    #define SVC_EVENTS_JOIN_TIMEOUT_MS 30000

static_assert(SVC_EVENTS_JOIN_TIMEOUT_MS > SVC_SD_BUS_DEFAULT_CALL_TIMEOUT_MS,
              "join timeout must exceed the sd-bus call timeout");
static_assert(SVC_EVENTS_JOIN_TIMEOUT_MS < SVC_SYSTEMD_DEFAULT_TIMEOUT_STOP_MS,
              "join timeout must be shorter than TimeoutStopSec");

    /**
     *  watchdog の確認周期を求めるための除数です (期限の 1/4)。
     *  on_run() の呼び出し間隔の上限 (期限の 1/2 未満) と合わせて、通知間隔の最大値を期限の 3/4 未満に保ちます。
     */
    #define SVC_WATCHDOG_CHECK_DIVISOR 4U

static_assert(SVC_WATCHDOG_CHECK_DIVISOR >= 2U, "watchdog check interval must be at most half of WatchdogSec");

    /**
     *  watchdog のタイマーの精度 [us] です。
     *  0 は sd_event_add_time() の既定の精度 (250 [ms]) を表します。
     */
    #define SVC_WATCHDOG_TIMER_ACCURACY_USEC 0U

    /** systemd-logind の D-Bus 接続先 (サービス名)。 */
    #define LOGIND_SERVICE "org.freedesktop.login1"
    /** systemd-logind の D-Bus 接続先 (オブジェクト パス)。 */
    #define LOGIND_OBJECT "/org/freedesktop/login1"
    /** systemd-logind の D-Bus 接続先 (インターフェイス)。 */
    #define LOGIND_INTERFACE "org.freedesktop.login1.Manager"

/*
 * ============================================================
 *  内部状態
 * ============================================================
 */

/**
 *  @brief          イベント監視スレッドの初期化の状態。
 */
typedef enum svc_linux_events_init_state
{
    SVC_LINUX_EVENTS_INIT_PENDING = 0,   /**< 初期化の完了を待っている。 */
    SVC_LINUX_EVENTS_INIT_SUCCEEDED = 1, /**< 必須の初期化が完了し、イベント ループへ入る。 */
    SVC_LINUX_EVENTS_INIT_FAILED = 2     /**< 必須の初期化に失敗し、スレッドは終了する。 */
} svc_linux_events_init_state;

/**
 *  @brief          サービスの動作段階。
 *
 *  watchdog の通知と、OS イベントと設定再読込の配送を、動作中の段階に限るために使います。
 */
typedef enum svc_linux_events_phase
{
    SVC_LINUX_EVENTS_PHASE_STARTING = 0, /**< 起動中 (READY=1 の通知前)。 */
    SVC_LINUX_EVENTS_PHASE_RUNNING = 1,  /**< 動作中 (READY=1 の通知後から STOPPING=1 の通知前まで)。 */
    SVC_LINUX_EVENTS_PHASE_STOPPING = 2  /**< 停止中 (STOPPING=1 の通知後)。 */
} svc_linux_events_phase;

/**
 *  @brief          イベント監視スレッドの内部状態。
 */
typedef struct svc_linux_events_ctx
{
    const svc_definition *def;        /**< サービス定義。svc_linux_events_start() で設定される。 */
    cplat_thread *thread;             /**< イベント監視スレッドのハンドル。未起動時は NULL。 */
    sd_event *event;                  /**< sd_event ループ。スレッド内で生成・解放します。 */
    sd_bus *bus;                      /**< system bus 接続。接続失敗時は NULL。 */
    sd_event_source *stop_source;     /**< 停止指示の監視。スレッド内で生成・解放します。 */
    sd_event_source *reload_source;   /**< SIGHUP 転送の監視。スレッド内で生成・解放します。 */
    sd_event_source *watchdog_source; /**< watchdog の確認のタイマー。watchdog が要求されていない場合は NULL。 */
    cplat_local_lock *init_lock;      /**< init_state を保護するロック。 */
    cplat_condvar *init_cv;           /**< init_state の変化を通知する条件変数。 */
    uint64_t watchdog_interval_usec;  /**< watchdog の確認周期 [us]。 */
    uint32_t watchdog_seen_count;     /**< 前回の確認で記録した on_run() の進行の計数。 */
    int watchdog_warned;              /**< on_run() の停滞の WARNING を出力済みかどうか。1 = 出力済み。 */
    int init_state;                   /**< 初期化の状態 (svc_linux_events_init_state の値)。init_lock で保護。 */
    int stop_fd;                      /**< 停止指示用 eventfd。未生成時は -1。 */
    int reload_fd;                    /**< SIGHUP 転送用 eventfd。未生成時は -1。 */
    int inhibit_sleep_fd;             /**< サスペンドの delay inhibitor lock。未取得時は -1。 */
    int inhibit_shutdown_fd;          /**< シャットダウンの delay inhibitor lock。未取得時は -1。 */
} svc_linux_events_ctx;

/** イベント監視スレッドの内部状態 (プロセスで 1 つ)。 */
static svc_linux_events_ctx s_ctx = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0U, 0U, 0, SVC_LINUX_EVENTS_INIT_PENDING, -1, -1, -1, -1};

/** 実行中のイベント ループの異常終了を記録します。1 = 異常終了した。 */
static cplat_atomic_i32 s_loop_failed = CPLAT_ATOMIC_INIT(0);

/** サービスの動作段階 (svc_linux_events_phase の値)。 */
static cplat_atomic_i32 s_phase = CPLAT_ATOMIC_INIT(SVC_LINUX_EVENTS_PHASE_STARTING);

/** SIGHUP ハンドラー設定前のアクション (svc_linux_events_stop() で復元する)。 */
static struct sigaction s_old_sighup_action;

/** SIGHUP ハンドラーを設定済みかどうか。1 = 設定済み。 */
static int s_sighup_installed = 0;

/*
 * ============================================================
 *  動作段階と異常終了の記録
 * ============================================================
 */

/**
 *  @brief          サービスが動作中の段階かを判定します。
 *  @return         動作中の場合は 1、それ以外は 0 を返します。
 */
static int is_running_phase(void)
{
    return (cplat_atomic_load_i32(&s_phase, CPLAT_MEMORY_ORDER_ACQUIRE) == SVC_LINUX_EVENTS_PHASE_RUNNING) ? 1 : 0;
}

/**
 *  @brief          イベント ループの異常終了を記録し、サービスの停止を要求します。
 */
static void record_loop_failure(void)
{
    cplat_atomic_store_i32(&s_loop_failed, 1, CPLAT_MEMORY_ORDER_RELEASE);
    svc_request_stop();
}

/* Doxygen コメントは、ヘッダーに記載 */

int svc_linux_events_loop_failed(void)
{
    return (cplat_atomic_load_i32(&s_loop_failed, CPLAT_MEMORY_ORDER_ACQUIRE) != 0) ? 1 : 0;
}

void svc_linux_events_enter_running(void)
{
    int32_t expected = SVC_LINUX_EVENTS_PHASE_STARTING;

    /* リロード後の READY=1 の再送では段階を変えないため、起動中の場合だけ動作中へ進める。結果は判定に使わない。 */
    (void)cplat_atomic_compare_exchange_i32(&s_phase, &expected, SVC_LINUX_EVENTS_PHASE_RUNNING,
                                            CPLAT_MEMORY_ORDER_ACQ_REL);
}

void svc_linux_events_enter_stopping(void)
{
    cplat_atomic_store_i32(&s_phase, SVC_LINUX_EVENTS_PHASE_STOPPING, CPLAT_MEMORY_ORDER_RELEASE);
}

/*
 * ============================================================
 *  SIGHUP ハンドラー (self-pipe)
 * ============================================================
 */

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

    if (s_ctx.reload_fd >= 0)
    {
        value = 1;
        written = write(s_ctx.reload_fd, &value, sizeof(value));
        (void)written;
    }
}

/*
 * ============================================================
 *  inhibitor lock
 * ============================================================
 */

/**
 *  @brief          logind の delay inhibitor lock を取得します。
 *  @param[in]      what    対象 ("sleep" または "shutdown")。
 *  @return         取得した lock の fd。失敗時は -1 を返します。
 *
 *  lock は fd の close で解放されます。\n
 *  delay lock のため、イベント発生から logind の InhibitDelayMaxSec
 *  (既定 5 [s]) が経過すると lock の解放を待たずに処理が進みます。
 */
static int acquire_inhibit_lock(const char *what)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = NULL;
    int fd;
    int lock_fd;
    int rc;

    lock_fd = -1;
    rc = sd_bus_call_method(s_ctx.bus, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "Inhibit", &error, &reply,
                            "ssss", what, s_ctx.def->name, "イベント コールバックの実行猶予を確保するため", "delay");
    if (rc < 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "inhibitor lock (%s) の取得に失敗しました: %s", what, strerror(-rc));
    }
    else
    {
        rc = sd_bus_message_read(reply, "h", &fd);
        if (rc < 0)
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                      "inhibitor lock (%s) の fd を取得できませんでした: %s", what, strerror(-rc));
        }
        else
        {
            /*
             * fd は reply メッセージが所有するため、複製して保持する。
             * 複製先は、標準入出力 (0 から 2) より後の最小の fd 番号とする。
             * see: https://man7.org/linux/man-pages/man2/fcntl.2.html
             */
            lock_fd = fcntl(fd, F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
            if (lock_fd < 0)
            {
                (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
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
    /*
     * 解放の処理であり、Linux の close は失敗しても fd を解放して再試行できないため、
     * この関数の cplat_close の戻り値は明示的に破棄する。
     * see: https://man7.org/linux/man-pages/man2/close.2.html
     */
    if (s_ctx.inhibit_sleep_fd >= 0)
    {
        (void)cplat_close(s_ctx.inhibit_sleep_fd, NULL);
        s_ctx.inhibit_sleep_fd = -1;
    }

    if (s_ctx.inhibit_shutdown_fd >= 0)
    {
        (void)cplat_close(s_ctx.inhibit_shutdown_fd, NULL);
        s_ctx.inhibit_shutdown_fd = -1;
    }
}

/*
 * ============================================================
 *  D-Bus シグナル ハンドラー
 * ============================================================
 */

/**
 *  @brief          OS イベントを、動作中の段階に限って配送します。
 *  @param[in]      info    イベントの付加情報。
 *
 *  動作中の段階でない場合は配送せず、トレースへ記録します。
 */
static void dispatch_event_while_running(const svc_event_info *info)
{
    if (is_running_phase() == 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                                  "動作中ではないため OS イベントを配送しません (種別: %d)。", (int)info->type);
        return;
    }

    svc_dispatch_event(s_ctx.def, info);
}

/**
 *  @brief          logind の PrepareForSleep シグナルを処理します。
 *  @param[in]      m           受信したシグナル メッセージ。
 *  @param[in]      userdata    未使用。
 *  @param[in]      ret_error   未使用。
 *  @return         常に 0 を返します。
 *
 *  サスペンド開始時はイベント配送後に sleep lock を解放してサスペンドを
 *  許可し、復帰時はイベント配送後に次のサスペンドに備えて再取得します。\n
 *  動作中の段階でない場合もイベントは配送しませんが、lock の解放と再取得は行います。
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
        dispatch_event_while_running(&info);

        /* lock を解放してサスペンドを許可する。 */
        if (s_ctx.inhibit_sleep_fd >= 0)
        {
            /*
             * Linux の close は失敗しても fd を解放して再試行できないため、戻り値は明示的に破棄する。
             * see: https://man7.org/linux/man-pages/man2/close.2.html
             */
            (void)cplat_close(s_ctx.inhibit_sleep_fd, NULL);
            s_ctx.inhibit_sleep_fd = -1;
        }
    }
    else
    {
        info.type = SVC_EVENT_POWER_RESUME;
        dispatch_event_while_running(&info);

        /* 次のサスペンドに備えて lock を再取得する。 */
        s_ctx.inhibit_sleep_fd = acquire_inhibit_lock("sleep");
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
        dispatch_event_while_running(&info);

        /* lock を解放してシャットダウンを許可する。 */
        if (s_ctx.inhibit_shutdown_fd >= 0)
        {
            /*
             * Linux の close は失敗しても fd を解放して再試行できないため、戻り値は明示的に破棄する。
             * see: https://man7.org/linux/man-pages/man2/close.2.html
             */
            (void)cplat_close(s_ctx.inhibit_shutdown_fd, NULL);
            s_ctx.inhibit_shutdown_fd = -1;
        }
    }
    else
    {
        /* シャットダウンが取り消された場合に備えて lock を再取得する。 */
        if (s_ctx.inhibit_shutdown_fd < 0)
        {
            s_ctx.inhibit_shutdown_fd = acquire_inhibit_lock("shutdown");
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
    dispatch_event_while_running(&info);

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

/*
 * ============================================================
 *  eventfd ハンドラー
 * ============================================================
 */

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

    /*
     * ループを終了できない場合も svc_linux_events_stop() が待機の上限の後にプロセスを異常終了させるため、
     * 戻り値は明示的に破棄する。
     */
    (void)sd_event_exit(sd_event_source_get_event(source), 0);

    return 0;
}

/**
 *  @brief          SIGHUP 転送 (eventfd) を受けて設定再読込を配送します。
 *  @param[in]      source      イベント ソース (未使用)。
 *  @param[in]      fd          reload 用 eventfd。
 *  @param[in]      revents     発生したイベント (未使用)。
 *  @param[in]      userdata    未使用。
 *  @return         常に 0 を返します。
 *
 *  on_reload が NULL の場合と、動作中の段階でない場合は、配送せずにトレースへ記録します。
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

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    if (s_ctx.def->on_reload == NULL)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                 "on_reload が設定されていないため、SIGHUP を無視します。");
        return 0;
    }

    if (is_running_phase() == 0)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                                 "動作中ではないため設定再読込要求を配送しません。");
        return 0;
    }

    svc_dispatch_reload(s_ctx.def);

    return 0;
}

/*
 * ============================================================
 *  watchdog
 * ============================================================
 */

/**
 *  @brief          watchdog の確認周期ごとに on_run() の進行を確かめ、WATCHDOG=1 を送ります。
 *  @param[in]      source      タイマーのイベント ソース。
 *  @param[in]      usec        タイマーの予定時刻 [us] (CLOCK_MONOTONIC)。
 *  @param[in]      userdata    未使用。
 *  @return         常に 0 を返します。
 *
 *  動作中の段階で、停止要求がなく、on_run() が進行した場合だけ WATCHDOG=1 を送ります。\n
 *  タイマーを再設定できない場合は、イベント ループの異常終了として記録し、停止を要求します。
 */
static int on_watchdog_timer(sd_event_source *source, uint64_t usec, void *userdata)
{
    int rc;

    (void)userdata;

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    if ((is_running_phase() != 0) && (svc_stop_requested() == 0))
    {
        if (svc_on_run_made_progress(&s_ctx.watchdog_seen_count) != 0)
        {
            rc = sd_notify(0, "WATCHDOG=1");
            if (rc < 0)
            {
                /* 次の周期で再び送る。 */
                (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                          "WATCHDOG=1 の送信に失敗しました: %s", strerror(-rc));
            }
        }
        else if (s_ctx.watchdog_warned == 0)
        {
            s_ctx.watchdog_warned = 1;
            (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                     "on_run が svc_wait_for_stop() を呼んでいないため、WATCHDOG=1 を送りません。");
        }
    }

    rc = sd_event_source_set_time(source, usec + s_ctx.watchdog_interval_usec);
    if (rc >= 0)
    {
        rc = sd_event_source_set_enabled(source, SD_EVENT_ONESHOT);
    }

    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "watchdog のタイマーを再設定できないため、サービスを停止します: %s", strerror(-rc));
        record_loop_failure();
    }

    return 0;
}

/**
 *  @brief          watchdog が要求されている場合に、確認のタイマーを登録します。
 *  @return         成功時と watchdog が要求されていない場合は 0、失敗時は -1 を返します。
 *
 *  sd_event_set_watchdog() は on_run() の状態によらず WATCHDOG=1 を送るため、使いません。\n
 *  Oracle Linux 8 の systemd 239 で使える API に合わせるため、sd_event_add_time_relative() は使いません。
 */
static int setup_watchdog_timer(void)
{
    uint64_t watchdog_usec = 0U;
    uint64_t now = 0U;
    int rc;

    rc = sd_watchdog_enabled(0, &watchdog_usec);
    if (rc < 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "watchdog の設定を読み取れません: %s", strerror(-rc));
        return -1;
    }

    if (rc == 0)
    {
        /* watchdog が要求されていない (WATCHDOG_USEC がない、systemd の外で実行した)。 */
        return 0;
    }

    s_ctx.watchdog_interval_usec = watchdog_usec / SVC_WATCHDOG_CHECK_DIVISOR;
    s_ctx.watchdog_warned = 0;

    /*
     * 進行の計数は、プロセスの中で以前に呼ばれた svc_wait_for_stop() の分も含む。
     * 以前の呼び出しを on_run() の進行と数えないよう、現在の計数を記録してから確認を始める。
     * 戻り値は記録のための呼び出しであり、判定に使わないため、明示的に破棄する。
     */
    s_ctx.watchdog_seen_count = 0U;
    (void)svc_on_run_made_progress(&s_ctx.watchdog_seen_count);

    rc = sd_event_now(s_ctx.event, CLOCK_MONOTONIC, &now);
    if (rc >= 0)
    {
        rc = sd_event_add_time(s_ctx.event, &s_ctx.watchdog_source, CLOCK_MONOTONIC, now + s_ctx.watchdog_interval_usec,
                               SVC_WATCHDOG_TIMER_ACCURACY_USEC, on_watchdog_timer, NULL);
    }

    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "watchdog のタイマーを登録できません: %s", strerror(-rc));
        s_ctx.watchdog_source = NULL;
        return -1;
    }

    return 0;
}

/*
 * ============================================================
 *  D-Bus 監視の構築
 * ============================================================
 */

/**
 *  @brief          system bus に接続して logind のシグナル監視を構築します。
 *
 *  接続や購読に失敗した場合は WARNING を出力して該当機能のみ無効化します
 *  (コンテナーなど D-Bus が存在しない環境への対応)。
 */
static void setup_bus_monitoring(void)
{
    int rc;

    rc = sd_bus_open_system(&s_ctx.bus);
    if (rc < 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "D-Bus (system bus) に接続できないため電源・セッション イベントは無効です: %s",
                                  strerror(-rc));
        s_ctx.bus = NULL;
        return;
    }

    rc = sd_bus_match_signal(s_ctx.bus, NULL, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "PrepareForSleep",
                             on_prepare_for_sleep, NULL);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "PrepareForSleep の購読に失敗しました: %s", strerror(-rc));
    }

    rc = sd_bus_match_signal(s_ctx.bus, NULL, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "PrepareForShutdown",
                             on_prepare_for_shutdown, NULL);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "PrepareForShutdown の購読に失敗しました: %s", strerror(-rc));
    }

    rc = sd_bus_match_signal(s_ctx.bus, NULL, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "SessionNew",
                             on_session_new, NULL);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "SessionNew の購読に失敗しました: %s", strerror(-rc));
    }

    rc = sd_bus_match_signal(s_ctx.bus, NULL, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE, "SessionRemoved",
                             on_session_removed, NULL);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "SessionRemoved の購読に失敗しました: %s", strerror(-rc));
    }

    /* イベント発生時にコールバックを実行する猶予を確保するため、delay lock を取得する。 */
    s_ctx.inhibit_sleep_fd = acquire_inhibit_lock("sleep");
    s_ctx.inhibit_shutdown_fd = acquire_inhibit_lock("shutdown");

    rc = sd_bus_attach_event(s_ctx.bus, s_ctx.event, SD_EVENT_PRIORITY_NORMAL);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "D-Bus をイベント ループに接続できないため電源・セッション イベントは無効です: %s",
                                  strerror(-rc));
        release_inhibit_locks();
        sd_bus_flush_close_unref(s_ctx.bus);
        s_ctx.bus = NULL;
    }
}

/*
 * ============================================================
 *  イベント監視スレッド
 * ============================================================
 */

/**
 *  @brief          初期化の状態を設定し、svc_linux_events_start() へ通知します。
 *  @param[in]      state   設定する状態 (svc_linux_events_init_state の値)。
 */
static void notify_init_state(const int state)
{
    cplat_local_lock_lock(s_ctx.init_lock);
    s_ctx.init_state = state;
    cplat_condvar_broadcast(s_ctx.init_cv);
    cplat_local_lock_unlock(s_ctx.init_lock);
}

/**
 *  @brief          スレッドの中で生成したイベント ソースとイベント ループを解放します。
 */
static void release_event_loop(void)
{
    if (s_ctx.watchdog_source != NULL)
    {
        s_ctx.watchdog_source = sd_event_source_unref(s_ctx.watchdog_source);
    }

    if (s_ctx.reload_source != NULL)
    {
        s_ctx.reload_source = sd_event_source_unref(s_ctx.reload_source);
    }

    if (s_ctx.stop_source != NULL)
    {
        s_ctx.stop_source = sd_event_source_unref(s_ctx.stop_source);
    }

    if (s_ctx.event != NULL)
    {
        s_ctx.event = sd_event_unref(s_ctx.event);
    }
}

/**
 *  @brief          イベント ループの必須の初期化を行います。
 *  @return         成功時は 0、失敗時は -1 を返します。
 *
 *  イベント ループの生成、watchdog が要求されている場合のタイマー登録、停止監視の登録、
 *  およびリロードの監視の登録を行います。いずれかが失敗した場合は、サービスを開始しません。
 */
static int setup_required_sources(void)
{
    int rc;

    rc = sd_event_new(&s_ctx.event);
    if (rc < 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "sd_event の生成に失敗しました: %s",
                                  strerror(-rc));
        s_ctx.event = NULL;
        return -1;
    }

    if (setup_watchdog_timer() != 0)
    {
        return -1;
    }

    rc = sd_event_add_io(s_ctx.event, &s_ctx.stop_source, s_ctx.stop_fd, EPOLLIN, on_stop_requested, NULL);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "停止監視の登録に失敗しました: %s",
                                  strerror(-rc));
        s_ctx.stop_source = NULL;
        return -1;
    }

    /* 同じ優先度の D-Bus シグナルが並んでいても、停止指示を先に処理する。 */
    rc = sd_event_source_set_priority(s_ctx.stop_source, SD_EVENT_PRIORITY_IMPORTANT);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "停止監視の優先度を設定できませんでした: %s", strerror(-rc));
    }

    rc = sd_event_add_io(s_ctx.event, &s_ctx.reload_source, s_ctx.reload_fd, EPOLLIN, on_reload_requested, NULL);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "reload 監視の登録に失敗しました: %s", strerror(-rc));
        s_ctx.reload_source = NULL;
        return -1;
    }

    return 0;
}

/**
 *  @brief          イベント監視スレッドの本体。
 *  @param[in]      arg 未使用。
 *
 *  必須の初期化を行って結果を svc_linux_events_start() へ通知し、成功した場合は sd_event ループを実行します。\n
 *  終了時に、スレッドの中で確保した資源を解放します。
 */
static void events_thread_func(void *arg)
{
    int rc;

    (void)arg;

    if (setup_required_sources() != 0)
    {
        release_event_loop();
        notify_init_state(SVC_LINUX_EVENTS_INIT_FAILED);
        return;
    }

    /* D-Bus の監視は任意の機能であり、失敗しても該当機能だけを無効にして続ける。 */
    if (s_ctx.def->on_event != NULL)
    {
        setup_bus_monitoring();
    }

    notify_init_state(SVC_LINUX_EVENTS_INIT_SUCCEEDED);

    rc = sd_event_loop(s_ctx.event);
    if (rc < 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "イベント ループがエラーで終了したため、サービスを停止します: %s", strerror(-rc));
        record_loop_failure();
    }

    release_inhibit_locks();

    if (s_ctx.bus != NULL)
    {
        sd_bus_flush_close_unref(s_ctx.bus);
        s_ctx.bus = NULL;
    }

    release_event_loop();
}

/*
 * ============================================================
 *  起動・停止 (svc_os_run_service から呼ばれる)
 * ============================================================
 */

/**
 *  @brief          start 失敗時・停止時にスレッド外の資源を解放します。
 *
 *  SIGHUP ハンドラーの復元 → eventfd の close の順に行います
 *  (close を先にするとハンドラーが解放済み fd へ書き込む可能性があるため)。\n
 *  起動の待ち合わせのロックと条件変数は、スレッドが通知を終えた後 (終了待ちの後) に破棄します。
 */
static void release_local_resources(void)
{
    if (s_sighup_installed != 0)
    {
        /*
         * 有効なシグナル番号 SIGHUP と保存済みの設定を渡し、sigaction の失敗の条件 (EFAULT、EINVAL) に
         * 当たらないため、戻り値は明示的に破棄する。
         * see: https://man7.org/linux/man-pages/man2/sigaction.2.html
         */
        (void)sigaction(SIGHUP, &s_old_sighup_action, NULL);
        s_sighup_installed = 0;
    }

    /*
     * 解放の処理であり、Linux の close は失敗しても fd を解放して再試行できないため、
     * この 2 つの cplat_close の戻り値は明示的に破棄する。
     * see: https://man7.org/linux/man-pages/man2/close.2.html
     */
    if (s_ctx.reload_fd >= 0)
    {
        (void)cplat_close(s_ctx.reload_fd, NULL);
        s_ctx.reload_fd = -1;
    }

    if (s_ctx.stop_fd >= 0)
    {
        (void)cplat_close(s_ctx.stop_fd, NULL);
        s_ctx.stop_fd = -1;
    }

    if (s_ctx.init_cv != NULL)
    {
        cplat_condvar_dispose(s_ctx.init_cv);
        s_ctx.init_cv = NULL;
    }

    if (s_ctx.init_lock != NULL)
    {
        cplat_local_lock_dispose(s_ctx.init_lock);
        s_ctx.init_lock = NULL;
    }

    s_ctx.def = NULL;
}

/**
 *  @brief          SIGHUP を reload 用 eventfd へ転送するハンドラーを設定します。
 *  @return         成功時は 0、失敗時は -1 を返します。
 */
static int install_sighup_handler(void)
{
    struct sigaction action = {0};

    action.sa_handler = sighup_signal_handler;

    /*
     * シグナル番号を取らない sigemptyset は、失敗の条件 (signum の不正) に当たらないため、
     * 戻り値は明示的に破棄する。
     * see: https://man7.org/linux/man-pages/man3/sigemptyset.3.html
     */
    (void)sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;

    if (sigaction(SIGHUP, &action, &s_old_sighup_action) != 0)
    {
        return -1;
    }

    s_sighup_installed = 1;

    return 0;
}

/* Doxygen コメントは、ヘッダーに記載 */

int svc_linux_events_start(const svc_definition *def)
{
    int result;
    int init_state;

    if (def == NULL)
    {
        return -1;
    }

    if (s_ctx.thread != NULL)
    {
        /* すでに起動済みの場合は何もしない。 */
        return 0;
    }

    s_ctx.def = def;
    s_ctx.init_state = SVC_LINUX_EVENTS_INIT_PENDING;
    cplat_atomic_store_i32(&s_loop_failed, 0, CPLAT_MEMORY_ORDER_RELEASE);
    cplat_atomic_store_i32(&s_phase, SVC_LINUX_EVENTS_PHASE_STARTING, CPLAT_MEMORY_ORDER_RELEASE);

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    if ((cplat_local_lock_create(&s_ctx.init_lock) != CPLAT_OK) || (cplat_condvar_create(&s_ctx.init_cv) != CPLAT_OK))
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "イベント監視スレッドの起動を待ち合わせるロックまたは条件変数を生成できません。");
        release_local_resources();
        return -1;
    }

    s_ctx.stop_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (s_ctx.stop_fd < 0)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "停止指示用 eventfd の生成に失敗しました。");
        release_local_resources();
        return -1;
    }

    /*
     * on_reload の有無によらず、SIGHUP を受ける準備をする。
     * ハンドラーがないと、SIGHUP の既定の動作でプロセスが終了するためである。
     */
    s_ctx.reload_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (s_ctx.reload_fd < 0)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "reload 用 eventfd の生成に失敗しました。");
        release_local_resources();
        return -1;
    }

    if (install_sighup_handler() != 0)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "SIGHUP ハンドラーの設定に失敗しました。");
        release_local_resources();
        return -1;
    }

    result = cplat_thread_create(&s_ctx.thread, events_thread_func, NULL);
    if (result != CPLAT_OK)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                  "イベント監視スレッドの起動に失敗しました (結果コード: %d)。", result);
        s_ctx.thread = NULL;
        release_local_resources();
        return -1;
    }

    /*
     * スレッドの必須の初期化の完了を無期限に待つ。
     * スレッドの中の呼び出しには個別の時間上限 (D-Bus の同期呼び出しは既定 25 [s]) があり、
     * 全体は systemd の TimeoutStartSec= (既定 90 [s]) が打ち切る。
     */
    cplat_local_lock_lock(s_ctx.init_lock);
    while (s_ctx.init_state == SVC_LINUX_EVENTS_INIT_PENDING)
    {
        cplat_condvar_wait(s_ctx.init_cv, s_ctx.init_lock);
    }
    init_state = s_ctx.init_state;
    cplat_local_lock_unlock(s_ctx.init_lock);

    if (init_state != SVC_LINUX_EVENTS_INIT_SUCCEEDED)
    {
        /* スレッドは失敗を通知した後に終了するため、終了を待ってから資源を解放する。 */
        cplat_thread_join(s_ctx.thread);
        s_ctx.thread = NULL;
        release_local_resources();
        return -1;
    }

    return 0;
}

int svc_linux_events_stop(void)
{
    int result;
    uint64_t value;
    ssize_t written;

    if (s_ctx.thread != NULL)
    {
        value = 1;
        written = write(s_ctx.stop_fd, &value, sizeof(value));
        (void)written;

        result = cplat_thread_timed_join(s_ctx.thread, SVC_EVENTS_JOIN_TIMEOUT_MS);
        if (result != CPLAT_OK)
        {
            /*
             * スレッドが動いたまま終了を続けると、スレッドが解放済みのサービス定義とトレースを参照する。
             * 続行が危険なため、資源を解放せずにプロセスを異常終了させる。
             * 利用側のコールバックの停滞による場合も含め、共通のコーディング規範の「プロセスの継続が危険な場合」に当たる。
             * トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。
             */
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                      "イベント監視スレッドが %d [ms] 以内に終了しないため、資源を解放せずに"
                                      "プロセスを異常終了します (結果コード: %d)。",
                                      SVC_EVENTS_JOIN_TIMEOUT_MS, result);
            CPLAT_ABORT("イベント監視スレッドが時間内に終了しません");
        }

        s_ctx.thread = NULL;
    }

    release_local_resources();

    if (cplat_atomic_load_i32(&s_loop_failed, CPLAT_MEMORY_ORDER_ACQUIRE) != 0)
    {
        return -1;
    }

    return 0;
}

#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)
    #pragma warning(disable : 4206)
#endif
