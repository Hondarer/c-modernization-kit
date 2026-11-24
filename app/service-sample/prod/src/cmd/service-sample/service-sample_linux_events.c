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
 *  SIGHUP は、プロセスの入口 (svc_os_prepare_run()) で、スレッドを生成する前に全スレッドでブロックし、
 *  イベント ループが sd_event_add_signal() で受けます。スレッドは作成元のマスクを引き継ぐため、
 *  入口でブロックすれば、すべてのスレッドでブロックされた状態になります。
 *  sd_event_add_signal() は、シグナルが全スレッドでブロックされていることを前提とします。
 *  cplat の tracer と停止要求の登録は、スレッドを生成しません (Linux)。
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_event_add_signal.html
 *  see: https://man7.org/linux/man-pages/man3/pthread_sigmask.3.html
 *  see: https://man7.org/linux/man-pages/man7/signal.7.html
 *  see: https://man7.org/linux/man-pages/man2/signalfd.2.html
 *  see: https://man7.org/linux/man-pages/man3/raise.3.html
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
    #include <sys/signalfd.h>
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
    #include "service-sample_internal.h"
    #include "service-sample_linux_events.h"

/* Doxygen コメントは、ヘッダーに記載 */

/*
 * ============================================================
 *  内部定数
 * ============================================================
 */

    /** logind の Inhibit の呼び出しの期限 [ms]。起動時の同期呼び出しと、運転中の非同期の呼び出しで、呼び出しごとに渡す。 */
    #define SVC_INHIBIT_CALL_TIMEOUT_MS 10000U

    /** 起動時に同期で呼ぶ Inhibit の回数 (sleep と shutdown)。 */
    #define SVC_INHIBIT_STARTUP_CALL_COUNT 2U

    /**
     *  systemd の既定の起動タイムアウト (DefaultTimeoutStartSec=) [ms]。unit に TimeoutStartSec= を指定していない。
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd-system.conf.html
     */
    #define SVC_SYSTEMD_DEFAULT_TIMEOUT_START_MS 90000U

    /**
     *  systemd の既定の停止タイムアウト (DefaultTimeoutStopSec=) [ms] です。
     *  unit に TimeoutStopSec= を指定していないため、この既定値が適用されます。
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd-system.conf.html
     */
    #define SVC_SYSTEMD_DEFAULT_TIMEOUT_STOP_MS 90000U

    /**
     *  イベント監視スレッドの終了を待機する時間 [ms]。
     *
     *  運転中の D-Bus の呼び出しはイベント ループを止めないため、この待ち時間が必要とする根拠は、
     *  利用者のコールバック (on_event、on_reload) の完了だけです。\n
     *  systemd の停止タイムアウト (TimeoutStopSec=。unit で指定していないため、既定の 90 [s]) より小さくします。\n
     *  unit の WatchdogSec (30 [s]) 以下です。\n
     *  この時間内に終了しない場合は、資源を解放せずにプロセスを異常終了させます。\n
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html \n
     *  see: https://www.freedesktop.org/software/systemd/man/latest/systemd-system.conf.html
     */
    #define SVC_EVENTS_JOIN_TIMEOUT_MS 30000

static_assert((SVC_INHIBIT_CALL_TIMEOUT_MS * SVC_INHIBIT_STARTUP_CALL_COUNT) < SVC_SYSTEMD_DEFAULT_TIMEOUT_START_MS,
              "inhibit calls at startup must finish before TimeoutStartSec");
static_assert(SVC_EVENTS_JOIN_TIMEOUT_MS < SVC_SYSTEMD_DEFAULT_TIMEOUT_STOP_MS,
              "join timeout must be shorter than TimeoutStopSec");
