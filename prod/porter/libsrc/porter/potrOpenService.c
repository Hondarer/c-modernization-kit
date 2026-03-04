/**
 *******************************************************************************
 *  @file           potrOpenService.c
 *  @brief          potrOpenService 関数の実装。
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
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <porter_const.h>
#include <porter.h>

#include "protocol/config.h"
#include "protocol/window.h"
#include "protocol/retransmit.h"
#include "potrContext.h"
#include "potrRecvThread.h"
#include "util/potrIpAddr.h"

/* ソケットを作成して bind する。成功時は PotrSocket を返す。失敗時は POTR_INVALID_SOCKET。 */
static PotrSocket open_socket_unicast(uint16_t port)
{
    PotrSocket         sock;
    struct sockaddr_in addr;
    int                reuse = 1;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == POTR_INVALID_SOCKET)
    {
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
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return POTR_INVALID_SOCKET;
    }

    return sock;
}

/* マルチキャストソケットを作成して bind・グループ参加する。 */
static PotrSocket open_socket_multicast(const PotrServiceDef *def)
{
    PotrSocket         sock;
    struct sockaddr_in addr;
    struct ip_mreq     mreq;
    int                reuse = 1;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == POTR_INVALID_SOCKET)
    {
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
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(def->src_port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return POTR_INVALID_SOCKET;
    }

    /* マルチキャストグループへ参加 */
    memset(&mreq, 0, sizeof(mreq));
    if (parse_ipv4_addr(def->multicast_group, &mreq.imr_multiaddr) != POTR_SUCCESS)
    {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return POTR_INVALID_SOCKET;
    }
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (const char *)&mreq, sizeof(mreq)) < 0)
    {
        closesocket(sock);
        return POTR_INVALID_SOCKET;
    }
#else
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &mreq, sizeof(mreq)) < 0)
    {
        close(sock);
        return POTR_INVALID_SOCKET;
    }
#endif

    return sock;
}

/* ブロードキャストソケットを作成して bind する。 */
static PotrSocket open_socket_broadcast(uint16_t port)
{
    PotrSocket         sock;
    struct sockaddr_in addr;
    int                reuse  = 1;
    int                bcast  = 1;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == POTR_INVALID_SOCKET)
    {
        return POTR_INVALID_SOCKET;
    }

#ifdef _WIN32
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&reuse, sizeof(reuse));
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
               (const char *)&bcast, sizeof(bcast));
#else
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof(bcast));
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return POTR_INVALID_SOCKET;
    }

    return sock;
}

/* doxygen コメントは、ヘッダに記載 */
POTR_API int POTRAPI potrOpenService(const char       *config_path,
                                     int               service_id,
                                     PotrRecvCallback  callback,
                                     PotrHandle       *handle)
{
    struct PotrContext_ *ctx;
    PotrSocket           sock;

    if (config_path == NULL || handle == NULL)
    {
        return POTR_ERROR;
    }

#ifdef _WIN32
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            return POTR_ERROR;
        }
    }
#endif

    ctx = (struct PotrContext_ *)malloc(sizeof(struct PotrContext_));
    if (ctx == NULL)
    {
        return POTR_ERROR;
    }
    memset(ctx, 0, sizeof(*ctx));
    ctx->sock = POTR_INVALID_SOCKET;

    /* 設定ファイルを読み込む */
    if (config_load_global(config_path, &ctx->global) != POTR_SUCCESS)
    {
        free(ctx);
        return POTR_ERROR;
    }

    if (config_load_service(config_path, service_id, &ctx->service) != POTR_SUCCESS)
    {
        free(ctx);
        return POTR_ERROR;
    }

    /* 通信種別に応じてソケットを作成 */
    switch (ctx->service.type)
    {
        case POTR_TYPE_UNICAST:
            sock = open_socket_unicast(ctx->service.dst_port);
            break;

        case POTR_TYPE_MULTICAST:
            sock = open_socket_multicast(&ctx->service);
            break;

        case POTR_TYPE_BROADCAST:
            sock = open_socket_broadcast(ctx->service.src_port);
            break;

        default:
            free(ctx);
            return POTR_ERROR;
    }

    if (sock == POTR_INVALID_SOCKET)
    {
        free(ctx);
        return POTR_ERROR;
    }

    ctx->sock     = sock;
    ctx->callback = callback;

    /* ウィンドウ・再送制御を初期化 */
    window_init(&ctx->send_window, 0, ctx->global.window_size);
    window_init(&ctx->recv_window, 0, ctx->global.window_size);
    retransmit_init(&ctx->retransmit,
                    ctx->global.retransmit_timeout_ms,
                    ctx->global.retransmit_count);

    /* 受信コールバックが指定された場合は受信スレッドを起動 */
    if (callback != NULL)
    {
        if (comm_recv_thread_start(ctx) != POTR_SUCCESS)
        {
#ifdef _WIN32
            closesocket(ctx->sock);
#else
            close(ctx->sock);
#endif
            free(ctx);
            return POTR_ERROR;
        }
    }

    *handle = ctx;
    return POTR_SUCCESS;
}
