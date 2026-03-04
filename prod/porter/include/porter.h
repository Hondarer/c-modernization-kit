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
     *  @param[in]      callback    データ受信時に呼び出されるコールバック関数。
     *                              受信不要の場合は NULL を指定可能。
     *  @param[out]     handle      成功時にセッションハンドルを格納するポインタ。
     *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
     *
     *  @details
     *  設定ファイルを解析してサービス定義を取得し、UDP ソケットを初期化します。\n
     *  通信種別に応じて以下の処理を行います。
     *
     *  | 通信種別              | ソケット設定                                     |
     *  | --------------------- | ------------------------------------------------ |
     *  | POTR_TYPE_UNICAST     | dst_port をバインド                              |
     *  | POTR_TYPE_MULTICAST   | src_port をバインド、グループ参加 (IGMP)        |
     *  | POTR_TYPE_BROADCAST   | src_port をバインド、SO_BROADCAST を設定        |
     *
     *  受信コールバックが指定された場合、内部で受信スレッドを起動します。
     *
     *  @par            使用例
     *  @code{.c}
     *  void on_recv(int service_id, const void *data, size_t len) {
     *      printf("service %d: received %zu bytes\n", service_id, len);
     *  }
     *
     *  PotrHandle handle;
     *  if (potrOpenService("porter-services.conf", 1, on_recv, &handle)
     *          == POTR_SUCCESS) {
     *      // 通信処理
     *      potrClose(handle);
     *  }
     *  @endcode
     *
     *  @warning        handle が NULL の場合は失敗を返します。\n
     *                  config_path が NULL または存在しない場合は失敗を返します。\n
     *                  指定した service_id が設定ファイルに存在しない場合は失敗を返します。
     *******************************************************************************
     */
    POTR_API extern int POTRAPI potrOpenService(const char       *config_path,
                                                int               service_id,
                                                PotrRecvCallback  callback,
                                                PotrHandle       *handle);

    /**
     *******************************************************************************
     *  @brief          データを送信します。
     *  @param[in]      handle    potrOpenService() で取得したセッションハンドル。
     *  @param[in]      data      送信するデータへのポインタ。
     *  @param[in]      len       送信するデータのバイト数。
     *  @param[in]      compress  0 以外を指定するとペイロードを圧縮して送信します。
     *                            0 を指定すると非圧縮で送信します。
     *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
     *
     *  @details
     *  通信種別に応じて以下の宛先へ UDP パケットを送信します。
     *
     *  | 通信種別              | 送信先                                     |
     *  | --------------------- | ------------------------------------------ |
     *  | POTR_TYPE_UNICAST     | 接続相手の dst_port 宛にユニキャスト送信   |
     *  | POTR_TYPE_MULTICAST   | multicast_group:src_port へ送信            |
     *  | POTR_TYPE_BROADCAST   | broadcast_addr:src_port へ送信             |
     *
     *  compress に 0 以外を指定した場合、内部で圧縮処理を行ってから送信します。\n
     *  受信側の PotrRecvCallback には、解凍済みの元データが渡されます。\n
     *  送受信ともにフラグメント化と組み合わせて使用できます。
     *
     *  @note
     *  圧縮フォーマットには raw DEFLATE (RFC 1951) を使用します。\n
     *  Linux (zlib) と Windows (Compression API MSZIP|COMPRESS_RAW) は
     *  同一フォーマットを出力するため、クロスプラットフォーム通信に対応します。
     *
     *  @warning        handle が NULL の場合は失敗を返します。\n
     *                  data が NULL の場合は失敗を返します。\n
     *                  len が POTR_MAX_MESSAGE_SIZE を超える場合は失敗を返します。
     *******************************************************************************
     */
    POTR_API extern int POTRAPI potrSend(PotrHandle  handle,
                                         const void *data,
                                         size_t      len,
                                         int         compress);

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
     *  @warning        handle が NULL の場合は失敗を返します。
     *******************************************************************************
     */
    POTR_API extern int POTRAPI potrClose(PotrHandle handle);

#ifdef __cplusplus
}
#endif

#endif /* PORTER_H */
