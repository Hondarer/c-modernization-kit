/**
 *******************************************************************************
 *  @file           potrConnectThread.c
 *  @brief          TCP 接続管理スレッドモジュール。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/23
 *  @version        1.0.0
 *
 *  @details
 *  SENDER: TCP connect/reconnect ループを管理するスレッドです。\n
 *  RECEIVER: TCP accept ループを管理するスレッドです。\n
 *  接続確立後、送受信・ヘルスチェックスレッドを起動し、\n
 *  recv スレッドが切断を検知して終了するまで待機してから再接続サイクルへ移行します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <time.h>
#endif

#include <porter_const.h>

#include "../potrContext.h"
#include "../infra/potrSendQueue.h"
#include "../protocol/window.h"
#include "../infra/potrLog.h"
#include "potrConnectThread.h"
#include "potrRecvThread.h"
#include "potrSendThread.h"
#include "potrHealthThread.h"

/* 現在時刻をミリ秒単位で返す (単調増加クロック) */
static uint64_t connect_get_ms(void)
{
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
#endif
}

/* TCP 接続ソケットをシャットダウン・クローズして INVALID にする */
static void close_tcp_conn(struct PotrContext_ *ctx)
{
    if (ctx->tcp_conn_fd[0] != POTR_INVALID_SOCKET)
    {
#ifdef _WIN32
        shutdown(ctx->tcp_conn_fd[0], SD_BOTH);
        closesocket(ctx->tcp_conn_fd[0]);
#else
        shutdown(ctx->tcp_conn_fd[0], SHUT_RDWR);
        close(ctx->tcp_conn_fd[0]);
#endif
        ctx->tcp_conn_fd[0] = POTR_INVALID_SOCKET;
    }
}

/* 再接続待機: reconnect_interval_ms 経過または停止シグナルまでスリープする */
static void reconnect_wait(struct PotrContext_ *ctx, uint32_t wait_ms)
{
#ifdef _WIN32
    EnterCriticalSection(&ctx->tcp_state_mutex);
    if (ctx->connect_thread_running)
    {
        SleepConditionVariableCS(&ctx->tcp_state_cv,
                                  &ctx->tcp_state_mutex,
                                  (DWORD)wait_ms);
    }
    LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
    struct timespec abs_ts;
    clock_gettime(CLOCK_REALTIME, &abs_ts);
    abs_ts.tv_sec  += (time_t)(wait_ms / 1000U);
    abs_ts.tv_nsec += (long)((wait_ms % 1000U) * 1000000UL);
    if (abs_ts.tv_nsec >= 1000000000L)
    {
        abs_ts.tv_sec++;
        abs_ts.tv_nsec -= 1000000000L;
    }
    pthread_mutex_lock(&ctx->tcp_state_mutex);
    if (ctx->connect_thread_running)
    {
        pthread_cond_timedwait(&ctx->tcp_state_cv, &ctx->tcp_state_mutex, &abs_ts);
    }
    pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif
}

/* recv スレッドの自然終了を待機してハンドルを解放する。
   TCP 接続断後 recv スレッドが自然終了する設計のため、ソケットはクローズしない。 */
static void join_recv_thread(struct PotrContext_ *ctx)
{
#ifdef _WIN32
    if (ctx->recv_thread != NULL)
    {
        WaitForSingleObject(ctx->recv_thread, INFINITE);
        CloseHandle(ctx->recv_thread);
        ctx->recv_thread = NULL;
    }
#else
    pthread_join(ctx->recv_thread, NULL);
#endif
}

/* 接続確立前のコンテキスト状態をリセットする。
   send_window の通番を 0 に戻し、フラグメントバッファや peer_session 状態をクリアする。
   呼び出しタイミング: start_connected_threads 前 (他スレッド未起動)。 */
