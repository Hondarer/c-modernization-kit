/**
 *******************************************************************************
 *  @file           potrRecvThread.c
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

#include <porter_const.h>

#include "protocol/packet.h"
#include "protocol/window.h"
#include "potrContext.h"
#include "potrRecvThread.h"
#include "compress/compress.h"

/* 送信元 IP が src_addr_resolved と一致するか確認する。
   src_addr が未設定の場合は常に 1 (合格) を返す。 */
static int check_src_addr(const struct PotrContext_ *ctx,
                           const struct sockaddr_in  *sender)
{
    if (ctx->service.src_addr[0] == '\0')
    {
        return 1; /* src_addr 未設定: フィルタなし */
    }
    return (sender->sin_addr.s_addr == ctx->src_addr_resolved.s_addr) ? 1 : 0;
}

/* セッションの採用判定を行い、必要であればコンテキストの相手セッション情報を更新する。
   採用すべきセッションなら 1、破棄すべき旧セッションなら 0 を返す。 */
static int check_and_update_session(struct PotrContext_ *ctx,
                                    const PotrPacket    *pkt)
{
    if (!ctx->peer_session_known)
    {
        /* 初回受信: そのまま採用 */
        ctx->peer_session_id      = pkt->session_id;
        ctx->peer_session_tv_sec  = pkt->session_tv_sec;
        ctx->peer_session_tv_nsec = pkt->session_tv_nsec;
        ctx->peer_session_known   = 1;
        return 1;
    }

    /* start_time が大きい → 新セッション */
    if (pkt->session_tv_sec > ctx->peer_session_tv_sec)
    {
        goto adopt;
    }
    if (pkt->session_tv_sec < ctx->peer_session_tv_sec)
    {
        return 0; /* 旧セッション */
    }

    /* start_time が等しい場合は session_id で判定 (タイブレーク) */
    if (pkt->session_tv_nsec > ctx->peer_session_tv_nsec)
    {
        goto adopt;
    }
    if (pkt->session_tv_nsec < ctx->peer_session_tv_nsec)
    {
        return 0;
    }

    if (pkt->session_id > ctx->peer_session_id)
    {
        goto adopt;
    }

    /* 既知のセッションと一致するか確認 */
    return (pkt->session_id == ctx->peer_session_id) ? 1 : 0;

adopt:
    ctx->peer_session_id      = pkt->session_id;
    ctx->peer_session_tv_sec  = pkt->session_tv_sec;
    ctx->peer_session_tv_nsec = pkt->session_tv_nsec;
    /* 新セッション採用時はウィンドウをリセットする */
    window_init(&ctx->recv_window, 0, ctx->global.window_size);
    return 1;
}

/** 受信バッファサイズ。ヘッダー + 最大ペイロード分を確保する。 */
#define RECV_BUF_SIZE (sizeof(PotrPacket))

/* NACK パケットを送信元へユニキャスト送信する */
static void send_nack(struct PotrContext_ *ctx,
                      const struct sockaddr_in *sender_addr,
                      uint32_t nack_seq)
{
    PotrPacket           nack_pkt;
    PotrPacketSessionHdr shdr;
    size_t               wire_len;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    if (packet_build_nack(&nack_pkt, &shdr, nack_seq) != POTR_SUCCESS)
    {
        return;
    }

    wire_len = packet_wire_size(&nack_pkt);

#ifdef _WIN32
    sendto(ctx->sock, (const char *)&nack_pkt, (int)wire_len, 0,
           (const struct sockaddr *)sender_addr, sizeof(*sender_addr));
#else
    sendto(ctx->sock, &nack_pkt, wire_len, 0,
           (const struct sockaddr *)sender_addr, sizeof(*sender_addr));
#endif
}

/* 受信データを解凍してコールバックに渡す */
static void recv_deliver(struct PotrContext_ *ctx,
                         const uint8_t      *payload,
                         size_t              payload_len,
                         int                 compressed)
{
    if (compressed)
    {
        size_t dec_len = sizeof(ctx->compress_buf);

        if (potr_decompress(ctx->compress_buf, &dec_len,
                            payload, payload_len) == 0)
        {
            ctx->callback(ctx->service.service_id,
                          ctx->compress_buf,
                          dec_len);
        }
        /* 解凍失敗時はパケットを破棄する */
    }
    else
    {
        ctx->callback(ctx->service.service_id, payload, payload_len);
    }
}

