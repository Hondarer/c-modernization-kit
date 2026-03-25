/**
 *******************************************************************************
 *  @file           potrHealthThread.c
 *  @brief          ヘルスチェックスレッドモジュール。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/08
 *  @version        1.0.0
 *
 *  @details
 *  送信者側で動作する定周期 PING 送信スレッドです。\n
 *  health_interval_ms 周期で PING パケットを送信します。\n
 *  受信者側から送信者へのヘルスチェックは行いません (一方向ヘルスチェック)。
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
    #include <time.h>
#endif

#include <porter_const.h>
#include <porter.h>

#include "../protocol/packet.h"
#include "../protocol/window.h"
#include "../potrContext.h"
#include "../potrPeerTable.h"
#include "potrHealthThread.h"
#include "../infra/potrLog.h"
#include "../infra/crypto/crypto.h"

#ifdef _WIN32
    typedef CRITICAL_SECTION PotrMutexLocal;
    #define POTR_MUTEX_LOCK_LOCAL(m)   EnterCriticalSection(m)
    #define POTR_MUTEX_UNLOCK_LOCAL(m) LeaveCriticalSection(m)
#else
    typedef pthread_mutex_t PotrMutexLocal;
    #define POTR_MUTEX_LOCK_LOCAL(m)   pthread_mutex_lock(m)
    #define POTR_MUTEX_UNLOCK_LOCAL(m) pthread_mutex_unlock(m)
#endif

/* TCP health スレッドに渡す引数 (path ごと) */
typedef struct
{
    struct PotrContext_ *ctx;
    int                  path_idx;
    int                  _pad;
} HealthArg;

static HealthArg s_health_args[POTR_MAX_PATH];

/* health_interval_ms ミリ秒、または停止シグナルが来るまでスリープする (path_idx 版) */
static void health_sleep(struct PotrContext_ *ctx, int path_idx, uint32_t interval_ms)
{
#ifdef _WIN32
    EnterCriticalSection(&ctx->health_mutex[path_idx]);
    if (ctx->health_running[path_idx])
    {
        SleepConditionVariableCS(&ctx->health_wakeup[path_idx],
                                  &ctx->health_mutex[path_idx], (DWORD)interval_ms);
    }
    LeaveCriticalSection(&ctx->health_mutex[path_idx]);
#else
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
#endif
}

/* 現在時刻をミリ秒単位で返す (単調増加クロック) */
static uint64_t health_get_ms(void)
{
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
#endif
}

/* ====================================================================
 * 非 TCP (UDP/マルチキャスト) 用ヘルスチェックスレッド本体
 * ==================================================================== */
