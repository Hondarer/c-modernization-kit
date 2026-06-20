/**
 *******************************************************************************
 *  @file           service-sample-impl.c
 *  @brief          クロスプラットフォーム サービス サンプルの実装。
 *  @author         Tetsuo Honda
 *  @date           2026/06/09
 *  @version        1.0.0
 *
 *  サービス定義と、ライフサイクル コールバックを実装します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>

#include "service-sample.h"

/* ============================================================
 *  サービス コールバック
 * ============================================================ */

/**
 *  @brief          サービス初期化処理の雛形。
 *  @param[in]      user_data 未使用。
 *  @return         成功時は 0、失敗時は 0 以外を返します。この雛形では失敗する処理がないため 0 固定で返します。
 */
static int on_start(void *user_data)
{
    (void)user_data;
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "起動処理 開始");
    /* TODO: ここに初期化処理を書く */
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "起動処理 終了");
    return 0;
}

/**
 *  @brief          サービス メインループの雛形。
 *  @param[in]      user_data 未使用。
 *  @return         成功時は 0、失敗時は 0 以外を返します。この雛形では失敗する処理がないため 0 固定で返します。
 *
 *  1 秒ごとに動作ログを出力し、停止要求を受け取るとループを抜けます。\n
 *  svc_set_status_text() による状態テキスト通知の利用例を含みます。
 */
static int on_run(void *user_data)
{
    char status_text[64];
    unsigned long cycle_count;

    (void)user_data;
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                          "動作中です。Ctrl+C または停止コマンドで終了します。");

    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サービス処理 開始");
    cycle_count = 0;
    while (svc_wait_for_stop(1000) == 0)
    {
        /* TODO: ここに周期処理を書く (現状は何もしない雛形) */
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_VERBOSE, NULL, "動作中...");

        /* 状態テキストの通知例 (Linux では systemctl status に表示される) */
        cycle_count++;
        snprintf(status_text, sizeof(status_text), "動作中 (周期処理 %lu 回目)", cycle_count);
        svc_set_status_text(status_text);
    }
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サービス処理 終了");
    return 0;
}

/**
 *  @brief          サービス停止処理の雛形。
 *  @param[in]      user_data 未使用。
 *  @return         成功時は 0、失敗時は 0 以外を返します。この雛形では失敗する処理がないため 0 固定で返します。
 */
static int on_stop(void *user_data)
{
    (void)user_data;
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "停止処理 開始");
    /* TODO: ここに停止処理を書く */
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "停止処理 終了");
    return 0;
}

/**
 *  @brief          OS イベント処理の雛形。
 *  @param[in]      info        イベントの付加情報。
 *  @param[in]      user_data   未使用。
 *
 *  on_run() とは別のスレッドから呼ばれます。短時間で戻るように実装します。
 */
static void on_event(const svc_event_info *info, void *user_data)
{
    (void)user_data;

    switch (info->type)
    {
    case SVC_EVENT_POWER_SUSPEND:
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サスペンドを開始します。");
        /* TODO: ここにサスペンド前処理を書く */
        break;
    case SVC_EVENT_POWER_RESUME:
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サスペンドから復帰しました。");
        /* TODO: ここに復帰処理を書く */
        break;
    case SVC_EVENT_SESSION_LOGON:
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                               "セッションがログオンしました (ID: %s)。", info->session_id);
        /* TODO: ここにログオン時処理を書く */
        break;
    case SVC_EVENT_SESSION_LOGOFF:
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                               "セッションがログオフしました (ID: %s)。", info->session_id);
        /* TODO: ここにログオフ時処理を書く */
        break;
    case SVC_EVENT_PRESHUTDOWN:
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                              "システムのシャットダウンが始まります。");
        /* TODO: ここにシャットダウン前処理を書く */
        break;
    default:
        com_util_tracer_writef(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_WARNING, NULL, "未知のイベントです (種別: %d)。",
                               (int)info->type);
        break;
    }
}

/**
 *  @brief          設定再読込処理の雛形。
 *  @param[in]      user_data   未使用。
 *
 *  on_run() とは別のスレッドから呼ばれます。短時間で戻るように実装します。
 */
static void on_reload(void *user_data)
{
    (void)user_data;
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "設定再読込 開始");
    /* TODO: ここに設定再読込処理を書く */
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "設定再読込 終了");
}

/* ============================================================
 *  サービス定義
 * ============================================================ */

/** サービス定義。 */
const svc_definition g_service_def = {"service-sample",
                                      "C Modernization-kit Service Sample",
                                      "c-modernization-kit のクロスプラットフォーム サービス サンプルです。",
                                      on_start,
                                      on_run,
                                      on_stop,
                                      NULL,
                                      on_event,
                                      on_reload};
