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

#include "../protocol/packet.h"
#include "../protocol/seqnum.h"
#include "../protocol/window.h"
#include "../potrContext.h"
#include "potrRecvThread.h"
#include "../infra/compress/compress.h"
#include "../infra/crypto/crypto.h"
#include "../infra/potrLog.h"

/* 前方宣言: 後で定義される関数 */
static void recv_deliver(struct PotrContext_ *ctx, const uint8_t *payload,
                         size_t payload_len, int compressed);
static void send_nack(struct PotrContext_ *ctx, uint32_t nack_seq);
static void raw_session_disconnect(struct PotrContext_ *ctx);

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
        /* 初回受信 (または FIN/タイムアウト後の再接続): セッション採用 + ウィンドウをリセット。
           pkt->seq_num で初期化することで、送信者の現在位置に直接同期し
           NACK/REJECT サイクルなしに再加入できる。
             DATA 着信時: window_init(DATA.seq_num) → push → pop → 即時 CONNECTED
             PING 着信時: window_init(PING.seq_num) → gap スキャン範囲がゼロ → NACK なし
           FIN/タイムアウト後は送信者が同一セッションのまま任意の seq から
           再開する可能性があるため pkt->seq_num を使用する。 */
        ctx->peer_session_id      = pkt->session_id;
        ctx->peer_session_tv_sec  = pkt->session_tv_sec;
        ctx->peer_session_tv_nsec = pkt->session_tv_nsec;
        ctx->peer_session_known   = 1;
        ctx->reorder_pending      = 0;
        POTR_LOG(POTR_LOG_TRACE,
                 "recv[service_id=%d]: new session (first contact), new_id=%u seq=%u",
                 ctx->service.service_id,
                 pkt->session_id, (unsigned)pkt->seq_num);
        window_init(&ctx->recv_window, pkt->seq_num,
                    ctx->global.window_size, ctx->global.max_payload);
        return 1;
    }

    /* (session_tv_sec, session_tv_nsec, session_id) の辞書順で新旧を判定する。
       - pkt > 既知セッション: 新セッション。return せずにフォールスルーし、
         関数末尾の「新セッション採用」ブロックで採用処理を行う。
       - pkt < 既知セッション: 旧セッション。即 return 0 で破棄する。
       - pkt == 既知セッション: 同一セッション。即 return 1 で通常受信を継続する。
       新セッションと判定された分岐は LOG のみで return しないため、
       if-else チェーンを抜けた後に必ず末尾の採用ブロックに到達する。 */
    if (pkt->session_tv_sec > ctx->peer_session_tv_sec)
    {
        /* 新セッション (tv_sec が大): フォールスルーして採用 */
        POTR_LOG(POTR_LOG_TRACE,
                 "recv[service_id=%d]: new session (tv_sec %lld > %lld)"
                 ", old_id=%u new_id=%u",
                 ctx->service.service_id,
                 (long long)pkt->session_tv_sec, (long long)ctx->peer_session_tv_sec,
                 ctx->peer_session_id, pkt->session_id);
    }
    else if (pkt->session_tv_sec < ctx->peer_session_tv_sec)
    {
        return 0; /* 旧セッション (tv_sec が小): 破棄 */
    }
    else if (pkt->session_tv_nsec > ctx->peer_session_tv_nsec)
    {
        /* 新セッション (tv_sec 同一・tv_nsec が大): フォールスルーして採用 */
        POTR_LOG(POTR_LOG_TRACE,
                 "recv[service_id=%d]: new session (tv_nsec %d > %d)"
                 ", old_id=%u new_id=%u",
                 ctx->service.service_id,
                 pkt->session_tv_nsec, ctx->peer_session_tv_nsec,
                 ctx->peer_session_id, pkt->session_id);
    }
    else if (pkt->session_tv_nsec < ctx->peer_session_tv_nsec)
    {
        return 0; /* 旧セッション (tv_sec 同一・tv_nsec が小): 破棄 */
    }
    else if (pkt->session_id > ctx->peer_session_id)
    {
        /* 新セッション (タイムスタンプ完全一致・session_id が大): フォールスルーして採用 */
        POTR_LOG(POTR_LOG_TRACE,
                 "recv[service_id=%d]: new session (id tiebreak %u > %u)",
                 ctx->service.service_id,
                 pkt->session_id, ctx->peer_session_id);
    }
    else
    {
        /* ここに到達するのは tv_sec == tv_sec かつ tv_nsec == tv_nsec かつ
           session_id <= peer_session_id の場合のみ。
           新セッション分岐はこの else には入らない。 */
        if (pkt->session_id == ctx->peer_session_id)
        {
            return 1; /* 同一セッション: 採用済みのため再初期化不要 */
        }
        else
        {
            return 0; /* 旧セッション (タイムスタンプ完全一致・session_id が小): 破棄 */
        }
    }

    /* 新セッション採用: コンテキストを更新しウィンドウ・リオーダー状態をリセットする。
       最初に受信したパケットの seq_num で初期化することで、送信者が先行して
       送信済みの seq に直接同期し、不要な NACK/REJECT サイクルを発生させない。
       送信者を先に起動して受信者が後から参加した場合も同様に機能する。 */
    ctx->peer_session_id      = pkt->session_id;
    ctx->peer_session_tv_sec  = pkt->session_tv_sec;
    ctx->peer_session_tv_nsec = pkt->session_tv_nsec;
    ctx->reorder_pending      = 0;
    window_init(&ctx->recv_window, pkt->seq_num,
                ctx->global.window_size, ctx->global.max_payload);
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

