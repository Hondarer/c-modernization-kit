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
    #include <arpa/inet.h>
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

/* connect/accept スレッドに渡す引数 */
typedef struct
{
    struct PotrContext_ *ctx;
    int                  path_idx;
    int                  _pad;
} ConnectArg;

/* 静的に確保した引数バッファ (path 数分) */
static ConnectArg s_connect_args[POTR_MAX_PATH];

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

/* TCP 接続ソケット [path_idx] をシャットダウン・クローズして INVALID にする */
static void close_tcp_conn(struct PotrContext_ *ctx, int path_idx)
{
    if (ctx->tcp_conn_fd[path_idx] != POTR_INVALID_SOCKET)
    {
#ifdef _WIN32
        shutdown(ctx->tcp_conn_fd[path_idx], SD_BOTH);
        closesocket(ctx->tcp_conn_fd[path_idx]);
#else
        shutdown(ctx->tcp_conn_fd[path_idx], SHUT_RDWR);
        close(ctx->tcp_conn_fd[path_idx]);
#endif
        ctx->tcp_conn_fd[path_idx] = POTR_INVALID_SOCKET;
    }
}

/* 再接続待機: reconnect_interval_ms 経過または停止シグナルまでスリープする */
static void reconnect_wait(struct PotrContext_ *ctx, int path_idx, uint32_t wait_ms)
{
#ifdef _WIN32
    EnterCriticalSection(&ctx->tcp_state_mutex);
    if (ctx->connect_thread_running[path_idx])
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
    if (ctx->connect_thread_running[path_idx])
    {
        pthread_cond_timedwait(&ctx->tcp_state_cv, &ctx->tcp_state_mutex, &abs_ts);
    }
    pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif
}

/* recv スレッド [path_idx] の自然終了を待機してハンドルを解放する。
   TCP 接続断後 recv スレッドが自然終了する設計のため、ソケットはクローズしない。 */
static void join_recv_thread(struct PotrContext_ *ctx, int path_idx)
{
#ifdef _WIN32
    if (ctx->recv_thread[path_idx] != NULL)
    {
        WaitForSingleObject(ctx->recv_thread[path_idx], INFINITE);
        CloseHandle(ctx->recv_thread[path_idx]);
        ctx->recv_thread[path_idx] = NULL;
    }
#else
    pthread_join(ctx->recv_thread[path_idx], NULL);
#endif
}

/* 接続確立前のコンテキスト状態をリセットする。
   フラグメントバッファや peer_session 状態をクリアする。
   TCP v2 マルチパスでは send_window 通番と health_alive は保持する
   (部分切断・再接続時のセッション継続のため)。
   呼び出しタイミング: start_connected_threads 前 (他スレッド未起動)。 */
static void reset_connection_state(struct PotrContext_ *ctx)
{
    ctx->peer_session_known = 0;
    ctx->frag_buf_len       = 0;
}

/* 全 path 切断時のリセット: send_window 通番・peer_session 状態・health_alive をクリアし
   DISCONNECTED イベントを発火する。
   呼び出しタイミング: tcp_active_paths が 0 になった直後 (tcp_state_mutex 非保護)。 */
static void reset_all_paths_disconnected(struct PotrContext_ *ctx)
{
    ctx->peer_session_known   = 0;
    ctx->frag_buf_len         = 0;
    ctx->send_window.next_seq = 0U;
    ctx->send_window.base_seq = 0U;
    if (ctx->send_window.valid != NULL)
    {
        memset(ctx->send_window.valid, 0,
               (size_t)ctx->send_window.window_size * sizeof(uint8_t));
    }
    if (ctx->health_alive)
    {
        ctx->health_alive = 0;
        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d]: DISCONNECTED (all paths down)",
                 ctx->service.service_id);
        if (ctx->callback != NULL)
        {
            ctx->callback(ctx->service.service_id, POTR_PEER_NA,
                          POTR_EVENT_DISCONNECTED, NULL, 0);
        }
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

/* 接続確立後に依存スレッドを起動する (path ごと)。
   SENDER および TCP_BIDIR RECEIVER: path_idx==0 の初回のみ send スレッドを起動する。
   各 path で recv スレッドと health スレッドを起動。
   TCP RECEIVER: recv スレッドのみ起動。
   失敗時は起動済みスレッドをすべて停止してから POTR_ERROR を返す。 */
