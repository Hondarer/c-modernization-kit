/**
 *******************************************************************************
 *  @file           window.h
 *  @brief          スライディングウィンドウ管理モジュールの内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

#include <libsimplecomm_const.h>
#include <libsimplecomm_type.h>

/**
 *  @brief  スライディングウィンドウ管理構造体。
 */
typedef struct
{
    CommPacket packets[COMM_MAX_WINDOW_SIZE]; /**< パケットバッファ。 */
    uint8_t    valid[COMM_MAX_WINDOW_SIZE];   /**< バッファ有効フラグ (1: 有効, 0: 空き)。 */
    uint32_t   base_seq;                      /**< ウィンドウ先頭の通番。 */
    uint32_t   next_seq;                      /**< 送信側: 次に割り当てる通番。受信側: 次に期待する通番。 */
    uint16_t   window_size;                   /**< ウィンドウサイズ (パケット数)。 */
    uint16_t   _pad;                          /**< パディング。 */
} CommWindow;

#ifdef __cplusplus
extern "C"
{
#endif

    extern void window_init(CommWindow *win, uint32_t initial_seq, uint16_t window_size);
    extern int  window_send_push(CommWindow *win, const CommPacket *packet);
    extern int  window_send_ack(CommWindow *win, uint32_t ack_num);
    extern int  window_send_full(const CommWindow *win);
    extern int  window_recv_push(CommWindow *win, const CommPacket *packet);
    extern int  window_recv_pop(CommWindow *win, CommPacket *packet);
    extern int  window_recv_needs_nack(const CommWindow *win, uint32_t *nack_num);

#ifdef __cplusplus
}
#endif

#endif /* WINDOW_H */
