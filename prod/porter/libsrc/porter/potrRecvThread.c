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
    #include <time.h>
    #include <errno.h>
#endif

#include <porter_const.h>

#include "protocol/packet.h"
#include "protocol/window.h"
#include "potrContext.h"
#include "potrRecvThread.h"
#include "compress/compress.h"

/* 前方宣言: recv_deliver は後で定義 */
static void recv_deliver(struct PotrContext_ *ctx, const uint8_t *payload,
                         size_t payload_len, int compressed);

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

/* 現在の CLOCK_MONOTONIC 時刻を取得する */
static void get_monotonic(int64_t *tv_sec, int32_t *tv_nsec)
{
#ifdef _WIN32
    ULONGLONG ms = GetTickCount64();
    *tv_sec  = (int64_t)(ms / 1000ULL);
    *tv_nsec = (int32_t)((ms % 1000ULL) * 1000000UL);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    *tv_sec  = (int64_t)ts.tv_sec;
    *tv_nsec = (int32_t)ts.tv_nsec;
#endif
}

/* 最終受信時刻のみ更新する (ヘルスチェック生存判定用)。 */
static void update_last_recv_time(struct PotrContext_ *ctx)
{
    get_monotonic(&ctx->last_recv_tv_sec, &ctx->last_recv_tv_nsec);
}

/* health_alive が dead → alive になった場合に CONNECTED イベントを発火する。 */
static void notify_health_alive(struct PotrContext_ *ctx)
{
    if (ctx->global.health_timeout_ms == 0)
    {
        return; /* ヘルスチェック無効 */
    }

    if (!ctx->health_alive)
    {
        ctx->health_alive = 1;
        if (ctx->callback != NULL)
        {
            ctx->callback(ctx->service.service_id,
                          POTR_EVENT_CONNECTED, NULL, 0);
        }
    }
}

/* recvfrom タイムアウト時に経過時間を確認し、必要なら DISCONNECTED イベントを発火する */
static void check_health_timeout(struct PotrContext_ *ctx)
{
    int64_t now_sec;
    int32_t now_nsec;
    int64_t elapsed_ms;

    if (ctx->global.health_timeout_ms == 0 || !ctx->health_alive)
    {
        return; /* ヘルスチェック無効、または既に dead */
    }

    if (ctx->last_recv_tv_sec == 0)
    {
        return; /* 一度も受信していない */
    }

    get_monotonic(&now_sec, &now_nsec);

    elapsed_ms = (now_sec - ctx->last_recv_tv_sec) * 1000LL
                 + (now_nsec - ctx->last_recv_tv_nsec) / 1000000L;

    if (elapsed_ms >= (int64_t)ctx->global.health_timeout_ms)
    {
        ctx->health_alive = 0;
        if (ctx->callback != NULL)
        {
            ctx->callback(ctx->service.service_id,
                          POTR_EVENT_DISCONNECTED, NULL, 0);
        }
    }
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

/* REJECT パケットを指定アドレスへ送信する (送信者が使用)。
   NACK を受信したが send_window に対象パケットが存在しない場合に呼び出す。 */
static void send_reject(struct PotrContext_      *ctx,
                        const struct sockaddr_in *dest_addr,
                        uint32_t                  seq_num)
{
    PotrPacket           reject_pkt;
    PotrPacketSessionHdr shdr;
    size_t               wire_len;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    if (packet_build_reject(&reject_pkt, &shdr, seq_num) != POTR_SUCCESS)
    {
        return;
    }

    wire_len = packet_wire_size(&reject_pkt);

#ifdef _WIN32
    sendto(ctx->sock, (const char *)&reject_pkt, (int)wire_len, 0,
           (const struct sockaddr *)dest_addr, sizeof(*dest_addr));
#else
    sendto(ctx->sock, &reject_pkt, wire_len, 0,
           (const struct sockaddr *)dest_addr, sizeof(*dest_addr));
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
                          POTR_EVENT_DATA,
                          ctx->compress_buf,
                          dec_len);
        }
        /* 解凍失敗時はパケットを破棄する */
    }
    else
    {
        ctx->callback(ctx->service.service_id, POTR_EVENT_DATA,
                      payload, payload_len);
    }
}

/* サブパケット 1 件のフラグメント結合・解凍・コールバック処理。
   window_recv_pop で取り出した外側パケットを packet_unpack_next で展開した
   各サブパケットに対して呼び出す。 */
