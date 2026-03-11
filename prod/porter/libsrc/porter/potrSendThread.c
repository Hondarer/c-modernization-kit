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
 *  potrSend の blocking 引数の値によらず常に起動しており、
 *  ノンブロッキング送信 (blocking = 0) 時のみキューが使用されます。
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

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <pthread.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <time.h>
#endif

#include <string.h>

#include <porter_const.h>

#include "potrContext.h"
#include "potrSendQueue.h"
#include "potrSendThread.h"
#include "protocol/packet.h"
#include "protocol/window.h"
#include "potrLog.h"

/* 現在時刻をミリ秒単位で返す (単調増加クロック) */
static uint64_t get_ms(void)
{
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
#endif
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

/* packed_buf からデータパケット (外側コンテナ) を構築して sendto する。
   seq_num を付与し、再送バッファ (send_window) にも登録する。 */
static void flush_packed(struct PotrContext_ *ctx,
                         const uint8_t *packed_buf, size_t packed_len)
{
    PotrPacket           outer_pkt;
    PotrPacketSessionHdr shdr;
    uint32_t             seq;
    size_t               wire_len;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;
    shdr._pad            = 0;

    /* send_window へのアクセスを排他制御する (送信スレッド・ヘルスチェックスレッド・受信スレッドが競合) */
#ifdef _WIN32
    EnterCriticalSection(&ctx->send_window_mutex);
#else
    pthread_mutex_lock(&ctx->send_window_mutex);
#endif

    seq = ctx->send_window.next_seq;

    if (packet_build_packed(&outer_pkt, &shdr, seq, packed_buf, packed_len)
        != POTR_SUCCESS)
    {
#ifdef _WIN32
        LeaveCriticalSection(&ctx->send_window_mutex);
#else
        pthread_mutex_unlock(&ctx->send_window_mutex);
#endif
        return;
    }

    window_send_push(&ctx->send_window, &outer_pkt);

#ifdef _WIN32
    LeaveCriticalSection(&ctx->send_window_mutex);
#else
    pthread_mutex_unlock(&ctx->send_window_mutex);
#endif

    wire_len = packet_wire_size(&outer_pkt);

    POTR_LOG(POTR_LOG_TRACE,
             "sender[service_id=%d]: DATA seq=%u packed_len=%zu",
             ctx->service.service_id, (unsigned)seq, packed_len);

    {
        int i;
        for (i = 0; i < ctx->n_path; i++)
        {
#ifdef _WIN32
            sendto(ctx->sock[i], (const char *)&outer_pkt, (int)wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr[i],
                   sizeof(ctx->dest_addr[i]));
#else
            sendto(ctx->sock[i], &outer_pkt, wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr[i],
                   sizeof(ctx->dest_addr[i]));
#endif
        }
    }

    /* ヘルスチェックスレッドが参照する最終送信時刻を更新し、
       スリープ中のヘルスチェックスレッドを起床させてタイマーをリセットする */
    ctx->last_send_ms = get_ms();

    if (ctx->health_running)
    {
#ifdef _WIN32
        EnterCriticalSection(&ctx->health_mutex);
        WakeConditionVariable(&ctx->health_wakeup);
        LeaveCriticalSection(&ctx->health_mutex);
#else
        pthread_mutex_lock(&ctx->health_mutex);
        pthread_cond_signal(&ctx->health_wakeup);
        pthread_mutex_unlock(&ctx->health_mutex);
#endif
    }
}

/* 送信スレッド本体 */
#ifdef _WIN32
static DWORD WINAPI send_thread_func(LPVOID arg)
#else
static void *send_thread_func(void *arg)
#endif
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

        /* パッキング試行 */
        {
            uint8_t  packed_buf[POTR_MAX_PAYLOAD];
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

                        if (packed_len + elem_size > POTR_MAX_PAYLOAD)
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

                        if (packed_len + elem_size > POTR_MAX_PAYLOAD)
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
            flush_packed(ctx, packed_buf, packed_len);

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

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/* doxygen コメントは、ヘッダに記載 */
int potr_send_thread_start(struct PotrContext_ *ctx)
{
    ctx->send_thread_running = 1;

#ifdef _WIN32
    InitializeCriticalSection(&ctx->send_window_mutex);
    ctx->send_thread = CreateThread(NULL, 0, send_thread_func, ctx, 0, NULL);
    if (ctx->send_thread == NULL)
    {
        ctx->send_thread_running = 0;
        DeleteCriticalSection(&ctx->send_window_mutex);
        return POTR_ERROR;
    }
#else
    pthread_mutex_init(&ctx->send_window_mutex, NULL);
    if (pthread_create(&ctx->send_thread, NULL, send_thread_func, ctx) != 0)
    {
        ctx->send_thread_running = 0;
        pthread_mutex_destroy(&ctx->send_window_mutex);
        return POTR_ERROR;
    }
#endif

    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
void potr_send_thread_stop(struct PotrContext_ *ctx)
{
    ctx->send_thread_running = 0;
    potr_send_queue_shutdown(&ctx->send_queue);

#ifdef _WIN32
    WaitForSingleObject(ctx->send_thread, INFINITE);
    CloseHandle(ctx->send_thread);
    DeleteCriticalSection(&ctx->send_window_mutex);
#else
    pthread_join(ctx->send_thread, NULL);
    pthread_mutex_destroy(&ctx->send_window_mutex);
#endif
}