static_assert(SVC_EVENTS_JOIN_TIMEOUT_MS <= (SVC_UNIT_WATCHDOG_SEC * 1000U),
              "join timeout must not exceed WatchdogSec");

    /**
     *  watchdog の確認周期を求めるための除数です (期限の 1/4)。
     *  on_run() の呼び出し間隔の上限 (期限の 1/2 未満) と合わせて、通知間隔の最大値を期限の 3/4 未満に保ちます。
     *  運転中の D-Bus の呼び出しはイベント ループを止めないため、この計算に D-Bus の項は入れません。
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
 *  @brief          logind の delay inhibitor lock 1 件の状態。
 *
 *  fd は複製した lock の fd です。未取得は -1 です。\n
 *  slot は応答待ちの非同期呼び出しです。応答待ちでなければ NULL です。\n
 *  'h' で受け取った fd は応答メッセージが所有するため、複製した fd だけを保持します。
 */
typedef struct svc_inhibit_lock
{
    const char *what;  /**< 対象 ("sleep" または "shutdown")。 */
    sd_bus_slot *slot; /**< 応答待ちの呼び出し。応答待ちでなければ NULL。 */
    int fd;            /**< 取得済みの lock の fd。未取得は -1。 */
    int pad;           /**< 明示的アラインメントです。0 を指定します。 */
} svc_inhibit_lock;

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
    sd_event_source *reload_source;   /**< SIGHUP の監視。スレッド内で生成・解放します。 */
    sd_event_source *watchdog_source; /**< watchdog の確認のタイマー。watchdog が要求されていない場合は NULL。 */
    cplat_local_lock *init_lock;      /**< init_state を保護するロック。 */
    cplat_condvar *init_cv;           /**< init_state の変化を通知する条件変数。 */
    cplat_local_lock *notify_lock;    /**< 動作段階と systemd 通知を直列化するロック。 */
    uint64_t watchdog_interval_usec;  /**< watchdog の確認周期 [us]。 */
    uint32_t watchdog_seen_count;     /**< 前回の確認で記録した on_run() の進行の計数。 */
    int watchdog_warned;              /**< on_run() の停滞の WARNING を出力済みかどうか。1 = 出力済み。 */
    int init_state;                   /**< 初期化の状態 (svc_linux_events_init_state の値)。init_lock で保護。 */
    int stop_fd;                      /**< 停止指示用 eventfd。未生成時は -1。 */
    svc_inhibit_lock inhibit_sleep;   /**< サスペンドの delay inhibitor lock。 */
    svc_inhibit_lock inhibit_shutdown; /**< シャットダウンの delay inhibitor lock。 */
} svc_linux_events_ctx;

/** イベント監視スレッドの内部状態 (プロセスで 1 つ)。 */
static svc_linux_events_ctx s_ctx = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0U, 0U, 0, SVC_LINUX_EVENTS_INIT_PENDING, -1,
    {"sleep", NULL, -1, 0},
    {"shutdown", NULL, -1, 0}};

/** 実行中のイベント ループの異常終了を記録します。1 = 異常終了した。 */
static cplat_atomic_i32 s_loop_failed = CPLAT_ATOMIC_INIT(0);

/** サービスの動作段階 (svc_linux_events_phase の値)。通知ロックで保護します。 */
static svc_linux_events_phase s_phase = SVC_LINUX_EVENTS_PHASE_STARTING;

/*
 * ============================================================
 *  動作段階と異常終了の記録
 * ============================================================
 */

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

/* Doxygen コメントは、ヘッダーに記載 */

