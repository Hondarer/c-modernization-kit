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
    #include <sys/select.h>
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

/* 送信元 IP が src_addr_resolved のいずれかと一致するか確認する。
   src_addr が未設定の場合は常に 1 (合格) を返す。 */
static int check_src_addr(const struct PotrContext_ *ctx,
                           const struct sockaddr_in  *sender)
{
    int i;
    if (ctx->service.src_addr[0][0] == '\0')
    {
        return 1;
    }
    for (i = 0; i < ctx->n_path; i++)
    {
        if (sender->sin_addr.s_addr == ctx->src_addr_resolved[i].s_addr)
        {
            return 1;
        }
    }
    return 0;
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

/** NACK 重複抑制の時間窓 (ミリ秒)。この時間内の同一 ack_num の NACK は破棄する。 */
#define POTR_NACK_DEDUP_MS 200U

/* 現在時刻をミリ秒単位で返す (単調増加クロック) */
static uint64_t get_ms_mono(void)
{
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
#endif
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

/* パスごとの最終受信時刻と peer_port を更新する */
static void update_path_recv(struct PotrContext_      *ctx,
                              int                       path_idx,
                              const struct sockaddr_in *sender)
{
    get_monotonic(&ctx->last_recv_tv_sec, &ctx->last_recv_tv_nsec);
    get_monotonic(&ctx->path_last_recv_sec[path_idx],
                  &ctx->path_last_recv_nsec[path_idx]);
    ctx->peer_port[path_idx] = sender->sin_port; /* NBO のまま格納 */
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

/* タイムアウト時に経過時間を確認し、必要なら peer_port クリアと DISCONNECTED イベントを発火する */
static void check_health_timeout(struct PotrContext_ *ctx)
{
    int64_t now_sec;
    int32_t now_nsec;
    int     i;

    if (ctx->global.health_timeout_ms == 0) return;

    get_monotonic(&now_sec, &now_nsec);

    /* パスごとのタイムアウト: peer_port をクリア */
    for (i = 0; i < ctx->n_path; i++)
    {
        int64_t elapsed_ms;

        if (ctx->path_last_recv_sec[i] == 0) continue;

        elapsed_ms = (now_sec  - ctx->path_last_recv_sec[i])  * 1000LL
                   + (now_nsec - ctx->path_last_recv_nsec[i]) / 1000000L;

        if (elapsed_ms >= (int64_t)ctx->global.health_timeout_ms)
        {
            ctx->peer_port[i]          = 0;
            ctx->path_last_recv_sec[i] = 0;
        }
    }

    /* 全体の health_alive 判定 */
    if (!ctx->health_alive || ctx->last_recv_tv_sec == 0) return;

    {
        int64_t elapsed_ms;
        elapsed_ms = (now_sec  - ctx->last_recv_tv_sec)  * 1000LL
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
}

/** 受信バッファサイズ。ヘッダー + 最大ペイロード分を確保する。 */
#define RECV_BUF_SIZE (sizeof(PotrPacket))

/* NACK パケットを全パスへ送信する */
static void send_nack(struct PotrContext_ *ctx, uint32_t nack_seq)
{
    PotrPacket           nack_pkt;
    PotrPacketSessionHdr shdr;
    size_t               wire_len;
    int                  i;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    if (packet_build_nack(&nack_pkt, &shdr, nack_seq) != POTR_SUCCESS) return;

    wire_len = packet_wire_size(&nack_pkt);

    for (i = 0; i < ctx->n_path; i++)
    {
        struct sockaddr_in dest;
        uint16_t           port;

        if (ctx->service.src_port != 0)
        {
            port = htons(ctx->service.src_port);
        }
        else
        {
            port = ctx->peer_port[i]; /* NBO */
        }

        if (port == 0) continue; /* ポート未観測のパスは送れない */

        memset(&dest, 0, sizeof(dest));
        dest.sin_family = AF_INET;
        dest.sin_addr   = ctx->src_addr_resolved[i];
        dest.sin_port   = port;

#ifdef _WIN32
        sendto(ctx->sock[i], (const char *)&nack_pkt, (int)wire_len, 0,
               (const struct sockaddr *)&dest, sizeof(dest));
#else
        sendto(ctx->sock[i], &nack_pkt, wire_len, 0,
               (const struct sockaddr *)&dest, sizeof(dest));
#endif
    }
}

/* REJECT パケットを全パスへ送信する */
static void send_reject(struct PotrContext_ *ctx, uint32_t seq_num)
{
    PotrPacket           reject_pkt;
    PotrPacketSessionHdr shdr;
    size_t               wire_len;
    int                  i;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    if (packet_build_reject(&reject_pkt, &shdr, seq_num) != POTR_SUCCESS) return;

    wire_len = packet_wire_size(&reject_pkt);

    for (i = 0; i < ctx->n_path; i++)
    {
#ifdef _WIN32
        sendto(ctx->sock[i], (const char *)&reject_pkt, (int)wire_len, 0,
               (const struct sockaddr *)&ctx->dest_addr[i],
               sizeof(ctx->dest_addr[i]));
#else
        sendto(ctx->sock[i], &reject_pkt, wire_len, 0,
               (const struct sockaddr *)&ctx->dest_addr[i],
               sizeof(ctx->dest_addr[i]));
#endif
    }
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

/* ペイロードエレメント 1 件のフラグメント結合・解凍・コールバック処理。
   window_recv_pop で取り出した外側パケットを packet_unpack_next で展開した
   各ペイロードエレメントに対して呼び出す。 */
static void deliver_payload_elem(struct PotrContext_ *ctx, const PotrPacket *elem)
{
    if (elem->flags & POTR_FLAG_MORE_FRAG)
    {
        /* 中間フラグメント: バッファに追記 */
        if (ctx->frag_buf_len + elem->payload_len <= POTR_MAX_MESSAGE_SIZE)
        {
            if (ctx->frag_buf_len == 0)
            {
                ctx->frag_compressed =
                    (elem->flags & POTR_FLAG_COMPRESSED) ? 1 : 0;
            }
            memcpy(ctx->frag_buf + ctx->frag_buf_len,
                   elem->payload, elem->payload_len);
            ctx->frag_buf_len += elem->payload_len;
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
        if (ctx->frag_buf_len + elem->payload_len <= POTR_MAX_MESSAGE_SIZE)
        {
            memcpy(ctx->frag_buf + ctx->frag_buf_len,
                   elem->payload, elem->payload_len);
            ctx->frag_buf_len += elem->payload_len;

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
        /* フラグメントなし: 直接コールバック */
        if (ctx->callback != NULL)
        {
            recv_deliver(ctx,
                         elem->payload,
                         (size_t)elem->payload_len,
                         (elem->flags & POTR_FLAG_COMPRESSED) ? 1 : 0);
        }
    }
}

/* recv_window から順序整列済みの外側パケットを取り出してペイロードエレメントを配信する。
   REJECT 処理後と通常受信処理の両方から呼び出す。 */
static void drain_recv_window(struct PotrContext_ *ctx)
{
    PotrPacket pop_pkt;

    while (window_recv_pop(&ctx->recv_window, &pop_pkt) == POTR_SUCCESS)
    {
        notify_health_alive(ctx);

        if (pop_pkt.flags & POTR_FLAG_PING)
        {
            continue; /* PING: 生存確認のみ、ペイロードエレメント展開不要 */
        }

        /* DATA: ペイロードエレメントを順に展開して配信 */
        {
            size_t     offset = 0;
            PotrPacket elem;

            while (packet_unpack_next(&pop_pkt, &offset, &elem) == POTR_SUCCESS)
            {
                deliver_payload_elem(ctx, &elem);
            }
        }
    }
}

/* 外側パケット (DATA / PING) を受信ウィンドウに投入し、順序整列済みパケットを配信する。
   再送・順序整列の単位は外側パケットであり、NACK も外側パケットの seq_num を指定する。 */
static void process_outer_pkt(struct PotrContext_ *ctx,
                               const PotrPacket    *pkt)
{
    uint32_t nack_num;

    if (window_recv_push(&ctx->recv_window, pkt) != POTR_SUCCESS)
    {
        return;
    }

    if (window_recv_needs_nack(&ctx->recv_window, &nack_num))
    {
        send_nack(ctx, nack_num);
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
    uint32_t            poll_ms;

    if (ctx->role == POTR_ROLE_RECEIVER && ctx->global.health_timeout_ms > 0U)
    {
        poll_ms = ctx->global.health_timeout_ms / 3U;
        if (poll_ms < 100U) poll_ms = 100U;
    }
    else
    {
        poll_ms = 500U;
    }

    while (ctx->running)
    {
        fd_set         readfds;
        struct timeval tv;
        int            ret;
        int            maxfd = -1;
        int            i;

        FD_ZERO(&readfds);
        for (i = 0; i < ctx->n_path; i++)
        {
            if (ctx->sock[i] == POTR_INVALID_SOCKET) continue;
#ifdef _WIN32
            FD_SET(ctx->sock[i], &readfds);
            /* Windows では SOCKET は UINT_PTR。maxfd の代わりに n_path を使う */
#else
            FD_SET(ctx->sock[i], &readfds);
            if (ctx->sock[i] > maxfd) maxfd = ctx->sock[i];
#endif
        }

#ifdef _WIN32
        /* Windows: select の第1引数は無視されるが 0 でなく n_path を渡す */
        maxfd = ctx->n_path;
#endif

        if (maxfd < 0) break;

        tv.tv_sec  = (long)(poll_ms / 1000U);
        tv.tv_usec = (long)((poll_ms % 1000U) * 1000U);

        ret = select(maxfd + 1, &readfds, NULL, NULL, &tv);

        if (ret < 0)
        {
            if (!ctx->running) break;
            continue;
        }

        if (ret == 0)
        {
            if (ctx->role == POTR_ROLE_RECEIVER)
                check_health_timeout(ctx);
            continue;
        }

        for (i = 0; i < ctx->n_path; i++)
        {
            int recv_len;
#ifdef _WIN32
            int sender_len;
#else
            socklen_t sender_len;
#endif

            if (ctx->sock[i] == POTR_INVALID_SOCKET) continue;
            if (!FD_ISSET(ctx->sock[i], &readfds)) continue;

            memset(&sender_addr, 0, sizeof(sender_addr));
            sender_len = sizeof(sender_addr);

#ifdef _WIN32
            recv_len = recvfrom(ctx->sock[i], (char *)buf, (int)sizeof(buf), 0,
                                (struct sockaddr *)&sender_addr, &sender_len);
            if (recv_len == SOCKET_ERROR) continue;
#else
            recv_len = (int)recvfrom(ctx->sock[i], buf, sizeof(buf), 0,
                                     (struct sockaddr *)&sender_addr,
                                     &sender_len);
            if (recv_len < 0) continue;
#endif

            if (packet_parse(&pkt, buf, (size_t)recv_len) != POTR_SUCCESS) continue;
            if (pkt.service_id != ctx->service.service_id) continue;
            if (!check_src_addr(ctx, &sender_addr)) continue;

            /* ── 送信者ロール: NACK のみ処理 ── */
            if (ctx->role == POTR_ROLE_SENDER)
            {
                if (pkt.flags & POTR_FLAG_NACK)
                {
                    int j;

                    /* 同一 ack_num の NACK が POTR_NACK_DEDUP_MS 以内に届いた場合は破棄 */
                    {
                        uint64_t now_ms = get_ms_mono();
                        if (ctx->last_nack_time_ms != 0
                            && pkt.ack_num == ctx->last_nack_ack_num
                            && (now_ms - ctx->last_nack_time_ms)
                               < (uint64_t)POTR_NACK_DEDUP_MS)
                        {
                            continue;
                        }
                        ctx->last_nack_ack_num = pkt.ack_num;
                        ctx->last_nack_time_ms = now_ms;
                    }

                    {
                        PotrPacket resend_pkt;
                        if (window_send_get(&ctx->send_window, ctx->last_nack_ack_num,
                                            &resend_pkt) == POTR_SUCCESS)
                        {
                            size_t wire_len = packet_wire_size(&resend_pkt);
                            for (j = 0; j < ctx->n_path; j++)
                            {
#ifdef _WIN32
                                sendto(ctx->sock[j], (const char *)&resend_pkt,
                                       (int)wire_len, 0,
                                       (const struct sockaddr *)&ctx->dest_addr[j],
                                       sizeof(ctx->dest_addr[j]));
#else
                                sendto(ctx->sock[j], &resend_pkt, wire_len, 0,
                                       (const struct sockaddr *)&ctx->dest_addr[j],
                                       sizeof(ctx->dest_addr[j]));
#endif
                            }
                        }
                        else
                        {
                            send_reject(ctx, ctx->last_nack_ack_num);
                        }
                    }
                }
                /* ACK・その他 (DATA/PING の反射など) は無視 */
                continue;
            }

            /* ── 受信者ロール: FIN / REJECT / DATA / PING を処理 ── */

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

            /* 受信時刻とパスポートを更新 (健全性タイムアウト計算用) */
            update_path_recv(ctx, i, &sender_addr);

            /* 外側パケットを受信ウィンドウに投入し、順序整列済みパケットを配信する */
            process_outer_pkt(ctx, &pkt);
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
    {
        int i;
        for (i = 0; i < ctx->n_path; i++)
        {
            if (ctx->sock[i] != POTR_INVALID_SOCKET)
            {
                closesocket(ctx->sock[i]);
                ctx->sock[i] = POTR_INVALID_SOCKET;
            }
        }
        if (ctx->recv_thread != NULL)
        {
            WaitForSingleObject(ctx->recv_thread, 3000);
            CloseHandle(ctx->recv_thread);
            ctx->recv_thread = NULL;
        }
    }
#else
    {
        int i;
        for (i = 0; i < ctx->n_path; i++)
        {
            if (ctx->sock[i] != POTR_INVALID_SOCKET)
            {
                shutdown(ctx->sock[i], SHUT_RD);
            }
        }
        pthread_join(ctx->recv_thread, NULL);
    }
#endif

    return POTR_SUCCESS;
}
