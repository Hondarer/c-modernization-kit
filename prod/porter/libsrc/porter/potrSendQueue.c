/**
 *******************************************************************************
 *  @file           potrSendQueue.c
 *  @brief          非同期送信キューの実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/08
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <string.h>

#ifndef _WIN32
    #include <time.h>
#endif

#include <porter_const.h>

#include "potrSendQueue.h"

/* --------------------------------------------------------------------------
 * プラットフォーム別 ミューテックス・条件変数 ラッパーマクロ
 * -------------------------------------------------------------------------- */
#ifdef _WIN32
    #define POTR_MUTEX_INIT(m)     InitializeCriticalSection(m)
    #define POTR_MUTEX_LOCK(m)     EnterCriticalSection(m)
    #define POTR_MUTEX_UNLOCK(m)   LeaveCriticalSection(m)
    #define POTR_MUTEX_DESTROY(m)  DeleteCriticalSection(m)
    #define POTR_COND_INIT(c)      InitializeConditionVariable(c)
    #define POTR_COND_WAIT(c, m)   SleepConditionVariableCS((c), (m), INFINITE)
    #define POTR_COND_SIGNAL(c)    WakeConditionVariable(c)
    #define POTR_COND_BROADCAST(c) WakeAllConditionVariable(c)
    #define POTR_COND_DESTROY(c)   ((void)0) /* Windows は破棄不要 */
#else
    #define POTR_MUTEX_INIT(m)     pthread_mutex_init((m), NULL)
    #define POTR_MUTEX_LOCK(m)     pthread_mutex_lock(m)
    #define POTR_MUTEX_UNLOCK(m)   pthread_mutex_unlock(m)
    #define POTR_MUTEX_DESTROY(m)  pthread_mutex_destroy(m)
    #define POTR_COND_INIT(c)      pthread_cond_init((c), NULL)
    #define POTR_COND_WAIT(c, m)   pthread_cond_wait((c), (m))
    #define POTR_COND_SIGNAL(c)    pthread_cond_signal(c)
    #define POTR_COND_BROADCAST(c) pthread_cond_broadcast(c)
    #define POTR_COND_DESTROY(c)   pthread_cond_destroy(c)
#endif