static void reset_connection_state(struct PotrContext_ *ctx)
{
    ctx->peer_session_known = 0;
    ctx->frag_buf_len       = 0;
    ctx->health_alive       = 0;

    /* send_window の通番をリセット (TCP 新規接続ごとに seq=0 から開始) */
    ctx->send_window.next_seq = 0U;
    ctx->send_window.base_seq = 0U;
    if (ctx->send_window.valid != NULL)
    {
        memset(ctx->send_window.valid, 0,
               (size_t)ctx->send_window.window_size * sizeof(uint8_t));
    }
}

/* 送信キューを再初期化する (reconnect 時に shutdown 済みのキューをリセットする)。
   depth と max_payload はキュー構造体から取得する。 */
static void reset_send_queue(struct PotrContext_ *ctx)
{
    size_t   depth       = ctx->send_queue.depth;
    uint16_t max_payload = (uint16_t)ctx->global.max_payload;
    potr_send_queue_destroy(&ctx->send_queue);
    (void)potr_send_queue_init(&ctx->send_queue, depth, max_payload);
}

/* 接続確立後に依存スレッドを起動する。
   SENDER および TCP_BIDIR RECEIVER: send スレッド・recv スレッド・health スレッドを起動。
   TCP RECEIVER: recv スレッドのみ起動。
   失敗時は起動済みスレッドをすべて停止してから POTR_ERROR を返す。 */
static int start_connected_threads(struct PotrContext_ *ctx)
{
    int is_bidir  = (ctx->service.type == POTR_TYPE_TCP_BIDIR);
    int is_sender = (ctx->role == POTR_ROLE_SENDER);

    /* SENDER または TCP_BIDIR RECEIVER: 送信スレッドを起動 */
    if (is_sender || is_bidir)
    {
        if (potr_send_thread_start(ctx) != POTR_SUCCESS)
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "connect_thread[service_id=%d]: send_thread_start failed",
                     ctx->service.service_id);
            return POTR_ERROR;
        }
    }

    /* 受信スレッドを起動 */
    if (comm_recv_thread_start(ctx) != POTR_SUCCESS)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "connect_thread[service_id=%d]: recv_thread_start failed",
                 ctx->service.service_id);
        if (is_sender || is_bidir)
        {
            /* ソケットクローズで send_thread が自然終了するよう仕向ける */
            close_tcp_conn(ctx);
            potr_send_thread_stop(ctx);
        }
        return POTR_ERROR;
    }

    /* SENDER または TCP_BIDIR RECEIVER: ヘルスチェックスレッドを起動 */
    if (is_sender || is_bidir)
    {
        if (potr_health_thread_start(ctx) != POTR_SUCCESS)
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "connect_thread[service_id=%d]: health_thread_start failed",
                     ctx->service.service_id);
            /* recv スレッドをソケットクローズで停止 */
            ctx->running = 0;
            close_tcp_conn(ctx);
            join_recv_thread(ctx);
            potr_send_thread_stop(ctx);
            return POTR_ERROR;
        }
    }

    return POTR_SUCCESS;
}

/* 接続断後に依存スレッドをすべて停止する。
   呼び出し前提: join_recv_thread 完了済み (recv スレッドは終了している)。 */
static void stop_connected_threads(struct PotrContext_ *ctx)
{
    int is_bidir  = (ctx->service.type == POTR_TYPE_TCP_BIDIR);
    int is_sender = (ctx->role == POTR_ROLE_SENDER);

    if (is_sender || is_bidir)
    {
        /* health スレッドを先に停止 (PING 送信が tcp_conn_fd を参照するため) */
        potr_health_thread_stop(ctx);

        /* 接続ソケットをクローズ (送信スレッドが send() でブロックしている場合を解除) */
        close_tcp_conn(ctx);

        /* 送信スレッドを停止 */
        potr_send_thread_stop(ctx);
    }
    else
    {
        close_tcp_conn(ctx);
    }
}

/* SENDER: タイムアウト付き TCP 接続を試みる。
   connect_timeout_ms が 0 の場合は OS デフォルト (ブロッキング) で接続する。
   成功時はソケットを返す。失敗時は POTR_INVALID_SOCKET を返す。 */