/* health_alive が dead → alive になった場合に CONNECTED イベントを発火する。
   ヘルスチェック有効/無効に関わらず health_alive フラグで接続状態を統一管理する。 */
static void notify_health_alive(struct PotrContext_ *ctx)
{
    if (!ctx->health_alive)
    {
        ctx->health_alive = 1;
        POTR_LOG(POTR_LOG_INFO,
                 "recv[service_id=%d]: CONNECTED",
                 ctx->service.service_id);
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
            POTR_LOG(POTR_LOG_WARN,
                     "recv[service_id=%d]: DISCONNECTED (timeout %lldms >= %ums)",
                     ctx->service.service_id,
                     (long long)elapsed_ms,
                     (unsigned)ctx->global.health_timeout_ms);
            if (ctx->callback != NULL)
            {
                ctx->callback(ctx->service.service_id,
                              POTR_EVENT_DISCONNECTED, NULL, 0);
            }

            /* FIN と同様にセッション状態をリセットして次の接続を受け入れ可能にする。
               peer_session_known をクリアすることで、送信者が同一セッションのまま
               復帰した場合でも window_init を経由して受信ウィンドウを初期化し、
               前セッションの next_seq が gap スキャンに影響しないようにする。 */
            ctx->peer_session_known = 0;
            ctx->reorder_pending    = 0;
            ctx->last_recv_tv_sec   = 0;
            window_init(&ctx->recv_window, 0,
                        ctx->global.window_size, ctx->global.max_payload);
        }
    }
}

/* 欠番 nack_num に対してリオーダー待機が完了しているか確認する。
   返値: 1 = 処理進行 (NACK/DISCONNECT を発行すべき)、0 = まだ待機中。
   reorder_timeout_ms == 0 の場合は常に 1 を返す (即時)。
   新しいギャップまたは欠番通番が変わった場合はタイマーをリセットして 0 を返す。
   同一欠番でタイムアウト経過後は reorder_pending を 0 にクリアして 1 を返す。 */
static int reorder_gap_ready(struct PotrContext_ *ctx, uint32_t nack_num)
{
    int64_t  now_sec;
    int32_t  now_nsec;
    uint32_t ms;

    if (ctx->global.reorder_timeout_ms == 0U) return 1;

    ms = ctx->global.reorder_timeout_ms;

    /* 新しいギャップ、または欠番通番が変わった: タイマーをリセットして待機開始 */
    if (!ctx->reorder_pending || ctx->reorder_nack_num != nack_num)
    {
        uint32_t effective_ms;
        get_monotonic(&now_sec, &now_nsec);

        /* マルチキャスト/ブロードキャスト通常モードでは NACK 送出タイミングを分散させる。
           複数受信者が同一欠番を同時に NACK すると送信者側で輻輳が発生するため、
           タイマー値を reorder_timeout_ms の 100%〜200% の範囲でジッタを付加する。
           ジッタ源: now_nsec の下位ビット (外部 RNG 不要・移植性高)。
           RAW 系は POTR_TYPE_MULTICAST_RAW / BROADCAST_RAW であり条件に該当しないため対象外。 */
        effective_ms = ms;
        if (ctx->service.type == POTR_TYPE_MULTICAST
            || ctx->service.type == POTR_TYPE_BROADCAST)
        {
            effective_ms = ms + (uint32_t)((uint32_t)now_nsec % ms);
        }

        ctx->reorder_pending       = 1;
        ctx->reorder_nack_num      = nack_num;
        ctx->reorder_deadline_sec  = now_sec  + (int64_t)(effective_ms / 1000U);
        ctx->reorder_deadline_nsec = now_nsec + (int32_t)((effective_ms % 1000U) * 1000000U);
        if (ctx->reorder_deadline_nsec >= 1000000000L)
        {
            ctx->reorder_deadline_sec++;
            ctx->reorder_deadline_nsec -= 1000000000L;
        }
        return 0; /* 待機開始 */
    }

    /* 同一欠番: タイムアウト確認 */
    get_monotonic(&now_sec, &now_nsec);
    if (now_sec > ctx->reorder_deadline_sec
        || (now_sec == ctx->reorder_deadline_sec
            && now_nsec >= ctx->reorder_deadline_nsec))
    {
        ctx->reorder_pending = 0;
        return 1; /* タイムアウト: 処理進行 */
    }

    return 0; /* まだ待機中 */
}

