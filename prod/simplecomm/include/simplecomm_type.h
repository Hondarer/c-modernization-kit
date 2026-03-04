/**
 *******************************************************************************
 *  @file           simplecomm_type.h
 *  @brief          簡易通信ライブラリの型定義ファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBSIMPLECOMM_TYPES_H
#define LIBSIMPLECOMM_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include <simplecomm_const.h>

/**
 *******************************************************************************
 *  @brief          通信種別。
 *
 *  @details
 *  サービス定義の通信方式を表します。
 *
 *  | 値                      | 説明                            |
 *  | ----------------------- | ------------------------------- |
 *  | COMM_TYPE_UNICAST       | 1:1 通信 (UDP ユニキャスト)     |
 *  | COMM_TYPE_MULTICAST     | 1:N 通信 (UDP マルチキャスト)   |
 *  | COMM_TYPE_BROADCAST     | 1:N 通信 (UDP ブロードキャスト) |
 *******************************************************************************
 */
typedef enum
{
    COMM_TYPE_UNICAST   = 1, /**< 1:1 通信 (UDP ユニキャスト)。 */
    COMM_TYPE_MULTICAST = 2, /**< 1:N 通信 (UDP マルチキャスト)。 */
    COMM_TYPE_BROADCAST = 3, /**< 1:N 通信 (UDP ブロードキャスト)。 */
} CommType;

/**
 *******************************************************************************
 *  @brief          サービス定義。
 *
 *  @details
 *  設定ファイルの [service.N] セクションから読み込まれるサービス設定です。
 *
 *  通信種別によって有効なフィールドが異なります。
 *
 *  | 通信種別              | サービスの識別子        | 有効なフィールド                                 |
 *  | --------------------- | ----------------------- | ------------------------------------------------ |
 *  | COMM_TYPE_UNICAST     | dst_port                | dst_port                                         |
 *  | COMM_TYPE_MULTICAST   | src_port                | src_port, multicast_group, ttl                   |
 *  | COMM_TYPE_BROADCAST   | src_port                | src_port, broadcast_addr                         |
 *******************************************************************************
 */
typedef struct
{
    int      service_id; /**< サービス ID。 */
    CommType type;       /**< 通信種別。 */

    /* COMM_TYPE_UNICAST */
    uint16_t dst_port; /**< 宛先ポート番号。サービスの識別子。(unicast のみ) */

    /* COMM_TYPE_MULTICAST / COMM_TYPE_BROADCAST */
    uint16_t src_port; /**< 送信元ポート番号。サービスの識別子。受信側はこのポートで待機する。(multicast / broadcast) */

    /* COMM_TYPE_MULTICAST */
    uint8_t ttl;      /**< マルチキャスト TTL。(multicast のみ) */
    uint8_t _pad[3];  /**< パディング。 */

    char multicast_group[COMM_MAX_ADDR_LEN]; /**< マルチキャストグループアドレス。(multicast のみ) */
    char broadcast_addr[COMM_MAX_ADDR_LEN];  /**< ブロードキャスト宛先アドレス。(broadcast のみ) */
} CommServiceDef;

/**
 *******************************************************************************
 *  @brief          グローバル設定。
 *
 *  @details
 *  設定ファイルの [global] セクションから読み込まれる共通プロトコル設定です。
 *******************************************************************************
 */
typedef struct
{
    uint32_t retransmit_timeout_ms; /**< 再送タイムアウト (ミリ秒)。 */
    uint16_t window_size;           /**< スライディングウィンドウサイズ (パケット数)。 */
    uint16_t max_payload;           /**< 最大ペイロード長 (バイト)。 */
    uint8_t  retransmit_count;      /**< 最大再送回数。 */
    uint8_t  _pad[3];               /**< パディング。 */
} CommGlobalConfig;

/**
 *******************************************************************************
 *  @brief          ネットワーク送受信用パケット構造体。
 *
 *  @details
 *  UDP で送受信される物理パケットのレイアウトです。\n
 *  各フィールドはネットワークバイトオーダー (ビッグエンディアン) で格納します。
 *******************************************************************************
 */
typedef struct
{
    uint32_t seq_num;                  /**< 通番。送信側が付与する連番。 */
    uint32_t ack_num;                  /**< 確認応答番号 / 再送要求番号。 */
    uint16_t flags;                    /**< パケット種別フラグ (COMM_FLAG_*)。 */
    uint16_t payload_len;              /**< ペイロード長 (バイト)。 */
    uint8_t  payload[COMM_MAX_PAYLOAD]; /**< ペイロードデータ。 */
} CommPacket;

/**
 *******************************************************************************
 *  @brief          セッションハンドル。
 *
 *  @details
 *  commOpenService() が返す不透明ポインタです。\n
 *  内部実装の詳細はライブラリ利用者からは隠蔽されます。
 *******************************************************************************
 */
typedef struct CommContext_ *CommHandle;

/**
 *******************************************************************************
 *  @brief          受信コールバック関数型。
 *
 *  @details
 *  データパケットを受信したとき、受信スレッドから呼び出されます。\n
 *  コールバック内でブロッキング処理を行わないようにしてください。
 *
 *  @param[in]      service_id  受信したサービスの ID。
 *  @param[in]      data        受信データへのポインタ。コールバック復帰後は無効になります。
 *  @param[in]      len         受信データのバイト数。
 *******************************************************************************
 */
typedef void (*CommRecvCallback)(int service_id, const void *data, size_t len);

#endif /* LIBSIMPLECOMM_TYPES_H */
