/**
 *******************************************************************************
 *  @file           commClose.c
 *  @brief          commClose 関数の実装。
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

#include <simplecomm_const.h>
#include <simplecomm.h>

#include "commContext.h"
#include "commRecvThread.h"
#include "util/commIpAddr.h"

/* doxygen コメントは、ヘッダに記載 */
COMM_API int COMMAPI commClose(CommHandle handle)
{
    struct CommContext_ *ctx = (struct CommContext_ *)handle;

    if (ctx == NULL)
    {
        return COMM_ERROR;
    }

    /* 受信スレッドを停止（スレッド停止内でソケットをクローズする） */
    if (ctx->running)
    {
        comm_recv_thread_stop(ctx);
    }
    else
    {
        /* 受信スレッドなしの場合はここでソケットをクローズ */
        if (ctx->sock != COMM_INVALID_SOCKET)
        {
#ifdef _WIN32
            /* マルチキャストのグループ離脱 */
            if (ctx->service.type == COMM_TYPE_MULTICAST)
            {
                struct ip_mreq mreq;
                memset(&mreq, 0, sizeof(mreq));
                if (parse_ipv4_addr(ctx->service.multicast_group, &mreq.imr_multiaddr)
                    == COMM_SUCCESS)
                {
                    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
                    setsockopt(ctx->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                               (const char *)&mreq, sizeof(mreq));
                }
            }
            closesocket(ctx->sock);
#else
            if (ctx->service.type == COMM_TYPE_MULTICAST)
            {
                struct ip_mreq mreq;
                memset(&mreq, 0, sizeof(mreq));
                if (parse_ipv4_addr(ctx->service.multicast_group, &mreq.imr_multiaddr)
                    == COMM_SUCCESS)
                {
                    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
                    setsockopt(ctx->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                               &mreq, sizeof(mreq));
                }
            }
            close(ctx->sock);
#endif
            ctx->sock = COMM_INVALID_SOCKET;
        }
    }

#ifdef _WIN32
    WSACleanup();
#endif

    free(ctx);
    return COMM_SUCCESS;
}
