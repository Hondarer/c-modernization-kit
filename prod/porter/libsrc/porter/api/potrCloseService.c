/**
 *******************************************************************************
 *  @file           potrCloseService.c
 *  @brief          potrCloseService 関数の実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#else /* _WIN32 */
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif /* _WIN32 */

#include <porter_const.h>
#include <porter.h>

#include "../potrContext.h"
#include "../potrPeerTable.h"
#include "../thread/potrRecvThread.h"
#include "../thread/potrHealthThread.h"
#include "../thread/potrConnectThread.h"
#include "../infra/potrSendQueue.h"
#include "../thread/potrSendThread.h"
#include "../protocol/packet.h"
#include "../protocol/window.h"
#include "../util/potrIpAddr.h"
#include "../infra/potrLog.h"
#include "../infra/crypto/crypto.h"

/* FIN パケットを全パスへ送信する */
static void send_fin(struct PotrContext_ *ctx)
{
    PotrPacket           fin_pkt;
    PotrPacketSessionHdr shdr;
    size_t               wire_len;
    int                  i;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;
    shdr._pad            = 0;

    if (packet_build_fin(&fin_pkt, &shdr) != POTR_SUCCESS) return;

    if (ctx->service.encrypt_enabled)
    {
        uint8_t  wire_buf[PACKET_HEADER_SIZE + POTR_CRYPTO_TAG_SIZE];
        uint8_t  nonce[POTR_CRYPTO_NONCE_SIZE];
        size_t   enc_out = POTR_CRYPTO_TAG_SIZE;

        fin_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
        fin_pkt.payload_len = htons((uint16_t)POTR_CRYPTO_TAG_SIZE);

        /* ノンス: session_id(4B) + flags(2B, FIN|ENCRYPTED NBO) + 0(4B) + padding(2B) */
        memcpy(nonce,      &fin_pkt.session_id, 4);
        memcpy(nonce + 4,  &fin_pkt.flags,      2);
        memset(nonce + 6,  0,                   4);
        memset(nonce + 10, 0,                   2);

        memcpy(wire_buf, &fin_pkt, PACKET_HEADER_SIZE);
        if (potr_encrypt(wire_buf + PACKET_HEADER_SIZE, &enc_out,
                         NULL, 0,
                         ctx->service.encrypt_key,
                         nonce,
                         wire_buf, PACKET_HEADER_SIZE) != 0)
        {
            return;
        }
        wire_len = PACKET_HEADER_SIZE + enc_out;

        for (i = 0; i < ctx->n_path; i++)
        {
            if (ctx->sock[i] == POTR_INVALID_SOCKET) continue;
#ifndef _WIN32
            sendto(ctx->sock[i], wire_buf, wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr[i],
                   sizeof(ctx->dest_addr[i]));
#else /* _WIN32 */
            sendto(ctx->sock[i], (const char *)wire_buf, (int)wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr[i],
                   sizeof(ctx->dest_addr[i]));
#endif /* _WIN32 */
        }
    }
    else
    {
        wire_len = packet_wire_size(&fin_pkt);

        for (i = 0; i < ctx->n_path; i++)
        {
            if (ctx->sock[i] == POTR_INVALID_SOCKET) continue;
#ifndef _WIN32
            sendto(ctx->sock[i], &fin_pkt, wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr[i],
                   sizeof(ctx->dest_addr[i]));
#else /* _WIN32 */
            sendto(ctx->sock[i], (const char *)&fin_pkt, (int)wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr[i],
                   sizeof(ctx->dest_addr[i]));
#endif /* _WIN32 */
        }
    }
}

