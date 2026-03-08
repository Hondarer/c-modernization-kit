/**
 *******************************************************************************
 *  @file           porter_type.h
 *  @brief          通信ライブラリの型定義ファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PORTER_TYPE_H
#define PORTER_TYPE_H

#include <stddef.h>
#include <stdint.h>

#include <porter_const.h>

/**
 *******************************************************************************
 *  @brief          通信種別。
 *
 *  @details
 *  サービス定義の通信方式を表します。
 *
 *  | 値                      | 説明                            |
 *  | ----------------------- | ------------------------------- |
 *  | POTR_TYPE_UNICAST       | 1:1 通信 (UDP ユニキャスト)     |
 *  | POTR_TYPE_MULTICAST     | 1:N 通信 (UDP マルチキャスト)   |
 *  | POTR_TYPE_BROADCAST     | 1:N 通信 (UDP ブロードキャスト) |
 *******************************************************************************
 */
typedef enum
{
    POTR_TYPE_UNICAST   = 1, /**< 1:1 通信 (UDP ユニキャスト)。 */
    POTR_TYPE_MULTICAST = 2, /**< 1:N 通信 (UDP マルチキャスト)。 */
    POTR_TYPE_BROADCAST = 3, /**< 1:N 通信 (UDP ブロードキャスト)。 */
} PotrType;

/**
 *******************************************************************************
 *  @brief          役割種別。
 *
 *  @details
 *  potrOpenService() の呼び出し元がデータを送信する役割か受信する役割かを明示します。
 *
 *  | 値                  | 説明       |
 *  | ------------------- | ---------- |
 *  | POTR_ROLE_SENDER    | 送信者     |
 *  | POTR_ROLE_RECEIVER  | 受信者     |
 *******************************************************************************
 */
typedef enum
{
    POTR_ROLE_SENDER   = 1, /**< 送信者。 */
    POTR_ROLE_RECEIVER = 2, /**< 受信者。 */
} PotrRole;

/**
 *******************************************************************************
 *  @brief          サービス定義。
 *
 *  @details
 *  設定ファイルの [service.N] セクションから読み込まれるサービス設定です。
 *
 *  通信種別によって有効なフィールドが異なります。
 *
 *  | 通信種別              | サービスの識別子        | 有効なフィールド                                                       |
 *  | --------------------- | ----------------------- | ---------------------------------------------------------------------- |
 *  | POTR_TYPE_UNICAST     | dst_port                | dst_addr, src_addr, dst_port, src_port (省略可)                        |
 *  | POTR_TYPE_MULTICAST   | dst_port                | src_addr, src_port (省略可), dst_port, multicast_group, ttl            |
 *  | POTR_TYPE_BROADCAST   | dst_port                | src_addr, src_port (省略可), dst_port, broadcast_addr                  |
 *******************************************************************************
 */
typedef struct
{
    int      service_id; /**< サービス ID。 */
    PotrType type;       /**< 通信種別。 */

    /* POTR_TYPE_UNICAST */
    uint16_t dst_port; /**< 宛先ポート番号。サービスの識別子。受信者の bind ポート / 送信者の送信先ポート。(全通信種別で必須) */

    /* POTR_TYPE_MULTICAST / POTR_TYPE_BROADCAST */
    uint16_t src_port; /**< 送信者の送信元 bind ポート番号。0 = OS 自動選定。(全通信種別で省略可) */

    /* POTR_TYPE_MULTICAST */
    uint8_t ttl;      /**< マルチキャスト TTL。(multicast のみ) */
    uint8_t _pad[3];  /**< パディング。 */

    char multicast_group[POTR_MAX_ADDR_LEN]; /**< マルチキャストグループアドレス。(multicast のみ) */
    char broadcast_addr[POTR_MAX_ADDR_LEN];  /**< ブロードキャスト宛先アドレス。(broadcast のみ) */

    /* POTR_TYPE_UNICAST */
    char dst_addr[POTR_MAX_ADDR_LEN]; /**< 宛先アドレス (IPv4 またはホスト名)。送信者は送信先、受信者は bind アドレス。(unicast のみ) */
    char src_addr[POTR_MAX_ADDR_LEN]; /**< 送信元アドレス (IPv4 またはホスト名)。送信者は bind / 送信インターフェース、受信者は送信元フィルタ。(全通信種別で必須) */
} PotrServiceDef;

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
} PotrGlobalConfig;

/**
 *******************************************************************************
 *  @brief          ネットワーク送受信用パケット構造体。
 *
 *  @details
 *  UDP で送受信される物理パケットのレイアウトです。\n
 *  各フィールドはネットワークバイトオーダー (ビッグエンディアン) で格納します。\n
 *  ヘッダー固定長: offsetof(PotrPacket, payload) = 32 バイト。
 *******************************************************************************
 */
typedef struct
{
    int32_t  service_id;               /**< サービス識別子 (NBO)。受信時に照合する。 */
    uint32_t session_id;               /**< セッション識別子 (NBO)。potrOpenService 時に決定する乱数。 */
    int64_t  session_tv_sec;           /**< セッション開始時刻 秒部 (NBO)。struct timespec の tv_sec 相当。 */
    int32_t  session_tv_nsec;          /**< セッション開始時刻 ナノ秒部 (NBO)。struct timespec の tv_nsec 相当。 */
    uint32_t seq_num;                  /**< 通番。送信側が付与する連番 (NBO)。 */
    uint32_t ack_num;                  /**< 確認応答番号 / 再送要求番号 (NBO)。 */
    uint16_t flags;                    /**< パケット種別フラグ (POTR_FLAG_*) (NBO)。 */
    uint16_t payload_len;              /**< ペイロード長 (バイト) (NBO)。 */
    uint8_t  payload[POTR_MAX_PAYLOAD]; /**< ペイロードデータ。 */
} PotrPacket;

/**
 *******************************************************************************
 *  @brief          セッションハンドル。
 *
 *  @details
 *  potrOpenService() が返す不透明ポインタです。\n
 *  内部実装の詳細はライブラリ利用者からは隠蔽されます。
 *******************************************************************************
 */
typedef struct PotrContext_ *PotrHandle;

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
typedef void (*PotrRecvCallback)(int service_id, const void *data, size_t len);

#endif /* PORTER_TYPE_H */
