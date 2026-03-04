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

/** @defgroup POTR_FLAG パケットフラグ
 *  @{
 */
#define POTR_FLAG_DATA       0x0001U /**< データパケットであることを示すフラグ。 */
#define POTR_FLAG_ACK        0x0002U /**< 確認応答パケットであることを示すフラグ。 */
#define POTR_FLAG_NACK       0x0004U /**< 再送要求パケットであることを示すフラグ。 */
#define POTR_FLAG_MORE_FRAG  0x0008U /**< 後続フラグメントが存在することを示すフラグ。 */
#define POTR_FLAG_COMPRESSED 0x0010U /**< ペイロードが圧縮されていることを示すフラグ。先頭 4 バイトが元サイズ (NBO)、続くデータが raw DEFLATE。 */
/** @} */

/** @defgroup POTR_DEFAULT デフォルト値
 *  @{
 */
#define POTR_DEFAULT_WINDOW_SIZE           16U    /**< デフォルトウィンドウサイズ (パケット数)。 */
#define POTR_DEFAULT_MAX_PAYLOAD           1400U  /**< デフォルト最大ペイロード長 (バイト)。 */
#define POTR_DEFAULT_RETRANSMIT_TIMEOUT_MS 1000U  /**< デフォルト再送タイムアウト (ミリ秒)。 */
#define POTR_DEFAULT_RETRANSMIT_COUNT      3U     /**< デフォルト最大再送回数。 */
#define POTR_DEFAULT_TTL                   1U     /**< デフォルトマルチキャスト TTL。 */
/** @} */

/** @defgroup POTR_LIMIT 上限値
 *  @{
 */
#define POTR_MAX_ADDR_LEN     64U    /**< アドレス文字列の最大長 (バイト、終端 NUL を含む)。 */
#define POTR_MAX_PAYLOAD      1400U  /**< ペイロードの最大長 (バイト)。 */
#define POTR_MAX_WINDOW_SIZE  256U   /**< ウィンドウサイズの最大値 (パケット数)。 */
#define POTR_MAX_SERVICES     64U    /**< 設定ファイルに定義できるサービスの最大数。 */
#define POTR_MAX_MESSAGE_SIZE 65535U /**< 1 回の potrSend で送信できる最大メッセージ長 (バイト)。フラグメント化により POTR_MAX_PAYLOAD を超えるメッセージも送受信できる。 */
/** @} */

#endif /* PORTER_CONST_H */
