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
 *  イベント監視スレッドのインターフェイスです。\n
 *  watchdog が要求されている場合は、on_run() の進行 (svc_on_run_made_progress()) を確かめたときだけ
 *  WATCHDOG=1 を送ります。\n
 *  OS イベントと設定再読込は、動作中の段階 (READY=1 の通知後から STOPPING=1 の通知前まで) に限って配送します。\n
 *  Linux (PLATFORM_LINUX) 専用です。実装は service-sample_linux_events.c に
 *  あり、service-sample_linux.c の svc_os_run_service() から使用します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SERVICE_SAMPLE_LINUX_EVENTS_PRIVATE_H
#define SERVICE_SAMPLE_LINUX_EVENTS_PRIVATE_H

#include "service-sample.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          イベント監視スレッドを起動します。
     *  @param[in]      def     サービス定義。NULL の場合は何もせず -1 を返します。
     *  @return         成功時は 0、必須の初期化が失敗した場合は -1 を返します。
     *
     *  スレッドは以下を担当します。\n
     *  - def->on_event が設定されている場合: systemd-logind (D-Bus) の
     *    PrepareForSleep / PrepareForShutdown / SessionNew / SessionRemoved を
     *    監視し、svc_dispatch_event() で配送します。サスペンドとシャットダウンの
     *    delay inhibitor lock も管理します。\n
     *  - SIGHUP を受けて svc_dispatch_reload() を呼びます。def->on_reload が NULL の場合は、
     *    WARNING のトレースを出力して無視します。\n
     *  - WATCHDOG_USEC が設定されている場合: 期限の 1/4 の周期で on_run() の進行を確かめ、
     *    WATCHDOG=1 を送ります。\n
     *  \n
     *  スレッドの必須の初期化 (イベント ループの生成、watchdog が要求されている場合のタイマー登録、
     *  停止監視の登録、SIGHUP の準備) の完了を待ってから戻ります。いずれかが失敗した場合は、
     *  ERROR のトレースを出力して -1 を返します。\n
     *  D-Bus に接続できない環境 (コンテナーなど) では該当イベントのみ
     *  無効化し、スレッド自体は継続します。
     */
    int svc_linux_events_start(const svc_definition *def);

    /**
     *  @brief          イベント監視スレッドを停止して資源を解放します。
     *  @return         成功時は 0、実行中にイベント ループが異常終了していた場合は -1 を返します。
     *
     *  svc_linux_events_start() が失敗していた場合や未起動の場合も安全に
     *  呼び出せます (資源の解放だけを行います)。\n
     *  スレッドの終了を一定時間 (30 [s]) 待機します。
     *
     *  @attention      時間内にスレッドが終了しない場合は、資源を解放せずにプロセスを異常終了させます。
     *                  スレッドが動いたまま終了を続けると、解放済みのサービス定義とトレースを参照するため、
     *                  共通のコーディング規範の「プロセスの継続が危険な場合」に当たります。
     *                  利用側のコールバックの停滞による場合も含みます。
     */
    int svc_linux_events_stop(void);

    /**
     *  @brief          実行中のイベント ループが異常終了したかを返します。
     *  @return         異常終了した場合は 1、それ以外は 0 を返します。
     *
     *  svc_os_notify_ready() が、READY=1 を送るかどうかの判定に使います。
     */
    int svc_linux_events_loop_failed(void);

    /**
     *  @brief          READY=1 の通知に合わせて、動作中の段階へ進めます。
     *
     *  svc_os_notify_ready() が READY=1 を送った後に呼びます。\n
     *  起動中の段階から動作中へ進めるのは最初の 1 回だけです。リロード後の READY=1 の再送では段階を変えません。
     */
    void svc_linux_events_enter_running(void);

    /**
     *  @brief          STOPPING=1 の通知に合わせて、停止中の段階へ進めます。
     *
     *  svc_os_notify_stopping() が STOPPING=1 を送る前に呼びます。
     *  以後は、WATCHDOG=1 を送らず、OS イベントと設定再読込を配送しません。
     */
    void svc_linux_events_enter_stopping(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SERVICE_SAMPLE_LINUX_EVENTS_PRIVATE_H */
