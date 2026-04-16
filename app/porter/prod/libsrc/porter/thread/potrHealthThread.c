/**
 *******************************************************************************
 *  @file           potrHealthThread.c
 *  @brief          ヘルスチェックスレッドモジュール。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/08
 *  @version        1.0.0
 *
 *  @details
 *  非 TCP 通信種別で動作する定周期 PING 送信スレッドです。\n
 *  health_interval_ms 周期で PING パケットを送信します。\n
 *  一方向通信では送信のみ、双方向通信では request/reply 形式の PING 送信を担います。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>
#include <string.h>
#include <inttypes.h>

#if defined(PLATFORM_LINUX)
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <time.h>
#elif defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#endif /* PLATFORM_ */

#include <porter_const.h>
#include <porter.h>

#include "../protocol/packet.h"
#include "../protocol/window.h"
#include "../potrContext.h"
#include "../potrPeerTable.h"
#include "potrHealthThread.h"
#include "../infra/potrLog.h"
#include "../infra/crypto/crypto.h"

#if defined(PLATFORM_LINUX)
    typedef pthread_mutex_t PotrMutexLocal;
    #define POTR_MUTEX_LOCK_LOCAL(m)   pthread_mutex_lock(m)
    #define POTR_MUTEX_UNLOCK_LOCAL(m) pthread_mutex_unlock(m)
#elif defined(PLATFORM_WINDOWS)
    typedef CRITICAL_SECTION PotrMutexLocal;
    #define POTR_MUTEX_LOCK_LOCAL(m)   EnterCriticalSection(m)
    #define POTR_MUTEX_UNLOCK_LOCAL(m) LeaveCriticalSection(m)
#endif /* PLATFORM_ */

/* TCP health スレッドに渡す引数 (path ごと) */
typedef struct
{
    struct PotrContext_ *ctx;
    int                  path_idx;
    int                  _pad;
} HealthArg;

static HealthArg s_health_args[POTR_MAX_PATH];

static void signal_health_thread(struct PotrContext_ *ctx, int path_idx)
{
    if (ctx == NULL
        || path_idx < 0
        || path_idx >= (int)POTR_MAX_PATH
        || !ctx->health_running[path_idx])
    {
        return;
    }

#if defined(PLATFORM_LINUX)
    pthread_mutex_lock(&ctx->health_mutex[path_idx]);
    if (ctx->health_running[path_idx])
    {
        pthread_cond_signal(&ctx->health_wakeup[path_idx]);
    }
    pthread_mutex_unlock(&ctx->health_mutex[path_idx]);
#elif defined(PLATFORM_WINDOWS)
    EnterCriticalSection(&ctx->health_mutex[path_idx]);
    if (ctx->health_running[path_idx])
    {
        WakeConditionVariable(&ctx->health_wakeup[path_idx]);
    }
    LeaveCriticalSection(&ctx->health_mutex[path_idx]);
#endif /* PLATFORM_ */
}

/* health_interval_ms ミリ秒、または停止シグナルが来るまでスリープする (path_idx 版) */
static void health_sleep(struct PotrContext_ *ctx, int path_idx, uint32_t interval_ms)
{
    /* オープン時割り込み PING フラグが立っていれば即リターン (初回スリープをスキップ) */
    if (ctx->health_send_immediate[path_idx])
    {
        ctx->health_send_immediate[path_idx] = 0;
        return;
    }

#if defined(PLATFORM_LINUX)
    struct timespec abs_ts;
    clock_gettime(CLOCK_REALTIME, &abs_ts);
    abs_ts.tv_sec  += (time_t)(interval_ms / 1000U);
    abs_ts.tv_nsec += (long)((interval_ms % 1000U) * 1000000UL);
    if (abs_ts.tv_nsec >= 1000000000L)
    {
        abs_ts.tv_sec++;
        abs_ts.tv_nsec -= 1000000000L;
    }
    pthread_mutex_lock(&ctx->health_mutex[path_idx]);
    if (ctx->health_running[path_idx])
    {
        pthread_cond_timedwait(&ctx->health_wakeup[path_idx],
                               &ctx->health_mutex[path_idx], &abs_ts);
    }
    pthread_mutex_unlock(&ctx->health_mutex[path_idx]);
#elif defined(PLATFORM_WINDOWS)
    EnterCriticalSection(&ctx->health_mutex[path_idx]);
    if (ctx->health_running[path_idx])
    {
        SleepConditionVariableCS(&ctx->health_wakeup[path_idx],
                                  &ctx->health_mutex[path_idx], (DWORD)interval_ms);
    }
    LeaveCriticalSection(&ctx->health_mutex[path_idx]);
#endif /* PLATFORM_ */
}

