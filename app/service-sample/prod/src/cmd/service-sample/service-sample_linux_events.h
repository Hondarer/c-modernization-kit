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
     *  停止監視の登録、SIGHUP の受信の登録、通知ロックの生成) の完了を待ってから戻ります。いずれかが失敗した場合は、
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
     *  スレッドの終了を一定時間 (30 [s]) 待機します。\n
     *  運転中の D-Bus の呼び出しはイベント ループを止めないため、この待ち時間が必要とする根拠は、
     *  利用者のコールバック (on_event、on_reload) の完了だけです。\n
     *  応答待ちの slot は、応答の完了を待たずに解除します。応答期限は、この待ち時間へ加算しません。
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
     *  @brief          systemd へ通知を送信します。
     *  @param[in]      message 送信する通知文字列。
     *  @return         sd_notify(3) の戻り値を返します。
     *
     *  OS 通知の送信だけを行い、ロック取得とトレース出力は行いません。
     */
    int sd_notify_send(const char *message);

    /**
     *  @brief          動作段階と systemd 通知を保護するロックを取得します。
     *
     *  イベント監視スレッドの生成前にロックを生成します。console モードなどロックが未生成の場合は何もしません。
     *  Linux で通知ロックを実際に取得する関数は、本関数だけです。
     */
    void svc_linux_events_notify_lock(void);

    /**
     *  @brief          動作段階と systemd 通知を保護するロックを解放します。
     *
     *  ロックが未生成の場合は何もしません。
     */
    void svc_linux_events_notify_unlock(void);

    /**
     *  @brief          READY=1 の通知に合わせて、動作中の段階へ進めます。
     *
     *  呼び出し元は通知ロックを保持します。本関数はロックを取得しません。\n
     *  起動中の段階から動作中へ進めるのは最初の 1 回だけです。リロード後の READY=1 の再送では段階を変えません。
     */
    void svc_linux_events_enter_running(void);

    /**
     *  @brief          STOPPING=1 の通知に合わせて、停止中の段階へ進めます。
     *
     *  呼び出し元は通知ロックを保持します。本関数はロックを取得しません。svc_os_notify_stopping() が
     *  STOPPING=1 を送る前に呼びます。
     *  以後は、WATCHDOG=1 を送らず、OS イベントと設定再読込を配送しません。
     */
    void svc_linux_events_enter_stopping(void);

    /**
     *  @brief          通知ロックを保持した状態で、サービスが動作中の段階かを返します。
     *  @return         動作中の場合は 1、それ以外は 0 を返します。
     *
     *  本関数はロックを取得しません。呼び出し元は通知ロックを保持します。
     */
    int svc_linux_events_is_running(void);

    /**
     *  @brief          通知ロックを保持した状態で、サービスが停止中の段階かを返します。
     *  @return         停止中の場合は 1、それ以外は 0 を返します。
     *
     *  本関数はロックを取得しません。呼び出し元は通知ロックを保持します。
     */
    int svc_linux_events_is_stopping(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SERVICE_SAMPLE_LINUX_EVENTS_PRIVATE_H */
