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

#include "potrContext.h"
#include "potrSendQueue.h"
#include "compress/compress.h"
#include "potrLog.h"

/* doxygen コメントは、ヘッダに記載 */
POTR_API int POTRAPI potrSend(PotrHandle handle, const void *data, size_t len,
                              int compress, int blocking)
{
    struct PotrContext_ *ctx       = (struct PotrContext_ *)handle;
    const uint8_t       *ptr      = (const uint8_t *)data;
    size_t               remaining;
    size_t               max_payload;
    uint16_t             base_flags = 0;

    if (ctx == NULL || data == NULL || len == 0 || len > POTR_MAX_MESSAGE_SIZE)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrSend: invalid argument (handle=%p data=%p len=%zu max=%u)",
                 (const void *)handle, data, len, (unsigned)POTR_MAX_MESSAGE_SIZE);
        return POTR_ERROR;
    }

    POTR_LOG(POTR_LOG_TRACE,
             "potrSend: service_id=%d len=%zu compress=%d blocking=%d",
             ctx->service.service_id, len, compress, blocking);

    /* 圧縮が要求された場合はペイロード全体を圧縮する */
    if (compress != 0)
    {
        size_t cmp_len = sizeof(ctx->compress_buf);

        if (potr_compress(ctx->compress_buf, &cmp_len,
                          (const uint8_t *)data, len) != 0)
        {
            POTR_LOG(POTR_LOG_ERROR,
                     "potrSend: service_id=%d compression failed (len=%zu)",
                     ctx->service.service_id, len);
            return POTR_ERROR;
        }

        POTR_LOG(POTR_LOG_TRACE,
                 "potrSend: service_id=%d compress %zu -> %zu bytes",
                 ctx->service.service_id, len, cmp_len);
        ptr        = ctx->compress_buf;
        len        = cmp_len;
        base_flags = POTR_FLAG_COMPRESSED;
    }

    remaining   = len;
    /* ペイロードエレメントヘッダー分を差し引いた実効フラグメントサイズ上限 */
    max_payload = ctx->global.max_payload;
    if (max_payload > POTR_MAX_PAYLOAD - POTR_PAYLOAD_ELEM_HDR_SIZE)
    {
        max_payload = POTR_MAX_PAYLOAD - POTR_PAYLOAD_ELEM_HDR_SIZE;
    }

    /* ブロッキング送信: 先行キューの sendto が全完了するまで待機する */
    if (blocking != 0)
    {
        potr_send_queue_wait_drained(&ctx->send_queue);
    }

    while (remaining > 0)
    {
        size_t   chunk     = (remaining > max_payload) ? max_payload : remaining;
        int      more_frag = (remaining > chunk);
        uint16_t flags     = base_flags;

        if (more_frag)
        {
            flags |= POTR_FLAG_MORE_FRAG;
        }

        if (potr_send_queue_push_wait(&ctx->send_queue, flags, ptr, (uint16_t)chunk,
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
    if (blocking != 0)
    {
        potr_send_queue_wait_drained(&ctx->send_queue);
    }

    return POTR_SUCCESS;
}