static PotrSocket tcp_connect_with_timeout(struct PotrContext_ *ctx)
{
    PotrSocket         sock;
    struct sockaddr_in addr;
    uint32_t           timeout_ms = ctx->service.connect_timeout_ms;
    int                reuse = 1;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == POTR_INVALID_SOCKET)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "connect_thread[service_id=%d]: socket() failed",
                 ctx->service.service_id);
        return POTR_INVALID_SOCKET;
    }

#ifdef _WIN32
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&reuse, sizeof(reuse));
#else
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr        = ctx->dst_addr_resolved[0];
    addr.sin_port        = htons(ctx->service.dst_port);

    if (timeout_ms == 0U)
    {
        /* ブロッキング接続 */
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            POTR_LOG(POTR_LOG_DEBUG,
                     "connect_thread[service_id=%d]: connect() failed (blocking)",
                     ctx->service.service_id);
#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif
            return POTR_INVALID_SOCKET;
        }
        return sock;
    }

    /* タイムアウト付き接続: ノンブロッキングモードで select を使う。
       停止シグナルに素早く応答するため 100ms 単位でポーリングする。 */
#ifdef _WIN32
    {
        u_long mode  = 1;
        int    ret;
        int    error = 0;
        int    errlen;

        ioctlsocket(sock, FIONBIO, &mode);

        ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
        if (ret == 0)
        {
            /* 即座に接続成功 */
            mode = 0;
            ioctlsocket(sock, FIONBIO, &mode);
            return sock;
        }
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            POTR_LOG(POTR_LOG_DEBUG,
                     "connect_thread[service_id=%d]: connect() failed (WSA error)",
                     ctx->service.service_id);
            closesocket(sock);
            return POTR_INVALID_SOCKET;
        }

        /* select ループ (100ms ポーリング) */
        {
            uint32_t elapsed_ms = 0U;
            int      ready      = 0;

            while (elapsed_ms < timeout_ms && ctx->connect_thread_running)
            {
                fd_set         writefds;
                struct timeval tv;
                uint32_t       poll_ms;

                poll_ms = timeout_ms - elapsed_ms;
                if (poll_ms > 100U) poll_ms = 100U;

                FD_ZERO(&writefds);
                FD_SET(sock, &writefds);
                tv.tv_sec  = (long)(poll_ms / 1000U);
                tv.tv_usec = (long)((poll_ms % 1000U) * 1000L);

                ret = select(0, NULL, &writefds, NULL, &tv);
                if (ret < 0)
                {
                    break;
                }
                if (ret > 0 && FD_ISSET(sock, &writefds))
                {
                    ready = 1;
                    break;
                }
                elapsed_ms += poll_ms;
            }

            if (!ready)
            {
                POTR_LOG(POTR_LOG_DEBUG,
                         "connect_thread[service_id=%d]: connect() timed out",
                         ctx->service.service_id);
                closesocket(sock);
                return POTR_INVALID_SOCKET;
            }
        }

        errlen = (int)sizeof(error);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&error, &errlen);
        if (error != 0)
        {
            POTR_LOG(POTR_LOG_DEBUG,
                     "connect_thread[service_id=%d]: connect() SO_ERROR=%d",
                     ctx->service.service_id, error);
            closesocket(sock);
            return POTR_INVALID_SOCKET;
        }

        /* ブロッキングモードに戻す */
        mode = 0;
        ioctlsocket(sock, FIONBIO, &mode);
        return sock;
    }
