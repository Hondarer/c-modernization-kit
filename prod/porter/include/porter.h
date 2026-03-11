/**
 *******************************************************************************
 *  @file           porter.h
 *  @brief          通信ライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  このライブラリは UDP/IP を用いたプラットフォーム間通信を提供します。\n
 *  Linux / Windows クロスプラットフォーム対応の DLL として提供されます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PORTER_H
#define PORTER_H

#include <porter_type.h>

/**
 *  @def            POTR_API
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                  | 値                       |
 *  | ----------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                    | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                   | (空)                     |
 *  | Windows / `POTR_STATIC` 定義時 (静的リンク)           | (空)                     |
 *  | Windows / `POTR_EXPORTS` 定義時 (DLL ビルド)          | `__declspec(dllexport)`  |
 *  | Windows / `POTR_EXPORTS` 未定義時 (DLL 利用側)        | `__declspec(dllimport)`  |
 */

/**
 *  @def            POTRAPI
 *  @brief          Windows 呼び出し規約マクロ。
 *
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#ifndef _WIN32
    #define POTR_API
    #define POTRAPI
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef POTR_STATIC
            #ifdef POTR_EXPORTS
                #define POTR_API __declspec(dllexport)
            #else /* POTR_EXPORTS */
                #define POTR_API __declspec(dllimport)
            #endif /* POTR_EXPORTS */
        #else      /* POTR_STATIC */
            #define POTR_API
        #endif /* POTR_STATIC */
    #else      /* __INTELLISENSE__ */
        #define POTR_API
    #endif /* __INTELLISENSE__ */
    #ifndef POTRAPI
        #define POTRAPI __stdcall
    #endif /* POTRAPI */
