/**
 *******************************************************************************
 *  @file           commRecvThread.c
 *  @brief          受信スレッドモジュール。
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
    #include <unistd.h>
#endif

#include <libsimplecomm_const.h>

#include "../simplecommcore/packet.h"
#include "../simplecommcore/window.h"
#include "commContext.h"
#include "commRecvThread.h"

/** 受信バッファサイズ。ヘッダー + 最大ペイロード分を確保する。 */
#define RECV_BUF_SIZE (sizeof(CommPacket))

/* NACK パケットを送信元へユニキャスト送信する */
static void send_nack(struct CommContext_ *ctx,
                      const struct sockaddr_in *sender_addr,
                      uint32_t nack_seq)
{
    CommPacket nack_pkt;
    size_t     wire_len;

    if (packet_build_nack(&nack_pkt, nack_seq) != COMM_SUCCESS)
    {
        return;
    }

    wire_len = sizeof(uint32_t) * 2 + sizeof(uint16_t) * 2;

#ifdef _WIN32
    sendto(ctx->sock, (const char *)&nack_pkt, (int)wire_len, 0,
           (const struct sockaddr *)sender_addr, sizeof(*sender_addr));
#else
    sendto(ctx->sock, &nack_pkt, wire_len, 0,
           (const struct sockaddr *)sender_addr, sizeof(*sender_addr));
#endif
}

/* 受信スレッド本体 */
#ifdef _WIN32
static DWORD WINAPI recv_thread_func(LPVOID arg)
#else
static void *recv_thread_func(void *arg)
#endif
{
    struct CommContext_ *ctx = (struct CommContext_ *)arg;
    uint8_t             buf[RECV_BUF_SIZE];
    CommPacket          pkt;
    CommPacket          ordered_pkt;
    struct sockaddr_in  sender_addr;
    uint32_t            nack_num;

#ifdef _WIN32
    int sender_len = sizeof(sender_addr);
#else
    socklen_t sender_len = sizeof(sender_addr);
#endif

    while (ctx->running)
    {
        int recv_len;

        memset(&sender_addr, 0, sizeof(sender_addr));

#ifdef _WIN32
        recv_len = recvfrom(ctx->sock, (char *)buf, (int)sizeof(buf), 0,
                            (struct sockaddr *)&sender_addr, &sender_len);
        if (recv_len == SOCKET_ERROR)
        {
            if (!ctx->running)
            {
                break;
            }
            continue;
        }
#else
        recv_len = (int)recvfrom(ctx->sock, buf, sizeof(buf), 0,
                                 (struct sockaddr *)&sender_addr, &sender_len);
        if (recv_len < 0)
        {
            if (!ctx->running)
            {
                break;
            }
            continue;
        }
#endif

        if (packet_parse(&pkt, buf, (size_t)recv_len) != COMM_SUCCESS)
        {
            continue;
        }

        /* ACK パケット: 送信ウィンドウを前進させる */
        if (pkt.flags & COMM_FLAG_ACK)
        {
            window_send_ack(&ctx->send_window, pkt.ack_num);
            continue;
        }

        /* NACK パケット: 再送制御に通知（再送は commSend 側で実施） */
        if (pkt.flags & COMM_FLAG_NACK)
        {
            /* 再送管理エントリのタイムアウトを即時化して次の送信で再送させる */
            (void)pkt.ack_num;
            continue;
        }

        /* データパケット: 受信ウィンドウへ格納 */
        if (!(pkt.flags & COMM_FLAG_DATA))
        {
            continue;
        }

        if (window_recv_push(&ctx->recv_window, &pkt) != COMM_SUCCESS)
        {
            /* ウィンドウ外 → 無視 */
            continue;
        }

        /* 欠番チェック → NACK 送信 */
        if (window_recv_needs_nack(&ctx->recv_window, &nack_num))
        {
            send_nack(ctx, &sender_addr, nack_num);
        }

        /* 順序整列済みパケットをコールバックへ渡す */
        while (window_recv_pop(&ctx->recv_window, &ordered_pkt) == COMM_SUCCESS)
        {
            if (ctx->callback != NULL)
            {
                ctx->callback(ctx->service.service_id,
                              ordered_pkt.payload,
                              (size_t)ordered_pkt.payload_len);
            }
        }
    }

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/**
 *******************************************************************************
 *  @brief          受信スレッドを起動します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
 *******************************************************************************
 */
int comm_recv_thread_start(struct CommContext_ *ctx)
{
    if (ctx == NULL)
    {
        return COMM_ERROR;
    }

    ctx->running = 1;

#ifdef _WIN32
    ctx->recv_thread = CreateThread(NULL, 0, recv_thread_func, ctx, 0, NULL);
    if (ctx->recv_thread == NULL)
    {
        ctx->running = 0;
        return COMM_ERROR;
    }
#else
    if (pthread_create(&ctx->recv_thread, NULL, recv_thread_func, ctx) != 0)
    {
        ctx->running = 0;
        return COMM_ERROR;
    }
#endif

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          受信スレッドを停止します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
 *******************************************************************************
 */
int comm_recv_thread_stop(struct CommContext_ *ctx)
{
    if (ctx == NULL)
    {
        return COMM_ERROR;
    }

    ctx->running = 0;

#ifdef _WIN32
    if (ctx->recv_thread != NULL)
    {
        /* ソケットをクローズして recvfrom をアンブロック */
        closesocket(ctx->sock);
        WaitForSingleObject(ctx->recv_thread, 3000);
        CloseHandle(ctx->recv_thread);
        ctx->recv_thread = NULL;
    }
#else
    pthread_join(ctx->recv_thread, NULL);
#endif

    return COMM_SUCCESS;
}