/* doxygen コメントは、ヘッダに記載 */
int potr_send_queue_init(PotrSendQueue *q)
{
    memset(q, 0, sizeof(*q));
    POTR_MUTEX_INIT(&q->mutex);
    POTR_COND_INIT(&q->not_empty);
    POTR_COND_INIT(&q->not_full);
    POTR_COND_INIT(&q->drained);
    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
void potr_send_queue_destroy(PotrSendQueue *q)
{
    POTR_COND_DESTROY(&q->drained);
    POTR_COND_DESTROY(&q->not_full);
    POTR_COND_DESTROY(&q->not_empty);
    POTR_MUTEX_DESTROY(&q->mutex);
}

/* doxygen コメントは、ヘッダに記載 */
int potr_send_queue_push(PotrSendQueue *q, uint16_t flags,
                         const void *payload, uint16_t payload_len)
{
    POTR_MUTEX_LOCK(&q->mutex);

    if (q->count >= POTR_SEND_QUEUE_DEPTH)
    {
        POTR_MUTEX_UNLOCK(&q->mutex);
        return POTR_ERROR;
    }

    q->entries[q->tail].flags       = flags;
    q->entries[q->tail].payload_len = payload_len;
    memcpy(q->entries[q->tail].payload, payload, payload_len);
    q->tail = (q->tail + 1U) % POTR_SEND_QUEUE_DEPTH;
    q->count++;

    POTR_COND_SIGNAL(&q->not_empty);
    POTR_MUTEX_UNLOCK(&q->mutex);

    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
int potr_send_queue_push_wait(PotrSendQueue *q, uint16_t flags,
                              const void *payload, uint16_t payload_len,
                              volatile int *running)
{
    POTR_MUTEX_LOCK(&q->mutex);

    while (q->count >= POTR_SEND_QUEUE_DEPTH)
    {
        if (!*running)
        {
            POTR_MUTEX_UNLOCK(&q->mutex);
            return POTR_ERROR;
        }
        POTR_COND_WAIT(&q->not_full, &q->mutex);
    }

    q->entries[q->tail].flags       = flags;
    q->entries[q->tail].payload_len = payload_len;
    memcpy(q->entries[q->tail].payload, payload, payload_len);
    q->tail = (q->tail + 1U) % POTR_SEND_QUEUE_DEPTH;
    q->count++;

    POTR_COND_SIGNAL(&q->not_empty);
    POTR_MUTEX_UNLOCK(&q->mutex);

    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
int potr_send_queue_pop(PotrSendQueue *q, PotrPayloadElem *out, volatile int *running)
{
    POTR_MUTEX_LOCK(&q->mutex);

    while (q->count == 0)
    {
        if (!*running)
        {
            POTR_MUTEX_UNLOCK(&q->mutex);
            return POTR_ERROR;
        }
        POTR_COND_WAIT(&q->not_empty, &q->mutex);
    }

    *out    = q->entries[q->head];
    q->head = (q->head + 1U) % POTR_SEND_QUEUE_DEPTH;
    q->count--;
    q->inflight++;

    POTR_COND_SIGNAL(&q->not_full);
    POTR_MUTEX_UNLOCK(&q->mutex);
    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
int potr_send_queue_peek(PotrSendQueue *q, PotrPayloadElem *out)
{
    POTR_MUTEX_LOCK(&q->mutex);

    if (q->count == 0)
    {
        POTR_MUTEX_UNLOCK(&q->mutex);
        return POTR_ERROR;
    }

    *out = q->entries[q->head]; /* head は送信スレッドのみが変更するので安全 */

    POTR_MUTEX_UNLOCK(&q->mutex);
    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
int potr_send_queue_peek_timed(PotrSendQueue *q, PotrPayloadElem *out,
                               uint32_t timeout_ms)
{
    POTR_MUTEX_LOCK(&q->mutex);

    if (q->count == 0)
    {
#ifdef _WIN32
        SleepConditionVariableCS(&q->not_empty, &q->mutex, (DWORD)timeout_ms);
#else
        struct timespec abs_ts;
        clock_gettime(CLOCK_REALTIME, &abs_ts);
        abs_ts.tv_sec  += (time_t)(timeout_ms / 1000U);
        abs_ts.tv_nsec += (long)((timeout_ms % 1000U) * 1000000UL);
        if (abs_ts.tv_nsec >= 1000000000L)
        {
            abs_ts.tv_sec++;
            abs_ts.tv_nsec -= 1000000000L;
        }
        pthread_cond_timedwait(&q->not_empty, &q->mutex, &abs_ts);
#endif
    }

    if (q->count == 0)
    {
        POTR_MUTEX_UNLOCK(&q->mutex);
        return POTR_ERROR;
    }

    *out = q->entries[q->head];

    POTR_MUTEX_UNLOCK(&q->mutex);
    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
int potr_send_queue_try_pop(PotrSendQueue *q, PotrPayloadElem *out)
{
    POTR_MUTEX_LOCK(&q->mutex);

    if (q->count == 0)
    {
        POTR_MUTEX_UNLOCK(&q->mutex);
        return POTR_ERROR;
    }

    *out    = q->entries[q->head];
    q->head = (q->head + 1U) % POTR_SEND_QUEUE_DEPTH;
    q->count--;
    q->inflight++;

    POTR_COND_SIGNAL(&q->not_full);
    POTR_MUTEX_UNLOCK(&q->mutex);
    return POTR_SUCCESS;
}

/* doxygen コメントは、ヘッダに記載 */
void potr_send_queue_complete(PotrSendQueue *q)
{
    POTR_MUTEX_LOCK(&q->mutex);

    if (q->inflight > 0U)
    {
        q->inflight--;
    }

    if (q->count == 0U && q->inflight == 0U)
    {
        POTR_COND_BROADCAST(&q->drained);
    }

    POTR_MUTEX_UNLOCK(&q->mutex);
}

/* doxygen コメントは、ヘッダに記載 */
void potr_send_queue_wait_drained(PotrSendQueue *q)
{
    POTR_MUTEX_LOCK(&q->mutex);

    while (q->count > 0U || q->inflight > 0U)
    {
        POTR_COND_WAIT(&q->drained, &q->mutex);
    }

    POTR_MUTEX_UNLOCK(&q->mutex);
}

/* doxygen コメントは、ヘッダに記載 */
void potr_send_queue_shutdown(PotrSendQueue *q)
{
    POTR_MUTEX_LOCK(&q->mutex);
    POTR_COND_BROADCAST(&q->not_empty);
    POTR_COND_BROADCAST(&q->not_full);
    POTR_MUTEX_UNLOCK(&q->mutex);
}
