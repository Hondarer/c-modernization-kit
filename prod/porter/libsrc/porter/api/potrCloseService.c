/**
 *******************************************************************************
 *  @file           potrCloseServiceService.c
 *  @brief          potrCloseServiceService 関数の実装。
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

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

#include <porter_const.h>
#include <porter.h>

#include "../potrContext.h"
#include "../thread/potrRecvThread.h"
#include "../thread/potrHealthThread.h"
#include "../infra/potrSendQueue.h"
#include "../thread/potrSendThread.h"
#include "../protocol/packet.h"
#include "../protocol/window.h"
#include "../util/potrIpAddr.h"
#include "../infra/potrLog.h"

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

    if (packet_build_fin(&fin_pkt, &shdr) != POTR_SUCCESS) return;

    wire_len = packet_wire_size(&fin_pkt);

    for (i = 0; i < ctx->n_path; i++)
    {
        if (ctx->sock[i] == POTR_INVALID_SOCKET) continue;
#ifdef _WIN32
        sendto(ctx->sock[i], (const char *)&fin_pkt, (int)wire_len, 0,
               (const struct sockaddr *)&ctx->dest_addr[i],
               sizeof(ctx->dest_addr[i]));
#else
        sendto(ctx->sock[i], &fin_pkt, wire_len, 0,
               (const struct sockaddr *)&ctx->dest_addr[i],
               sizeof(ctx->dest_addr[i]));
#endif
    }
}

/* doxygen コメントは、ヘッダに記載 */
POTR_API int POTRAPI potrCloseService(PotrHandle handle)
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

    /* ヘルスチェックスレッドを停止 (送信者のみ) */
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
        send_fin(ctx);
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
#ifdef _WIN32
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
#else
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
#endif
            ctx->sock[i] = POTR_INVALID_SOCKET;
        }
    }

#ifdef _WIN32
    WSACleanup();
#endif

    /* 送受信ウィンドウと動的バッファを解放 */
    window_destroy(&ctx->send_window);
    window_destroy(&ctx->recv_window);
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
