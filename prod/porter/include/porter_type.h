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
 *  | 通信種別              | サービスの識別子        | 有効なフィールド                                                                            |
 *  | --------------------- | ----------------------- | ------------------------------------------------------------------------------------------- |
 *  | POTR_TYPE_UNICAST     | dst_port                | src_addr1〜4, dst_addr1〜4, dst_port, src_port (省略可)                                    |
 *  | POTR_TYPE_MULTICAST   | dst_port                | src_addr1〜4, src_port (省略可), dst_port, multicast_group, ttl                            |
 *  | POTR_TYPE_BROADCAST   | dst_port                | src_addr1〜4, src_port (省略可), dst_port, broadcast_addr                                  |
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
    uint8_t  ttl;           /**< マルチキャスト TTL。(multicast のみ) */
    uint8_t  _pad[3];       /**< パディング (pack_wait_ms を 4 バイト境界に揃える)。 */
    uint32_t pack_wait_ms;  /**< パッキング待ち時間 (ミリ秒)。最初の送信要求からこの時間だけ待ち合わせ、複数メッセージを 1 パケットにまとめる。0 = 即時送信 (パッキング待ちなし)。パケット容量が満杯になった場合はタイマーを無視して即時送信する。 */

    char multicast_group[POTR_MAX_ADDR_LEN]; /**< マルチキャストグループアドレス。(multicast のみ) */
    char broadcast_addr[POTR_MAX_ADDR_LEN];  /**< ブロードキャスト宛先アドレス。省略時は 255.255.255.255。(broadcast のみ) */

    /* マルチパス対応 (src_addr は全通信種別、dst_addr は unicast のみ有効) */
    char src_addr[POTR_MAX_PATH][POTR_MAX_ADDR_LEN]; /**< 送信元アドレス [0]=src_addr1 〜 [3]=src_addr4。送信者は bind / 送信インターフェース、受信者は送信元フィルタ。(全通信種別で必須) */
    char dst_addr[POTR_MAX_PATH][POTR_MAX_ADDR_LEN]; /**< 宛先アドレス [0]=dst_addr1 〜 [3]=dst_addr4。送信者は送信先、受信者は bind アドレス。(unicast のみ) */
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
    uint16_t window_size;        /**< スライディングウィンドウサイズ (パケット数)。 */
    uint16_t max_payload;        /**< 最大ペイロード長 (バイト)。 */
    uint32_t health_interval_ms; /**< ヘルスチェック PING 送信間隔 (ミリ秒)。送信者が使用。0 = ヘルスチェック無効。 */
    uint32_t health_timeout_ms;  /**< ヘルスチェックタイムアウト閾値 (ミリ秒)。この時間内に有効なパケットが受信できなければ切断と判断する。受信者が使用。0 = ヘルスチェック無効。 */
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
    uint32_t ack_num;                  /**< 再送要求番号 / 再送不能通番 (NBO)。NACK では要求通番、REJECT では再送不能通番を格納する。 */
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
 *  @brief          受信イベント種別。
 *
 *  @details
 *  PotrRecvCallback の第 2 引数に渡されるイベント種別です。
 *
 *  | 値                       | 説明                                                         |
 *  | ------------------------ | ------------------------------------------------------------ |
 *  | POTR_EVENT_DATA          | データ受信。data/len に受信内容が格納されます。              |
 *  | POTR_EVENT_CONNECTED     | 送信者からの疎通を初検知、または切断後の復帰を検知しました。 |
 *  | POTR_EVENT_DISCONNECTED  | health_timeout_ms 以内に有効なパケットが受信できず切断を検知しました。 |
 *
 *  @note
 *  POTR_EVENT_CONNECTED / POTR_EVENT_DISCONNECTED は、data=NULL, len=0 で呼び出されます。\n
 *  すべてのイベントは受信スレッドから直列に呼び出されるため、順序性が保証されます。\n
 *  ヘルスチェック無効時 (health_timeout_ms=0) は、通常の疎通監視による CONNECTED / DISCONNECTED は発生しません。\n
 *  ただし、送信者が potrClose を呼び出した際に送出する FIN パケットを受信した場合は、ヘルスチェック設定に関わらず DISCONNECTED が発生します。
 *******************************************************************************
 */
typedef enum
{
    POTR_EVENT_DATA         = 0, /**< データ受信。data/len に内容が格納される。 */
    POTR_EVENT_CONNECTED    = 1, /**< 送信者からの疎通を初検知 or 復帰。data=NULL, len=0。 */
    POTR_EVENT_DISCONNECTED = 2, /**< health_timeout_ms 以内に有効なパケットが受信できず切断を検知。data=NULL, len=0。 */
} PotrEvent;

/**
 *******************************************************************************
 *  @brief          受信コールバック関数型。
 *
 *  @details
 *  データ受信またはヘルスチェック状態変化時に、受信スレッドから呼び出されます。\n
 *  コールバック内でブロッキング処理を行わないようにしてください。\n
 *  すべてのイベントは受信スレッドから直列に呼び出されるため、順序性が保証されます。
 *
 *  @param[in]      service_id  サービスの ID。
 *  @param[in]      event       イベント種別 (PotrEvent)。
 *  @param[in]      data        受信データへのポインタ。POTR_EVENT_DATA 時のみ有効。
 *                              コールバック復帰後は無効になります。
 *  @param[in]      len         受信データのバイト数。POTR_EVENT_DATA 時のみ有効。
 *******************************************************************************
 */
typedef void (*PotrRecvCallback)(int service_id, PotrEvent event,
                                 const void *data, size_t len);

/**
 *******************************************************************************
 *  @brief          ログレベル。
 *
 *  @details
 *  potrLogConfig() の level 引数に指定するログ出力レベルです。\n
 *  指定したレベル以上のメッセージのみが出力されます。
 *
 *  | 値                  | 説明                                           |
 *  | ------------------- | ---------------------------------------------- |
 *  | POTR_LOG_TRACE      | トレース (最詳細)。パケット単位の動作を記録。  |
 *  | POTR_LOG_DEBUG      | デバッグ情報。設定値・ソケット操作等を記録。   |
 *  | POTR_LOG_INFO       | 情報。サービスのライフサイクルイベントを記録。 |
 *  | POTR_LOG_WARN       | 警告。回復可能な異常 (NACK・REJECT 等) を記録。|
 *  | POTR_LOG_ERROR      | エラー。操作の失敗を記録。                     |
 *  | POTR_LOG_FATAL      | 致命的エラー。回復不能な障害を記録。           |
 *  | POTR_LOG_OFF        | ログ出力を無効にします (デフォルト)。          |
 *******************************************************************************
 */
typedef enum
{
    POTR_LOG_TRACE = 0, /**< トレース (最詳細)。 */
    POTR_LOG_DEBUG = 1, /**< デバッグ情報。 */
    POTR_LOG_INFO  = 2, /**< 情報。 */
    POTR_LOG_WARN  = 3, /**< 警告。 */
    POTR_LOG_ERROR = 4, /**< エラー。 */
    POTR_LOG_FATAL = 5, /**< 致命的エラー。 */
    POTR_LOG_OFF   = 6, /**< ログ出力を無効にします。 */
} PotrLogLevel;

#endif /* PORTER_TYPE_H */
