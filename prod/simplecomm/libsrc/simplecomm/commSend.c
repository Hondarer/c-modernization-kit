/**
 *******************************************************************************
 *  @file           commSend.c
 *  @brief          commSend 関数の実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
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
#endif

#include <simplecomm_const.h>
#include <simplecomm.h>

#include "protocol/packet.h"
#include "protocol/seqnum.h"
#include "protocol/window.h"
#include "commContext.h"
#include "util/commIpAddr.h"

/* 通信種別に応じた送信先アドレスを構築する */
static int build_dest_addr(const struct CommContext_ *ctx,
                            struct sockaddr_in        *dest)
{
    memset(dest, 0, sizeof(*dest));
    dest->sin_family = AF_INET;

    switch (ctx->service.type)
    {
        case COMM_TYPE_UNICAST:
            /* unicast: 接続相手のポートへ送信。
               相手アドレスは受信時に取得するため、ここでは未設定。
               本実装では送信先ホストは呼び出し元が別途設定すると想定し、
               サービスの dst_port を宛先ポートとして使用する。 */
            dest->sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* 暫定: 同一ホスト */
            dest->sin_port        = htons(ctx->service.dst_port);
            break;

        case COMM_TYPE_MULTICAST:
            if (parse_ipv4_addr(ctx->service.multicast_group, &dest->sin_addr)
                != COMM_SUCCESS)
            {
                return COMM_ERROR;
            }
            dest->sin_port        = htons(ctx->service.src_port);
            break;

        case COMM_TYPE_BROADCAST:
            if (parse_ipv4_addr(ctx->service.broadcast_addr, &dest->sin_addr)
                != COMM_SUCCESS)
            {
                return COMM_ERROR;
            }
            dest->sin_port        = htons(ctx->service.src_port);
            break;

        default:
            return COMM_ERROR;
    }

    return COMM_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
COMM_API int COMMAPI commSend(CommHandle handle, const void *data, size_t len)
{
    struct CommContext_ *ctx       = (struct CommContext_ *)handle;
    const uint8_t       *ptr      = (const uint8_t *)data;
    struct sockaddr_in   dest;
    size_t               remaining;
    size_t               max_payload;

    if (ctx == NULL || data == NULL || len == 0 || len > COMM_MAX_MESSAGE_SIZE)
    {
        return COMM_ERROR;
    }

    if (build_dest_addr(ctx, &dest) != COMM_SUCCESS)
    {
        return COMM_ERROR;
    }

    remaining   = len;
    max_payload = ctx->global.max_payload;

    while (remaining > 0)
    {
        CommPacket pkt;
        size_t     chunk     = (remaining > max_payload) ? max_payload : remaining;
        int        more_frag = (remaining > chunk);
        uint32_t   seq;
        size_t     wire_len;

        /* 送信ウィンドウが満杯の場合はエラー返し（ブロッキングは未実装） */
        if (window_send_full(&ctx->send_window))
        {
            return COMM_ERROR;
        }

        seq = ctx->send_window.next_seq;

        if (packet_build_data(&pkt, seq, ptr, chunk) != COMM_SUCCESS)
        {
            return COMM_ERROR;
        }

        if (more_frag)
        {
            pkt.flags |= COMM_FLAG_MORE_FRAG;
        }

        if (window_send_push(&ctx->send_window, &pkt) != COMM_SUCCESS)
        {
            return COMM_ERROR;
        }

        wire_len = packet_wire_size(&pkt);

#ifdef _WIN32
        if (sendto(ctx->sock, (const char *)&pkt, (int)wire_len, 0,
                   (const struct sockaddr *)&dest, sizeof(dest)) == SOCKET_ERROR)
        {
            return COMM_ERROR;
        }
#else
        if (sendto(ctx->sock, &pkt, wire_len, 0,
                   (const struct sockaddr *)&dest, sizeof(dest)) < 0)
        {
            return COMM_ERROR;
        }
#endif

        ptr       += chunk;
        remaining -= chunk;
    }

    return COMM_SUCCESS;
}