static void deliver_sub_pkt(struct PotrContext_ *ctx, const PotrPacket *sub_pkt)
{
    if (sub_pkt->flags & POTR_FLAG_MORE_FRAG)
    {
        /* 中間フラグメント: バッファに追記 */
        if (ctx->frag_buf_len + sub_pkt->payload_len <= POTR_MAX_MESSAGE_SIZE)
        {
            if (ctx->frag_buf_len == 0)
            {
                ctx->frag_compressed =
                    (sub_pkt->flags & POTR_FLAG_COMPRESSED) ? 1 : 0;
            }
            memcpy(ctx->frag_buf + ctx->frag_buf_len,
                   sub_pkt->payload, sub_pkt->payload_len);
            ctx->frag_buf_len += sub_pkt->payload_len;
        }
        else
        {
            ctx->frag_buf_len    = 0;
            ctx->frag_compressed = 0;
        }
    }
    else if (ctx->frag_buf_len > 0)
    {
        /* 最終フラグメント: バッファに追記してコールバック */
        if (ctx->frag_buf_len + sub_pkt->payload_len <= POTR_MAX_MESSAGE_SIZE)
        {
            memcpy(ctx->frag_buf + ctx->frag_buf_len,
                   sub_pkt->payload, sub_pkt->payload_len);
            ctx->frag_buf_len += sub_pkt->payload_len;

            if (ctx->callback != NULL)
            {
                recv_deliver(ctx, ctx->frag_buf,
                             ctx->frag_buf_len, ctx->frag_compressed);
            }
        }
        ctx->frag_buf_len    = 0;
        ctx->frag_compressed = 0;
    }
    else
    {
        /* 単体パケット: 直接コールバック */
        if (ctx->callback != NULL)
        {
            recv_deliver(ctx,
                         sub_pkt->payload,
                         (size_t)sub_pkt->payload_len,
                         (sub_pkt->flags & POTR_FLAG_COMPRESSED) ? 1 : 0);
        }
    }
}

/* recv_window から順序整列済みの外側パケットを取り出してサブパケットを配信する。
   REJECT 処理後と通常受信処理の両方から呼び出す。 */
static void drain_recv_window(struct PotrContext_ *ctx)
{
    PotrPacket pop_pkt;

    while (window_recv_pop(&ctx->recv_window, &pop_pkt) == POTR_SUCCESS)
    {
        notify_health_alive(ctx);

        if (pop_pkt.flags & POTR_FLAG_PING)
        {
            continue; /* PING: 生存確認のみ、サブパケット展開不要 */
        }

        /* DATA: サブパケットを順に展開して配信 */
        {
            size_t     offset = 0;
            PotrPacket sub_pkt;

            while (packet_unpack_next(&pop_pkt, &offset, &sub_pkt) == POTR_SUCCESS)
            {
                deliver_sub_pkt(ctx, &sub_pkt);
            }
        }
    }
}

/* 外側パケット (DATA / PING) を受信ウィンドウに投入し、順序整列済みパケットを配信する。
   再送・順序整列の単位は外側パケットであり、NACK も外側パケットの seq_num を指定する。 */