void svc_linux_events_notify_lock(void)
{
    if (s_ctx.notify_lock != NULL)
    {
        cplat_local_lock_lock(s_ctx.notify_lock);
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

void svc_linux_events_notify_unlock(void)
{
    if (s_ctx.notify_lock != NULL)
    {
        cplat_local_lock_unlock(s_ctx.notify_lock);
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

void svc_linux_events_enter_running(void)
{
    /* リロード後の READY=1 の再送では段階を変えないため、起動中の場合だけ動作中へ進める。 */
    if (s_phase == SVC_LINUX_EVENTS_PHASE_STARTING)
    {
        s_phase = SVC_LINUX_EVENTS_PHASE_RUNNING;
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

void svc_linux_events_enter_stopping(void)
{
    s_phase = SVC_LINUX_EVENTS_PHASE_STOPPING;
}

/* Doxygen コメントは、ヘッダーに記載 */

int svc_linux_events_is_running(void)
{
    return (s_phase == SVC_LINUX_EVENTS_PHASE_RUNNING) ? 1 : 0;
}

/* Doxygen コメントは、ヘッダーに記載 */

int svc_linux_events_is_stopping(void)
{
    return (s_phase == SVC_LINUX_EVENTS_PHASE_STOPPING) ? 1 : 0;
}

/*
 * ============================================================
 *  inhibitor lock
 * ============================================================
 */

/**
 *  @brief          logind の Inhibit の要求メッセージを作ります。
 *  @param[in]      what        対象 ("sleep" または "shutdown")。
 *  @param[out]     message     作成したメッセージ。失敗時は NULL。
 *  @return         成功時は CPLAT_OK、失敗時は負の systemd のエラーを返します。
 *
 *  引数は、対象、サービス名、理由、"delay" の 4 つです。\n
 *  作成に失敗した場合は、作成済みのメッセージがあれば 1 回だけ解放し、*message を NULL に戻します。
 */
static int build_inhibit_message(const char *what, sd_bus_message **message)
{
    const char *reason = "イベント コールバックの実行猶予を確保するため";
    const char *mode = "delay";
    const char *service_name;
    int rc;

    *message = NULL;
    rc = sd_bus_message_new_method_call(s_ctx.bus, message, LOGIND_SERVICE, LOGIND_OBJECT, LOGIND_INTERFACE,
                                        "Inhibit");
    if (rc < 0)
    {
        if (*message != NULL)
        {
            /*
             * 作成途中のメッセージは、この関数が所有するため、1 回だけ解放する。
             * 解放後のポインターは判定に使わないため、戻り値は明示的に破棄する。
             */
            (void)sd_bus_message_unref(*message);
            *message = NULL;
        }

        return rc;
    }

    service_name = s_ctx.def->name;
    /* 's' は、文字列そのものではなく、const char * を指すポインターを渡す。 */
    rc = sd_bus_message_append_basic(*message, 's', &what);
    if (rc >= 0)
    {
        rc = sd_bus_message_append_basic(*message, 's', &service_name);
    }

    if (rc >= 0)
    {
        rc = sd_bus_message_append_basic(*message, 's', &reason);
    }

    if (rc >= 0)
    {
        rc = sd_bus_message_append_basic(*message, 's', &mode);
    }

    if (rc < 0)
    {
        /* 作成済みのメッセージは、この関数が所有するため、1 回だけ解放する。解放後のポインターは判定に使わないため、戻り値は明示的に破棄する。 */
        (void)sd_bus_message_unref(*message);
        *message = NULL;
        return rc;
    }

    return CPLAT_OK;
}

/**
 *  @brief          応答から delay inhibitor lock の fd を複製して保持します。
 *  @param[in,out]  lock    対象の lock。
 *  @param[in]      reply   応答メッセージ。'h' の fd は、このメッセージが所有します。
 *  @return         成功時は CPLAT_OK、失敗時は CPLAT_ERR_UNKNOWN を返します。
 *
 *  すでに fd を保持している場合は、メッセージを読まず、既存の fd を変えません。\n
 *  読み取りの成功は 1 です。0 は配列の終端、負値は失敗であり、どちらも fcntl() を呼びません。\n
 *  複製に失敗した場合は、lock->fd を -1 のままにし、借用した fd は閉じません。\n
 *  see: https://github.com/systemd/systemd/blob/v239/src/libsystemd/sd-bus/bus-message.c \n
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_message_read_basic.html \n
 *  see: https://man7.org/linux/man-pages/man2/fcntl.2.html
 */
static int store_inhibit_fd(svc_inhibit_lock *lock, sd_bus_message *reply)
{
    int fd;
    int lock_fd;

    if (lock->fd != -1)
    {
        return CPLAT_OK;
    }

    fd = -1;
    if (sd_bus_message_read_basic(reply, 'h', &fd) <= 0)
    {
        return CPLAT_ERR_UNKNOWN;
    }

    /*
     * fd は reply メッセージが所有するため、複製して保持する。
     * 複製先は、標準入出力 (0 から 2) より後の最小の fd 番号とする。
     * 借用した fd は閉じない。
     * see: https://man7.org/linux/man-pages/man2/fcntl.2.html
     */
    lock_fd = fcntl(fd, F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
    if (lock_fd < 0)
    {
        return CPLAT_ERR_UNKNOWN;
    }

    lock->fd = lock_fd;

    return CPLAT_OK;
}

/**
 *  @brief          起動時に、logind の delay inhibitor lock を同期で取得します。
 *  @param[in,out]  lock    対象の lock。未取得の fd は -1 のままにします。
 *
 *  期限は、呼び出しごとに 10 [s] です。失敗しても起動は続け、WARNING を出します。\n
 *  要求メッセージ、非 NULL の応答、sd_bus_error は、成功と失敗のすべての経路で 1 回解放します。\n
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_call.html \n
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_error.html
 */
static void acquire_inhibit_lock_sync(svc_inhibit_lock *lock)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus_message *reply = NULL;
    int rc;

    lock->fd = -1;
    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    rc = build_inhibit_message(lock->what, &m);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "inhibitor lock (%s) の要求の作成に失敗しました: %s", lock->what, strerror(-rc));
    }
    else
    {
        rc = sd_bus_call(s_ctx.bus, m, (uint64_t)SVC_INHIBIT_CALL_TIMEOUT_MS * 1000ULL, &error, &reply);
        if (rc < 0)
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                      "inhibitor lock (%s) の取得に失敗しました: %s", lock->what, strerror(-rc));
        }
        else if (store_inhibit_fd(lock, reply) != CPLAT_OK)
        {
            (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                      "inhibitor lock (%s) の fd を保存できませんでした。", lock->what);
        }
    }

    if (m != NULL)
    {
        /* 要求メッセージは、この関数が所有するため、1 回解放する。解放後のポインターは判定に使わないため、戻り値は明示的に破棄する。 */
        (void)sd_bus_message_unref(m);
    }

    if (reply != NULL)
    {
        /* 同期の応答は、この関数が所有するため、1 回解放する。解放後のポインターは判定に使わないため、戻り値は明示的に破棄する。 */
        (void)sd_bus_message_unref(reply);
    }

    sd_bus_error_free(&error);
}

/**
 *  @brief          非同期の Inhibit の応答を処理します。
 *  @param[in]      m           応答メッセージ。借用であり、この関数は解放しません。
 *  @param[in]      userdata    対象の svc_inhibit_lock。
 *  @param[in]      ret_error   出力用のエラー。この関数は解放しません。
 *  @return         常に 0 を返します。期限切れの後に自動では再試行しません。
 *
 *  先頭で応答待ちの slot を解除します。応答が method error の場合、読み取りに失敗した場合、
 *  複製に失敗した場合は、fd を -1 のままにします。\n
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_call_async.html \n
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_slot_unref.html \n
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_error.html \n
 *  see: https://github.com/systemd/systemd/blob/v239/src/libsystemd/sd-bus/sd-bus.c
 */
static int on_inhibit_reply(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    svc_inhibit_lock *lock;
    const sd_bus_error *bus_error;
    const char *error_name;

    lock = (svc_inhibit_lock *)userdata;
    if (lock->slot != NULL)
    {
        lock->slot = sd_bus_slot_unref(lock->slot);
    }

    (void)ret_error;

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    if (sd_bus_message_is_method_error(m, NULL) != 0)
    {
        bus_error = sd_bus_message_get_error(m);
        error_name = "";
        if ((bus_error != NULL) && (bus_error->name != NULL))
        {
            error_name = bus_error->name;
        }

        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "inhibitor lock (%s) の応答がエラーです: %s", lock->what, error_name);
        return 0;
    }

    if (store_inhibit_fd(lock, m) != CPLAT_OK)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "inhibitor lock (%s) の fd を保存できませんでした。", lock->what);
    }

    return 0;
}