/* ====================================================================
 * 非 TCP (UDP/マルチキャスト) 用ヘルスチェックスレッド本体
 * ==================================================================== */
#if defined(PLATFORM_LINUX)
static void *health_thread_func(void *arg)
#elif defined(PLATFORM_WINDOWS)
static DWORD WINAPI health_thread_func(LPVOID arg)
#endif /* PLATFORM_ */
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)arg;
    PotrPacketSessionHdr shdr;

    shdr.service_id = ctx->service.service_id;
    /* 1:1 モード: session はコンテキストから取得 (N:1 は後述のループで各ピアから取得) */
    if (!ctx->is_multi_peer)
    {
        shdr.session_id      = ctx->session_id;
        shdr.session_tv_sec  = ctx->session_tv_sec;
        shdr.session_tv_nsec = ctx->session_tv_nsec;
    }

    while (ctx->health_running[0])
    {
        health_sleep(ctx, 0, ctx->global.health_interval_ms);

        if (!ctx->health_running[0]) break;

        /* PING を送信する */
        if (ctx->is_multi_peer)
        {
            /* N:1 モード: アクティブ全ピアへ PING を送信する */
            int i;

            POTR_MUTEX_LOCK_LOCAL(&ctx->peers_mutex);

            for (i = 0; i < ctx->max_peers && ctx->health_running[0]; i++)
            {
                PotrPacket           ping_pkt;
                PotrPacketSessionHdr peer_shdr;
                uint32_t             seq;
                size_t               wire_len;
                int                  k;

                if (!ctx->peers[i].active) continue;

                peer_shdr.service_id      = ctx->service.service_id;
                peer_shdr.session_id      = ctx->peers[i].session_id;
                peer_shdr.session_tv_sec  = ctx->peers[i].session_tv_sec;
                peer_shdr.session_tv_nsec = ctx->peers[i].session_tv_nsec;

#if defined(PLATFORM_LINUX)
                pthread_mutex_lock(&ctx->peers[i].send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
                EnterCriticalSection(&ctx->peers[i].send_window_mutex);
#endif /* PLATFORM_ */
                seq = ctx->peers[i].send_window.next_seq;
                /* N:1 (UNICAST_BIDIR_N1) は双方向 PING。ピアごとの自端パス受信状態をペイロードに設定する。 */
                {
                    uint8_t hs[POTR_MAX_PATH];
                    potr_copy_path_ping_state(hs, ctx->peers[i].path_ping_state, POTR_MAX_PATH);
                    packet_build_ping(&ping_pkt, &peer_shdr, seq, hs, (uint16_t)POTR_MAX_PATH);
                }
#if defined(PLATFORM_LINUX)
                pthread_mutex_unlock(&ctx->peers[i].send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
                LeaveCriticalSection(&ctx->peers[i].send_window_mutex);
#endif /* PLATFORM_ */

                if (ctx->service.encrypt_enabled)
                {
                    uint8_t  wire_buf[PACKET_HEADER_SIZE + POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE];
                    uint8_t  nonce[POTR_CRYPTO_NONCE_SIZE];
                    size_t   enc_out = POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE;

                    ping_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
                    ping_pkt.payload_len = htons((uint16_t)(POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE));

                    memcpy(nonce,      &ping_pkt.session_id, 4);
                    memcpy(nonce + 4,  &ping_pkt.flags,      2);
                    memcpy(nonce + 6,  &ping_pkt.seq_num,    4);
                    memset(nonce + 10, 0,                    2);

                    memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
                    memcpy(wire_buf + PACKET_HEADER_SIZE, ping_pkt.payload, POTR_MAX_PATH);
                    if (potr_encrypt(wire_buf + PACKET_HEADER_SIZE, &enc_out,
                                     wire_buf + PACKET_HEADER_SIZE, POTR_MAX_PATH,
                                     ctx->service.encrypt_key,
                                     nonce,
                                     wire_buf, PACKET_HEADER_SIZE) != 0)
                    {
                        continue;
                    }
                    wire_len = PACKET_HEADER_SIZE + enc_out;

                    for (k = 0; k < (int)POTR_MAX_PATH; k++)
                    {
                        if (ctx->peers[i].dest_addr[k].sin_family == 0) continue;
#if defined(PLATFORM_LINUX)
                        sendto(ctx->sock[k], wire_buf, wire_len, 0,
                               (const struct sockaddr *)&ctx->peers[i].dest_addr[k],
                               sizeof(ctx->peers[i].dest_addr[k]));
#elif defined(PLATFORM_WINDOWS)
                        sendto(ctx->sock[k], (const char *)wire_buf, (int)wire_len, 0,
                               (const struct sockaddr *)&ctx->peers[i].dest_addr[k],
                               sizeof(ctx->peers[i].dest_addr[k]));
#endif /* PLATFORM_ */
                    }
                }
                else
                {
                    uint8_t wire_buf[PACKET_HEADER_SIZE + POTR_MAX_PATH];
                    memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
                    memcpy(wire_buf + PACKET_HEADER_SIZE, ping_pkt.payload, POTR_MAX_PATH);
                    wire_len = PACKET_HEADER_SIZE + POTR_MAX_PATH;

                    for (k = 0; k < (int)POTR_MAX_PATH; k++)
                    {
                        if (ctx->peers[i].dest_addr[k].sin_family == 0) continue;
#if defined(PLATFORM_LINUX)
                        sendto(ctx->sock[k], wire_buf, wire_len, 0,
                               (const struct sockaddr *)&ctx->peers[i].dest_addr[k],
                               sizeof(ctx->peers[i].dest_addr[k]));
#elif defined(PLATFORM_WINDOWS)
                        sendto(ctx->sock[k], (const char *)wire_buf, (int)wire_len, 0,
                               (const struct sockaddr *)&ctx->peers[i].dest_addr[k],
                               sizeof(ctx->peers[i].dest_addr[k]));
#endif /* PLATFORM_ */
                    }
                }

                POTR_LOG(POTR_TRACE_VERBOSE,
                         "health[service_id=%" PRId64 "]: PING peer=%u seq=%u",
                         ctx->service.service_id,
                         (unsigned)ctx->peers[i].peer_id, (unsigned)seq);
            }

            POTR_MUTEX_UNLOCK_LOCAL(&ctx->peers_mutex);
        }
        else
        {
            /* 1:1 UDP/マルチキャスト: 単一宛先へ PING を送信する */
            PotrPacket ping_pkt;
            uint32_t   seq;
            size_t     wire_len;
            int        build_result;
            uint8_t    health_states[POTR_MAX_PATH];
            int        k;

            /* 双方向 PING (UNICAST_BIDIR) のみ自端のパス受信状態を設定する。
               それ以外 (UNICAST/MULTICAST/BROADCAST/RAW 系) は PING を受信しないため UNDEFINED を設定する。 */
            if (ctx->service.type == POTR_TYPE_UNICAST_BIDIR)
            {
                potr_copy_path_ping_state(health_states, ctx->path_ping_state, POTR_MAX_PATH);
            }
            else
            {
                memset(health_states, POTR_PING_STATE_UNDEFINED, POTR_MAX_PATH);
            }

            POTR_MUTEX_LOCK_LOCAL(&ctx->send_window_mutex);
            seq          = ctx->send_window.next_seq;
            build_result = packet_build_ping(&ping_pkt, &shdr, seq,
                                             health_states, (uint16_t)POTR_MAX_PATH);
            POTR_MUTEX_UNLOCK_LOCAL(&ctx->send_window_mutex);

            if (build_result != POTR_SUCCESS) { continue; }

            POTR_LOG(POTR_TRACE_VERBOSE,
                     "health[service_id=%" PRId64 "]: PING seq=%u",
                     ctx->service.service_id, (unsigned)seq);

            if (ctx->service.encrypt_enabled)
            {
                uint8_t  wire_buf[PACKET_HEADER_SIZE + POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE];
                uint8_t  nonce[POTR_CRYPTO_NONCE_SIZE];
                size_t   enc_out = POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE;

                ping_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
                ping_pkt.payload_len = htons((uint16_t)(POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE));

                memcpy(nonce,      &ping_pkt.session_id, 4);
                memcpy(nonce + 4,  &ping_pkt.flags,      2);
                memcpy(nonce + 6,  &ping_pkt.seq_num,    4);
                memset(nonce + 10, 0,                    2);

                memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
                memcpy(wire_buf + PACKET_HEADER_SIZE, health_states, POTR_MAX_PATH);
                if (potr_encrypt(wire_buf + PACKET_HEADER_SIZE, &enc_out,
                                 wire_buf + PACKET_HEADER_SIZE, POTR_MAX_PATH,
                                 ctx->service.encrypt_key,
                                 nonce,
                                 wire_buf, PACKET_HEADER_SIZE) != 0) { continue; }
                wire_len = PACKET_HEADER_SIZE + enc_out;

                for (k = 0; k < ctx->n_path; k++)
                {
#if defined(PLATFORM_LINUX)
                    sendto(ctx->sock[k], wire_buf, wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr[k],
                           sizeof(ctx->dest_addr[k]));
#elif defined(PLATFORM_WINDOWS)
                    sendto(ctx->sock[k], (const char *)wire_buf, (int)wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr[k],
                           sizeof(ctx->dest_addr[k]));
#endif /* PLATFORM_ */
                }
            }
            else
            {
                uint8_t wire_buf[PACKET_HEADER_SIZE + POTR_MAX_PATH];
                memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
                memcpy(wire_buf + PACKET_HEADER_SIZE, health_states, POTR_MAX_PATH);
                wire_len = PACKET_HEADER_SIZE + POTR_MAX_PATH;

                for (k = 0; k < ctx->n_path; k++)
                {
#if defined(PLATFORM_LINUX)
                    sendto(ctx->sock[k], wire_buf, wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr[k],
                           sizeof(ctx->dest_addr[k]));
#elif defined(PLATFORM_WINDOWS)
                    sendto(ctx->sock[k], (const char *)wire_buf, (int)wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr[k],
                           sizeof(ctx->dest_addr[k]));
#endif /* PLATFORM_ */
                }
            }
        }
    }

#if defined(PLATFORM_LINUX)
    return NULL;
#elif defined(PLATFORM_WINDOWS)
    return 0;
#endif /* PLATFORM_ */
}

/* ====================================================================
 * TCP 用ヘルスチェックスレッド本体 (path ごと)
 * ==================================================================== */
#if defined(PLATFORM_LINUX)
static void *tcp_health_thread_func(void *arg)
#elif defined(PLATFORM_WINDOWS)
static DWORD WINAPI tcp_health_thread_func(LPVOID arg)
#endif /* PLATFORM_ */
{
    HealthArg           *harg     = (HealthArg *)arg;
    struct PotrContext_ *ctx      = harg->ctx;
    int                  path_idx = harg->path_idx;
    PotrPacketSessionHdr shdr;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    POTR_LOG(POTR_TRACE_VERBOSE,
             "tcp_health[service_id=%" PRId64 " path=%d]: starting",
             ctx->service.service_id, path_idx);

    while (ctx->health_running[path_idx])
    {
        PotrPacket ping_pkt;
        uint32_t   seq;
        size_t     wire_len;
        int        build_result;

        /* 固定間隔でスリープ */
        health_sleep(ctx, path_idx, ctx->global.health_interval_ms);

        if (!ctx->health_running[path_idx]) break;

        /* 接続中でなければ PING をスキップ */
        if (ctx->tcp_active_paths == 0
            || ctx->tcp_conn_fd[path_idx] == POTR_INVALID_SOCKET)
        {
            continue;
        }

        /* PING パケットを構築する */
        /* TCP / TCP_BIDIR は双方向 PING。自端のパス受信状態をペイロードに設定する。 */
        {
            uint8_t health_states[POTR_MAX_PATH];
            potr_copy_path_ping_state(health_states, ctx->path_ping_state, POTR_MAX_PATH);

            POTR_MUTEX_LOCK_LOCAL(&ctx->send_window_mutex);
            /* session 情報を最新に更新 (再接続時に変わっている可能性がある) */
            shdr.session_id      = ctx->session_id;
            shdr.session_tv_sec  = ctx->session_tv_sec;
            shdr.session_tv_nsec = ctx->session_tv_nsec;
            seq          = ctx->send_window.next_seq;
            build_result = packet_build_ping(&ping_pkt, &shdr, seq,
                                             health_states, (uint16_t)POTR_MAX_PATH);
            POTR_MUTEX_UNLOCK_LOCAL(&ctx->send_window_mutex);

            if (build_result != POTR_SUCCESS) { continue; }

            POTR_LOG(POTR_TRACE_VERBOSE,
                     "tcp_health[service_id=%" PRId64 " path=%d]: PING seq=%u",
                     ctx->service.service_id, path_idx, (unsigned)seq);

            if (ctx->service.encrypt_enabled)
            {
                uint8_t wire_buf[PACKET_HEADER_SIZE + POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE];
                uint8_t nonce[POTR_CRYPTO_NONCE_SIZE];
                size_t  enc_out = POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE;

                ping_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
                ping_pkt.payload_len = htons((uint16_t)(POTR_MAX_PATH + POTR_CRYPTO_TAG_SIZE));

                memcpy(nonce,      &ping_pkt.session_id, 4);
                memcpy(nonce + 4,  &ping_pkt.flags,      2);
                memcpy(nonce + 6,  &ping_pkt.seq_num,    4);
                memset(nonce + 10, 0,                    2);

                memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
                memcpy(wire_buf + PACKET_HEADER_SIZE, health_states, POTR_MAX_PATH);
                if (potr_encrypt(wire_buf + PACKET_HEADER_SIZE, &enc_out,
                                 wire_buf + PACKET_HEADER_SIZE, POTR_MAX_PATH,
                                 ctx->service.encrypt_key,
                                 nonce,
                                 wire_buf, PACKET_HEADER_SIZE) != 0) { continue; }
                wire_len = PACKET_HEADER_SIZE + enc_out;

                POTR_MUTEX_LOCK_LOCAL(&ctx->tcp_send_mutex[path_idx]);
                if (ctx->tcp_conn_fd[path_idx] != POTR_INVALID_SOCKET)
                {
#if defined(PLATFORM_LINUX)
                    {
                        size_t        sent = 0;
                        const uint8_t *p   = wire_buf;
                        while (sent < wire_len)
                        {
                            ssize_t n = send(ctx->tcp_conn_fd[path_idx],
                                             p + sent, wire_len - sent, 0);
                            if (n <= 0) break;
                            sent += (size_t)n;
                        }
                    }
#elif defined(PLATFORM_WINDOWS)
                    send(ctx->tcp_conn_fd[path_idx],
                         (const char *)wire_buf, (int)wire_len, 0);
#endif /* PLATFORM_ */
                }
                POTR_MUTEX_UNLOCK_LOCAL(&ctx->tcp_send_mutex[path_idx]);
            }
            else
            {
                uint8_t wire_buf[PACKET_HEADER_SIZE + POTR_MAX_PATH];
                memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
                memcpy(wire_buf + PACKET_HEADER_SIZE, health_states, POTR_MAX_PATH);
                wire_len = PACKET_HEADER_SIZE + POTR_MAX_PATH;

                POTR_MUTEX_LOCK_LOCAL(&ctx->tcp_send_mutex[path_idx]);
                if (ctx->tcp_conn_fd[path_idx] != POTR_INVALID_SOCKET)
                {
#if defined(PLATFORM_LINUX)
                    {
                        size_t        sent = 0;
                        const uint8_t *p   = wire_buf;
                        while (sent < wire_len)
                        {
                            ssize_t n = send(ctx->tcp_conn_fd[path_idx],
                                             p + sent, wire_len - sent, 0);
                            if (n <= 0) break;
                            sent += (size_t)n;
                        }
                    }
#elif defined(PLATFORM_WINDOWS)
                    send(ctx->tcp_conn_fd[path_idx],
                         (const char *)wire_buf, (int)wire_len, 0);
#endif /* PLATFORM_ */
                }
                POTR_MUTEX_UNLOCK_LOCAL(&ctx->tcp_send_mutex[path_idx]);
            }
        }
    }

    POTR_LOG(POTR_TRACE_VERBOSE,
             "tcp_health[service_id=%" PRId64 " path=%d]: exited",
             ctx->service.service_id, path_idx);

#if defined(PLATFORM_LINUX)
    return NULL;
#elif defined(PLATFORM_WINDOWS)
    return 0;
#endif /* PLATFORM_ */
}

/**
 *******************************************************************************
 *  @brief          非 TCP ヘルスチェックスレッドを起動します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  ctx->global.health_interval_ms が 0 の場合は起動しません (POTR_SUCCESS を返します)。
 *******************************************************************************
 */
int potr_health_thread_start(struct PotrContext_ *ctx)
{
    if (ctx == NULL) { return POTR_ERROR; }

    if (ctx->global.health_interval_ms == 0)
    {
        POTR_LOG(POTR_TRACE_VERBOSE,
                 "health_thread[service_id=%" PRId64 "]: disabled (health_interval_ms=0)",
                 ctx->service.service_id);
        return POTR_SUCCESS;
    }

    POTR_LOG(POTR_TRACE_VERBOSE,
             "health_thread[service_id=%" PRId64 "]: starting (interval=%ums)",
             ctx->service.service_id,
             (unsigned)ctx->global.health_interval_ms);

#if defined(PLATFORM_LINUX)
    pthread_mutex_init(&ctx->health_mutex[0], NULL);
    pthread_cond_init(&ctx->health_wakeup[0], NULL);
#elif defined(PLATFORM_WINDOWS)
    InitializeCriticalSection(&ctx->health_mutex[0]);
    InitializeConditionVariable(&ctx->health_wakeup[0]);
#endif /* PLATFORM_ */

    ctx->health_running[0] = 1;

#if defined(PLATFORM_LINUX)
    if (pthread_create(&ctx->health_thread[0], NULL, health_thread_func, ctx) != 0)
    {
        ctx->health_running[0] = 0;
        POTR_LOG(POTR_TRACE_ERROR,
                 "health_thread[service_id=%" PRId64 "]: pthread_create failed",
                 ctx->service.service_id);
        return POTR_ERROR;
    }
#elif defined(PLATFORM_WINDOWS)
    ctx->health_thread[0] = CreateThread(NULL, 0, health_thread_func, ctx, 0, NULL);
    if (ctx->health_thread[0] == NULL)
    {
        ctx->health_running[0] = 0;
        POTR_LOG(POTR_TRACE_ERROR,
                 "health_thread[service_id=%" PRId64 "]: CreateThread failed",
                 ctx->service.service_id);
        return POTR_ERROR;
    }
#endif /* PLATFORM_ */

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          非 TCP ヘルスチェックスレッドを停止します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *******************************************************************************
 */
int potr_health_thread_stop(struct PotrContext_ *ctx)
{
    if (ctx == NULL) { return POTR_ERROR; }
    if (!ctx->health_running[0]) { return POTR_SUCCESS; }

    ctx->health_running[0] = 0;

#if defined(PLATFORM_LINUX)
    pthread_mutex_lock(&ctx->health_mutex[0]);
    pthread_cond_signal(&ctx->health_wakeup[0]);
    pthread_mutex_unlock(&ctx->health_mutex[0]);

    pthread_join(ctx->health_thread[0], NULL);

    pthread_cond_destroy(&ctx->health_wakeup[0]);
    pthread_mutex_destroy(&ctx->health_mutex[0]);
#elif defined(PLATFORM_WINDOWS)
    if (ctx->health_thread[0] != NULL)
    {
        EnterCriticalSection(&ctx->health_mutex[0]);
        WakeConditionVariable(&ctx->health_wakeup[0]);
        LeaveCriticalSection(&ctx->health_mutex[0]);

        WaitForSingleObject(ctx->health_thread[0], INFINITE);
        CloseHandle(ctx->health_thread[0]);
        ctx->health_thread[0] = NULL;
    }
    DeleteCriticalSection(&ctx->health_mutex[0]);
    /* Windows の CONDITION_VARIABLE は破棄不要 */
#endif /* PLATFORM_ */

    return POTR_SUCCESS;
}

void potr_health_thread_wake(struct PotrContext_ *ctx)
{
    signal_health_thread(ctx, 0);
}

/**
 *******************************************************************************
 *  @brief          TCP ヘルスチェックスレッドを path ごとに起動します。
 *  @param[in,out]  ctx       セッションコンテキストへのポインタ。
 *  @param[in]      path_idx  パスインデックス (0 ～ n_path-1)。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  ctx->global.health_interval_ms が 0 の場合は起動しません (POTR_SUCCESS を返します)。
 *******************************************************************************
 */
int potr_tcp_health_thread_start(struct PotrContext_ *ctx, int path_idx)
{
    if (ctx == NULL) { return POTR_ERROR; }

    if (ctx->global.health_interval_ms == 0)
    {
        POTR_LOG(POTR_TRACE_VERBOSE,
                 "tcp_health_thread[service_id=%" PRId64 " path=%d]: disabled",
                 ctx->service.service_id, path_idx);
        return POTR_SUCCESS;
    }

    s_health_args[path_idx].ctx      = ctx;
    s_health_args[path_idx].path_idx = path_idx;

    ctx->health_running[path_idx] = 1;

#if defined(PLATFORM_LINUX)
    if (pthread_create(&ctx->health_thread[path_idx], NULL,
                       tcp_health_thread_func, &s_health_args[path_idx]) != 0)
    {
        ctx->health_running[path_idx] = 0;
        POTR_LOG(POTR_TRACE_ERROR,
                 "tcp_health_thread[service_id=%" PRId64 " path=%d]: pthread_create failed",
                 ctx->service.service_id, path_idx);
        return POTR_ERROR;
    }
#elif defined(PLATFORM_WINDOWS)
    ctx->health_thread[path_idx] = CreateThread(NULL, 0,
                                                tcp_health_thread_func,
                                                &s_health_args[path_idx], 0, NULL);
    if (ctx->health_thread[path_idx] == NULL)
    {
        ctx->health_running[path_idx] = 0;
        POTR_LOG(POTR_TRACE_ERROR,
                 "tcp_health_thread[service_id=%" PRId64 " path=%d]: CreateThread failed",
                 ctx->service.service_id, path_idx);
        return POTR_ERROR;
    }
#endif /* PLATFORM_ */

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          TCP ヘルスチェックスレッドを停止します。
 *  @param[in,out]  ctx       セッションコンテキストへのポインタ。
 *  @param[in]      path_idx  パスインデックス (0 ～ n_path-1)。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *******************************************************************************
 */
int potr_tcp_health_thread_stop(struct PotrContext_ *ctx, int path_idx)
{
    if (ctx == NULL) { return POTR_ERROR; }
    if (!ctx->health_running[path_idx]) { return POTR_SUCCESS; }

    ctx->health_running[path_idx] = 0;

#if defined(PLATFORM_LINUX)
    pthread_mutex_lock(&ctx->health_mutex[path_idx]);
    pthread_cond_signal(&ctx->health_wakeup[path_idx]);
    pthread_mutex_unlock(&ctx->health_mutex[path_idx]);

    pthread_join(ctx->health_thread[path_idx], NULL);
#elif defined(PLATFORM_WINDOWS)
    if (ctx->health_thread[path_idx] != NULL)
    {
        EnterCriticalSection(&ctx->health_mutex[path_idx]);
        WakeConditionVariable(&ctx->health_wakeup[path_idx]);
        LeaveCriticalSection(&ctx->health_mutex[path_idx]);

        WaitForSingleObject(ctx->health_thread[path_idx], INFINITE);
        CloseHandle(ctx->health_thread[path_idx]);
        ctx->health_thread[path_idx] = NULL;
    }
#endif /* PLATFORM_ */

    return POTR_SUCCESS;
}

void potr_tcp_health_thread_wake(struct PotrContext_ *ctx, int path_idx)
{
    signal_health_thread(ctx, path_idx);
}

void potr_tcp_health_thread_wake_all(struct PotrContext_ *ctx)
{
    int i;

    if (ctx == NULL)
    {
        return;
    }

    for (i = 0; i < ctx->n_path; i++)
    {
        signal_health_thread(ctx, i);
    }
}