static void process_outer_pkt(struct PotrContext_      *ctx,
                               const PotrPacket         *pkt,
                               const struct sockaddr_in *sender_addr)
{
    uint32_t nack_num;

    if (window_recv_push(&ctx->recv_window, pkt) != POTR_SUCCESS)
    {
        return;
    }

    if (window_recv_needs_nack(&ctx->recv_window, &nack_num))
    {
        send_nack(ctx, sender_addr, nack_num);
    }

    drain_recv_window(ctx);
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
    struct sockaddr_in  sender_addr;

#ifdef _WIN32
    int sender_len = sizeof(sender_addr);
#else
    socklen_t sender_len = sizeof(sender_addr);
#endif

    /* SO_RCVTIMEO を設定してタイムアウトポーリングを有効にする。
       受信者: health_timeout_ms / 3 (最小 100ms) または 500ms。
       送信者: 500ms (スレッド停止検出のみ)。 */
    {
        uint32_t poll_ms;

        if (ctx->role == POTR_ROLE_RECEIVER && ctx->global.health_timeout_ms > 0U)
        {
            poll_ms = ctx->global.health_timeout_ms / 3U;
            if (poll_ms < 100U) poll_ms = 100U;
        }
        else
        {
            poll_ms = 500U;
        }

#ifdef _WIN32
        {
            DWORD tv_ms = (DWORD)poll_ms;
            setsockopt(ctx->sock, SOL_SOCKET, SO_RCVTIMEO,
                       (const char *)&tv_ms, sizeof(tv_ms));
        }
#else
        {
            struct timeval tv;
            tv.tv_sec  = (time_t)(poll_ms / 1000U);
            tv.tv_usec = (suseconds_t)((poll_ms % 1000U) * 1000U);
            setsockopt(ctx->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        }
#endif
    }

    while (ctx->running)
    {
        int recv_len;

        memset(&sender_addr, 0, sizeof(sender_addr));
        sender_len = sizeof(sender_addr);

#ifdef _WIN32
        recv_len = recvfrom(ctx->sock, (char *)buf, (int)sizeof(buf), 0,
                            (struct sockaddr *)&sender_addr, &sender_len);
        if (recv_len == SOCKET_ERROR)
        {
            if (!ctx->running) break;
            if (WSAGetLastError() == WSAETIMEDOUT)
            {
                if (ctx->role == POTR_ROLE_RECEIVER)
                    check_health_timeout(ctx);
            }
            continue;
        }
#else
        recv_len = (int)recvfrom(ctx->sock, buf, sizeof(buf), 0,
                                 (struct sockaddr *)&sender_addr, &sender_len);
        if (recv_len < 0)
        {
            if (!ctx->running) break;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                if (ctx->role == POTR_ROLE_RECEIVER)
                    check_health_timeout(ctx);
            }
            continue;
        }
#endif

        if (packet_parse(&pkt, buf, (size_t)recv_len) != POTR_SUCCESS)
        {
            continue;
        }

        /* service_id 照合 */
        if (pkt.service_id != ctx->service.service_id)
        {
            continue;
        }

        /* 送信元 IP フィルタ */
        if (!check_src_addr(ctx, &sender_addr))
        {
            continue;
        }

        /* ── 送信者ロール: NACK のみ処理 ── */
        if (ctx->role == POTR_ROLE_SENDER)
        {
            if (pkt.flags & POTR_FLAG_NACK)
            {
                PotrPacket resend_pkt;
                if (window_send_get(&ctx->send_window, pkt.ack_num, &resend_pkt)
                    == POTR_SUCCESS)
                {
                    /* send_window に存在: 外側パケットをそのまま再送 */
                    size_t wire_len = packet_wire_size(&resend_pkt);
#ifdef _WIN32
                    sendto(ctx->sock, (const char *)&resend_pkt,
                           (int)wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr,
                           sizeof(ctx->dest_addr));
#else
                    sendto(ctx->sock, &resend_pkt, wire_len, 0,
                           (const struct sockaddr *)&ctx->dest_addr,
                           sizeof(ctx->dest_addr));
#endif
                }
                else
                {
                    /* send_window から evict 済み: REJECT を返す */
                    send_reject(ctx, &sender_addr, pkt.ack_num);
                }
            }
            /* ACK・その他 (DATA/PING の反射など) は無視 */
            continue;
        }

        /* ── 受信者ロール: PING / DATA / REJECT / FIN を処理 ── */

        /* FIN: 送信者からの正常終了通知 → 即座に DISCONNECTED へ遷移 */
        if (pkt.flags & POTR_FLAG_FIN)
        {
            if (!check_and_update_session(ctx, &pkt))
            {
                continue; /* 旧セッションの FIN → 無視 */
            }

            /* ヘルスチェック有効時: health_alive で重複発火を防止する */
            if (ctx->health_alive)
            {
                ctx->health_alive = 0;
                if (ctx->callback != NULL)
                {
                    ctx->callback(ctx->service.service_id,
                                  POTR_EVENT_DISCONNECTED, NULL, 0);
                }
            }
            else if (ctx->global.health_timeout_ms == 0 && ctx->peer_session_known)
            {
                /* ヘルスチェック無効時: peer_session_known で重複発火を防止する */
                if (ctx->callback != NULL)
                {
                    ctx->callback(ctx->service.service_id,
                                  POTR_EVENT_DISCONNECTED, NULL, 0);
                }
            }

            /* セッション状態をリセットして次の接続を受け入れ可能にする */
            ctx->peer_session_known = 0;
            ctx->last_recv_tv_sec   = 0;
            continue;
        }

        /* REJECT: 欠落外側パケットをスキップして後続パケットを配信する */
        if (pkt.flags & POTR_FLAG_REJECT)
        {
            if (!check_and_update_session(ctx, &pkt))
            {
                continue;
            }

            /* DISCONNECTED イベント発火 (alive のときのみ。連続 REJECT で重複しない) */
            if (ctx->health_alive)
            {
                ctx->health_alive = 0;
                if (ctx->callback != NULL)
                {
                    ctx->callback(ctx->service.service_id,
                                  POTR_EVENT_DISCONNECTED, NULL, 0);
                }
            }

            /* 欠落外側パケットをスキップして recv_window を前進させる */
            window_recv_skip(&ctx->recv_window, pkt.ack_num);

            /* 後続パケットを配信 (最初の pop で CONNECTED を発火) */
            drain_recv_window(ctx);
            continue;
        }

        /* NACK / ACK は受信者では無視 */
        if (!(pkt.flags & (POTR_FLAG_DATA | POTR_FLAG_PING)))
        {
            continue;
        }

        /* セッション照合 */
        if (!check_and_update_session(ctx, &pkt))
        {
            continue;
        }

        /* 受信時刻を更新 (健全性タイムアウト計算用) */
        update_last_recv_time(ctx);

        /* 外側パケットを受信ウィンドウに投入し、順序整列済みパケットを配信する */
        process_outer_pkt(ctx, &pkt, &sender_addr);
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
        ctx->sock = POTR_INVALID_SOCKET;
        WaitForSingleObject(ctx->recv_thread, 3000);
        CloseHandle(ctx->recv_thread);
        ctx->recv_thread = NULL;
    }
#else
    /* shutdown (SHUT_RD) でブロック中の recvfrom を即時アンブロックする。
       close と異なり FD を維持するため FD 再利用リスクがなく、
       sendto (NACK 再送) も引き続き有効。ソケットの close は呼び出し元で行う。 */
    shutdown(ctx->sock, SHUT_RD);
    pthread_join(ctx->recv_thread, NULL);
#endif

    return POTR_SUCCESS;
}