static int start_connected_threads(struct PotrContext_ *ctx, int path_idx)
{
    int is_bidir  = (ctx->service.type == POTR_TYPE_TCP_BIDIR);
    int is_sender = (ctx->role == POTR_ROLE_SENDER);

    /* SENDER または TCP_BIDIR RECEIVER: path[0] の初回接続時のみ送信スレッドを起動 */
    if ((is_sender || is_bidir) && path_idx == 0 && !ctx->send_thread_running)
    {
        if (potr_send_thread_start(ctx) != POTR_SUCCESS)
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "connect_thread[service_id=%d]: send_thread_start failed",
                     ctx->service.service_id);
            return POTR_ERROR;
        }
    }

    /* recv スレッドを path ごとに起動 */
    if (tcp_recv_thread_start(ctx, path_idx) != POTR_SUCCESS)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "connect_thread[service_id=%d]: tcp_recv_thread_start failed"
                 " (path=%d)",
                 ctx->service.service_id, path_idx);
        if ((is_sender || is_bidir) && path_idx == 0 && !ctx->send_thread_running)
        {
            close_tcp_conn(ctx, path_idx);
            potr_send_thread_stop(ctx);
        }
        return POTR_ERROR;
    }

    /* SENDER または TCP_BIDIR RECEIVER: health スレッドを path ごとに起動 */
    if (is_sender || is_bidir)
    {
        if (potr_tcp_health_thread_start(ctx, path_idx) != POTR_SUCCESS)
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "connect_thread[service_id=%d]: tcp_health_thread_start failed"
                     " (path=%d)",
                     ctx->service.service_id, path_idx);
            /* recv スレッドをソケットクローズで自然終了させる */
            ctx->running[path_idx] = 0;
            close_tcp_conn(ctx, path_idx);
            join_recv_thread(ctx, path_idx);
            return POTR_ERROR;
        }
    }

    return POTR_SUCCESS;
}

/* 接続断後に依存スレッドを停止する (path ごと)。
   呼び出し前提: join_recv_thread(path_idx) 完了済み (recv スレッドは終了している)。
   注意: 送信スレッドは共有のため、potr_connect_thread_stop で全 path join 後に停止する。 */
static void stop_connected_threads(struct PotrContext_ *ctx, int path_idx)
{
    int is_bidir  = (ctx->service.type == POTR_TYPE_TCP_BIDIR);
    int is_sender = (ctx->role == POTR_ROLE_SENDER);

    if (is_sender || is_bidir)
    {
        /* health スレッドを先に停止 (PING 送信が tcp_conn_fd を参照するため) */
        potr_tcp_health_thread_stop(ctx, path_idx);
    }

    /* 接続ソケットをクローズ */
    close_tcp_conn(ctx, path_idx);
}

/* SENDER: path_idx 番目の宛先へ TCP 接続を試みる。
   connect_timeout_ms が 0 の場合は OS デフォルト (ブロッキング) で接続する。
   成功時はソケットを返す。失敗時は POTR_INVALID_SOCKET を返す。 */
