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

#include <porter_const.h>
#include <porter.h>

#include "../potrContext.h"
#include "../infra/potrSendQueue.h"
#include "../infra/compress/compress.h"
#include "../infra/potrLog.h"

/* doxygen コメントは、ヘッダに記載 */
POTR_API int POTRAPI potrSend(PotrHandle handle, const void *data, size_t len,
                              int flags)
{
    struct PotrContext_ *ctx       = (struct PotrContext_ *)handle;
    const uint8_t       *ptr      = (const uint8_t *)data;
    size_t               remaining;
    size_t               max_payload;
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
             "potrSend: service_id=%d len=%zu flags=0x%x",
             ctx->service.service_id, len, (unsigned)flags);

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

    remaining   = len;
    /* ペイロードエレメントヘッダー (6B) を差し引いた実効フラグメントサイズ上限 */
    max_payload = ctx->global.max_payload - POTR_PAYLOAD_ELEM_HDR_SIZE;
    /* 暗号化有効時は GCM 認証タグ (16B) 分をさらに差し引く */
    if (ctx->service.encrypt_enabled)
    {
        max_payload -= POTR_CRYPTO_TAG_SIZE;
    }

    /* ブロッキング送信: 先行キューの sendto が全完了するまで待機する */
    if ((flags & POTR_SEND_BLOCKING) != 0)
    {
        potr_send_queue_wait_drained(&ctx->send_queue);
    }

    while (remaining > 0)
    {
        size_t   chunk;
        if (remaining > max_payload)
        {
            chunk = max_payload;
        }
        else
        {
            chunk = remaining;
        }
        int      more_frag  = (remaining > chunk);
        uint16_t elem_flags = base_flags;

        if (more_frag)
        {
            elem_flags |= POTR_FLAG_MORE_FRAG;
        }

        if (potr_send_queue_push_wait(&ctx->send_queue, elem_flags, ptr, (uint16_t)chunk,
                                      &ctx->send_thread_running)
            != POTR_SUCCESS)
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "potrSend: service_id=%d send queue push failed"
                     " (send_thread_running=%d)",
                     ctx->service.service_id, ctx->send_thread_running);
            return POTR_ERROR;
        }

        ptr       += chunk;
        remaining -= chunk;
    }

    /* ブロッキング送信: 自身のメッセージの sendto 完了まで待機する */
    if ((flags & POTR_SEND_BLOCKING) != 0)
    {
        potr_send_queue_wait_drained(&ctx->send_queue);
    }

    return POTR_SUCCESS;
}