/* doxygen コメントは、ヘッダに記載 */
POTR_EXPORT int POTR_API potrCloseService(PotrHandle handle)
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)handle;

    if (ctx == NULL)
    {
        POTR_LOG(POTR_LOG_ERROR, "potrCloseService: handle is NULL");
        return POTR_ERROR;
    }

    POTR_LOG(POTR_LOG_INFO,
             "potrCloseService: service_id=%d closing",
             ctx->service.service_id);

    /* TCP: 接続管理スレッドを停止する (send/recv/health スレッドは connect スレッド内で停止) */
    if (potr_is_tcp_type(ctx->service.type))
    {
        POTR_LOG(POTR_LOG_DEBUG,
                 "potrCloseService: service_id=%d stopping connect thread (TCP)",
                 ctx->service.service_id);
        potr_connect_thread_stop(ctx);

        /* 送信キューを破棄 (SENDER / TCP_BIDIR のみ) */
        if (ctx->role == POTR_ROLE_SENDER
            || ctx->service.type == POTR_TYPE_TCP_BIDIR)
        {
            potr_send_queue_destroy(&ctx->send_queue);
        }

        /* TCP mutex / condvar を解放 */
#ifndef _WIN32
        {
            int i;
            pthread_mutex_destroy(&ctx->tcp_state_mutex);
            pthread_cond_destroy(&ctx->tcp_state_cv);
            for (i = 0; i < (int)POTR_MAX_PATH; i++)
            {
                pthread_mutex_destroy(&ctx->tcp_send_mutex[i]);
                pthread_mutex_destroy(&ctx->health_mutex[i]);
                pthread_cond_destroy(&ctx->health_wakeup[i]);
            }
            pthread_mutex_destroy(&ctx->recv_window_mutex);
        }
#else /* _WIN32 */
        {
            int i;
            DeleteCriticalSection(&ctx->tcp_state_mutex);
            /* Windows の CONDITION_VARIABLE は破棄不要 */
            for (i = 0; i < (int)POTR_MAX_PATH; i++)
            {
                DeleteCriticalSection(&ctx->tcp_send_mutex[i]);
                DeleteCriticalSection(&ctx->health_mutex[i]);
            }
            DeleteCriticalSection(&ctx->recv_window_mutex);
        }
#endif /* _WIN32 */

        /* 送受信ウィンドウと動的バッファを解放 */
        window_destroy(&ctx->send_window);
        window_destroy(&ctx->recv_window);
        free(ctx->frag_buf);
        free(ctx->compress_buf);
        free(ctx->crypto_buf);
        free(ctx->recv_buf);
        free(ctx->send_wire_buf);

#ifdef _WIN32
        WSACleanup();
#endif /* _WIN32 */

        POTR_LOG(POTR_LOG_INFO,
                 "potrCloseService: service closed (TCP)");
        free(ctx);
        return POTR_SUCCESS;
    }

    /* 非 TCP: ヘルスチェックスレッドを停止 (送信者のみ) */
    if (ctx->health_running)
    {
        POTR_LOG(POTR_LOG_DEBUG,
                 "potrCloseService: service_id=%d stopping health thread",
                 ctx->service.service_id);
        potr_health_thread_stop(ctx);
    }

    /* 送信スレッドを停止してキューを破棄 (送信者のみ) */
    if (ctx->send_thread_running)
    {
        POTR_LOG(POTR_LOG_DEBUG,
                 "potrCloseService: service_id=%d flushing send queue and sending FIN",
                 ctx->service.service_id);
        potr_send_queue_wait_drained(&ctx->send_queue);
        if (ctx->is_multi_peer)
        {
            /* N:1: 全アクティブピアへ FIN を送信してピアテーブルを破棄 */
            peer_table_destroy(ctx);
        }
        else
        {
            /* 1:1: 単一宛先へ FIN を送信 */
            send_fin(ctx);
        }
        potr_send_thread_stop(ctx);
        potr_send_queue_destroy(&ctx->send_queue);
    }

    /* 受信スレッドを停止する */
    if (ctx->running)
    {
        POTR_LOG(POTR_LOG_DEBUG,
                 "potrCloseService: service_id=%d stopping recv thread",
                 ctx->service.service_id);
        comm_recv_thread_stop(ctx);
    }

    /* ソケットをクローズ (recv スレッドの有無によらず実施)。
       Windows: comm_recv_thread_stop 内で closesocket 済みの場合は INVALID_SOCKET になっているため
                guard により二重 close を回避する。
       Linux: comm_recv_thread_stop は shutdown のみで close しないため、ここで必ず close する。 */
    {
        int i;
        for (i = 0; i < ctx->n_path; i++)
        {
            if (ctx->sock[i] == POTR_INVALID_SOCKET) continue;
#ifndef _WIN32
            if (potr_raw_base_type(ctx->service.type) == POTR_TYPE_MULTICAST)
            {
                struct ip_mreq mreq;
                memset(&mreq, 0, sizeof(mreq));
                if (parse_ipv4_addr(ctx->service.multicast_group, &mreq.imr_multiaddr)
                    == POTR_SUCCESS)
                {
                    mreq.imr_interface = ctx->src_addr_resolved[i];
                    setsockopt(ctx->sock[i], IPPROTO_IP, IP_DROP_MEMBERSHIP,
                               &mreq, sizeof(mreq));
                }
            }
            close(ctx->sock[i]);
#else /* _WIN32 */
            if (potr_raw_base_type(ctx->service.type) == POTR_TYPE_MULTICAST)
            {
                struct ip_mreq mreq;
                memset(&mreq, 0, sizeof(mreq));
                if (parse_ipv4_addr(ctx->service.multicast_group, &mreq.imr_multiaddr)
                    == POTR_SUCCESS)
                {
                    mreq.imr_interface = ctx->src_addr_resolved[i];
                    setsockopt(ctx->sock[i], IPPROTO_IP, IP_DROP_MEMBERSHIP,
                               (const char *)&mreq, sizeof(mreq));
                }
            }
            closesocket(ctx->sock[i]);
#endif /* _WIN32 */
            ctx->sock[i] = POTR_INVALID_SOCKET;
        }
    }

#ifdef _WIN32
    WSACleanup();
#endif /* _WIN32 */

    /* 送受信ウィンドウと動的バッファを解放 */
    if (!ctx->is_multi_peer)
    {
        /* 1:1 モード: コンテキストレベルのウィンドウを解放する */
        window_destroy(&ctx->send_window);
        window_destroy(&ctx->recv_window);
    }
    else if (ctx->peers != NULL)
    {
        /* N:1 モード: 送信スレッド未起動の場合もピアテーブルを解放する
           (既に peer_table_destroy 済みの場合は ctx->peers が NULL になっている) */
        peer_table_destroy(ctx);
    }
    free(ctx->frag_buf);
    free(ctx->compress_buf);
    free(ctx->crypto_buf);
    free(ctx->recv_buf);
    free(ctx->send_wire_buf);

    POTR_LOG(POTR_LOG_INFO,
             "potrCloseService: service closed");

    free(ctx);
    return POTR_SUCCESS;
}