/**
 *  @brief          応答待ちの Inhibit の登録を解除します。
 *  @param[in,out]  lock    対象の lock。slot は NULL にします。
 *
 *  logind へ届いた要求自体は取り消しません。登録を解除すると、コールバックは呼ばれません。\n
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_slot_unref.html \n
 *  see: https://github.com/systemd/systemd/blob/v239/src/libsystemd/sd-bus/bus-slot.c
 */
static void cancel_inhibit_request(svc_inhibit_lock *lock)
{
    if (lock->slot != NULL)
    {
        lock->slot = sd_bus_slot_unref(lock->slot);
    }
}

/**
 *  @brief          運転中に、logind の delay inhibitor lock を非同期で要求します。
 *  @param[in,out]  lock    対象の lock。
 *
 *  取得済み、または応答待ちの場合は、新しい要求を作りません。\n
 *  期限は、呼び出しごとに 10 [s] です。失敗しても自動では再試行しません。\n
 *  要求メッセージは、成功と失敗を問わず 1 回解放します。\n
 *  see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_call_async.html
 */
static void request_inhibit_lock_async(svc_inhibit_lock *lock)
{
    sd_bus_message *m = NULL;
    int rc;

    if ((lock->fd >= 0) || (lock->slot != NULL))
    {
        return;
    }

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    rc = build_inhibit_message(lock->what, &m);
    if (rc < 0)
    {
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "inhibitor lock (%s) の要求の作成に失敗しました: %s", lock->what, strerror(-rc));
        return;
    }

    rc = sd_bus_call_async(s_ctx.bus, &lock->slot, m, on_inhibit_reply, lock,
                           (uint64_t)SVC_INHIBIT_CALL_TIMEOUT_MS * 1000ULL);
    /*
     * 要求メッセージは、呼び出し元であるこの関数が所有するため、成功と失敗を問わず 1 回解放する。
     * 解放後のポインターは判定に使わないため、戻り値は明示的に破棄する。
     */
    (void)sd_bus_message_unref(m);

    if (rc < 0)
    {
        if (lock->slot != NULL)
        {
            lock->slot = sd_bus_slot_unref(lock->slot);
        }

        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "inhibitor lock (%s) の非同期の要求に失敗しました: %s", lock->what, strerror(-rc));
    }
}

