/**
 *******************************************************************************
 *  @file           service-sample-impl.c
 *  @brief          クロスプラットフォーム サービスのサンプル処理を実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/28
 *  @version        2.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <assert.h>
#include <stdio.h>

#include <cplat/crt/stdio.h>
#include <cplat/service/service.h>

#include "service-sample.h"

/** 状態テキストを組み立てる配列の大きさ [byte] (終端 NUL を含む)。 */
#define SAMPLE_STATUS_TEXT_SIZE 64U

static_assert(SAMPLE_STATUS_TEXT_SIZE >= 53U, "状態テキストの文面と 20 桁と終端 NUL が収まること");

/** 周期処理の待機間隔 [ms]。 */
#define SAMPLE_CYCLE_INTERVAL_MS 1000

static int on_start(void *user_data);
static int on_run(void *user_data);
static int on_stop(void *user_data);
static void on_event(const cplat_service_event_info *info, void *user_data);
static void on_reload(void *user_data);

/** サービス定義。 */
const cplat_service_definition g_service_def = {"service-sample",
                                                "C Modernization-kit Service Sample",
                                                "c-modernization-kit のクロスプラットフォーム サービス サンプルです。",
                                                on_start,
                                                on_run,
                                                on_stop,
                                                NULL,
                                                on_event,
                                                on_reload,
                                                NULL};

static int on_start(void *user_data)
{
    (void)user_data;

    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "起動処理 開始");
    /* TODO: ここに初期化処理を書く */
    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "起動処理 終了");

    return 0;
}

static int on_run(void *user_data)
{
    char status_text[SAMPLE_STATUS_TEXT_SIZE];
    unsigned long cycle_count = 0;

    (void)user_data;

    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                             "動作中です。Ctrl+C または停止コマンドで終了します。");
    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "サービス処理 開始");

    while (cplat_service_wait_for_stop(SAMPLE_CYCLE_INTERVAL_MS) == CPLAT_ERR_TIMEOUT)
    {
        (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_VERBOSE, NULL, "動作中...");
        cycle_count++;
        (void)cplat_snprintf(status_text, sizeof(status_text), "動作中 (周期処理 %lu 回目)", cycle_count);
        cplat_service_set_status_text(status_text);
    }

    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "サービス処理 終了");

    return 0;
}

static int on_stop(void *user_data)
{
    (void)user_data;

    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "停止処理 開始");
    /* TODO: ここに停止処理を書く */
    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "停止処理 終了");

    return 0;
}

static void on_event(const cplat_service_event_info *info, void *user_data)
{
    (void)user_data;

    switch (info->type)
    {
    case CPLAT_SERVICE_EVENT_POWER_SUSPEND:
        (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "サスペンドを開始します。");
        break;
    case CPLAT_SERVICE_EVENT_POWER_RESUME:
        (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                                 "サスペンドから復帰しました。");
        break;
    case CPLAT_SERVICE_EVENT_SESSION_LOGON:
        (void)cplat_tracer_writef(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                                  "セッションがログオンしました (ID: %s)。", info->session_id);
        break;
    case CPLAT_SERVICE_EVENT_SESSION_LOGOFF:
        (void)cplat_tracer_writef(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                                  "セッションがログオフしました (ID: %s)。", info->session_id);
        break;
    case CPLAT_SERVICE_EVENT_PRESHUTDOWN:
        (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL,
                                 "システムのシャットダウンが始まります。");
        break;
    default:
        (void)cplat_tracer_writef(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_WARNING, NULL,
                                  "未知のイベントです (種別: %d)。", (int)info->type);
        break;
    }
}

static void on_reload(void *user_data)
{
    (void)user_data;

    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "設定再読込 開始");
    /* TODO: ここに設定再読込処理を書く */
    (void)cplat_tracer_write(service_sample_get_tracer(), CPLAT_TRACE_LEVEL_INFO, NULL, "設定再読込 終了");
}