#else
    {
        int       flags;
        int       ret;
        int       error  = 0;
        socklen_t errlen;

        flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);

        ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
        if (ret == 0)
        {
            /* 即座に接続成功 */
            fcntl(sock, F_SETFL, flags);
            return sock;
        }
        if (errno != EINPROGRESS)
        {
            POTR_LOG(POTR_LOG_DEBUG,
                     "connect_thread[service_id=%d]: connect() failed (errno=%d)",
                     ctx->service.service_id, errno);
            close(sock);
            return POTR_INVALID_SOCKET;
        }

        /* select ループ (100ms ポーリング) */
        {
            uint32_t elapsed_ms = 0U;
            int      ready      = 0;

            while (elapsed_ms < timeout_ms && ctx->connect_thread_running)
            {
                fd_set         writefds;
                struct timeval tv;
                uint32_t       poll_ms;

                poll_ms = timeout_ms - elapsed_ms;
                if (poll_ms > 100U) poll_ms = 100U;

                FD_ZERO(&writefds);
                FD_SET(sock, &writefds);
                tv.tv_sec  = (long)(poll_ms / 1000U);
                tv.tv_usec = (long)((poll_ms % 1000U) * 1000L);

                ret = select(sock + 1, NULL, &writefds, NULL, &tv);
                if (ret < 0)
                {
                    if (errno == EINTR) { elapsed_ms += poll_ms; continue; }
                    break;
                }
                if (ret > 0)
                {
                    ready = 1;
                    break;
                }
                elapsed_ms += poll_ms;
            }

            if (!ready)
            {
                POTR_LOG(POTR_LOG_DEBUG,
                         "connect_thread[service_id=%d]: connect() timed out",
                         ctx->service.service_id);
                close(sock);
                return POTR_INVALID_SOCKET;
            }
        }

        errlen = (socklen_t)sizeof(error);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &errlen);
        if (error != 0)
        {
            POTR_LOG(POTR_LOG_DEBUG,
                     "connect_thread[service_id=%d]: connect() SO_ERROR=%d",
                     ctx->service.service_id, error);
            close(sock);
            return POTR_INVALID_SOCKET;
        }

        /* ブロッキングモードに戻す */
        fcntl(sock, F_SETFL, flags);
        return sock;
    }
#endif
}

/* SENDER 用接続ループ */
static void sender_connect_loop(struct PotrContext_ *ctx)
{
    int is_reconnect = 0; /* 初回接続フラグ */

    while (ctx->connect_thread_running)
    {
        PotrSocket sock;

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d]: connecting to %s:%u ...",
                 ctx->service.service_id,
                 ctx->service.dst_addr[0],
                 (unsigned)ctx->service.dst_port);

        sock = tcp_connect_with_timeout(ctx);

        if (sock == POTR_INVALID_SOCKET)
        {
            if (!ctx->connect_thread_running) break;

            if (ctx->service.reconnect_interval_ms == 0U)
            {
                POTR_LOG(POTR_LOG_INFO,
                         "connect_thread[service_id=%d]: connect failed, "
                         "no reconnect (reconnect_interval_ms=0)",
                         ctx->service.service_id);
                break;
            }

            POTR_LOG(POTR_LOG_DEBUG,
                     "connect_thread[service_id=%d]: connect failed, "
                     "retrying in %ums",
                     ctx->service.service_id,
                     (unsigned)ctx->service.reconnect_interval_ms);
            reconnect_wait(ctx, ctx->service.reconnect_interval_ms);
            continue;
        }

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d]: TCP connected",
                 ctx->service.service_id);

        ctx->tcp_conn_fd[0]        = sock;
        ctx->tcp_last_ping_recv_ms = connect_get_ms();
        ctx->tcp_connected         = 1;

        reset_connection_state(ctx);

        /* 再接続時: shutdown 済みのキューをリセット */
        if (is_reconnect)
        {
            reset_send_queue(ctx);
        }

        if (start_connected_threads(ctx) != POTR_SUCCESS)
        {
            ctx->tcp_connected = 0;
            close_tcp_conn(ctx);

            if (!ctx->connect_thread_running) break;
            if (ctx->service.reconnect_interval_ms == 0U) break;

            reconnect_wait(ctx, ctx->service.reconnect_interval_ms);
            is_reconnect = 1;
            continue;
        }

        /* recv スレッドが接続断を検知して自然終了するまで待機する */
        join_recv_thread(ctx);

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d]: TCP disconnected",
                 ctx->service.service_id);

        stop_connected_threads(ctx);
        ctx->tcp_connected = 0;

        if (!ctx->connect_thread_running) break;
        if (ctx->service.reconnect_interval_ms == 0U)
        {
            POTR_LOG(POTR_LOG_INFO,
                     "connect_thread[service_id=%d]: no reconnect "
                     "(reconnect_interval_ms=0)",
                     ctx->service.service_id);
            break;
        }

        POTR_LOG(POTR_LOG_DEBUG,
                 "connect_thread[service_id=%d]: waiting %ums before reconnect",
                 ctx->service.service_id,
                 (unsigned)ctx->service.reconnect_interval_ms);
        reconnect_wait(ctx, ctx->service.reconnect_interval_ms);
        is_reconnect = 1;
    }
}