/**
 *  @brief          delay inhibitor lock 1 件の応答待ちと fd を解放します。
 *  @param[in,out]  lock    対象の lock。
 *
 *  応答待ちの slot を先に解除し、fd を 1 回だけ閉じます。close が失敗しても再試行せず、fd を -1 にします。\n
 *  閉じる前にバスは解放しません。\n
 *  see: https://man7.org/linux/man-pages/man2/close.2.html
 */
static void release_inhibit_lock(svc_inhibit_lock *lock)
{
    cancel_inhibit_request(lock);

    if (lock->fd >= 0)
    {
        /*
         * Linux の close は失敗しても fd を解放して再試行できないため、戻り値は明示的に破棄する。
         * see: https://man7.org/linux/man-pages/man2/close.2.html
         */
        (void)cplat_close(lock->fd, NULL);
        lock->fd = -1;
    }
}

/**
 *  @brief          保持しているすべての inhibitor lock を解放します。
 */
static void release_inhibit_locks(void)
{
    release_inhibit_lock(&s_ctx.inhibit_sleep);
    release_inhibit_lock(&s_ctx.inhibit_shutdown);
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
    int running;

    svc_linux_events_notify_lock();
    running = svc_linux_events_is_running();
    svc_linux_events_notify_unlock();

    if (running == 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                                  "動作中ではないため OS イベントを配送しません (種別: %d)。", (int)info->type);
        return;
    }

    svc_dispatch_event(s_ctx.def, info);
}

/**
 *  @brief          PrepareForSleep の開始と復帰を処理します。
 *  @param[in]      starting    0 以外はサスペンドの開始、0 は復帰。
 *
 *  開始時は、応答待ちを解除してからイベントを配送し、その後に fd を閉じます。\n
 *  復帰時は、イベントを配送してから非同期で lock を要求します。\n
 *  動作中の段階でない場合もイベントは配送しませんが、lock の解放と再取得は行います。
 */