#ifdef _WIN32
static DWORD WINAPI health_thread_func(LPVOID arg)
#else
static void *health_thread_func(void *arg)
#endif
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)arg;
    PotrPacketSessionHdr shdr;

    shdr.service_id = ctx->service.service_id;
    shdr._pad       = 0;
    /* 1:1 モード: session はコンテキストから取得 (N:1 は後述のループで各ピアから取得) */
    if (!ctx->is_multi_peer)
    {
        shdr.session_id      = ctx->session_id;
        shdr.session_tv_sec  = ctx->session_tv_sec;
        shdr.session_tv_nsec = ctx->session_tv_nsec;
    }

    while (ctx->health_running[0])
    {
        uint32_t sleep_ms;
        uint64_t last_send = ctx->last_send_ms;

        if (last_send == 0)
        {
            sleep_ms = ctx->global.health_interval_ms;
        }
        else
        {
            uint64_t elapsed = health_get_ms() - last_send;
            sleep_ms = (elapsed >= (uint64_t)ctx->global.health_interval_ms)
                       ? 1U
                       : (uint32_t)(ctx->global.health_interval_ms - elapsed);
        }

        health_sleep(ctx, 0, sleep_ms);

        if (!ctx->health_running[0]) break;

        /* データ送信によりタイマーがリセットされた場合は PING 不要 */
        last_send = ctx->last_send_ms;
        if (last_send != 0)
        {
            uint64_t elapsed = health_get_ms() - last_send;
            if (elapsed < (uint64_t)ctx->global.health_interval_ms)
            {
                POTR_LOG(POTR_LOG_TRACE,
                         "health[service_id=%d]: PING timer reset",
                         ctx->service.service_id);
                continue;
            }
        }

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
                peer_shdr._pad            = 0;

#ifdef _WIN32
                EnterCriticalSection(&ctx->peers[i].send_window_mutex);
#else
                pthread_mutex_lock(&ctx->peers[i].send_window_mutex);
#endif
                seq = ctx->peers[i].send_window.next_seq;
                packet_build_ping(&ping_pkt, &peer_shdr, seq, 0U);
#ifdef _WIN32
                LeaveCriticalSection(&ctx->peers[i].send_window_mutex);
#else
                pthread_mutex_unlock(&ctx->peers[i].send_window_mutex);
#endif

                if (ctx->service.encrypt_enabled)
                {
                    uint8_t  wire_buf[PACKET_HEADER_SIZE + POTR_CRYPTO_TAG_SIZE];
                    uint8_t  nonce[POTR_CRYPTO_NONCE_SIZE];
                    size_t   enc_out = POTR_CRYPTO_TAG_SIZE;

                    ping_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
                    ping_pkt.payload_len = htons((uint16_t)POTR_CRYPTO_TAG_SIZE);

                    memcpy(nonce,      &ping_pkt.session_id, 4);
                    memcpy(nonce + 4,  &ping_pkt.flags,      2);
                    memcpy(nonce + 6,  &ping_pkt.seq_num,    4);
                    memset(nonce + 10, 0,                    2);

                    memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
                    if (potr_encrypt(wire_buf + PACKET_HEADER_SIZE, &enc_out,
                                     NULL, 0,
                                     ctx->service.encrypt_key,
                                     nonce,
                                     wire_buf, PACKET_HEADER_SIZE) != 0)
                    {
                        continue;
                    }
                    wire_len = PACKET_HEADER_SIZE + enc_out;

                    for (k = 0; k < ctx->peers[i].n_paths; k++)
                    {
#ifdef _WIN32
                        sendto(ctx->sock[0], (const char *)wire_buf, (int)wire_len, 0,
                               (const struct sockaddr *)&ctx->peers[i].dest_addr[k],
                               sizeof(ctx->peers[i].dest_addr[k]));
#else
                        sendto(ctx->sock[0], wire_buf, wire_len, 0,
                               (const struct sockaddr *)&ctx->peers[i].dest_addr[k],
                               sizeof(ctx->peers[i].dest_addr[k]));
#endif
                    }
                }
                else
                {
                    wire_len = packet_wire_size(&ping_pkt);

                    for (k = 0; k < ctx->peers[i].n_paths; k++)
                    {
#ifdef _WIN32
                        sendto(ctx->sock[0], (const char *)&ping_pkt, (int)wire_len, 0,
                               (const struct sockaddr *)&ctx->peers[i].dest_addr[k],
                               sizeof(ctx->peers[i].dest_addr[k]));
#else
                        sendto(ctx->sock[0], &ping_pkt, wire_len, 0,
                               (const struct sockaddr *)&ctx->peers[i].dest_addr[k],
                               sizeof(ctx->peers[i].dest_addr[k]));
#endif
                    }
                }

                POTR_LOG(POTR_LOG_TRACE,
                         "health[service_id=%d]: PING peer=%u seq=%u",
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

            POTR_MUTEX_LOCK_LOCAL(&ctx->send_window_mutex);
            seq          = ctx->send_window.next_seq;
            build_result = packet_build_ping(&ping_pkt, &shdr, seq, 0U);
            POTR_MUTEX_UNLOCK_LOCAL(&ctx->send_window_mutex);

            if (build_result != POTR_SUCCESS) { continue; }

            POTR_LOG(POTR_LOG_TRACE,
                     "health[service_id=%d]: PING seq=%u",
                     ctx->service.service_id, (unsigned)seq);

            if (ctx->service.encrypt_enabled)
            {
                uint8_t  wire_buf[PACKET_HEADER_SIZE + POTR_CRYPTO_TAG_SIZE];
                uint8_t  nonce[POTR_CRYPTO_NONCE_SIZE];
                size_t   enc_out = POTR_CRYPTO_TAG_SIZE;
                int      k;

                ping_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
                ping_pkt.payload_len = htons((uint16_t)POTR_CRYPTO_TAG_SIZE);

                memcpy(nonce,      &ping_pkt.session_id, 4);
                memcpy(nonce + 4,  &ping_pkt.flags,      2);
                memcpy(nonce + 6,  &ping_pkt.seq_num,    4);
                memset(nonce + 10, 0,                    2);

                memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
                if (potr_encrypt(wire_buf + PACKET_HEADER_SIZE, &enc_out,
                                 NULL, 0,
                                 ctx->service.encrypt_key,
                                 nonce,
                                 wire_buf, PACKET_HEADER_SIZE) != 0) { continue; }
                wire_len = PACKET_HEADER_SIZE + enc_out;

                for (k = 0; k < ctx->n_path; k++)
                {
#ifdef _WIN32
                    sendto(ctx->sock[k], (const char *)wire_buf, (int)wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr[k],
                           sizeof(ctx->dest_addr[k]));
#else
                    sendto(ctx->sock[k], wire_buf, wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr[k],
                           sizeof(ctx->dest_addr[k]));
#endif
                }
            }
            else
            {
                int k;
                wire_len = packet_wire_size(&ping_pkt);

                for (k = 0; k < ctx->n_path; k++)
                {
#ifdef _WIN32
                    sendto(ctx->sock[k], (const char *)&ping_pkt, (int)wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr[k],
                           sizeof(ctx->dest_addr[k]));
#else
                    sendto(ctx->sock[k], &ping_pkt, wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr[k],
                           sizeof(ctx->dest_addr[k]));
#endif
                }
            }
        }

        ctx->last_send_ms = health_get_ms();
    }

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/* ====================================================================
 * TCP 用ヘルスチェックスレッド本体 (path ごと)
 * ==================================================================== */