/* RECEIVER 用 accept ループ */
static void receiver_accept_loop(struct PotrContext_ *ctx)
{
    int is_bidir     = (ctx->service.type == POTR_TYPE_TCP_BIDIR);
    int is_reconnect = 0;

    while (ctx->connect_thread_running)
    {
        PotrSocket conn;

        conn = accept(ctx->tcp_listen_sock, NULL, NULL);

        if (conn == POTR_INVALID_SOCKET)
        {
            if (!ctx->connect_thread_running) break;
            /* 一時的なエラー: ループ継続 */
            POTR_LOG(POTR_LOG_TRACE,
                     "connect_thread[service_id=%d]: accept() error, retrying",
                     ctx->service.service_id);
            continue;
        }

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d]: TCP accepted",
                 ctx->service.service_id);

        ctx->tcp_conn_fd[0]        = conn;
        ctx->tcp_last_ping_recv_ms = connect_get_ms();
        ctx->tcp_connected         = 1;

        reset_connection_state(ctx);

        /* TCP_BIDIR 再接続時: shutdown 済みのキューをリセット */
        if (is_bidir && is_reconnect)
        {
            reset_send_queue(ctx);
        }

        if (start_connected_threads(ctx) != POTR_SUCCESS)
        {
            ctx->tcp_connected = 0;
            close_tcp_conn(ctx);
            is_reconnect = 1;
            continue;
        }

        /* recv スレッドが接続断を検知して自然終了するまで待機する */
        join_recv_thread(ctx);

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d]: TCP connection closed",
                 ctx->service.service_id);

        stop_connected_threads(ctx);
        ctx->tcp_connected = 0;
        is_reconnect = 1;
        /* ループ継続: 次の accept へ */
    }
}

/* 接続管理スレッド本体 */
#ifdef _WIN32
static DWORD WINAPI connect_thread_func(LPVOID arg)
#else
static void *connect_thread_func(void *arg)
#endif
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)arg;

    POTR_LOG(POTR_LOG_DEBUG,
             "connect_thread[service_id=%d]: started (role=%s type=%s)",
             ctx->service.service_id,
             (ctx->role == POTR_ROLE_SENDER) ? "SENDER" : "RECEIVER",
             (ctx->service.type == POTR_TYPE_TCP_BIDIR) ? "TCP_BIDIR" : "TCP");

    if (ctx->role == POTR_ROLE_SENDER)
    {
        sender_connect_loop(ctx);
    }
    else
    {
        receiver_accept_loop(ctx);
    }

    ctx->connect_thread_running = 0;

    POTR_LOG(POTR_LOG_DEBUG,
             "connect_thread[service_id=%d]: exited",
             ctx->service.service_id);

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/**
 *******************************************************************************
 *  @brief          TCP 接続管理スレッドを起動します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  呼び出し前提:
 *  - SENDER: dst_addr_resolved および dst_port が設定済みであること。
 *  - RECEIVER: tcp_listen_sock が listen 状態であること。
 *  - tcp_state_mutex / tcp_state_cv / tcp_send_mutex が初期化済みであること。
 *******************************************************************************
 */
