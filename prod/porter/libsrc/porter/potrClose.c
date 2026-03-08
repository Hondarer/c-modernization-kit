/**
 *******************************************************************************
 *  @file           potrClose.c
 *  @brief          potrClose 関数の実装。
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

#include "potrContext.h"
#include "potrRecvThread.h"
#include "potrHealthThread.h"
#include "potrSendQueue.h"
#include "potrSendThread.h"
#include "protocol/packet.h"
#include "util/potrIpAddr.h"

/* FIN パケットを宛先へ直接 sendto する */
static void send_fin(struct PotrContext_ *ctx)
{
    PotrPacket           fin_pkt;
    PotrPacketSessionHdr shdr;
    size_t               wire_len;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    if (packet_build_fin(&fin_pkt, &shdr) != POTR_SUCCESS)
    {
        return;
    }

    wire_len = packet_wire_size(&fin_pkt);

#ifdef _WIN32
    sendto(ctx->sock, (const char *)&fin_pkt, (int)wire_len, 0,
           (const struct sockaddr *)&ctx->dest_addr, sizeof(ctx->dest_addr));
#else
    sendto(ctx->sock, &fin_pkt, wire_len, 0,
           (const struct sockaddr *)&ctx->dest_addr, sizeof(ctx->dest_addr));
#endif
}

/* doxygen コメントは、ヘッダに記載 */
POTR_API int POTRAPI potrClose(PotrHandle handle)
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)handle;

    if (ctx == NULL)
    {
        return POTR_ERROR;
    }

    /* ヘルスチェックスレッドを停止 (送信者のみ) */
    if (ctx->health_running)
    {
        potr_health_thread_stop(ctx);
    }

    /* 送信スレッドを停止してキューを破棄 (送信者のみ) */
    if (ctx->send_thread_running)
    {
        potr_send_queue_wait_drained(&ctx->send_queue);
        send_fin(ctx);
        potr_send_thread_stop(ctx);
        potr_send_queue_destroy(&ctx->send_queue);
    }

    /* 受信スレッドを停止（スレッド停止内でソケットをクローズする） */
    if (ctx->running)
    {
        comm_recv_thread_stop(ctx);
    }
    else
    {
        /* 受信スレッドなしの場合はここでソケットをクローズ */
        if (ctx->sock != POTR_INVALID_SOCKET)
        {
#ifdef _WIN32
            /* マルチキャストのグループ離脱 */
            if (ctx->service.type == POTR_TYPE_MULTICAST)
            {
                struct ip_mreq mreq;
                memset(&mreq, 0, sizeof(mreq));
                if (parse_ipv4_addr(ctx->service.multicast_group, &mreq.imr_multiaddr)
                    == POTR_SUCCESS)
                {
                    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
                    setsockopt(ctx->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                               (const char *)&mreq, sizeof(mreq));
                }
            }
            closesocket(ctx->sock);
#else
            if (ctx->service.type == POTR_TYPE_MULTICAST)
            {
                struct ip_mreq mreq;
                memset(&mreq, 0, sizeof(mreq));
                if (parse_ipv4_addr(ctx->service.multicast_group, &mreq.imr_multiaddr)
                    == POTR_SUCCESS)
                {
                    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
                    setsockopt(ctx->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                               &mreq, sizeof(mreq));
                }
            }
            close(ctx->sock);
#endif
            ctx->sock = POTR_INVALID_SOCKET;
        }
    }

#ifdef _WIN32
    WSACleanup();
#endif

    free(ctx);
    return POTR_SUCCESS;
}
