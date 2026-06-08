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

#include "service-sample.h"

/* ============================================================
 *  サービス コールバック
 * ============================================================ */

/**
 *  @brief          サービス初期化処理の雛形。
 *  @param[in]      user_data 未使用。
 *  @return         常に 0 (成功) を返します。
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
 *
 *  1 秒ごとに動作ログを出力し、停止要求を受け取るとループを抜けます。
 */
static void on_run(void *user_data)
{
    (void)user_data;
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL,
                          "動作中です。Ctrl+C または停止コマンドで終了します。");

    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サービス処理 開始");
    while (svc_wait_for_stop(1000) == 0)
    {
        /* TODO: ここに周期処理を書く (現状は何もしない雛形) */
        com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_VERBOSE, NULL, "動作中...");
    }
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "サービス処理 終了");
}

/**
 *  @brief          サービス停止処理の雛形。
 *  @param[in]      user_data 未使用。
 */
static void on_stop(void *user_data)
{
    (void)user_data;
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "停止処理 開始");
    /* TODO: ここに停止処理を書く */
    com_util_tracer_write(svc_get_tracer(), COM_UTIL_TRACE_LEVEL_INFO, NULL, "停止処理 終了");
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
                                      NULL};