int potr_connect_thread_start(struct PotrContext_ *ctx)
{
    if (ctx == NULL)
    {
        return POTR_ERROR;
    }

    ctx->connect_thread_running = 1;

    POTR_LOG(POTR_LOG_DEBUG,
             "connect_thread[service_id=%d]: starting",
             ctx->service.service_id);

#ifdef _WIN32
    ctx->connect_thread = CreateThread(NULL, 0, connect_thread_func, ctx, 0, NULL);
    if (ctx->connect_thread == NULL)
    {
        ctx->connect_thread_running = 0;
        POTR_LOG(POTR_LOG_ERROR,
                 "connect_thread[service_id=%d]: CreateThread failed",
                 ctx->service.service_id);
        return POTR_ERROR;
    }
#else
    if (pthread_create(&ctx->connect_thread, NULL, connect_thread_func, ctx) != 0)
    {
        ctx->connect_thread_running = 0;
        POTR_LOG(POTR_LOG_ERROR,
                 "connect_thread[service_id=%d]: pthread_create failed",
                 ctx->service.service_id);
        return POTR_ERROR;
    }
#endif

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          TCP 接続管理スレッドを停止します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *
 *  @details
 *  1. connect_thread_running フラグをクリアして停止を通知する。
 *  2. tcp_state_cv をシグナルして reconnect sleep を中断する。
 *  3. tcp_listen_sock (RECEIVER) をクローズして accept ブロックを解除する。
 *  4. tcp_conn_fd[0] をクローズして接続中の recv スレッドを解除する。
 *  5. connect_thread の終了を待機する。
 *  依存スレッド (send/recv/health) のクリーンアップは connect_thread 内で行われる。
 *******************************************************************************
 */
void potr_connect_thread_stop(struct PotrContext_ *ctx)
{
    if (ctx == NULL || !ctx->connect_thread_running)
    {
        return;
    }

    ctx->connect_thread_running = 0;

    /* reconnect_wait 中のスリープを即時解除する */
#ifdef _WIN32
    EnterCriticalSection(&ctx->tcp_state_mutex);
    WakeConditionVariable(&ctx->tcp_state_cv);
    LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
    pthread_mutex_lock(&ctx->tcp_state_mutex);
    pthread_cond_signal(&ctx->tcp_state_cv);
    pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif

    /* RECEIVER: accept ブロックを解除するため listen ソケットをクローズ */
    if (ctx->role == POTR_ROLE_RECEIVER
        && ctx->tcp_listen_sock != POTR_INVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(ctx->tcp_listen_sock);
#else
        shutdown(ctx->tcp_listen_sock, SHUT_RDWR);
        close(ctx->tcp_listen_sock);
#endif
        ctx->tcp_listen_sock = POTR_INVALID_SOCKET;
    }

    /* 接続ソケットをクローズ (recv ループをブロック解除) */
    if (ctx->tcp_conn_fd[0] != POTR_INVALID_SOCKET)
    {
#ifdef _WIN32
        shutdown(ctx->tcp_conn_fd[0], SD_BOTH);
        closesocket(ctx->tcp_conn_fd[0]);
#else
        shutdown(ctx->tcp_conn_fd[0], SHUT_RDWR);
        close(ctx->tcp_conn_fd[0]);
#endif
        ctx->tcp_conn_fd[0] = POTR_INVALID_SOCKET;
    }

    /* connect スレッドの終了を待機する */
#ifdef _WIN32
    if (ctx->connect_thread != NULL)
    {
        WaitForSingleObject(ctx->connect_thread, INFINITE);
        CloseHandle(ctx->connect_thread);
        ctx->connect_thread = NULL;
    }
#else
    pthread_join(ctx->connect_thread, NULL);
#endif

    POTR_LOG(POTR_LOG_DEBUG,
             "connect_thread[service_id=%d]: stopped",
             ctx->service.service_id);
}