#endif     /* _WIN32 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *******************************************************************************
     *  @brief          設定ファイルから指定サービスを開きます。
     *  @param[in]      config_path 設定ファイルのパス。
     *  @param[in]      service_id  開くサービスの ID。
     *  @param[in]      role        役割種別。POTR_ROLE_SENDER または POTR_ROLE_RECEIVER。
     *  @param[in]      callback    イベント発生時に呼び出されるコールバック関数 (PotrRecvCallback)。
     *                              POTR_ROLE_RECEIVER の場合は必須。データ受信・接続検知・切断検知を受け取る。
     *                              POTR_ROLE_SENDER の場合は NULL を指定すること。
     *  @param[out]     handle      成功時にセッションハンドルを格納するポインタ。
     *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
     *
     *  @details
     *  設定ファイルを解析してサービス定義を取得し、UDP ソケットを初期化します。\n
     *  role と callback の組み合わせが不正な場合は POTR_ERROR を返します。\n
     *  role と設定ファイルの IP アドレスが不整合 (bind 失敗など) の場合も POTR_ERROR を返します。\n
     *  通信種別・役割に応じて以下のソケット設定を行います。
     *
     *  | 通信種別              | 役割     | bind アドレス     | bind ポート   |
     *  | --------------------- | -------- | ----------------- | ------------- |
     *  | POTR_TYPE_UNICAST     | 送信者   | src_addr          | src_port      |
     *  | POTR_TYPE_UNICAST     | 受信者   | dst_addr          | dst_port      |
     *  | POTR_TYPE_MULTICAST   | 送信者   | INADDR_ANY        | src_port      |
     *  | POTR_TYPE_MULTICAST   | 受信者   | INADDR_ANY        | dst_port      |
     *  | POTR_TYPE_BROADCAST   | 送信者   | src_addr          | src_port      |
     *  | POTR_TYPE_BROADCAST   | 受信者   | INADDR_ANY        | dst_port      |
     *
     *  POTR_ROLE_RECEIVER の場合、内部で受信スレッドを起動します。
     *
     *  @par            使用例 (受信者)
     *  @code{.c}
     *  void on_recv(int service_id, PotrEvent event,
     *               const void *data, size_t len) {
     *      if (event == POTR_EVENT_CONNECTED)
     *          printf("service %d: connected\n", service_id);
     *      else if (event == POTR_EVENT_DISCONNECTED)
     *          printf("service %d: disconnected\n", service_id);
     *      else
     *          printf("service %d: received %zu bytes\n", service_id, len);
     *  }
     *
     *  PotrHandle handle;
     *  if (potrOpenService("porter-services.conf", 1001,
     *                      POTR_ROLE_RECEIVER, on_recv, &handle) == POTR_SUCCESS) {
     *      // 受信待機中 (受信スレッドが動作)
     *      potrCloseService(handle);
     *  }
     *  @endcode
     *
     *  @par            使用例 (送信者)
     *  @code{.c}
     *  PotrHandle handle;
     *  if (potrOpenService("porter-services.conf", 1001,
     *                      POTR_ROLE_SENDER, NULL, &handle) == POTR_SUCCESS) {
     *      potrSend(handle, "hello", 5, 0);
     *      potrCloseService(handle);
     *  }
     *  @endcode
     *
     *  @warning        handle が NULL の場合は失敗を返します。\n
     *                  config_path が NULL または存在しない場合は失敗を返します。\n
     *                  指定した service_id が設定ファイルに存在しない場合は失敗を返します。\n
     *                  POTR_ROLE_RECEIVER かつ callback が NULL の場合は失敗を返します。\n
     *                  POTR_ROLE_SENDER かつ callback が NULL でない場合は失敗を返します。
     *******************************************************************************
     */
    POTR_API extern int POTRAPI potrOpenService(const char       *config_path,
                                                int               service_id,
                                                PotrRole          role,
                                                PotrRecvCallback  callback,
                                                PotrHandle       *handle);

    /**
     *******************************************************************************
     *  @brief          メッセージを送信します。
     *  @param[in]      handle      potrOpenService() で取得したセッションハンドル。
     *  @param[in]      data        送信するメッセージへのポインタ。
     *  @param[in]      len         送信するメッセージのバイト数。
     *  @param[in]      compress    0 以外を指定するとメッセージを圧縮して送信します。
     *                              0 を指定すると非圧縮で送信します。
     *  @param[in]      blocking    0 以外を指定するとブロッキング送信を行います。
     *                              0 を指定するとノンブロッキング送信を行います。
     *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
     *
     *  @details
     *  通信種別に応じて以下の宛先へ UDP パケットを送信します。
     *
     *  | 通信種別              | 送信先                                     |
     *  | --------------------- | ------------------------------------------ |
     *  | POTR_TYPE_UNICAST     | 接続相手の dst_port 宛にユニキャスト送信   |
     *  | POTR_TYPE_MULTICAST   | multicast_group:dst_port へ送信            |
     *  | POTR_TYPE_BROADCAST   | broadcast_addr:dst_port へ送信             |
     *
     *  compress に 0 以外を指定した場合、内部で圧縮処理を行ってから送信します。\n
     *  受信側の PotrRecvCallback には、解凍済みの元メッセージが渡されます。\n
     *  送受信ともにフラグメント化と組み合わせて使用できます。
     *
     *  @par            ノンブロッキング送信 (blocking = 0)
     *  メッセージを内部送信キューに登録して即座に返ります。\n
     *  実際の sendto はバックグラウンド送信スレッドが非同期に実行します。\n
     *  キューが満杯の場合は空きが生じるまで待機してからメッセージを登録し、登録後に返ります。
     *
     *  @par            ブロッキング送信 (blocking != 0)
     *  呼び出し前に滞留しているノンブロッキング送信のメッセージが
     *  すべて sendto 完了するまで待機します。\n
     *  その後、本呼び出しのメッセージをキューを通じて sendto して返ります。\n
     *  本関数が返った時点で、自身のメッセージの sendto は完了しています。
     *
     *  @note
     *  圧縮フォーマットには raw DEFLATE (RFC 1951) を使用します。\n
     *  Linux (zlib) と Windows (Compression API MSZIP|COMPRESS_RAW) は
     *  同一フォーマットを出力するため、クロスプラットフォーム通信に対応します。\n
     *  本関数はシングルスレッドから呼び出すことを前提としています。
     *  複数スレッドから同時に呼び出した場合の動作は未定義です。
     *
     *  @warning        handle が NULL の場合は失敗を返します。\n
     *                  data が NULL の場合は失敗を返します。\n
     *                  len が 0 の場合は失敗を返します。\n
     *                  len が POTR_MAX_MESSAGE_SIZE を超える場合は失敗を返します。\n
     *                  送信スレッドが停止している場合 (potrCloseService 呼び出し後など) は失敗を返します。
     *******************************************************************************
     */
    POTR_API extern int POTRAPI potrSend(PotrHandle  handle,
                                         const void *data,
                                         size_t      len,
                                         int         compress,
                                         int         blocking);

    /**
     *******************************************************************************
     *  @brief          サービスを閉じます。
     *  @param[in]      handle  potrOpenService() で取得したセッションハンドル。
     *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
     *
     *  @details
     *  受信スレッドを停止し、ソケットをクローズしてリソースを解放します。\n
     *  マルチキャストの場合はグループから離脱します。\n
     *  本関数呼び出し後、handle は無効となります。
     *
     *  @note
     *  本関数呼び出し時、POTR_EVENT_DISCONNECTED コールバックは発火しません。\n
     *  アプリケーション自身が明示的にサービスを閉じる操作は、接続状態変化イベントとして通知しない設計です。\n
     *  送信者 (POTR_ROLE_SENDER) が本関数を呼び出した場合は、FIN パケットが送信されます。
     *  FIN を受信した受信者側では POTR_EVENT_DISCONNECTED が発火します。
     *
     *  @warning        handle が NULL の場合は失敗を返します。
     *******************************************************************************
     */
    POTR_API extern int POTRAPI potrCloseService(PotrHandle handle);

    /**
     *******************************************************************************
     *  @brief          ロガーを設定します。
     *  @param[in]      level       出力する最低ログレベル。POTR_LOG_OFF でログ無効 (デフォルト)。
     *  @param[in]      log_file    ログファイルのパス。NULL または空文字列を指定するとファイル出力なし。
     *  @param[in]      console     0 以外を指定すると標準エラー出力 (stderr) にも出力します。
     *  @return         成功時は POTR_SUCCESS、log_file が開けない場合は POTR_ERROR を返します。
     *
     *  @details
     *  本関数は potrOpenService() の前に呼び出してください。\n
     *  複数回呼び出した場合は最後の設定が有効になります。\n
     *  スレッドセーフです。
     *
     *  @par            出力先
     *  | OS      | 出力先                                                                    |
     *  | ------- | ------------------------------------------------------------------------- |
     *  | Linux   | syslog (常時)、ログファイル (log_file 指定時)、stderr (console 指定時)   |
     *  | Windows | ログファイル (log_file 指定時)、stderr (console 指定時)                   |
     *
     *  @par            ログフォーマット (ファイル / stderr)
     *  @code
     *  [YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] [file.c:line] message
     *  @endcode
     *
     *  @par            ログレベル一覧
     *  | レベル          | 値 | syslog priority | 出力内容                          |
     *  | --------------- | -- | --------------- | --------------------------------- |
     *  | POTR_LOG_TRACE  |  0 | LOG_DEBUG       | パケット送受信・スレッド動作      |
     *  | POTR_LOG_DEBUG  |  1 | LOG_DEBUG       | ソケット操作・設定値              |
     *  | POTR_LOG_INFO   |  2 | LOG_INFO        | サービス開始・終了・接続状態変化  |
     *  | POTR_LOG_WARN   |  3 | LOG_WARNING     | NACK・REJECT・回復可能な異常      |
     *  | POTR_LOG_ERROR  |  4 | LOG_ERR         | 操作失敗                          |
     *  | POTR_LOG_FATAL  |  5 | LOG_CRIT        | 致命的エラー                      |
     *  | POTR_LOG_OFF    |  6 | -               | ログ無効 (デフォルト)             |
     *
     *  @par            使用例
     *  @code{.c}
     *  // INFO 以上をファイルと stderr に出力
     *  potrLogConfig(POTR_LOG_INFO, "/var/log/porter.log", 1);
     *
     *  // DEBUG 以上をファイルのみに出力 (Linux では syslog にも出力)
     *  potrLogConfig(POTR_LOG_DEBUG, "/tmp/porter.log", 0);
     *
     *  // ログを無効化
     *  potrLogConfig(POTR_LOG_OFF, NULL, 0);
     *  @endcode
     *
     *  @warning        log_file に指定したパスが書き込み不可の場合は POTR_ERROR を返します。\n
     *                  本関数はロガー状態を再設定するため、呼び出し中にログを出力するスレッドが
     *                  存在する場合でも安全に動作します (内部でミューテックスを使用)。
     *******************************************************************************
     */
    POTR_API extern int POTRAPI potrLogConfig(PotrLogLevel  level,
                                              const char   *log_file,
                                              int           console);

#ifdef __cplusplus
}
#endif

#endif /* PORTER_H */