/* select() タイムアウト時に、リオーダー待機中の欠番がタイムアウトしていれば処理する。
   通常モード: NACK を送出する。
   RAW モード: DISCONNECTED を発行してセッションをリセットし、次のパケットで再同期する。 */
static void check_reorder_timeout(struct PotrContext_ *ctx)
{
    if (!ctx->reorder_pending) return;
    if (!reorder_gap_ready(ctx, ctx->reorder_nack_num)) return;

    /* reorder_gap_ready が 1 を返した時点で reorder_pending は既にクリア済み */
    if (potr_is_raw_type(ctx->service.type))
    {
        /* RAW モード: DISCONNECTED を発行してセッション状態をリセットする。
           次のパケット受信時に check_and_update_session で window_init が呼ばれ
           自然に再同期する。 */
        raw_session_disconnect(ctx);
        ctx->peer_session_known = 0;
        ctx->last_recv_tv_sec   = 0;
        window_init(&ctx->recv_window, 0,
                    ctx->global.window_size, ctx->global.max_payload);
    }
    else
    {
        POTR_LOG(POTR_LOG_DEBUG,
                 "recv[service_id=%d]: NACK seq=%u (reorder timeout)",
                 ctx->service.service_id, (unsigned)ctx->reorder_nack_num);
        send_nack(ctx, ctx->reorder_nack_num);
    }
}

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
        size_t dec_len = ctx->compress_buf_size;

        if (potr_decompress(ctx->compress_buf, &dec_len,
                            payload, payload_len) == 0)
        {
            POTR_LOG(POTR_LOG_TRACE,
                     "recv[service_id=%d]: decompress %zu -> %zu bytes",
                     ctx->service.service_id, payload_len, dec_len);
            ctx->callback(ctx->service.service_id,
                          POTR_EVENT_DATA,
                          ctx->compress_buf,
                          dec_len);
        }
        else
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "recv[service_id=%d]: decompress failed (src_len=%zu)",
                     ctx->service.service_id, payload_len);
        }
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
        if (ctx->frag_buf_len + elem->payload_len <= ctx->global.max_message_size)
        {
            if (ctx->frag_buf_len == 0)
            {
                if (elem->flags & POTR_FLAG_COMPRESSED)
                {
                    ctx->frag_compressed = 1;
                }
                else
                {
                    ctx->frag_compressed = 0;
                }
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
        if (ctx->frag_buf_len + elem->payload_len <= ctx->global.max_message_size)
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
            int is_compressed;
            if (elem->flags & POTR_FLAG_COMPRESSED)
            {
                is_compressed = 1;
            }
            else
            {
                is_compressed = 0;
            }
            recv_deliver(ctx,
                         elem->payload,
                         (size_t)elem->payload_len,
                         is_compressed);
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

        const char *pkt_type_str;
        if (pop_pkt.flags & POTR_FLAG_PING)
        {
            pkt_type_str = "PING";
        }
        else
        {
            pkt_type_str = "DATA";
        }
        POTR_LOG(POTR_LOG_TRACE,
                 "recv[service_id=%d]: pop seq=%u %s",
                 ctx->service.service_id,
                 (unsigned)pop_pkt.seq_num,
                 pkt_type_str);

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

/* RAW モード用: DISCONNECTED イベントを発行してセッション状態を部分的にリセットする。
   ウィンドウリセットは呼び出し元が行う (新しい基点通番が確定してから呼ぶため)。
   フラグメント組み立てバッファも破棄する。 */
static void raw_session_disconnect(struct PotrContext_ *ctx)
{
    if (ctx->health_alive)
    {
        ctx->health_alive = 0;
        POTR_LOG(POTR_LOG_WARN,
                 "recv[service_id=%d]: RAW DISCONNECTED (gap detected)",
                 ctx->service.service_id);
        if (ctx->callback != NULL)
        {
            ctx->callback(ctx->service.service_id,
                          POTR_EVENT_DISCONNECTED, NULL, 0);
        }
    }

    /* フラグメント組み立て途中状態を破棄 */
    ctx->frag_buf_len    = 0;
    ctx->frag_compressed = 0;
}

/* 外側パケット (DATA / PING) を受信ウィンドウに投入し、順序整列済みパケットを配信する。
   再送・順序整列の単位は外側パケットであり、NACK も外側パケットの seq_num を指定する。
   RAW モードでは NACK を送信せず、ギャップ検出時は DISCONNECTED を発行してウィンドウを
   新しい基点通番でリセットする。 */
static void process_outer_pkt(struct PotrContext_ *ctx,
                               const PotrPacket    *pkt)
{
    uint32_t nack_num;
    uint32_t stretch;
    int      is_raw = potr_is_raw_type(ctx->service.type);

    if (window_recv_push(&ctx->recv_window, pkt) != POTR_SUCCESS)
    {
        if (is_raw)
        {
            /* ウィンドウ満杯: DISCONNECTED を発行し、受信したパケットの通番でリセットしてから
               再投入する。再投入は必ず成功する (空ウィンドウの先頭スロット)。 */
            POTR_LOG(POTR_LOG_ERROR,
                     "recv[service_id=%d]: RAW recv_window full, resetting to seq=%u",
                     ctx->service.service_id, (unsigned)pkt->seq_num);
            raw_session_disconnect(ctx);
            window_recv_reset(&ctx->recv_window, pkt->seq_num);
            if (window_recv_push(&ctx->recv_window, pkt) != POTR_SUCCESS)
            {
                /* リセット直後の再投入失敗は想定外 */
                POTR_LOG(POTR_LOG_ERROR,
                         "recv[service_id=%d]: RAW window re-push failed seq=%u (bug)",
                         ctx->service.service_id, (unsigned)pkt->seq_num);
                return;
            }
        }
        else
        {
            /* 通番がウィンドウ範囲外のためドロップ (受信ウィンドウ満杯、または古い重複パケット)。
               受信者は next_seq を待ち続けるが、ヘルスチェックや後続パケット到着時に NACK が送られる。 */
            POTR_LOG(POTR_LOG_ERROR,
                     "recv[service_id=%d]: recv_window full (100%%), dropping seq=%u"
                     " (base_seq=%u window_size=%u)",
                     ctx->service.service_id, (unsigned)pkt->seq_num,
                     (unsigned)ctx->recv_window.base_seq,
                     (unsigned)ctx->recv_window.window_size);
            return;
        }
    }

    /* ウィンドウ利用率チェック: 今回のパケットが占める先頭からの距離で推定する。
       stretch = pkt->seq_num - base_seq + 1 。push 成功後は [1, window_size] の範囲。 */
    stretch = (uint32_t)(pkt->seq_num - ctx->recv_window.base_seq) + 1U;
    if (stretch * 10U >= (uint32_t)ctx->recv_window.window_size * 8U)
    {
        POTR_LOG(POTR_LOG_WARN,
                 "recv[service_id=%d]: recv_window utilization high (%u/%u >= 80%%)"
                 " seq=%u base_seq=%u",
                 ctx->service.service_id,
                 (unsigned)stretch, (unsigned)ctx->recv_window.window_size,
                 (unsigned)pkt->seq_num, (unsigned)ctx->recv_window.base_seq);
    }

    if (window_recv_needs_nack(&ctx->recv_window, &nack_num))
    {
        if (is_raw)
        {
            /* ギャップ検出: リオーダー待機を確認してから DISCONNECTED を発行する。
               reorder_timeout_ms > 0 のとき、タイムアウト前はウィンドウに今のパケットを
               残したまま待機する。タイムアウト後または即時モードでは reset + 再 push。
               skip ループは push 済みパケットのスロットマッピングを壊すため使用しない。 */
            if (reorder_gap_ready(ctx, nack_num))
            {
                raw_session_disconnect(ctx);
                window_recv_reset(&ctx->recv_window, pkt->seq_num);
                if (window_recv_push(&ctx->recv_window, pkt) != POTR_SUCCESS)
                {
                    /* リセット直後の再投入失敗は想定外 */
                    POTR_LOG(POTR_LOG_ERROR,
                             "recv[service_id=%d]: RAW gap re-push failed seq=%u (bug)",
                             ctx->service.service_id, (unsigned)pkt->seq_num);
                    return;
                }
            }
            /* else: リオーダー待機中。パケットはウィンドウに push 済み。 */
        }
        else
        {
            if (reorder_gap_ready(ctx, nack_num))
            {
                POTR_LOG(POTR_LOG_DEBUG,
                         "recv[service_id=%d]: NACK seq=%u",
                         ctx->service.service_id, (unsigned)nack_num);
                send_nack(ctx, nack_num);
            }
            /* else: リオーダー待機中。NACK 送出を保留。 */
        }
    }
    else
    {
        /* 欠番なし: 待機中の欠番が埋まった (または元から欠番なし) */
        ctx->reorder_pending = 0;
    }

    drain_recv_window(ctx);

    /* drain 後に next_seq が前進した結果、新たな欠番が先頭に現れる場合は NACK を送る。
       例: seq=3,4 欠落・seq=5 着時、drain 前は NACK(3)、seq=3 再送着→ drain で pop 後
       next_seq=4 が欠番になるが次のパケット到着まで NACK(4) が遅延するのを防ぐ。
       RAW モードで reorder_gap_ready が 1 を返した場合は reset + 再 push によりウィンドウに
       パケット 1 つのみ残るため drain 後は空となりここに到達しない。
       リオーダー待機中 (RAW/通常) は drain で前進しないため post-drain 欠番も保留のまま。 */
    if (window_recv_needs_nack(&ctx->recv_window, &nack_num))
    {
        if (!is_raw && reorder_gap_ready(ctx, nack_num))
        {
            POTR_LOG(POTR_LOG_DEBUG,
                     "recv[service_id=%d]: NACK seq=%u (post-drain)",
                     ctx->service.service_id, (unsigned)nack_num);
            send_nack(ctx, nack_num);
        }
        /* RAW モードでは reset + 再 push 後は到達しない。リオーダー待機中は
           check_reorder_timeout がタイムアウト処理を担う。 */
    }
    else
    {
        ctx->reorder_pending = 0;
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
    uint8_t            *buf = ctx->recv_buf; /* PACKET_HEADER_SIZE + max_payload バイト */
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
    /* reorder_timeout_ms が有効な場合は poll 間隔を短縮してタイムアウト精度を確保する */
    if (ctx->role == POTR_ROLE_RECEIVER && ctx->global.reorder_timeout_ms > 0U)
    {
        if (ctx->global.reorder_timeout_ms < poll_ms)
        {
            poll_ms = ctx->global.reorder_timeout_ms;
        }
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
            {
                check_health_timeout(ctx);
                if (ctx->global.reorder_timeout_ms > 0U)
                {
                    check_reorder_timeout(ctx);
                }
            }
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
            recv_len = recvfrom(ctx->sock[i], (char *)buf,
                                (int)(PACKET_HEADER_SIZE + ctx->global.max_payload),
                                0, (struct sockaddr *)&sender_addr, &sender_len);
#else
            recv_len = (int)recvfrom(ctx->sock[i], buf,
                                     PACKET_HEADER_SIZE + ctx->global.max_payload,
                                     0, (struct sockaddr *)&sender_addr,
                                     &sender_len);
#endif
            if (recv_len <= 0)
            {
                if (!ctx->running) break; /* 正常終了: ソケットクローズによる割り込み */
                POTR_LOG(POTR_LOG_TRACE,
                         "recv[service_id=%d]: recvfrom returned %d",
                         ctx->service.service_id, recv_len);
                continue;
            }

            if (packet_parse(&pkt, buf, (size_t)recv_len) != POTR_SUCCESS)
            {
                POTR_LOG(POTR_LOG_TRACE,
                         "recv[service_id=%d]: packet parse failed (len=%d)",
                         ctx->service.service_id, recv_len);
                continue;
            }
            if (pkt.service_id != ctx->service.service_id)
            {
                POTR_LOG(POTR_LOG_TRACE,
                         "recv[service_id=%d]: ignored packet for service_id=%d",
                         ctx->service.service_id, pkt.service_id);
                continue;
            }
            if (!check_src_addr(ctx, &sender_addr)) continue;

            /* 暗号化パケットを復号する
             *   - POTR_FLAG_DATA | POTR_FLAG_ENCRYPTED の組み合わせのみ対象
             *   - 復号後 pkt.payload / pkt.payload_len を書き換えて以降の処理を透過させる
             *   - 認証失敗 (タグ不一致) は即座に破棄する
             */
            if ((pkt.flags & (POTR_FLAG_DATA | POTR_FLAG_ENCRYPTED))
                == (POTR_FLAG_DATA | POTR_FLAG_ENCRYPTED))
            {
                uint8_t nonce[POTR_CRYPTO_NONCE_SIZE];
                size_t  dec_len = ctx->crypto_buf_size;
                /* pkt.session_id / pkt.seq_num は packet_parse 後はホストオーダー */
                uint32_t sid_nbo = htonl(pkt.session_id);
                uint32_t seq_nbo = htonl(pkt.seq_num);

                memcpy(nonce,     &sid_nbo, 4);
                memcpy(nonce + 4, &seq_nbo, 4);
                memset(nonce + 8, 0,        4);

                /* AAD = 受信 raw バイト先頭 32B (NBO、送信側と同一) */
                if (potr_decrypt(ctx->crypto_buf, &dec_len,
                                 pkt.payload, pkt.payload_len,
                                 ctx->service.encrypt_key,
                                 nonce,
                                 buf, PACKET_HEADER_SIZE) != 0)
                {
                    POTR_LOG(POTR_LOG_TRACE,
                             "recv[service_id=%d]: decrypt failed (auth) seq=%u",
                             ctx->service.service_id, (unsigned)pkt.seq_num);
                    continue;
                }

                pkt.payload     = ctx->crypto_buf;
                pkt.payload_len = (uint16_t)dec_len;
                pkt.flags       = (uint16_t)(pkt.flags & ~POTR_FLAG_ENCRYPTED);
            }

            /* ── 送信者ロール: NACK のみ処理 ── */
            if (ctx->role == POTR_ROLE_SENDER)
            {
                /* RAW モードは再送バッファを持たないため NACK を無視する */
                if (potr_is_raw_type(ctx->service.type))
                {
                    continue;
                }

                if (pkt.flags & POTR_FLAG_NACK)
                {
                    int j;

                    /* 同一 ack_num の NACK が POTR_NACK_DEDUP_MS 以内に届いた場合は破棄 */
                    {
                        uint64_t now_ms    = get_ms_mono();
                        int      dedup_idx;
                        int      is_dup    = 0;

                        for (dedup_idx = 0;
                             dedup_idx < (int)POTR_NACK_DEDUP_SLOTS;
                             dedup_idx++)
                        {
                            const PotrNackDedupEntry *e =
                                &ctx->nack_dedup_buf[dedup_idx];
                            if (e->time_ms != 0
                                && e->ack_num == pkt.ack_num
                                && (now_ms - e->time_ms)
                                   < (uint64_t)POTR_NACK_DEDUP_MS)
                            {
                                is_dup = 1;
                                break;
                            }
                        }

                        if (is_dup)
                        {
                            continue;
                        }

                        ctx->nack_dedup_buf[ctx->nack_dedup_next].ack_num =
                            pkt.ack_num;
                        ctx->nack_dedup_buf[ctx->nack_dedup_next].time_ms =
                            now_ms;
                        ctx->nack_dedup_next =
                            (uint8_t)((ctx->nack_dedup_next + 1U)
                                      % POTR_NACK_DEDUP_SLOTS);
                    }

                    {
                        PotrPacket resend_pkt;
                        int        get_result;
                        size_t     wire_len = 0;

                        /* send_window へのアクセスを排他制御する (送信スレッド・ヘルスチェックスレッドと競合)。
                           ミューテックス保持中に recv_buf へ wire データを組み立て、
                           プールスロットが上書きされる前にコピーを完了させる。 */
#ifdef _WIN32
                        EnterCriticalSection(&ctx->send_window_mutex);
#else
                        pthread_mutex_lock(&ctx->send_window_mutex);
#endif
                        get_result = window_send_get(&ctx->send_window,
                                                     pkt.ack_num,
                                                     &resend_pkt);

                        if (get_result == POTR_SUCCESS)
                        {
                            /* [NBO ヘッダー 32B][ペイロード] を recv_buf に組み立てる */
                            wire_len = packet_wire_size(&resend_pkt);
                            memcpy(ctx->recv_buf, &resend_pkt, PACKET_HEADER_SIZE);
                            memcpy(ctx->recv_buf + PACKET_HEADER_SIZE,
                                   resend_pkt.payload,
                                   wire_len - PACKET_HEADER_SIZE);
                        }

#ifdef _WIN32
                        LeaveCriticalSection(&ctx->send_window_mutex);
#else
                        pthread_mutex_unlock(&ctx->send_window_mutex);
#endif

                        if (get_result == POTR_SUCCESS)
                        {
                            POTR_LOG(POTR_LOG_DEBUG,
                                     "sender[service_id=%d]: NACK received seq=%u"
                                     " -> retransmit",
                                     ctx->service.service_id,
                                     (unsigned)pkt.ack_num);
                            for (j = 0; j < ctx->n_path; j++)
                            {
#ifdef _WIN32
                                sendto(ctx->sock[j], (const char *)ctx->recv_buf,
                                       (int)wire_len, 0,
                                       (const struct sockaddr *)&ctx->dest_addr[j],
                                       sizeof(ctx->dest_addr[j]));
#else
                                sendto(ctx->sock[j], ctx->recv_buf, wire_len, 0,
                                       (const struct sockaddr *)&ctx->dest_addr[j],
                                       sizeof(ctx->dest_addr[j]));
#endif
                            }
                        }
                        else
                        {
                            POTR_LOG(POTR_LOG_WARN,
                                     "sender[service_id=%d]: NACK seq=%u not in window"
                                     " -> REJECT",
                                     ctx->service.service_id,
                                     (unsigned)pkt.ack_num);
                            send_reject(ctx, pkt.ack_num);
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

                POTR_LOG(POTR_LOG_INFO,
                         "recv[service_id=%d]: FIN received -> DISCONNECTED",
                         ctx->service.service_id);

                /* health_alive で重複発火を防止する (ヘルスチェック有無によらず接続済み状態のみ) */
                if (ctx->health_alive)
                {
                    ctx->health_alive = 0;
                    if (ctx->callback != NULL)
                    {
                        ctx->callback(ctx->service.service_id,
                                      POTR_EVENT_DISCONNECTED, NULL, 0);
                    }
                }

                /* セッション状態をリセットして次の接続を受け入れ可能にする。
                   受信ウィンドウも初期化して前セッションの next_seq を残さない。 */
                ctx->peer_session_known = 0;
                ctx->reorder_pending    = 0;
                ctx->last_recv_tv_sec   = 0;
                window_init(&ctx->recv_window, 0,
                            ctx->global.window_size, ctx->global.max_payload);
                continue;
            }

            /* REJECT: 欠落外側パケットをスキップして後続パケットを配信する */
            if (pkt.flags & POTR_FLAG_REJECT)
            {
                /* RAW モードは REJECT を発生させない (念のため無視) */
                if (potr_is_raw_type(ctx->service.type))
                {
                    continue;
                }

                if (!check_and_update_session(ctx, &pkt))
                {
                    continue;
                }

                /* 送信元から受信できている = 生存確認としてタイムアウトをリセットする */
                update_path_recv(ctx, i, &sender_addr);

                POTR_LOG(POTR_LOG_WARN,
                         "recv[service_id=%d]: REJECT received seq=%u"
                         " (packet unrecoverable)",
                         ctx->service.service_id, (unsigned)pkt.ack_num);

                /* DISCONNECTED イベント発火 (接続済みのときのみ。連続 REJECT で重複しない) */
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
                ctx->reorder_pending = 0;
                window_recv_skip(&ctx->recv_window, pkt.ack_num);

                /* 後続パケットを配信 (ウィンドウに溜まっていれば最初の pop で CONNECTED を発火) */
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

            {
                const char *pkt_kind_str;
                if (pkt.flags & POTR_FLAG_PING)
                {
                    pkt_kind_str = "PING";
                }
                else
                {
                    pkt_kind_str = "DATA";
                }
                POTR_LOG(POTR_LOG_TRACE,
                         "recv[service_id=%d]: %s seq=%u path=%d",
                         ctx->service.service_id,
                         pkt_kind_str,
                         (unsigned)pkt.seq_num, i);
            }

            if (pkt.flags & POTR_FLAG_PING)
            {
                /* PING はウィンドウ外: NACK・再送の対象外。
                   seq_num は送信側の next_seq (消費前) を示す。
                   通常モード: [recv_window.next_seq, seq_num) の範囲を全スキャンして欠番を一括 NACK する。
                   RAW モード: ギャップがあれば DISCONNECTED を発行してウィンドウを新基点にリセットする。 */
                if (potr_is_raw_type(ctx->service.type))
                {
                    /* pkt.seq_num が next_seq より前方にある (= ギャップあり) か判定する。
                       seqnum_in_window で window_size 以内の前方範囲のみを対象とし、
                       古い PING (next_seq より後方) は無視する。
                       reorder_timeout_ms > 0 のとき、タイムアウト前は DISCONNECTED を保留する。 */
                    if (pkt.seq_num != ctx->recv_window.next_seq
                        && seqnum_in_window(pkt.seq_num,
                                            ctx->recv_window.next_seq + 1U,
                                            ctx->recv_window.window_size))
                    {
                        if (reorder_gap_ready(ctx, ctx->recv_window.next_seq))
                        {
                            raw_session_disconnect(ctx);
                            window_recv_reset(&ctx->recv_window, pkt.seq_num);
                        }
                        /* else: リオーダー待機中。次の PING/DATA 到着時に再判定する。 */
                    }
                    else
                    {
                        ctx->reorder_pending = 0; /* ギャップなし */
                    }
                    notify_health_alive(ctx);
                }
                else
                {
                    notify_health_alive(ctx);

                    {
                        /* 先頭欠番のリオーダー待機を確認してから NACK スキャンを行う。
                           最初の欠番が待機中の場合はその後続の欠番も一括保留する。
                           スキャン完了 (全有効または全 NACK 送出済み) 時にリオーダー状態をクリアする。 */
                        uint32_t scan_seq = ctx->recv_window.next_seq;
                        while (scan_seq != pkt.seq_num
                               && seqnum_in_window(scan_seq, ctx->recv_window.base_seq,
                                                   ctx->recv_window.window_size))
                        {
                            uint16_t idx = (uint16_t)((scan_seq - ctx->recv_window.base_seq)
                                                      % ctx->recv_window.window_size);
                            if (!ctx->recv_window.valid[idx])
                            {
                                if (reorder_gap_ready(ctx, scan_seq))
                                {
                                    POTR_LOG(POTR_LOG_DEBUG,
                                             "recv[service_id=%d]: NACK seq=%u (PING gap scan)",
                                             ctx->service.service_id, (unsigned)scan_seq);
                                    send_nack(ctx, scan_seq);
                                }
                                else
                                {
                                    break; /* リオーダー待機中: 後続欠番も保留 */
                                }
                            }
                            scan_seq++;
                        }
                        /* スキャン完了: リオーダー状態をクリア */
                        if (scan_seq == pkt.seq_num)
                        {
                            ctx->reorder_pending = 0;
                        }
                    }
                }
            }
            else
            {
                /* DATA: ウィンドウ経由で順序整列・配信 (RAW モードも同じ経路。
                   RAW モードではギャップ検出時に NACK の代わりに DISCONNECTED を発行する)。 */
                process_outer_pkt(ctx, &pkt);
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

    POTR_LOG(POTR_LOG_DEBUG,
             "recv_thread[service_id=%d]: starting",
             ctx->service.service_id);

#ifdef _WIN32
    ctx->recv_thread = CreateThread(NULL, 0, recv_thread_func, ctx, 0, NULL);
    if (ctx->recv_thread == NULL)
    {
        ctx->running = 0;
        POTR_LOG(POTR_LOG_ERROR,
                 "recv_thread[service_id=%d]: CreateThread failed",
                 ctx->service.service_id);
        return POTR_ERROR;
    }
#else
    if (pthread_create(&ctx->recv_thread, NULL, recv_thread_func, ctx) != 0)
    {
        ctx->running = 0;
        POTR_LOG(POTR_LOG_ERROR,
                 "recv_thread[service_id=%d]: pthread_create failed",
                 ctx->service.service_id);
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