static PotrSocket tcp_connect_with_timeout(struct PotrContext_ *ctx, int path_idx)
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

    if (ctx->service.src_addr[path_idx][0] != '\0' || ctx->service.src_port != 0)
    {
        struct sockaddr_in bind_addr;
        memset(&bind_addr, 0, sizeof(bind_addr));
        bind_addr.sin_family = AF_INET;
        if (ctx->service.src_addr[path_idx][0] != '\0')
        {
            bind_addr.sin_addr = ctx->src_addr_resolved[path_idx];
        }
        else
        {
            bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        }
        bind_addr.sin_port = htons(ctx->service.src_port); /* 0 = エフェメラル */

#ifdef _WIN32
        if (bind(sock, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR)
#else
        if (bind(sock, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0)
#endif
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "connect_thread[service_id=%d]: bind() failed",
                     ctx->service.service_id);
#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif
            return POTR_INVALID_SOCKET;
        }
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr        = ctx->dst_addr_resolved[path_idx];
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

            while (elapsed_ms < timeout_ms && ctx->connect_thread_running[path_idx])
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

            while (elapsed_ms < timeout_ms && ctx->connect_thread_running[path_idx])
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

/* SENDER 用接続ループ (path ごと) */
static void sender_connect_loop(struct PotrContext_ *ctx, int path_idx)
{
    int is_reconnect = 0; /* 初回接続フラグ */

    while (ctx->connect_thread_running[path_idx])
    {
        PotrSocket sock;
        int        active_count;

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d path=%d]: connecting to %s:%u ...",
                 ctx->service.service_id, path_idx,
                 ctx->service.dst_addr[path_idx],
                 (unsigned)ctx->service.dst_port);

        sock = tcp_connect_with_timeout(ctx, path_idx);

        if (sock == POTR_INVALID_SOCKET)
        {
            if (!ctx->connect_thread_running[path_idx]) break;

            if (ctx->service.reconnect_interval_ms == 0U)
            {
                POTR_LOG(POTR_LOG_INFO,
                         "connect_thread[service_id=%d path=%d]: connect failed, "
                         "no reconnect (reconnect_interval_ms=0)",
                         ctx->service.service_id, path_idx);
                break;
            }

            POTR_LOG(POTR_LOG_DEBUG,
                     "connect_thread[service_id=%d path=%d]: connect failed, "
                     "retrying in %ums",
                     ctx->service.service_id, path_idx,
                     (unsigned)ctx->service.reconnect_interval_ms);
            reconnect_wait(ctx, path_idx, ctx->service.reconnect_interval_ms);
            continue;
        }

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d path=%d]: TCP connected",
                 ctx->service.service_id, path_idx);

        ctx->tcp_conn_fd[path_idx]               = sock;
        ctx->tcp_last_ping_recv_ms[path_idx]     = connect_get_ms();
        ctx->tcp_last_ping_req_recv_ms[path_idx] = connect_get_ms();

        /* tcp_active_paths カウンタをインクリメント (tcp_state_mutex 保護) */
#ifdef _WIN32
        EnterCriticalSection(&ctx->tcp_state_mutex);
        active_count = ++ctx->tcp_active_paths;
        LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
        pthread_mutex_lock(&ctx->tcp_state_mutex);
        active_count = ++ctx->tcp_active_paths;
        pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif
        (void)active_count; /* CONNECTED イベントは recv スレッドが最初のパケット受信時に発火 */

        reset_connection_state(ctx);

        /* 再接続時 (path[0] のみ): 全 path 切断後の再起動ではキューをリセット */
        if (is_reconnect && path_idx == 0)
        {
            reset_send_queue(ctx);
        }

        if (start_connected_threads(ctx, path_idx) != POTR_SUCCESS)
        {
            /* スレッド起動失敗: カウンタを戻す */
#ifdef _WIN32
            EnterCriticalSection(&ctx->tcp_state_mutex);
            active_count = --ctx->tcp_active_paths;
            LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
            pthread_mutex_lock(&ctx->tcp_state_mutex);
            active_count = --ctx->tcp_active_paths;
            pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif
            if (active_count == 0)
            {
                reset_all_paths_disconnected(ctx);
            }

            if (!ctx->connect_thread_running[path_idx]) break;
            if (ctx->service.reconnect_interval_ms == 0U) break;

            reconnect_wait(ctx, path_idx, ctx->service.reconnect_interval_ms);
            is_reconnect = 1;
            continue;
        }

        /* recv スレッドが接続断を検知して自然終了するまで待機する */
        join_recv_thread(ctx, path_idx);

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d path=%d]: TCP disconnected",
                 ctx->service.service_id, path_idx);

        stop_connected_threads(ctx, path_idx);

        /* tcp_active_paths カウンタをデクリメント (tcp_state_mutex 保護) */
