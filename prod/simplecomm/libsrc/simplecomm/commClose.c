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

#include <libsimplecomm_const.h>
#include <libsimplecomm.h>

#include "commContext.h"
#include "commRecvThread.h"

/* IPv4 文字列をネットワークバイトオーダーへ変換する。 */
static int parse_ipv4_addr(const char *ip_str, struct in_addr *out_addr)
{
    if (ip_str == NULL || out_addr == NULL)
    {
        return COMM_ERROR;
    }

    return (inet_pton(AF_INET, ip_str, out_addr) == 1) ? COMM_SUCCESS : COMM_ERROR;
}

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
