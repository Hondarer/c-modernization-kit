/**
 *******************************************************************************
 *  @file           potrDisconnectPeer.c
 *  @brief          potrDisconnectPeer 関数の実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/23
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <porter_const.h>
#include <porter.h>

#include "../potrContext.h"
#include "../potrPeerTable.h"
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

/* doxygen コメントは、ヘッダに記載 */
POTR_EXPORT int POTR_API potrDisconnectPeer(PotrHandle handle, PotrPeerId peer_id)
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)handle;

    if (ctx == NULL)
    {
        POTR_LOG(POTR_LOG_ERROR, "potrDisconnectPeer: handle is NULL");
        return POTR_ERROR;
    }

    if (peer_id == POTR_PEER_NA || peer_id == POTR_PEER_ALL)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrDisconnectPeer: service_id=%d invalid peer_id=%u"
                 " (POTR_PEER_NA or POTR_PEER_ALL not allowed)",
                 ctx != NULL ? ctx->service.service_id : 0, (unsigned)peer_id);
        return POTR_ERROR;
    }

    if (!ctx->is_multi_peer)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrDisconnectPeer: service_id=%d not in N:1 mode",
                 ctx->service.service_id);
        return POTR_ERROR;
    }

    POTR_MUTEX_LOCK_LOCAL(&ctx->peers_mutex);

    {
        PotrPeerContext *peer = peer_find_by_id(ctx, peer_id);

        if (peer == NULL)
        {
            POTR_MUTEX_UNLOCK_LOCAL(&ctx->peers_mutex);
            POTR_LOG(POTR_LOG_ERROR,
                     "potrDisconnectPeer: service_id=%d peer_id=%u not found",
                     ctx->service.service_id, (unsigned)peer_id);
            return POTR_ERROR;
        }

        POTR_LOG(POTR_LOG_INFO,
                 "potrDisconnectPeer: service_id=%d peer_id=%u disconnecting",
                 ctx->service.service_id, (unsigned)peer_id);

        /* FIN を送信 */
        peer_send_fin(ctx, peer);

        /* 接続済み状態のみ DISCONNECTED コールバックを発火 */
        if (peer->health_alive && ctx->callback != NULL)
        {
            peer->health_alive = 0;
            ctx->callback(ctx->service.service_id, peer->peer_id,
                          POTR_EVENT_DISCONNECTED, NULL, 0);
        }

        /* ピアリソースを解放 */
        peer_free(ctx, peer);
    }

    POTR_MUTEX_UNLOCK_LOCAL(&ctx->peers_mutex);
    return POTR_SUCCESS;
}
