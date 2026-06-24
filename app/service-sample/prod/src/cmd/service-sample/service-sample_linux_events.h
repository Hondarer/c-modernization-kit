/**
 *******************************************************************************
 *  @file           service-sample_linux_events.h
 *  @brief          Linux の OS イベントを監視するスレッドのインターフェイスを宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/06/10
 *  @version        1.0.0
 *
 *  systemd-logind (D-Bus) の電源・セッション・シャットダウン前イベント、
 *  SIGHUP による設定再読込、systemd watchdog への応答を担当する
 *  イベント監視スレッドのインターフェースです。\n
 *  Linux (PLATFORM_LINUX) 専用です。実装は service-sample_linux_events.c に
 *  あり、service-sample_linux.c の svc_os_run_service() から使用します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SERVICE_SAMPLE_LINUX_EVENTS_H
#define SERVICE_SAMPLE_LINUX_EVENTS_H

#include "service-sample.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          イベント監視スレッドを起動します。
     *  @param[in]      def     サービス定義。NULL の場合は何もせず -1 を返します。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  スレッドは以下を担当します。\n
     *  - def->on_event が設定されている場合: systemd-logind (D-Bus) の
     *    PrepareForSleep / PrepareForShutdown / SessionNew / SessionRemoved を
     *    監視し、svc_dispatch_event() で配送します。サスペンドとシャットダウンの
     *    delay inhibitor lock も管理します。\n
     *  - def->on_reload が設定されている場合: SIGHUP を受けて
     *    svc_dispatch_reload() を呼びます。\n
     *  - WATCHDOG_USEC が設定されている場合: systemd watchdog に自動応答します。\n
     *  \n
     *  D-Bus に接続できない環境 (コンテナーなど) では該当イベントのみ
     *  無効化し、スレッド自体は継続します。\n
     *  失敗時もサービス本体の動作には影響しません (該当機能が無効になるだけ)。
     */
    int svc_linux_events_start(const svc_definition *def);

    /**
     *  @brief          イベント監視スレッドを停止して資源を解放します。
     *
     *  svc_linux_events_start() が失敗していた場合や未起動の場合も安全に
     *  呼び出せます (何もしません)。\n
     *  スレッドの終了を一定時間待機し、終了しない場合は切り離して継続します。
     */
    void svc_linux_events_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SERVICE_SAMPLE_LINUX_EVENTS_H */