#ifdef _WIN32
static DWORD WINAPI tcp_health_thread_func(LPVOID arg)
#else
static void *tcp_health_thread_func(void *arg)
#endif
{
    HealthArg           *harg     = (HealthArg *)arg;
    struct PotrContext_ *ctx      = harg->ctx;
    int                  path_idx = harg->path_idx;
    PotrPacketSessionHdr shdr;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;
    shdr._pad            = 0;

    POTR_LOG(POTR_LOG_DEBUG,
             "tcp_health[service_id=%d path=%d]: starting",
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

        /* PING タイムアウト判定 (health_timeout_ms 超過でソケットをクローズ) */
        if (ctx->global.health_timeout_ms > 0)
        {
            uint64_t last = ctx->tcp_last_ping_recv_ms[path_idx];
            if (last > 0
                && health_get_ms() - last > (uint64_t)ctx->global.health_timeout_ms)
            {
                POTR_LOG(POTR_LOG_WARN,
                         "tcp_health[service_id=%d path=%d]: PING timeout"
                         " (%llu ms), closing connection",
                         ctx->service.service_id, path_idx,
                         (unsigned long long)(health_get_ms() - last));
                /* ソケットをクローズ → recv スレッドが切断を検知する */
#ifdef _WIN32
                shutdown(ctx->tcp_conn_fd[path_idx], SD_BOTH);
                closesocket(ctx->tcp_conn_fd[path_idx]);
#else
                shutdown(ctx->tcp_conn_fd[path_idx], SHUT_RDWR);
                close(ctx->tcp_conn_fd[path_idx]);
#endif
                ctx->tcp_conn_fd[path_idx] = POTR_INVALID_SOCKET;
                continue; /* connect スレッドに停止されるまで継続 */
            }
        }

        /* 接続中でなければ PING をスキップ */
        if (ctx->tcp_active_paths == 0
            || ctx->tcp_conn_fd[path_idx] == POTR_INVALID_SOCKET)
        {
            continue;
        }

        /* PING パケットを構築する */
        POTR_MUTEX_LOCK_LOCAL(&ctx->send_window_mutex);
        /* session 情報を最新に更新 (再接続時に変わっている可能性がある) */
        shdr.session_id      = ctx->session_id;
        shdr.session_tv_sec  = ctx->session_tv_sec;
        shdr.session_tv_nsec = ctx->session_tv_nsec;
        seq          = ctx->send_window.next_seq;
        build_result = packet_build_ping(&ping_pkt, &shdr, seq, 0U);
        POTR_MUTEX_UNLOCK_LOCAL(&ctx->send_window_mutex);

        if (build_result != POTR_SUCCESS) { continue; }

        POTR_LOG(POTR_LOG_TRACE,
                 "tcp_health[service_id=%d path=%d]: PING seq=%u",
                 ctx->service.service_id, path_idx, (unsigned)seq);

        if (ctx->service.encrypt_enabled)
        {
            uint8_t wire_buf[PACKET_HEADER_SIZE + POTR_CRYPTO_TAG_SIZE];
            uint8_t nonce[POTR_CRYPTO_NONCE_SIZE];
            size_t  enc_out = POTR_CRYPTO_TAG_SIZE;

            ping_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
            ping_pkt.payload_len = htons((uint16_t)POTR_CRYPTO_TAG_SIZE);

            memcpy(nonce,      &ping_pkt.session_id, 4);
            memcpy(nonce + 4,  &ping_pkt.flags,      2);
            memcpy(nonce + 6,  &ping_pkt.seq_num,    4);
            memset(nonce + 10, 0,                    2);

            memcpy(wire_buf, &ping_pkt, PACKET_HEADER_SIZE);
            if (potr_encrypt(wire_buf + PACKET_HEADER_SIZE, &enc_out,
                             NULL, 0,
                             ctx->service.encrypt_key,
                             nonce,
                             wire_buf, PACKET_HEADER_SIZE) != 0) { continue; }
            wire_len = PACKET_HEADER_SIZE + enc_out;

            POTR_MUTEX_LOCK_LOCAL(&ctx->tcp_send_mutex[path_idx]);
            if (ctx->tcp_conn_fd[path_idx] != POTR_INVALID_SOCKET)
            {
#ifdef _WIN32
                send(ctx->tcp_conn_fd[path_idx],
                     (const char *)wire_buf, (int)wire_len, 0);
#else
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
#endif
            }
            POTR_MUTEX_UNLOCK_LOCAL(&ctx->tcp_send_mutex[path_idx]);
        }
        else
        {
            wire_len = packet_wire_size(&ping_pkt);

            POTR_MUTEX_LOCK_LOCAL(&ctx->tcp_send_mutex[path_idx]);
            if (ctx->tcp_conn_fd[path_idx] != POTR_INVALID_SOCKET)
            {
#ifdef _WIN32
                send(ctx->tcp_conn_fd[path_idx],
                     (const char *)&ping_pkt, (int)wire_len, 0);
#else
                {
                    size_t        sent = 0;
                    const uint8_t *p   = (const uint8_t *)&ping_pkt;
                    while (sent < wire_len)
                    {
                        ssize_t n = send(ctx->tcp_conn_fd[path_idx],
                                         p + sent, wire_len - sent, 0);
                        if (n <= 0) break;
                        sent += (size_t)n;
                    }
                }
#endif
            }
            POTR_MUTEX_UNLOCK_LOCAL(&ctx->tcp_send_mutex[path_idx]);
        }
    }

    POTR_LOG(POTR_LOG_DEBUG,
             "tcp_health[service_id=%d path=%d]: exited",
             ctx->service.service_id, path_idx);

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
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
        POTR_LOG(POTR_LOG_DEBUG,
                 "health_thread[service_id=%d]: disabled (health_interval_ms=0)",
                 ctx->service.service_id);
        return POTR_SUCCESS;
    }

    POTR_LOG(POTR_LOG_DEBUG,
             "health_thread[service_id=%d]: starting (interval=%ums)",
             ctx->service.service_id,
             (unsigned)ctx->global.health_interval_ms);

