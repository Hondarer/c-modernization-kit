/**
 *******************************************************************************
 *  @file           potrSend.c
 *  @brief          potrSend 関数の実装。
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

#include <porter_const.h>
#include <porter.h>

#include "protocol/packet.h"
#include "protocol/seqnum.h"
#include "protocol/window.h"
#include "potrContext.h"
#include "compress/compress.h"
#include "util/potrIpAddr.h"

/* 通信種別に応じた送信先アドレスを構築する */
static int build_dest_addr(const struct PotrContext_ *ctx,
                            struct sockaddr_in        *dest)
{
    memset(dest, 0, sizeof(*dest));
    dest->sin_family = AF_INET;

    switch (ctx->service.type)
    {
        case POTR_TYPE_UNICAST:
            dest->sin_addr = ctx->dst_addr_resolved;
            dest->sin_port = htons(ctx->service.dst_port);
            break;

        case POTR_TYPE_MULTICAST:
            if (parse_ipv4_addr(ctx->service.multicast_group, &dest->sin_addr)
                != POTR_SUCCESS)
            {
                return POTR_ERROR;
            }
            dest->sin_port = htons(ctx->service.dst_port);
            break;

        case POTR_TYPE_BROADCAST:
            if (parse_ipv4_addr(ctx->service.broadcast_addr, &dest->sin_addr)
                != POTR_SUCCESS)
            {
                return POTR_ERROR;
            }
            dest->sin_port = htons(ctx->service.dst_port);
            break;

        default:
            return POTR_ERROR;
    }

    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
POTR_API int POTRAPI potrSend(PotrHandle handle, const void *data, size_t len,
                              int compress)
{
    struct PotrContext_  *ctx       = (struct PotrContext_ *)handle;
    const uint8_t        *ptr      = (const uint8_t *)data;
    struct sockaddr_in    dest;
    PotrPacketSessionHdr  shdr;
    size_t                remaining;
    size_t                max_payload;
    uint16_t              data_flags = POTR_FLAG_DATA;

    if (ctx == NULL || data == NULL || len == 0 || len > POTR_MAX_MESSAGE_SIZE)
    {
        return POTR_ERROR;
    }

    /* 圧縮が要求された場合はペイロード全体を圧縮する */
    if (compress != 0)
    {
        size_t cmp_len = sizeof(ctx->compress_buf);

        if (potr_compress(ctx->compress_buf, &cmp_len,
                          (const uint8_t *)data, len) != 0)
        {
            return POTR_ERROR;
        }

        ptr        = ctx->compress_buf;
        len        = cmp_len;
        data_flags = (uint16_t)(POTR_FLAG_DATA | POTR_FLAG_COMPRESSED);
    }

    if (build_dest_addr(ctx, &dest) != POTR_SUCCESS)
    {
        return POTR_ERROR;
    }

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    remaining   = len;
    max_payload = ctx->global.max_payload;

    while (remaining > 0)
    {
        PotrPacket pkt;
        size_t     chunk     = (remaining > max_payload) ? max_payload : remaining;
        int        more_frag = (remaining > chunk);
        uint32_t   seq;
        size_t     wire_len;

        /* 送信ウィンドウが満杯の場合はエラー返し（ブロッキングは未実装） */
        if (window_send_full(&ctx->send_window))
        {
            return POTR_ERROR;
        }

        seq = ctx->send_window.next_seq;

        if (packet_build_data(&pkt, &shdr, seq, ptr, chunk) != POTR_SUCCESS)
        {
            return POTR_ERROR;
        }

        /* 圧縮フラグと後続フラグメントフラグを付与 */
        pkt.flags |= (uint16_t)(data_flags & ~(uint16_t)POTR_FLAG_DATA);
        if (more_frag)
        {
            pkt.flags |= POTR_FLAG_MORE_FRAG;
        }

        if (window_send_push(&ctx->send_window, &pkt) != POTR_SUCCESS)
        {
            return POTR_ERROR;
        }

        wire_len = packet_wire_size(&pkt);

#ifdef _WIN32
        if (sendto(ctx->sock, (const char *)&pkt, (int)wire_len, 0,
                   (const struct sockaddr *)&dest, sizeof(dest)) == SOCKET_ERROR)
        {
            return POTR_ERROR;
        }
#else
        if (sendto(ctx->sock, &pkt, wire_len, 0,
                   (const struct sockaddr *)&dest, sizeof(dest)) < 0)
        {
            return POTR_ERROR;
        }
#endif

        ptr       += chunk;
        remaining -= chunk;
    }

    return POTR_SUCCESS;
}
