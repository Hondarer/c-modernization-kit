/**
 *******************************************************************************
 *  @file           potrSendThread.c
 *  @brief          非同期送信スレッドの実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/08
 *  @version        1.0.0
 *
 *  @details
 *  送信キュー (PotrSendQueue) からペイロードエレメントを取り出して
 *  外側パケット (POTR_FLAG_DATA) を構築し sendto を呼び出す送信スレッド。\n
 *  potrOpenService (POTR_ROLE_SENDER) 時に起動し、potrCloseService 時に停止します。\n
 *  potrSend の flags 引数の値によらず常に起動しており、
 *  ノンブロッキング送信 (POTR_SEND_BLOCKING なし) 時のみキューが使用されます。
 *
 *  @par            通番管理
 *  すべてのデータパケットはパックコンテナ形式で送受信します。\n
 *  再送・順序整列の単位は外側パケット (UDP datagram) であり、\n
 *  通番は本スレッドが外側パケットを構築する際に付与します。\n
 *  送信ウィンドウ (ctx->send_window) への登録も本スレッドが行います。
 *
 *  @par            パッキング機能
 *  POTR_MAX_PAYLOAD - POTR_PAYLOAD_ELEM_HDR_SIZE 以下のフラグメントが
 *  複数キューに滞留している場合、送信スレッドが 1 つの外側パケットに
 *  まとめて送信します。\n
 *  以下の場合は単体 (ペイロードエレメント 1 件) のコンテナとして送信します。
 *  - MORE_FRAG フラグが付いているエントリ (フラグメント化メッセージの途中)
 *  - キューに追加エントリが存在しない場合
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)
    #include <pthread.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <time.h>
    #include <poll.h>
#elif defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    #include <windows.h>
#endif /* PLATFORM_ */

#include <string.h>
#include <inttypes.h>

#include <porter_const.h>

#include "../potrContext.h"
#include "../potrPeerTable.h"
#include "../infra/potrSendQueue.h"
#include "potrSendThread.h"
#include "../protocol/packet.h"
#include "../protocol/window.h"
#include "../infra/potrLog.h"
#include "../infra/crypto/crypto.h"

/* 現在時刻をミリ秒単位で返す (単調増加クロック) */
static uint64_t get_ms(void)
{
#if defined(PLATFORM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
#elif defined(PLATFORM_WINDOWS)
    return (uint64_t)GetTickCount64();
#endif /* PLATFORM_ */
}

/* ペイロードエレメントを packed_buf に追記する */
static void append_payload_elem(uint8_t *packed_buf, size_t *packed_len,
                               const PotrPayloadElem *entry)
{
    uint16_t flags_nbo = htons(entry->flags);
    uint32_t plen_nbo  = htonl((uint32_t)entry->payload_len);

    memcpy(packed_buf + *packed_len, &flags_nbo,       2); *packed_len += 2;
    memcpy(packed_buf + *packed_len, &plen_nbo,        4); *packed_len += 4;
    memcpy(packed_buf + *packed_len, entry->payload, entry->payload_len);
    *packed_len += entry->payload_len;
}

/* TCP 接続ソケットへ全バイトを確実に送信する。送信中は tcp_send_mutex を保持する。
   戻り値: POTR_SUCCESS (全バイト送信成功) / POTR_ERROR (切断 or エラー)。 */
static int tcp_send_all(PotrSocket fd, PotrMutex *mtx,
                        const uint8_t *buf, size_t len)
{
    size_t sent = 0;
    int    ret  = POTR_SUCCESS;

#if defined(PLATFORM_LINUX)
    pthread_mutex_lock(mtx);
    while (sent < len)
    {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n <= 0) { ret = POTR_ERROR; break; }
        sent += (size_t)n;
    }
    pthread_mutex_unlock(mtx);
#elif defined(PLATFORM_WINDOWS)
    EnterCriticalSection(mtx);
    while (sent < len)
    {
        int n = send(fd, (const char *)(buf + sent), (int)(len - sent), 0);
        if (n <= 0) { ret = POTR_ERROR; break; }
        sent += (size_t)n;
    }
    LeaveCriticalSection(mtx);