static void handle_prepare_for_sleep(int starting)
{
    svc_event_info info;

    info.session_id = NULL;

    if (starting != 0)
    {
        cancel_inhibit_request(&s_ctx.inhibit_sleep);
        info.type = SVC_EVENT_POWER_SUSPEND;
        dispatch_event_while_running(&info);

        /* イベントの配送の後に fd を閉じ、サスペンドを許可する。 */
        if (s_ctx.inhibit_sleep.fd >= 0)
        {
            /*
             * Linux の close は失敗しても fd を解放して再試行できないため、戻り値は明示的に破棄する。
             * see: https://man7.org/linux/man-pages/man2/close.2.html
             */
            (void)cplat_close(s_ctx.inhibit_sleep.fd, NULL);
            s_ctx.inhibit_sleep.fd = -1;
        }
    }
    else
    {
        info.type = SVC_EVENT_POWER_RESUME;
        dispatch_event_while_running(&info);
        request_inhibit_lock_async(&s_ctx.inhibit_sleep);
    }
}

/**
 *  @brief          PrepareForShutdown の開始と取り消しを処理します。
 *  @param[in]      starting    0 以外はシャットダウンの開始、0 は取り消し。
 *
 *  開始時は、応答待ちを解除してからイベントを配送し、その後に fd を閉じます。\n
 *  取り消し時は、イベントを配送せず、非同期で lock を要求します。\n
 *  実際のサービス停止は、この後 systemd が送る SIGTERM (既存の停止経路) で処理されます。
 */
static void handle_prepare_for_shutdown(int starting)
{
    svc_event_info info;

    if (starting != 0)
    {
        cancel_inhibit_request(&s_ctx.inhibit_shutdown);
        info.type = SVC_EVENT_PRESHUTDOWN;
        info.session_id = NULL;
        dispatch_event_while_running(&info);

        /* イベントの配送の後に fd を閉じ、シャットダウンを許可する。 */
        if (s_ctx.inhibit_shutdown.fd >= 0)
        {
            /*
             * Linux の close は失敗しても fd を解放して再試行できないため、戻り値は明示的に破棄する。
             * see: https://man7.org/linux/man-pages/man2/close.2.html
             */
            (void)cplat_close(s_ctx.inhibit_shutdown.fd, NULL);
            s_ctx.inhibit_shutdown.fd = -1;
        }
    }
    else
    {
        request_inhibit_lock_async(&s_ctx.inhibit_shutdown);
    }
}

/**
 *  @brief          logind の PrepareForSleep シグナルを処理します。
 *  @param[in]      m           受信したシグナル メッセージ。
 *  @param[in]      userdata    未使用。
 *  @param[in]      ret_error   未使用。
 *  @return         常に 0 を返します。
 */
static int on_prepare_for_sleep(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    int starting;

    (void)userdata;
    (void)ret_error;

    if (sd_bus_message_read(m, "b", &starting) < 0)
    {
        return 0;
    }

    handle_prepare_for_sleep(starting);

    return 0;
}

/**
 *  @brief          logind の PrepareForShutdown シグナルを処理します。
 *  @param[in]      m           受信したシグナル メッセージ。
 *  @param[in]      userdata    未使用。
 *  @param[in]      ret_error   未使用。
 *  @return         常に 0 を返します。
 */