#ifdef _WIN32
    InitializeCriticalSection(&ctx->health_mutex[0]);
    InitializeConditionVariable(&ctx->health_wakeup[0]);
#else
    pthread_mutex_init(&ctx->health_mutex[0], NULL);
    pthread_cond_init(&ctx->health_wakeup[0], NULL);
#endif

    ctx->health_running[0] = 1;

#ifdef _WIN32
    ctx->health_thread[0] = CreateThread(NULL, 0, health_thread_func, ctx, 0, NULL);
    if (ctx->health_thread[0] == NULL)
    {
        ctx->health_running[0] = 0;
        POTR_LOG(POTR_LOG_ERROR,
                 "health_thread[service_id=%d]: CreateThread failed",
                 ctx->service.service_id);
        return POTR_ERROR;
    }
#else
    if (pthread_create(&ctx->health_thread[0], NULL, health_thread_func, ctx) != 0)
    {
        ctx->health_running[0] = 0;
        POTR_LOG(POTR_LOG_ERROR,
                 "health_thread[service_id=%d]: pthread_create failed",
                 ctx->service.service_id);
        return POTR_ERROR;
    }
#endif

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

#ifdef _WIN32
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
#else
    pthread_mutex_lock(&ctx->health_mutex[0]);
    pthread_cond_signal(&ctx->health_wakeup[0]);
    pthread_mutex_unlock(&ctx->health_mutex[0]);

    pthread_join(ctx->health_thread[0], NULL);

    pthread_cond_destroy(&ctx->health_wakeup[0]);
    pthread_mutex_destroy(&ctx->health_mutex[0]);