/* 受信スレッド本体 */
#ifdef _WIN32
static DWORD WINAPI recv_thread_func(LPVOID arg)
#else
static void *recv_thread_func(void *arg)
#endif
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)arg;
    uint8_t             buf[RECV_BUF_SIZE];
    PotrPacket          pkt;
    PotrPacket          ordered_pkt;
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

        if (packet_parse(&pkt, buf, (size_t)recv_len) != POTR_SUCCESS)
        {
            continue;
        }

        /* service_id 照合: 不一致は破棄 */
        if (pkt.service_id != ctx->service.service_id)
        {
            continue;
        }

        /* unicast: 送信元 IP フィルタ */
        if (!check_src_addr(ctx, &sender_addr))
        {
            continue;
        }

        /* ACK / NACK はセッション照合不要 (service_id のみで十分) */

        /* ACK パケット: 送信ウィンドウを前進させる */
        if (pkt.flags & POTR_FLAG_ACK)
        {
            window_send_ack(&ctx->send_window, pkt.ack_num);
            continue;
        }

        /* NACK パケット: 再送制御に通知（再送は potrSend 側で実施） */
        if (pkt.flags & POTR_FLAG_NACK)
        {
            /* 再送管理エントリのタイムアウトを即時化して次の送信で再送させる */
            (void)pkt.ack_num;
            continue;
        }

        /* データパケット: 受信ウィンドウへ格納 */
        if (!(pkt.flags & POTR_FLAG_DATA))
        {
            continue;
        }

        /* セッション照合: 旧セッションのパケットは破棄 */
        if (!check_and_update_session(ctx, &pkt))
        {
            continue;
        }

        if (window_recv_push(&ctx->recv_window, &pkt) != POTR_SUCCESS)
        {
            /* ウィンドウ外 → 無視 */
            continue;
        }

        /* 欠番チェック → NACK 送信 */
        if (window_recv_needs_nack(&ctx->recv_window, &nack_num))
        {
            send_nack(ctx, &sender_addr, nack_num);
        }

        /* 順序整列済みパケットをコールバックへ渡す（フラグメント結合・解凍対応） */
        while (window_recv_pop(&ctx->recv_window, &ordered_pkt) == POTR_SUCCESS)
        {
            if (ordered_pkt.flags & POTR_FLAG_MORE_FRAG)
            {
                /* 後続フラグメントあり: バッファに蓄積する */
                if (ctx->frag_buf_len + ordered_pkt.payload_len
                    <= POTR_MAX_MESSAGE_SIZE)
                {
                    /* 最初のフラグメントで圧縮フラグを記録する */
                    if (ctx->frag_buf_len == 0)
                    {
                        ctx->frag_compressed =
                            (ordered_pkt.flags & POTR_FLAG_COMPRESSED) ? 1 : 0;
                    }
                    memcpy(ctx->frag_buf + ctx->frag_buf_len,
                           ordered_pkt.payload,
                           ordered_pkt.payload_len);
                    ctx->frag_buf_len += ordered_pkt.payload_len;
                }
                else
                {
                    /* バッファオーバーフロー: 受信途中のフラグメントを破棄 */
                    ctx->frag_buf_len    = 0;
                    ctx->frag_compressed = 0;
                }
            }
            else if (ctx->frag_buf_len > 0)
            {
                /* フラグメントの最終パケット: バッファに追記してコールバック */
                if (ctx->frag_buf_len + ordered_pkt.payload_len
                    <= POTR_MAX_MESSAGE_SIZE)
                {
                    memcpy(ctx->frag_buf + ctx->frag_buf_len,
                           ordered_pkt.payload,
                           ordered_pkt.payload_len);
                    ctx->frag_buf_len += ordered_pkt.payload_len;

                    if (ctx->callback != NULL)
                    {
                        recv_deliver(ctx,
                                     ctx->frag_buf,
                                     ctx->frag_buf_len,
                                     ctx->frag_compressed);
                    }
                }
                ctx->frag_buf_len    = 0;
                ctx->frag_compressed = 0;
            }
            else
            {
                /* 非フラグメントパケット */
                if (ctx->callback != NULL)
                {
                    recv_deliver(ctx,
                                 ordered_pkt.payload,
                                 (size_t)ordered_pkt.payload_len,
                                 (ordered_pkt.flags & POTR_FLAG_COMPRESSED) ? 1 : 0);
                }
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
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *******************************************************************************
 */
int comm_recv_thread_start(struct PotrContext_ *ctx)
{
    if (ctx == NULL)
    {
        return POTR_ERROR;
    }

    ctx->running = 1;

#ifdef _WIN32
    ctx->recv_thread = CreateThread(NULL, 0, recv_thread_func, ctx, 0, NULL);
    if (ctx->recv_thread == NULL)
    {
        ctx->running = 0;
        return POTR_ERROR;
    }
#else
    if (pthread_create(&ctx->recv_thread, NULL, recv_thread_func, ctx) != 0)
    {
        ctx->running = 0;
        return POTR_ERROR;
    }
#endif

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          受信スレッドを停止します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *******************************************************************************
 */
int comm_recv_thread_stop(struct PotrContext_ *ctx)
{
    if (ctx == NULL)
    {
        return POTR_ERROR;
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

    return POTR_SUCCESS;
}
