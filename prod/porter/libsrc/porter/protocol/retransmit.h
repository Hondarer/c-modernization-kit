/**
 *******************************************************************************
 *  @file           retransmit.h
 *  @brief          再送制御モジュールの内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef RETRANSMIT_H
#define RETRANSMIT_H

#include <stdint.h>

#include <porter_const.h>

/**
 *  @brief  再送制御エントリ。
 *
 *  @details
 *  送信済みパケット 1 つに対する再送管理情報です。
 */
typedef struct
{
    uint32_t seq_num;      /**< 管理対象の通番。 */
    uint32_t send_time_ms; /**< 最後に送信した時刻 (ミリ秒)。 */
    uint8_t  retry_count;  /**< 現在の再送回数。 */
    uint8_t  active;       /**< 1: 管理中, 0: 空き。 */
    uint8_t  _pad[2];      /**< パディング。 */
} RetransmitEntry;

/**
 *  @brief  再送制御管理構造体。
 */
typedef struct
{
    RetransmitEntry entries[POTR_MAX_WINDOW_SIZE]; /**< 再送管理エントリ配列。 */
    uint32_t        timeout_ms;                    /**< 再送タイムアウト (ミリ秒)。 */
    uint8_t         max_retry;                     /**< 最大再送回数。 */
    uint8_t         _pad[3];                       /**< パディング。 */
} PotrRetransmit;

#ifdef __cplusplus
extern "C"
{
#endif

    extern void retransmit_init(PotrRetransmit *rt,
                                uint32_t timeout_ms, uint8_t max_retry);
    extern int  retransmit_register(PotrRetransmit *rt,
                                    uint32_t seq_num, uint32_t now_ms);
    extern int  retransmit_ack(PotrRetransmit *rt, uint32_t seq_num);
    extern int  retransmit_check(PotrRetransmit *rt, uint32_t now_ms,
                                 uint32_t *seq_num_out);

#ifdef __cplusplus
}
#endif

#endif /* RETRANSMIT_H */