#endif

    return POTR_SUCCESS;
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
        POTR_LOG(POTR_LOG_DEBUG,
                 "tcp_health_thread[service_id=%d path=%d]: disabled",
                 ctx->service.service_id, path_idx);
        return POTR_SUCCESS;
    }

    s_health_args[path_idx].ctx      = ctx;
    s_health_args[path_idx].path_idx = path_idx;

    ctx->health_running[path_idx] = 1;

#ifdef _WIN32
    ctx->health_thread[path_idx] = CreateThread(NULL, 0,
                                                tcp_health_thread_func,
                                                &s_health_args[path_idx], 0, NULL);
    if (ctx->health_thread[path_idx] == NULL)
    {
        ctx->health_running[path_idx] = 0;
        POTR_LOG(POTR_LOG_ERROR,
                 "tcp_health_thread[service_id=%d path=%d]: CreateThread failed",
                 ctx->service.service_id, path_idx);
        return POTR_ERROR;
    }
#else
    if (pthread_create(&ctx->health_thread[path_idx], NULL,
                       tcp_health_thread_func, &s_health_args[path_idx]) != 0)
    {
        ctx->health_running[path_idx] = 0;
        POTR_LOG(POTR_LOG_ERROR,
                 "tcp_health_thread[service_id=%d path=%d]: pthread_create failed",
                 ctx->service.service_id, path_idx);
        return POTR_ERROR;
    }
#endif

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

#ifdef _WIN32
    if (ctx->health_thread[path_idx] != NULL)
    {
        EnterCriticalSection(&ctx->health_mutex[path_idx]);
        WakeConditionVariable(&ctx->health_wakeup[path_idx]);
        LeaveCriticalSection(&ctx->health_mutex[path_idx]);

        WaitForSingleObject(ctx->health_thread[path_idx], INFINITE);
        CloseHandle(ctx->health_thread[path_idx]);
        ctx->health_thread[path_idx] = NULL;
    }
#else
    pthread_mutex_lock(&ctx->health_mutex[path_idx]);
    pthread_cond_signal(&ctx->health_wakeup[path_idx]);
    pthread_mutex_unlock(&ctx->health_mutex[path_idx]);

    pthread_join(ctx->health_thread[path_idx], NULL);
#endif

    return POTR_SUCCESS;
}