static int on_prepare_for_shutdown(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    int starting;

    (void)userdata;
    (void)ret_error;

    if (sd_bus_message_read(m, "b", &starting) < 0)
    {
        return 0;
    }

    handle_prepare_for_shutdown(starting);

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
 *  @brief          SIGHUP を受けて設定再読込を配送します。
 *  @param[in]      source      イベント ソース (未使用)。
 *  @param[in]      info        受信したシグナルの情報 (未使用)。
 *  @param[in]      userdata    未使用。
 *  @return         常に 0 を返します。
 *
 *  on_reload が NULL の場合と、動作中の段階でない場合は、配送せずにトレースへ記録します。
 */
static int on_reload_requested(sd_event_source *source, const struct signalfd_siginfo *info, void *userdata)
{
    int running;

    (void)source;
    (void)info;
    (void)userdata;

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    if (s_ctx.def->on_reload == NULL)
    {
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                 "on_reload が設定されていないため、SIGHUP を無視します。");
        return 0;
    }

    svc_linux_events_notify_lock();
    running = svc_linux_events_is_running();
    svc_linux_events_notify_unlock();

    if (running == 0)
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
    int running;
    int progressed;
    int stop_requested;
    int trace_stall_allowed;
    int notify_result = 0;
    int trace_stall = 0;

    (void)userdata;

    progressed = svc_on_run_made_progress(&s_ctx.watchdog_seen_count);
    stop_requested = svc_stop_requested();
    trace_stall_allowed = (s_ctx.watchdog_warned == 0) ? 1 : 0;
    svc_linux_events_notify_lock();
    running = svc_linux_events_is_running();

    /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
    if ((running != 0) && (stop_requested == 0))
    {
        if (progressed != 0)
        {
            notify_result = sd_notify_send("WATCHDOG=1");
        }
        else if (trace_stall_allowed != 0)
        {
            trace_stall = 1;
        }
    }
    svc_linux_events_notify_unlock();

    if (trace_stall != 0)
    {
        s_ctx.watchdog_warned = 1;
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                 "on_run が svc_wait_for_stop() を呼んでいないため、WATCHDOG=1 を送りません。");
    }
    else if (notify_result < 0)
    {
        /* 次の周期で再び送る。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "WATCHDOG=1 の送信に失敗しました: %s", strerror(-notify_result));
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

    /* イベント発生時にコールバックを実行する猶予を確保するため、delay lock を同期で取得する。 */
    acquire_inhibit_lock_sync(&s_ctx.inhibit_sleep);
    acquire_inhibit_lock_sync(&s_ctx.inhibit_shutdown);

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

    /*
     * on_reload の有無によらず SIGHUP を登録する。ブロックは svc_os_prepare_run() が済ませている。
     * -EBUSY は、SIGHUP がスレッドでブロックされていないこと (svc_os_prepare_run() の呼び忘れ) を示す。
     */
    rc = sd_event_add_signal(s_ctx.event, &s_ctx.reload_source, SIGHUP, on_reload_requested, NULL);
    if (rc < 0)
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、この関数のトレースの戻り値は明示的に破棄する。 */
        (void)cplat_tracer_writef(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL, "reload 監視の登録に失敗しました: %s",
                                  strerror(-rc));
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
 *  停止指示用の eventfd を閉じます。SIGHUP のブロックは、プロセスが終わるまで戻しません。\n
 *  起動の待ち合わせのロックと条件変数は、スレッドが通知を終えた後 (終了待ちの後) に破棄します。
 */
static void release_local_resources(void)
{
    /*
     * 解放の処理であり、Linux の close は失敗しても fd を解放して再試行できないため、
     * この関数の cplat_close の戻り値は明示的に破棄する。
     * see: https://man7.org/linux/man-pages/man2/close.2.html
     */
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

    if (s_ctx.notify_lock != NULL)
    {
        cplat_local_lock_dispose(s_ctx.notify_lock);
        s_ctx.notify_lock = NULL;
    }

    s_ctx.def = NULL;
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

    /* 通知入口がイベント監視スレッドの前に動くことはありませんが、段階の初期化も同じロックで保護します。 */
    if ((cplat_local_lock_create(&s_ctx.notify_lock) != CPLAT_OK) || (s_ctx.notify_lock == NULL))
    {
        /* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
        (void)cplat_tracer_write(svc_get_tracer(), CPLAT_TRACE_LEVEL_ERROR, NULL,
                                 "通知ロックを生成できないため、イベント監視スレッドを起動しません。");
        release_local_resources();
        return -1;
    }

    svc_linux_events_notify_lock();
    s_phase = SVC_LINUX_EVENTS_PHASE_STARTING;
    svc_linux_events_notify_unlock();

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
     * 起動時の Inhibit の同期呼び出しは、1 回 10 [s] で、sleep と shutdown の 2 回であり、READY=1 の前に終わる。
     * 全体は systemd の TimeoutStartSec= (既定 90 [s]) が打ち切る。
     * see: https://www.freedesktop.org/software/systemd/man/latest/sd_bus_call.html
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
