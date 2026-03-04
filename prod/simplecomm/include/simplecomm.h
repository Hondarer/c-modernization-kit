/**
 *******************************************************************************
 *  @file           simplecomm.h
 *  @brief          簡易通信ライブラリ (動的リンク用) のヘッダーファイル。
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

#ifndef LIBSIMPLECOMM_H
#define LIBSIMPLECOMM_H

#include <simplecomm_type.h>

/**
 *  @def            COMM_API
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                  | 値                       |
 *  | ----------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                    | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                   | (空)                     |
 *  | Windows / `COMM_STATIC` 定義時 (静的リンク)           | (空)                     |
 *  | Windows / `COMM_EXPORTS` 定義時 (DLL ビルド)          | `__declspec(dllexport)`  |
 *  | Windows / `COMM_EXPORTS` 未定義時 (DLL 利用側)        | `__declspec(dllimport)`  |
 */

/**
 *  @def            COMMAPI
 *  @brief          Windows 呼び出し規約マクロ。
 *
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#ifndef _WIN32
    #define COMM_API
    #define COMMAPI
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef COMM_STATIC
            #ifdef COMM_EXPORTS
                #define COMM_API __declspec(dllexport)
            #else /* COMM_EXPORTS */
                #define COMM_API __declspec(dllimport)
            #endif /* COMM_EXPORTS */
        #else      /* COMM_STATIC */
            #define COMM_API
        #endif /* COMM_STATIC */
    #else      /* __INTELLISENSE__ */
        #define COMM_API
    #endif /* __INTELLISENSE__ */
    #ifndef COMMAPI
        #define COMMAPI __stdcall
    #endif /* COMMAPI */
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
     *  @param[in]      callback    データ受信時に呼び出されるコールバック関数。
     *                              受信不要の場合は NULL を指定可能。
     *  @param[out]     handle      成功時にセッションハンドルを格納するポインタ。
     *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
     *
     *  @details
     *  設定ファイルを解析してサービス定義を取得し、UDP ソケットを初期化します。\n
     *  通信種別に応じて以下の処理を行います。
     *
     *  | 通信種別              | ソケット設定                                     |
     *  | --------------------- | ------------------------------------------------ |
     *  | COMM_TYPE_UNICAST     | dst_port をバインド                              |
     *  | COMM_TYPE_MULTICAST   | src_port をバインド、グループ参加 (IGMP)        |
     *  | COMM_TYPE_BROADCAST   | src_port をバインド、SO_BROADCAST を設定        |
     *
     *  受信コールバックが指定された場合、内部で受信スレッドを起動します。
     *
     *  @par            使用例
     *  @code{.c}
     *  void on_recv(int service_id, const void *data, size_t len) {
     *      printf("service %d: received %zu bytes\n", service_id, len);
     *  }
     *
     *  CommHandle handle;
     *  if (commOpenService("simplecomm-services.conf", 1, on_recv, &handle)
     *          == COMM_SUCCESS) {
     *      // 通信処理
     *      commClose(handle);
     *  }
     *  @endcode
     *
     *  @warning        handle が NULL の場合は失敗を返します。\n
     *                  config_path が NULL または存在しない場合は失敗を返します。\n
     *                  指定した service_id が設定ファイルに存在しない場合は失敗を返します。
     *******************************************************************************
     */
    COMM_API extern int COMMAPI commOpenService(const char       *config_path,
                                                int               service_id,
                                                CommRecvCallback  callback,
                                                CommHandle       *handle);

    /**
     *******************************************************************************
     *  @brief          データを送信します。
     *  @param[in]      handle  commOpenService() で取得したセッションハンドル。
     *  @param[in]      data    送信するデータへのポインタ。
     *  @param[in]      len     送信するデータのバイト数。
     *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
     *
     *  @details
     *  通信種別に応じて以下の宛先へ UDP パケットを送信します。
     *
     *  | 通信種別              | 送信先                                     |
     *  | --------------------- | ------------------------------------------ |
     *  | COMM_TYPE_UNICAST     | 接続相手の dst_port 宛にユニキャスト送信   |
     *  | COMM_TYPE_MULTICAST   | multicast_group:src_port へ送信            |
     *  | COMM_TYPE_BROADCAST   | broadcast_addr:src_port へ送信             |
     *
     *  送信ウィンドウが満杯の場合は、ウィンドウに空きが生じるまでブロックします。
     *
     *  @warning        handle が NULL の場合は失敗を返します。\n
     *                  data が NULL の場合は失敗を返します。\n
     *                  len が COMM_MAX_PAYLOAD を超える場合は失敗を返します。
     *******************************************************************************
     */
    COMM_API extern int COMMAPI commSend(CommHandle  handle,
                                         const void *data,
                                         size_t      len);

    /**
     *******************************************************************************
     *  @brief          サービスを閉じます。
     *  @param[in]      handle  commOpenService() で取得したセッションハンドル。
     *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
     *
     *  @details
     *  受信スレッドを停止し、ソケットをクローズしてリソースを解放します。\n
     *  マルチキャストの場合はグループから離脱します。\n
     *  本関数呼び出し後、handle は無効となります。
     *
     *  @warning        handle が NULL の場合は失敗を返します。
     *******************************************************************************
     */
    COMM_API extern int COMMAPI commClose(CommHandle handle);

#ifdef __cplusplus
}
#endif

#endif /* LIBSIMPLECOMM_H */