#endif /* PLATFORM_ */

    return ret;
}

/* send_wire_buf の [PACKET_HEADER_SIZE .. PACKET_HEADER_SIZE+packed_len-1] に
   詰め済みのペイロードから外側コンテナを構築して送信する。
   seq_num を付与する。UDP では再送バッファ (send_window) にも登録する。
   send_wire_buf = [NBO ヘッダー 32B][packed_payload packed_len B] として組み立てる。 */
static void flush_packed(struct PotrContext_ *ctx, size_t packed_len)
{
    PotrPacket           outer_pkt;
    PotrPacketSessionHdr shdr;
    uint32_t             seq;
    size_t               wire_len;
    uint8_t             *packed_buf = ctx->send_wire_buf + PACKET_HEADER_SIZE;
    int                  is_tcp     = potr_is_tcp_type(ctx->service.type);

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    /* send_window へのアクセスを排他制御する (送信スレッド・ヘルスチェックスレッド・受信スレッドが競合) */
#if defined(PLATFORM_LINUX)
    pthread_mutex_lock(&ctx->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
    EnterCriticalSection(&ctx->send_window_mutex);
#endif /* PLATFORM_ */

    seq = ctx->send_window.next_seq;

    if (packet_build_packed(&outer_pkt, &shdr, seq, packed_buf, packed_len)
        != POTR_SUCCESS)
    {
#if defined(PLATFORM_LINUX)
        pthread_mutex_unlock(&ctx->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
        LeaveCriticalSection(&ctx->send_window_mutex);
#endif /* PLATFORM_ */
        return;
    }

    if (ctx->service.encrypt_enabled)
    {
        /* 暗号化パス:
         *   1. ENCRYPTED フラグを OR (outer_pkt.flags は既に NBO)
         *   2. payload_len を packed_len + TAG_SIZE に更新
         *   3. nonce = session_id(4B NBO) + flags(2B NBO) + seq_num(4B NBO) + padding(2B)
         *   4. AAD  = outer_pkt ヘッダー 32B (NBO)
         *   5. packed_buf → ctx->crypto_buf に暗号化
         *   6. UDP: window_send_push に暗号化済みペイロードを登録
         *      TCP: ウィンドウ登録不要 (再送は TCP 層が担保); next_seq のみインクリメント
         *   7. send_wire_buf に暗号化済みデータを組立て
         */
        uint8_t  nonce[POTR_CRYPTO_NONCE_SIZE];
        size_t   enc_len = ctx->crypto_buf_size;

        outer_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
        outer_pkt.payload_len = htons((uint16_t)(packed_len + POTR_CRYPTO_TAG_SIZE));

        /* ノンス: session_id(4B NBO) + flags(2B NBO) + seq_num(4B NBO) + padding(2B)
         * outer_pkt の各フィールドは既に NBO */
        memcpy(nonce,      &outer_pkt.session_id, 4);
        memcpy(nonce + 4,  &outer_pkt.flags,      2);
        memcpy(nonce + 6,  &outer_pkt.seq_num,    4);
        memset(nonce + 10, 0,                     2);

        if (potr_encrypt(ctx->crypto_buf, &enc_len,
                         packed_buf, packed_len,
                         ctx->service.encrypt_key,
                         nonce,
                         (const uint8_t *)&outer_pkt, PACKET_HEADER_SIZE) != 0)
        {
#if defined(PLATFORM_LINUX)
            pthread_mutex_unlock(&ctx->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
            LeaveCriticalSection(&ctx->send_window_mutex);
#endif /* PLATFORM_ */
            POTR_LOG(POTR_TRACE_ERROR,
                     "sender[service_id=%" PRId64 "]: encrypt failed seq=%u",
                     ctx->service.service_id, (unsigned)seq);
            return;
        }

        if (is_tcp)
        {
            /* TCP: ウィンドウ登録不要。next_seq をインクリメントして mutex を解放 */
            ctx->send_window.next_seq++;
#if defined(PLATFORM_LINUX)
            pthread_mutex_unlock(&ctx->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
            LeaveCriticalSection(&ctx->send_window_mutex);
#endif /* PLATFORM_ */
        }
        else
        {
            /* window には暗号化済みペイロードを格納して NACK 再送時に再暗号化不要にする */
            outer_pkt.payload = ctx->crypto_buf;
            window_send_push(&ctx->send_window, &outer_pkt);
#if defined(PLATFORM_LINUX)
            pthread_mutex_unlock(&ctx->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
            LeaveCriticalSection(&ctx->send_window_mutex);
#endif /* PLATFORM_ */
        }

        /* wire 組立: NBO ヘッダー + 暗号文 + タグ */
        memcpy(ctx->send_wire_buf, &outer_pkt, PACKET_HEADER_SIZE);
        memcpy(ctx->send_wire_buf + PACKET_HEADER_SIZE, ctx->crypto_buf, enc_len);
        wire_len = PACKET_HEADER_SIZE + enc_len;

        POTR_LOG(POTR_TRACE_VERBOSE,
                 "sender[service_id=%" PRId64 "]: DATA(enc) seq=%u packed_len=%zu enc_len=%zu",
                 ctx->service.service_id, (unsigned)seq, packed_len, enc_len);
    }
    else
    {
        if (is_tcp)
        {
            /* TCP: ウィンドウ登録不要。next_seq をインクリメントして mutex を解放 */
            ctx->send_window.next_seq++;
#if defined(PLATFORM_LINUX)
            pthread_mutex_unlock(&ctx->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
            LeaveCriticalSection(&ctx->send_window_mutex);
#endif /* PLATFORM_ */
        }
        else
        {
            window_send_push(&ctx->send_window, &outer_pkt);
#if defined(PLATFORM_LINUX)
            pthread_mutex_unlock(&ctx->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
            LeaveCriticalSection(&ctx->send_window_mutex);
#endif /* PLATFORM_ */
        }

        /* NBO ヘッダー (32B) を send_wire_buf 先頭に書き込む (ペイロードは既に直後に配置済み) */
        memcpy(ctx->send_wire_buf, &outer_pkt, PACKET_HEADER_SIZE);
        wire_len = PACKET_HEADER_SIZE + packed_len;

        POTR_LOG(POTR_TRACE_VERBOSE,
                 "sender[service_id=%" PRId64 "]: DATA seq=%u packed_len=%zu",
                 ctx->service.service_id, (unsigned)seq, packed_len);
    }

    if (is_tcp)
    {
        /* TCP v2: アクティブな全 path にループ送信する */
        if (ctx->tcp_active_paths > 0)
        {
            int i;
            for (i = 0; i < ctx->n_path; i++)
            {
                int pr;

                if (ctx->tcp_conn_fd[i] == POTR_INVALID_SOCKET) continue;

                /* 送信バッファの空きを確認 (非ブロッキング) */
#if defined(PLATFORM_LINUX)
                {
                    struct pollfd pfd;
                    pfd.fd      = ctx->tcp_conn_fd[i];
                    pfd.events  = POLLOUT;
                    pfd.revents = 0;
                    pr = poll(&pfd, 1, 0);
                    if (pr > 0 && (pfd.revents & POLLOUT))
                    {
                        if (ctx->buf_full_suppress_cnt[i] > 0
                            && ++ctx->buf_full_suppress_cnt[i] > 10)
                        {
                            ctx->buf_full_suppress_cnt[i] = 0;
                        }
                        tcp_send_all(ctx->tcp_conn_fd[i], &ctx->tcp_send_mutex[i],
                                     ctx->send_wire_buf, wire_len);
                    }
                    else
                    {
                        if (ctx->buf_full_suppress_cnt[i] == 0)
                        {
                            POTR_LOG(POTR_TRACE_ERROR,
                                     "send_thread[service_id=%" PRId64 "]: path[%d]"
                                     " send buffer full, packet skipped",
                                     ctx->service.service_id, i);
                            ctx->buf_full_suppress_cnt[i] = 1;
                        }
                    }
                }
#elif defined(PLATFORM_WINDOWS)
                {
                    WSAPOLLFD pfd;
                    pfd.fd      = ctx->tcp_conn_fd[i];
                    pfd.events  = POLLOUT;
                    pfd.revents = 0;
                    pr = WSAPoll(&pfd, 1, 0);
                    if (pr > 0 && (pfd.revents & POLLOUT))
                    {
                        if (ctx->buf_full_suppress_cnt[i] > 0
                            && ++ctx->buf_full_suppress_cnt[i] > 10)
                        {
                            ctx->buf_full_suppress_cnt[i] = 0;
                        }
                        tcp_send_all(ctx->tcp_conn_fd[i], &ctx->tcp_send_mutex[i],
                                     ctx->send_wire_buf, wire_len);
                    }
                    else
                    {
                        if (ctx->buf_full_suppress_cnt[i] == 0)
                        {
                            POTR_LOG(POTR_TRACE_ERROR,
                                     "send_thread[service_id=%" PRId64 "]: path[%d]"
                                     " send buffer full, packet skipped",
                                     ctx->service.service_id, i);
                            ctx->buf_full_suppress_cnt[i] = 1;
                        }
                    }
                }
#endif /* PLATFORM_ */
            }
        }
    }
    else
    {
        int i;
        for (i = 0; i < ctx->n_path; i++)
        {
#if defined(PLATFORM_LINUX)
            sendto(ctx->sock[i], ctx->send_wire_buf, wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr[i],
                   sizeof(ctx->dest_addr[i]));
#elif defined(PLATFORM_WINDOWS)
            sendto(ctx->sock[i], (const char *)ctx->send_wire_buf, (int)wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr[i],
                   sizeof(ctx->dest_addr[i]));
#endif /* PLATFORM_ */
        }
    }

    /* ヘルスチェックスレッドが参照する最終送信時刻を更新し、
       スリープ中のヘルスチェックスレッドを起床させてタイマーをリセットする */
    ctx->last_send_ms = get_ms();

    if (ctx->health_running[0])
    {
#if defined(PLATFORM_LINUX)
        pthread_mutex_lock(&ctx->health_mutex[0]);
        pthread_cond_signal(&ctx->health_wakeup[0]);
        pthread_mutex_unlock(&ctx->health_mutex[0]);
#elif defined(PLATFORM_WINDOWS)
        EnterCriticalSection(&ctx->health_mutex[0]);
        WakeConditionVariable(&ctx->health_wakeup[0]);
        LeaveCriticalSection(&ctx->health_mutex[0]);
#endif /* PLATFORM_ */
    }
}

/* N:1 モード専用: ピアの send_window を使ってパックコンテナを構築して sendto する */
static void flush_packed_peer(struct PotrContext_ *ctx, PotrPeerContext *peer,
                               size_t packed_len)
{
    PotrPacket           outer_pkt;
    PotrPacketSessionHdr shdr;
    uint32_t             seq;
    size_t               wire_len;
    uint8_t             *packed_buf = ctx->send_wire_buf + PACKET_HEADER_SIZE;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = peer->session_id;
    shdr.session_tv_sec  = peer->session_tv_sec;
    shdr.session_tv_nsec = peer->session_tv_nsec;

#if defined(PLATFORM_LINUX)
    pthread_mutex_lock(&peer->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
    EnterCriticalSection(&peer->send_window_mutex);
#endif /* PLATFORM_ */

    seq = peer->send_window.next_seq;

    if (packet_build_packed(&outer_pkt, &shdr, seq, packed_buf, packed_len)
        != POTR_SUCCESS)
    {
#if defined(PLATFORM_LINUX)
        pthread_mutex_unlock(&peer->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
        LeaveCriticalSection(&peer->send_window_mutex);
#endif /* PLATFORM_ */
        return;
    }

    if (ctx->service.encrypt_enabled)
    {
        uint8_t nonce[POTR_CRYPTO_NONCE_SIZE];
        size_t  enc_len = ctx->crypto_buf_size;

        outer_pkt.flags      |= htons(POTR_FLAG_ENCRYPTED);
        outer_pkt.payload_len = htons((uint16_t)(packed_len + POTR_CRYPTO_TAG_SIZE));

        /* ノンス: session_id(4B NBO) + flags(2B NBO) + seq_num(4B NBO) + padding(2B)
         * outer_pkt の各フィールドは既に NBO */
        memcpy(nonce,      &outer_pkt.session_id, 4);
        memcpy(nonce + 4,  &outer_pkt.flags,      2);
        memcpy(nonce + 6,  &outer_pkt.seq_num,    4);
        memset(nonce + 10, 0,                     2);

        if (potr_encrypt(ctx->crypto_buf, &enc_len,
                         packed_buf, packed_len,
                         ctx->service.encrypt_key,
                         nonce,
                         (const uint8_t *)&outer_pkt, PACKET_HEADER_SIZE) != 0)
        {
#if defined(PLATFORM_LINUX)
            pthread_mutex_unlock(&peer->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
            LeaveCriticalSection(&peer->send_window_mutex);
#endif /* PLATFORM_ */
            POTR_LOG(POTR_TRACE_ERROR,
                     "sender[service_id=%" PRId64 "]: peer=%u encrypt failed seq=%u",
                     ctx->service.service_id, (unsigned)peer->peer_id, (unsigned)seq);
            return;
        }

        outer_pkt.payload = ctx->crypto_buf;
        window_send_push(&peer->send_window, &outer_pkt);

#if defined(PLATFORM_LINUX)
        pthread_mutex_unlock(&peer->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
        LeaveCriticalSection(&peer->send_window_mutex);
#endif /* PLATFORM_ */

        memcpy(ctx->send_wire_buf, &outer_pkt, PACKET_HEADER_SIZE);
        memcpy(ctx->send_wire_buf + PACKET_HEADER_SIZE, ctx->crypto_buf, enc_len);
        wire_len = PACKET_HEADER_SIZE + enc_len;

        POTR_LOG(POTR_TRACE_VERBOSE,
                 "sender[service_id=%" PRId64 "]: peer=%u DATA(enc) seq=%u packed_len=%zu",
                 ctx->service.service_id, (unsigned)peer->peer_id,
                 (unsigned)seq, packed_len);
    }
    else
    {
        window_send_push(&peer->send_window, &outer_pkt);

#if defined(PLATFORM_LINUX)
        pthread_mutex_unlock(&peer->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
        LeaveCriticalSection(&peer->send_window_mutex);
#endif /* PLATFORM_ */

        memcpy(ctx->send_wire_buf, &outer_pkt, PACKET_HEADER_SIZE);
        wire_len = PACKET_HEADER_SIZE + packed_len;

        POTR_LOG(POTR_TRACE_VERBOSE,
                 "sender[service_id=%" PRId64 "]: peer=%u DATA seq=%u packed_len=%zu",
                 ctx->service.service_id, (unsigned)peer->peer_id,
                 (unsigned)seq, packed_len);
    }

    /* N:1 はインデックス = ctx->sock[] の添字として全パスへ送信する */
    {
        int k;
        for (k = 0; k < (int)POTR_MAX_PATH; k++)
        {
            if (peer->dest_addr[k].sin_family == 0) continue;
#if defined(PLATFORM_LINUX)
            sendto(ctx->sock[k], ctx->send_wire_buf, wire_len, 0,
                   (const struct sockaddr *)&peer->dest_addr[k],
                   sizeof(peer->dest_addr[k]));
#elif defined(PLATFORM_WINDOWS)
            sendto(ctx->sock[k], (const char *)ctx->send_wire_buf, (int)wire_len, 0,
                   (const struct sockaddr *)&peer->dest_addr[k],
                   sizeof(peer->dest_addr[k]));
#endif /* PLATFORM_ */
        }
    }

    ctx->last_send_ms = get_ms();
}

/* N:1 モード専用: キューからエントリを取り出してピアへパッキング送信する */
static void send_packed_peer_mode(struct PotrContext_ *ctx, PotrPayloadElem *first)
{
    PotrPeerId       target_peer_id = first->peer_id;
    PotrPeerContext *peer           = NULL;
    uint8_t         *packed_buf    = ctx->send_wire_buf + PACKET_HEADER_SIZE;
    size_t           packed_len    = 0;
    int              n_dequeued    = 1;

    /* ピアを検索 (peers_mutex は lookup だけ保護、送信中は解放する) */
#if defined(PLATFORM_LINUX)
    pthread_mutex_lock(&ctx->peers_mutex);
#elif defined(PLATFORM_WINDOWS)
    EnterCriticalSection(&ctx->peers_mutex);
#endif /* PLATFORM_ */
    peer = peer_find_by_id(ctx, target_peer_id);
#if defined(PLATFORM_LINUX)
    pthread_mutex_unlock(&ctx->peers_mutex);
#elif defined(PLATFORM_WINDOWS)
    LeaveCriticalSection(&ctx->peers_mutex);
#endif /* PLATFORM_ */

    if (peer == NULL)
    {
        /* 切断済みピア宛エントリ: 破棄 */
        potr_send_queue_complete(&ctx->send_queue);
        return;
    }

    append_payload_elem(packed_buf, &packed_len, first);

    /* 同一ピア宛の追加エントリを即時パッキング (pack_wait なし) */
    if (!(first->flags & POTR_FLAG_MORE_FRAG))
    {
        PotrPayloadElem next;

        while (potr_send_queue_peek(&ctx->send_queue, &next) == POTR_SUCCESS)
        {
            size_t elem_size;

            if (next.peer_id != target_peer_id) break;
            if (next.flags & POTR_FLAG_MORE_FRAG) break;

            elem_size = POTR_PAYLOAD_ELEM_HDR_SIZE + (size_t)next.payload_len;
            if (packed_len + elem_size > (size_t)ctx->global.max_payload
                                         - (ctx->service.encrypt_enabled
                                            ? POTR_CRYPTO_TAG_SIZE : 0U))
            {
                break;
            }

            if (potr_send_queue_try_pop(&ctx->send_queue, &next) != POTR_SUCCESS)
            {
                break;
            }

            append_payload_elem(packed_buf, &packed_len, &next);
            n_dequeued++;
        }
    }

    flush_packed_peer(ctx, peer, packed_len);

    {
        int i;
        for (i = 0; i < n_dequeued; i++)
        {
            potr_send_queue_complete(&ctx->send_queue);
        }
    }
}

/* 送信スレッド本体 */
#if defined(PLATFORM_LINUX)
static void *send_thread_func(void *arg)
#elif defined(PLATFORM_WINDOWS)
static DWORD WINAPI send_thread_func(LPVOID arg)
#endif /* PLATFORM_ */
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)arg;
    PotrPayloadElem        first;

    for (;;)
    {
        /* キューからエントリを取り出す (ブロッキング) */
        if (potr_send_queue_pop(&ctx->send_queue, &first,
                                &ctx->send_thread_running) != POTR_SUCCESS)
        {
            break;
        }

        /* N:1 モード: peer_id でルーティングして送信 */
        if (ctx->is_multi_peer)
        {
            send_packed_peer_mode(ctx, &first);
            continue;
        }

        /* パッキング試行 */
        {
            /* packed_buf は send_wire_buf のヘッダー直後領域を直接使用 (ゼロコピー) */
            uint8_t *packed_buf = ctx->send_wire_buf + PACKET_HEADER_SIZE;
            size_t   packed_len = 0;
            int      n_dequeued = 1;

            append_payload_elem(packed_buf, &packed_len, &first);

            /* MORE_FRAG エントリはパッキング不可。そのまま単体コンテナとして送信。 */
            if (!(first.flags & POTR_FLAG_MORE_FRAG))
            {
                uint32_t pack_wait_ms = ctx->service.pack_wait_ms;

                if (pack_wait_ms > 0)
                {
                    /* パッキング待ちあり: タイムアウトまで追加エントリを待ち合わせる */
                    uint64_t      deadline = get_ms() + pack_wait_ms;
                    PotrPayloadElem next;

                    for (;;)
                    {
                        uint64_t now = get_ms();
                        uint32_t remaining;
                        size_t   elem_size;

                        if (now >= deadline)
                        {
                            break; /* タイムアウト */
                        }

                        remaining = (uint32_t)(deadline - now);

                        if (potr_send_queue_peek_timed(&ctx->send_queue, &next, remaining)
                            != POTR_SUCCESS)
                        {
                            break; /* タイムアウト (エントリなし) */
                        }

                        if (next.flags & POTR_FLAG_MORE_FRAG)
                        {
                            break; /* MORE_FRAG はパッキング不可 */
                        }

                        elem_size = POTR_PAYLOAD_ELEM_HDR_SIZE + (size_t)next.payload_len;

                        if (packed_len + elem_size > (size_t)ctx->global.max_payload
                                                     - (ctx->service.encrypt_enabled
                                                        ? POTR_CRYPTO_TAG_SIZE : 0U))
                        {
                            break; /* 容量満杯: 即時送信してタイマーリセット */
                        }

                        if (potr_send_queue_try_pop(&ctx->send_queue, &next)
                            != POTR_SUCCESS)
                        {
                            break; /* 競合防止 (通常発生しない) */
                        }

                        append_payload_elem(packed_buf, &packed_len, &next);
                        n_dequeued++;
                    }
                }
                else
                {
                    /* パッキング待ちなし: キューにあるエントリを即時まとめる */
                    PotrPayloadElem next;

                    while (potr_send_queue_peek(&ctx->send_queue, &next) == POTR_SUCCESS)
                    {
                        size_t elem_size;

                        if (next.flags & POTR_FLAG_MORE_FRAG)
                        {
                            break;
                        }

                        elem_size = POTR_PAYLOAD_ELEM_HDR_SIZE + (size_t)next.payload_len;

                        if (packed_len + elem_size > (size_t)ctx->global.max_payload
                                                     - (ctx->service.encrypt_enabled
                                                        ? POTR_CRYPTO_TAG_SIZE : 0U))
                        {
                            break;
                        }

                        if (potr_send_queue_try_pop(&ctx->send_queue, &next)
                            != POTR_SUCCESS)
                        {
                            break;
                        }

                        append_payload_elem(packed_buf, &packed_len, &next);
                        n_dequeued++;
                    }
                }
            }

            /* 外側パケットを構築して送信 */
            flush_packed(ctx, packed_len);

            /* デキューした全エントリ分の inflight を減算 */
            {
                int i;
                for (i = 0; i < n_dequeued; i++)
                {
                    potr_send_queue_complete(&ctx->send_queue);
                }
            }
        }
    }

#if defined(PLATFORM_LINUX)
    return NULL;
#elif defined(PLATFORM_WINDOWS)
    return 0;
#endif /* PLATFORM_ */
}

/* doxygen コメントは、ヘッダに記載 */
int potr_send_thread_start(struct PotrContext_ *ctx)
{
    ctx->send_thread_running = 1;

#if defined(PLATFORM_LINUX)
    pthread_mutex_init(&ctx->send_window_mutex, NULL);
    if (pthread_create(&ctx->send_thread, NULL, send_thread_func, ctx) != 0)
    {
        ctx->send_thread_running = 0;
        pthread_mutex_destroy(&ctx->send_window_mutex);
        return POTR_ERROR;
    }
#elif defined(PLATFORM_WINDOWS)
    InitializeCriticalSection(&ctx->send_window_mutex);
    ctx->send_thread = CreateThread(NULL, 0, send_thread_func, ctx, 0, NULL);
    if (ctx->send_thread == NULL)
    {
        ctx->send_thread_running = 0;
        DeleteCriticalSection(&ctx->send_window_mutex);
        return POTR_ERROR;
    }
#endif /* PLATFORM_ */

    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
void potr_send_thread_stop(struct PotrContext_ *ctx)
{
    ctx->send_thread_running = 0;
    potr_send_queue_shutdown(&ctx->send_queue);

#if defined(PLATFORM_LINUX)
    pthread_join(ctx->send_thread, NULL);
    pthread_mutex_destroy(&ctx->send_window_mutex);
#elif defined(PLATFORM_WINDOWS)
    WaitForSingleObject(ctx->send_thread, INFINITE);
    CloseHandle(ctx->send_thread);
    DeleteCriticalSection(&ctx->send_window_mutex);
#endif /* PLATFORM_ */
}