#ifdef _WIN32
        EnterCriticalSection(&ctx->tcp_state_mutex);
        active_count = --ctx->tcp_active_paths;
        LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
        pthread_mutex_lock(&ctx->tcp_state_mutex);
        active_count = --ctx->tcp_active_paths;
        pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif

        if (active_count == 0)
        {
            /* 全 path 切断: send_window 通番と session をリセット */
            reset_all_paths_disconnected(ctx);
        }

        if (!ctx->connect_thread_running[path_idx]) break;
        if (ctx->service.reconnect_interval_ms == 0U)
        {
            POTR_LOG(POTR_LOG_INFO,
                     "connect_thread[service_id=%d path=%d]: no reconnect "
                     "(reconnect_interval_ms=0)",
                     ctx->service.service_id, path_idx);
            break;
        }

        POTR_LOG(POTR_LOG_DEBUG,
                 "connect_thread[service_id=%d path=%d]: waiting %ums before reconnect",
                 ctx->service.service_id, path_idx,
                 (unsigned)ctx->service.reconnect_interval_ms);
        reconnect_wait(ctx, path_idx, ctx->service.reconnect_interval_ms);
        is_reconnect = 1;
    }
}

/* RECEIVER 用 accept ループ (path ごと) */
static void receiver_accept_loop(struct PotrContext_ *ctx, int path_idx)
{
    int is_bidir     = (ctx->service.type == POTR_TYPE_TCP_BIDIR);
    int is_reconnect = 0;

    while (ctx->connect_thread_running[path_idx])
    {
        PotrSocket         conn;
        struct sockaddr_in peer_addr;
        socklen_t          peer_len = (socklen_t)sizeof(peer_addr);
        int                active_count;

        conn = accept(ctx->tcp_listen_sock[path_idx],
                      (struct sockaddr *)&peer_addr, &peer_len);

        if (conn == POTR_INVALID_SOCKET)
        {
            if (!ctx->connect_thread_running[path_idx]) break;
            /* 一時的なエラー: ループ継続 */
            POTR_LOG(POTR_LOG_TRACE,
                     "connect_thread[service_id=%d path=%d]: accept() error, retrying",
                     ctx->service.service_id, path_idx);
            continue;
        }

        /* 接続元フィルタ: src_addr[path_idx] / src_port が指定されていれば一致確認 */
        {
            int filtered = 0;
            if (ctx->service.src_addr[path_idx][0] != '\0')
            {
                if (peer_addr.sin_addr.s_addr !=
                    ctx->src_addr_resolved[path_idx].s_addr)
                {
                    filtered = 1;
                }
            }
            if (!filtered && ctx->service.src_port != 0)
            {
                if (ntohs(peer_addr.sin_port) != ctx->service.src_port)
                {
                    filtered = 1;
                }
            }
            if (filtered)
            {
                POTR_LOG(POTR_LOG_INFO,
                         "connect_thread[service_id=%d path=%d]: rejected connection"
                         " from %s:%u (src filter)",
                         ctx->service.service_id, path_idx,
                         inet_ntoa(peer_addr.sin_addr),
                         (unsigned)ntohs(peer_addr.sin_port));
#ifdef _WIN32
                closesocket(conn);
#else
                close(conn);
#endif
                continue;
            }
        }

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d path=%d]: TCP accepted from %s:%u",
                 ctx->service.service_id, path_idx,
                 inet_ntoa(peer_addr.sin_addr),
                 (unsigned)ntohs(peer_addr.sin_port));

        ctx->tcp_conn_fd[path_idx]               = conn;
        ctx->tcp_last_ping_recv_ms[path_idx]     = connect_get_ms();
        ctx->tcp_last_ping_req_recv_ms[path_idx] = connect_get_ms();

        /* tcp_active_paths カウンタをインクリメント (tcp_state_mutex 保護) */
#ifdef _WIN32
        EnterCriticalSection(&ctx->tcp_state_mutex);
        active_count = ++ctx->tcp_active_paths;
        LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
        pthread_mutex_lock(&ctx->tcp_state_mutex);
        active_count = ++ctx->tcp_active_paths;
        pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif
        (void)active_count; /* CONNECTED イベントは recv スレッドが最初のパケット受信時に発火 */

        reset_connection_state(ctx);

        /* TCP_BIDIR 再接続時 (path[0] のみ): shutdown 済みのキューをリセット */
        if (is_bidir && is_reconnect && path_idx == 0)
        {
            reset_send_queue(ctx);
        }

        if (start_connected_threads(ctx, path_idx) != POTR_SUCCESS)
        {
#ifdef _WIN32
            EnterCriticalSection(&ctx->tcp_state_mutex);
            active_count = --ctx->tcp_active_paths;
            LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
            pthread_mutex_lock(&ctx->tcp_state_mutex);
            active_count = --ctx->tcp_active_paths;
            pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif
            if (active_count == 0)
            {
                reset_all_paths_disconnected(ctx);
            }
            close_tcp_conn(ctx, path_idx);
            is_reconnect = 1;
            continue;
        }

        /* recv スレッドが接続断を検知して自然終了するまで待機する */
        join_recv_thread(ctx, path_idx);

        POTR_LOG(POTR_LOG_INFO,
                 "connect_thread[service_id=%d path=%d]: TCP connection closed",
                 ctx->service.service_id, path_idx);

        stop_connected_threads(ctx, path_idx);

        /* tcp_active_paths カウンタをデクリメント (tcp_state_mutex 保護) */
