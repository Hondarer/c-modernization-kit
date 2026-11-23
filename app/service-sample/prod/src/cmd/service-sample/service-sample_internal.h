/**
 *******************************************************************************
 *  @file           service-sample_internal.h
 *  @brief          service-sample の OS 別の実装で共有する定数を定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/21
 *  @version        1.0.0
 *
 *  service-sample_linux.c と service-sample_windows.c が共有する定数です。\n
 *  OS をまたいで同じ意味の値を 1 か所で定義します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SERVICE_SAMPLE_INTERNAL_H
#define SERVICE_SAMPLE_INTERNAL_H

/** 実行ファイル パスを格納する配列の大きさ [byte] (終端 NUL を含む)。 */
#define SVC_EXEC_PATH_MAX 4096U

/**
 *  異常終了から自動再起動までの遅延 [s]。
 *
 *  Linux では unit の RestartSec= に、Windows では SCM の failure actions の遅延に使います。
 */
#define SVC_FAILURE_RESTART_DELAY_SEC 5U

/**
 *  unit の WatchdogSec= に書く秒数。
 *
 *  service-sample_linux.c の unit 生成と、イベント監視スレッドの停止待ちの static_assert が、同じ値を使います。
 */
#define SVC_UNIT_WATCHDOG_SEC 30U

#endif /* SERVICE_SAMPLE_INTERNAL_H */
