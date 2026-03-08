/**
 *******************************************************************************
 *  @file           potrHealthThread.c
 *  @brief          ヘルスチェックスレッドモジュール。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/08
 *  @version        1.0.0
 *
 *  @details
 *  送信者側で動作する定周期 PING 送信スレッドです。\n
 *  health_interval_ms 周期で PING パケットを送信します。\n
 *  受信者への応答要求は行いません (一方向ヘルスチェック)。
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
    #include <time.h>
#endif

#include <porter_const.h>
#include <porter.h>

#include "protocol/packet.h"
#include "protocol/window.h"
#include "potrContext.h"
#include "potrHealthThread.h"

/* health_interval_ms ミリ秒スリープする */
static void health_sleep(uint32_t interval_ms)
{
#ifdef _WIN32
    Sleep((DWORD)interval_ms);
#else
    struct timespec ts;
    ts.tv_sec  = (time_t)(interval_ms / 1000U);
    ts.tv_nsec = (long)((interval_ms % 1000U) * 1000000UL);
    nanosleep(&ts, NULL);
#endif
}

/* ヘルスチェックスレッド本体 */
#ifdef _WIN32
static DWORD WINAPI health_thread_func(LPVOID arg)
#else
static void *health_thread_func(void *arg)
#endif
{
    struct PotrContext_ *ctx = (struct PotrContext_ *)arg;
    PotrPacketSessionHdr shdr;

    shdr.service_id      = ctx->service.service_id;
    shdr.session_id      = ctx->session_id;
    shdr.session_tv_sec  = ctx->session_tv_sec;
    shdr.session_tv_nsec = ctx->session_tv_nsec;

    while (ctx->health_running)
    {
        health_sleep(ctx->global.health_interval_ms);

        if (!ctx->health_running)
        {
            break;
        }

        {
            PotrPacket ping_pkt;
            uint32_t   seq      = ctx->send_window.next_seq;
            size_t     wire_len;

            if (packet_build_ping(&ping_pkt, &shdr, seq) != POTR_SUCCESS)
            {
                continue;
            }

            if (window_send_push(&ctx->send_window, &ping_pkt) != POTR_SUCCESS)
            {
                continue;
            }

            wire_len = packet_wire_size(&ping_pkt);

#ifdef _WIN32
            sendto(ctx->sock, (const char *)&ping_pkt, (int)wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr,
                   sizeof(ctx->dest_addr));
#else
            sendto(ctx->sock, &ping_pkt, wire_len, 0,
                   (const struct sockaddr *)&ctx->dest_addr,
                   sizeof(ctx->dest_addr));
#endif
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
 *  @brief          ヘルスチェックスレッドを起動します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  ctx->global.health_interval_ms が 0 の場合は起動しません (POTR_SUCCESS を返します)。
 *******************************************************************************
 */
int potr_health_thread_start(struct PotrContext_ *ctx)
{
    if (ctx == NULL)
    {
        return POTR_ERROR;
    }

    if (ctx->global.health_interval_ms == 0)
    {
        return POTR_SUCCESS; /* ヘルスチェック無効 */
    }

    ctx->health_running = 1;

#ifdef _WIN32
    ctx->health_thread = CreateThread(NULL, 0, health_thread_func, ctx, 0, NULL);
    if (ctx->health_thread == NULL)
    {
        ctx->health_running = 0;
        return POTR_ERROR;
    }
#else
    if (pthread_create(&ctx->health_thread, NULL, health_thread_func, ctx) != 0)
    {
        ctx->health_running = 0;
        return POTR_ERROR;
    }
#endif

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          ヘルスチェックスレッドを停止します。
 *  @param[in,out]  ctx  セッションコンテキストへのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *******************************************************************************
 */
int potr_health_thread_stop(struct PotrContext_ *ctx)
{
    if (ctx == NULL)
    {
        return POTR_ERROR;
    }

    if (!ctx->health_running)
    {
        return POTR_SUCCESS;
    }

    ctx->health_running = 0;

#ifdef _WIN32
    if (ctx->health_thread != NULL)
    {
        WaitForSingleObject(ctx->health_thread, (DWORD)(ctx->global.health_interval_ms + 1000U));
        CloseHandle(ctx->health_thread);
        ctx->health_thread = NULL;
    }
#else
    pthread_join(ctx->health_thread, NULL);
#endif

    return POTR_SUCCESS;
}
