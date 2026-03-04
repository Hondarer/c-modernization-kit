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
#include "util/potrIpAddr.h"

/* doxygen コメントは、ヘッダに記載 */
POTR_API int POTRAPI potrClose(PotrHandle handle)
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)handle;

    if (ctx == NULL)
    {
        return POTR_ERROR;
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