#ifdef _WIN32
        EnterCriticalSection(&ctx->tcp_state_mutex);
        active_count = --ctx->tcp_active_paths;
        LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
        pthread_mutex_lock(&ctx->tcp_state_mutex);
        active_count = --ctx->tcp_active_paths;
        pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif

        if (active_count == 0)
        {
            reset_all_paths_disconnected(ctx);
        }

        is_reconnect = 1;
        /* ループ継続: 次の accept へ */
    }
}

/* 接続管理スレッド本体 (ConnectArg* を受け取り、path ごとに動作) */
#ifdef _WIN32
static DWORD WINAPI connect_thread_func(LPVOID arg)
#else
static void *connect_thread_func(void *arg)
#endif
{
    ConnectArg          *carg     = (ConnectArg *)arg;
    struct PotrContext_ *ctx      = carg->ctx;
    int                  path_idx = carg->path_idx;

    POTR_LOG(POTR_LOG_DEBUG,
             "connect_thread[service_id=%d path=%d]: started (role=%s type=%s)",
             ctx->service.service_id, path_idx,
             (ctx->role == POTR_ROLE_SENDER) ? "SENDER" : "RECEIVER",
             (ctx->service.type == POTR_TYPE_TCP_BIDIR) ? "TCP_BIDIR" : "TCP");

    if (ctx->role == POTR_ROLE_SENDER)
    {
        sender_connect_loop(ctx, path_idx);
    }
    else
    {
        receiver_accept_loop(ctx, path_idx);
    }

    ctx->connect_thread_running[path_idx] = 0;

    POTR_LOG(POTR_LOG_DEBUG,
             "connect_thread[service_id=%d path=%d]: exited",
             ctx->service.service_id, path_idx);

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/**
 *******************************************************************************
 *  @brief          TCP 接続管理スレッドを起動します (path 数分)。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  呼び出し前提:
 *  - SENDER: dst_addr_resolved[i] および dst_port が設定済みであること (n_path 分)。
 *  - RECEIVER: tcp_listen_sock[i] が listen 状態であること (n_path 分)。
 *  - tcp_state_mutex / tcp_state_cv / tcp_send_mutex[] が初期化済みであること。
 *******************************************************************************
 */
int potr_connect_thread_start(struct PotrContext_ *ctx)
{
    int i;

    if (ctx == NULL)
    {
        return POTR_ERROR;
    }

    POTR_LOG(POTR_LOG_DEBUG,
             "connect_thread[service_id=%d]: starting %d path(s)",
             ctx->service.service_id, ctx->n_path);

    for (i = 0; i < ctx->n_path; i++)
    {
        ctx->connect_thread_running[i] = 1;
        s_connect_args[i].ctx      = ctx;
        s_connect_args[i].path_idx = i;

#ifdef _WIN32
        ctx->connect_thread[i] = CreateThread(NULL, 0, connect_thread_func,
                                               &s_connect_args[i], 0, NULL);
        if (ctx->connect_thread[i] == NULL)
        {
            ctx->connect_thread_running[i] = 0;
            POTR_LOG(POTR_LOG_ERROR,
                     "connect_thread[service_id=%d path=%d]: CreateThread failed",
                     ctx->service.service_id, i);
            /* 起動済み path のスレッドは potr_connect_thread_stop で停止する */
            return POTR_ERROR;
        }
#else
        if (pthread_create(&ctx->connect_thread[i], NULL,
                           connect_thread_func, &s_connect_args[i]) != 0)
        {
            ctx->connect_thread_running[i] = 0;
            POTR_LOG(POTR_LOG_ERROR,
                     "connect_thread[service_id=%d path=%d]: pthread_create failed",
                     ctx->service.service_id, i);
            return POTR_ERROR;
        }
#endif
    }

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          TCP 接続管理スレッドを停止します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *
 *  @details
 *  1. connect_thread_running[i] フラグを全 path クリアして停止を通知する。
 *  2. tcp_state_cv をブロードキャストして reconnect sleep を中断する。
 *  3. tcp_listen_sock[i] (RECEIVER) を全 path クローズして accept ブロックを解除する。
 *  4. tcp_conn_fd[i] を全 path クローズして recv ループをブロック解除する。
 *  5. connect_thread[i] の終了を全 path 待機する。
 *  6. 送信スレッドを停止する (全 path join 後)。
 *  依存スレッド (recv/health) のクリーンアップは各 connect_thread 内で行われる。
 *******************************************************************************
 */
void potr_connect_thread_stop(struct PotrContext_ *ctx)
{
    int i;
    int any_running = 0;

    if (ctx == NULL) { return; }

    for (i = 0; i < ctx->n_path; i++)
    {
        if (ctx->connect_thread_running[i]) { any_running = 1; break; }
    }
    if (!any_running) { return; }

    /* 1. 全 path の停止フラグをクリア */
    for (i = 0; i < ctx->n_path; i++)
    {
        ctx->connect_thread_running[i] = 0;
    }

    /* 2. reconnect_wait 中の全スレッドを起床させる */
#ifdef _WIN32
    EnterCriticalSection(&ctx->tcp_state_mutex);
    WakeAllConditionVariable(&ctx->tcp_state_cv);
    LeaveCriticalSection(&ctx->tcp_state_mutex);
#else
    pthread_mutex_lock(&ctx->tcp_state_mutex);
    pthread_cond_broadcast(&ctx->tcp_state_cv);
    pthread_mutex_unlock(&ctx->tcp_state_mutex);
#endif

    /* 3. RECEIVER: 全 path の listen ソケットをクローズして accept をアンブロック */
    if (ctx->role == POTR_ROLE_RECEIVER)
    {
        for (i = 0; i < ctx->n_path; i++)
        {
            if (ctx->tcp_listen_sock[i] == POTR_INVALID_SOCKET) continue;
#ifdef _WIN32
            closesocket(ctx->tcp_listen_sock[i]);
#else
            shutdown(ctx->tcp_listen_sock[i], SHUT_RDWR);
            close(ctx->tcp_listen_sock[i]);
#endif
            ctx->tcp_listen_sock[i] = POTR_INVALID_SOCKET;
        }
    }

    /* 4. 全 path の接続ソケットをクローズして recv ループをアンブロック */
    for (i = 0; i < ctx->n_path; i++)
    {
        if (ctx->tcp_conn_fd[i] == POTR_INVALID_SOCKET) continue;
#ifdef _WIN32
        shutdown(ctx->tcp_conn_fd[i], SD_BOTH);
        closesocket(ctx->tcp_conn_fd[i]);
#else
        shutdown(ctx->tcp_conn_fd[i], SHUT_RDWR);
        close(ctx->tcp_conn_fd[i]);
#endif
        ctx->tcp_conn_fd[i] = POTR_INVALID_SOCKET;
    }

    /* 5. 全 connect スレッドの終了を待機する */
#ifdef _WIN32
    for (i = 0; i < ctx->n_path; i++)
    {
        if (ctx->connect_thread[i] == NULL) continue;
        WaitForSingleObject(ctx->connect_thread[i], INFINITE);
        CloseHandle(ctx->connect_thread[i]);
        ctx->connect_thread[i] = NULL;
    }
#else
    for (i = 0; i < ctx->n_path; i++)
    {
        pthread_join(ctx->connect_thread[i], NULL);
    }
#endif

    /* 6. 送信スレッドを停止する (全 path join 後) */
    potr_send_thread_stop(ctx);

    POTR_LOG(POTR_LOG_DEBUG,
             "connect_thread[service_id=%d]: all paths stopped",
             ctx->service.service_id);
}
