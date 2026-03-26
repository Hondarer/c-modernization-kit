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

#include <stdlib.h>

#include <porter_const.h>
#include <porter.h>

#include "../potrContext.h"
#include "../potrPeerTable.h"
#include "../infra/potrSendQueue.h"
#include "../infra/compress/compress.h"
#include "../infra/potrLog.h"

#ifndef _WIN32
    #include <pthread.h>
    typedef pthread_mutex_t PotrMutexLocal;
    #define POTR_MUTEX_LOCK_LOCAL(m)   pthread_mutex_lock(m)
    #define POTR_MUTEX_UNLOCK_LOCAL(m) pthread_mutex_unlock(m)
#else /* _WIN32 */
    #include <winsock2.h>
    typedef CRITICAL_SECTION PotrMutexLocal;
    #define POTR_MUTEX_LOCK_LOCAL(m)   EnterCriticalSection(m)
    #define POTR_MUTEX_UNLOCK_LOCAL(m) LeaveCriticalSection(m)
#endif /* _WIN32 */

/* N:1 モードで 1 ピアへ send を行う内部実装 (peers_mutex 取得不要・呼び出し元で検索済み) */
static int send_to_peer(struct PotrContext_ *ctx, PotrPeerId peer_id,
                        const uint8_t *ptr, size_t len, int flags,
                        uint16_t base_flags)
{
    size_t   remaining  = len;
    size_t   max_payload;

    max_payload = ctx->global.max_payload - POTR_PAYLOAD_ELEM_HDR_SIZE;
    if (ctx->service.encrypt_enabled)
    {
        max_payload -= POTR_CRYPTO_TAG_SIZE;
    }

    if ((flags & POTR_SEND_BLOCKING) != 0)
    {
        potr_send_queue_wait_drained(&ctx->send_queue);
    }

    while (remaining > 0)
    {
        size_t   chunk     = (remaining > max_payload) ? max_payload : remaining;
        int      more_frag = (remaining > chunk);
        uint16_t elem_flags = base_flags;

        if (more_frag)
        {
            elem_flags |= POTR_FLAG_MORE_FRAG;
        }

        if (potr_send_queue_push_wait(&ctx->send_queue, peer_id,
                                      elem_flags, ptr, (uint16_t)chunk,
                                      &ctx->send_thread_running) != POTR_SUCCESS)
        {
            return POTR_ERROR;
        }

        ptr       += chunk;
        remaining -= chunk;
    }

    if ((flags & POTR_SEND_BLOCKING) != 0)
    {
        potr_send_queue_wait_drained(&ctx->send_queue);
    }

    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
POTR_EXPORT int POTR_API potrSend(PotrHandle handle, PotrPeerId peer_id,
                              const void *data, size_t len, int flags)
{
    struct PotrContext_ *ctx       = (struct PotrContext_ *)handle;
    const uint8_t       *ptr      = (const uint8_t *)data;
    uint16_t             base_flags = 0;

    if (ctx == NULL || data == NULL || len == 0
        || len > (size_t)ctx->global.max_message_size)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrSend: invalid argument (handle=%p data=%p len=%zu max=%u)",
                 (const void *)handle, data, len,
                 (ctx != NULL) ? (unsigned)ctx->global.max_message_size : 0U);
        return POTR_ERROR;
    }

    POTR_LOG(POTR_LOG_TRACE,
             "potrSend: service_id=%d peer_id=%u len=%zu flags=0x%x",
             ctx->service.service_id, (unsigned)peer_id, len, (unsigned)flags);

    /* TCP: 全 path 切断中の場合は POTR_ERROR_DISCONNECTED を返す */
    if (potr_is_tcp_type(ctx->service.type) && ctx->tcp_active_paths == 0)
    {
        POTR_LOG(POTR_LOG_DEBUG,
                 "potrSend: service_id=%d TCP not connected",
                 ctx->service.service_id);
        return POTR_ERROR_DISCONNECTED;
    }

    /* RAW モードは常にブロッキング送信 */
    if (potr_is_raw_type(ctx->service.type))
    {
        flags |= POTR_SEND_BLOCKING;
    }

    /* 圧縮が要求された場合はペイロード全体を圧縮する */
    if ((flags & POTR_SEND_COMPRESS) != 0)
    {
        size_t cmp_len = ctx->compress_buf_size;

        if (potr_compress(ctx->compress_buf, &cmp_len,
                          (const uint8_t *)data, len) != 0)
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "potrSend: service_id=%d compression failed (len=%zu)",
                     ctx->service.service_id, len);
            return POTR_ERROR;
        }

        if (cmp_len < len)
        {
            POTR_LOG(POTR_LOG_TRACE,
                     "potrSend: service_id=%d compress %zu -> %zu bytes",
                     ctx->service.service_id, len, cmp_len);
            ptr        = ctx->compress_buf;
            len        = cmp_len;
            base_flags = POTR_FLAG_COMPRESSED;
        }
        else
        {
            POTR_LOG(POTR_LOG_TRACE,
                     "potrSend: service_id=%d compression skipped"
                     " (compressed %zu >= original %zu bytes), sending uncompressed",
                     ctx->service.service_id, cmp_len, len);
            /* 圧縮効果なし: 非圧縮のまま送信 (ptr, len, base_flags は初期値を維持) */
        }
    }

    /* N:1 モード: peer_id に基づくルーティング */
    if (ctx->is_multi_peer)
    {
        if (peer_id == POTR_PEER_NA)
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "potrSend: service_id=%d N:1 mode requires valid peer_id (got POTR_PEER_NA)",
                     ctx->service.service_id);
            return POTR_ERROR;
        }

        if (peer_id == POTR_PEER_ALL)
        {
            /* 全アクティブピアへ送信: peers_mutex を保持しない状態で送信するため
             * まず peer_id リストを収集してからキューに積む */
            PotrPeerId *ids;
            int         n_ids = 0;
            int         i;
            int         result = POTR_SUCCESS;

            ids = (PotrPeerId *)malloc((size_t)ctx->max_peers * sizeof(PotrPeerId));
            if (ids == NULL)
            {
                POTR_LOG(POTR_LOG_ERROR,
                         "potrSend: service_id=%d PEER_ALL malloc failed",
                         ctx->service.service_id);
                return POTR_ERROR;
            }

            POTR_MUTEX_LOCK_LOCAL(&ctx->peers_mutex);
            for (i = 0; i < ctx->max_peers; i++)
            {
                if (ctx->peers[i].active)
                {
                    ids[n_ids++] = ctx->peers[i].peer_id;
                }
            }
            POTR_MUTEX_UNLOCK_LOCAL(&ctx->peers_mutex);

            for (i = 0; i < n_ids; i++)
            {
                if (send_to_peer(ctx, ids[i], ptr, len, flags, base_flags) != POTR_SUCCESS)
                {
                    result = POTR_ERROR;
                }
            }
            free(ids);
            return result;
        }
        else
        {
            /* 指定ピアへ送信: 存在確認だけ mutex で保護し、送信は mutex 外で行う */
            POTR_MUTEX_LOCK_LOCAL(&ctx->peers_mutex);
            if (peer_find_by_id(ctx, peer_id) == NULL)
            {
                POTR_MUTEX_UNLOCK_LOCAL(&ctx->peers_mutex);
                POTR_LOG(POTR_LOG_ERROR,
                         "potrSend: service_id=%d peer_id=%u not found",
                         ctx->service.service_id, (unsigned)peer_id);
                return POTR_ERROR;
            }
            POTR_MUTEX_UNLOCK_LOCAL(&ctx->peers_mutex);

            return send_to_peer(ctx, peer_id, ptr, len, flags, base_flags);
        }
    }

    /* 1:1 モード: peer_id は無視 */
    return send_to_peer(ctx, POTR_PEER_NA, ptr, len, flags, base_flags);
}
