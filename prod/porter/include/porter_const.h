/**
 *******************************************************************************
 *  @file           porter_const.h
 *  @brief          通信ライブラリの定数ファイル。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PORTER_CONST_H
#define PORTER_CONST_H

/** @defgroup POTR_RESULT 戻り値
 *  @{
 */
#define POTR_SUCCESS  0  /**< 成功の戻り値を表す定数。 */
#define POTR_ERROR   -1  /**< 失敗の戻り値を表す定数。 */
/** @} */

/** @defgroup POTR_OUTER_FLAG 外側パケットフラグ (PotrPacket.flags)
 *  @{
 *  @details
 *  UDP で送受信される外側パケット (PotrPacket) の flags フィールドに設定するフラグです。\n
 *  ペイロードエレメントのフラグ (@ref POTR_ELEM_FLAG) とは独立したビット空間で管理します。
 */
#define POTR_FLAG_DATA       0x0001U /**< データパケット (パックコンテナ) であることを示すフラグ。常に設定される。 */
#define POTR_FLAG_NACK       0x0002U /**< 再送要求パケットであることを示すフラグ。ack_num に要求通番を格納する。 */
#define POTR_FLAG_PING       0x0004U /**< ヘルスチェック要求パケットであることを示すフラグ。ペイロードなし。 */
#define POTR_FLAG_REJECT     0x0008U /**< 再送不能通知パケットであることを示すフラグ。ack_num に再送不能な通番を格納する。 */
#define POTR_FLAG_FIN        0x0010U /**< 正常終了通知パケットであることを示すフラグ。送信者が potrClose 時に送出し、受信者は即座に DISCONNECTED へ遷移する。ペイロードなし。 */
/** @} */

/** @defgroup POTR_ELEM_FLAG ペイロードエレメントフラグ (パックコンテナ内エレメントヘッダー.flags)
 *  @{
 *  @details
 *  POTR_FLAG_DATA パケットのペイロード内に格納されるペイロードエレメントのヘッダー flags フィールドに設定するフラグです。\n
 *  圧縮・フラグメント化はメッセージ単位で行われ、ペイロードエレメント単位で管理します。\n
 *  外側パケットのフラグ (@ref POTR_OUTER_FLAG) には設定しません。
 */
#define POTR_FLAG_MORE_FRAG  0x0001U /**< 後続フラグメントが存在することを示すペイロードエレメントフラグ。メッセージが複数ペイロードエレメントに分割された場合、最終フラグメント以外に設定する。 */
#define POTR_FLAG_COMPRESSED 0x0002U /**< ペイロードが圧縮されていることを示すペイロードエレメントフラグ。圧縮はメッセージ単位で行い、全フラグメントのペイロードエレメントに設定する。先頭 4 バイトが元サイズ (NBO)、続くデータが raw DEFLATE。 */
/** @} */

/** @defgroup POTR_DEFAULT デフォルト値
 *  @{
 */
#define POTR_DEFAULT_WINDOW_SIZE  16U    /**< デフォルトウィンドウサイズ (パケット数)。 */
#define POTR_DEFAULT_MAX_PAYLOAD  1400U  /**< デフォルト最大ペイロード長 (バイト)。 */
#define POTR_DEFAULT_TTL          1U     /**< デフォルトマルチキャスト TTL。 */
#define POTR_DEFAULT_HEALTH_INTERVAL_MS    0U     /**< デフォルトヘルスチェック送信間隔 (ミリ秒)。0 = 無効。 */
#define POTR_DEFAULT_HEALTH_TIMEOUT_MS     0U     /**< デフォルトヘルスチェックタイムアウト (ミリ秒)。0 = 無効。 */
#define POTR_DEFAULT_PACK_WAIT_MS          0U     /**< デフォルトパッキング待ち時間 (ミリ秒)。0 = 即時送信。 */
/** @} */

/** @defgroup POTR_LIMIT 上限値
 *  @{
 */
#define POTR_MAX_ADDR_LEN      64U    /**< アドレス文字列の最大長 (バイト、終端 NUL を含む)。 */
#define POTR_MAX_PAYLOAD       1400U  /**< ペイロードの最大長 (バイト)。 */
#define POTR_MAX_WINDOW_SIZE   256U   /**< ウィンドウサイズの最大値 (パケット数)。 */
#define POTR_MAX_SERVICES      64U    /**< 設定ファイルに定義できるサービスの最大数。 */
#define POTR_MAX_MESSAGE_SIZE  65535U /**< 1 回の potrSend で送信できる最大メッセージ長 (バイト)。フラグメント化により POTR_MAX_PAYLOAD を超えるメッセージも送受信できる。 */
#define POTR_SEND_QUEUE_DEPTH    1024U  /**< 非同期送信キューの最大エントリ数。メッセージがフラグメント化される場合、1 メッセージが複数エントリを占有する。 */
#define POTR_PAYLOAD_ELEM_HDR_SIZE  4U  /**< パックコンテナ内ペイロードエレメントのヘッダーサイズ (バイト)。flags (2): POTR_FLAG_MORE_FRAG / POTR_FLAG_COMPRESSED を格納 + payload_len (2)。通番は外側パケットで管理するためペイロードエレメントには含まない。 */
/** @} */

#endif /* PORTER_CONST_H */
