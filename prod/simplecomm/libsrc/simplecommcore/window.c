/**
 *******************************************************************************
 *  @file           window.c
 *  @brief          スライディングウィンドウ管理モジュール。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <string.h>

#include <libsimplecomm_const.h>

#include "seqnum.h"
#include "window.h"

/**
 *******************************************************************************
 *  @brief          ウィンドウを初期化します。
 *  @param[out]     win         初期化するウィンドウ構造体へのポインタ。
 *  @param[in]      initial_seq 初期通番。
 *  @param[in]      window_size ウィンドウサイズ (パケット数)。
 *******************************************************************************
 */
void window_init(CommWindow *win, uint32_t initial_seq, uint16_t window_size)
{
    if (win == NULL)
    {
        return;
    }

    memset(win, 0, sizeof(*win));
    win->base_seq   = initial_seq;
    win->next_seq   = initial_seq;
    win->window_size = (window_size <= COMM_MAX_WINDOW_SIZE)
                           ? window_size
                           : (uint16_t)COMM_MAX_WINDOW_SIZE;
}

/* ウィンドウ内インデックス計算 */
static uint16_t win_index(const CommWindow *win, uint32_t seq)
{
    return (uint16_t)((seq - win->base_seq) % win->window_size);
}

/* ---------- 送信側 ---------- */

/**
 *******************************************************************************
 *  @brief          送信ウィンドウにパケットを積みます。
 *  @param[in,out]  win     送信ウィンドウ構造体へのポインタ。
 *  @param[in]      packet  積むパケットへのポインタ。
 *  @return         成功時は COMM_SUCCESS、ウィンドウ満杯の場合は COMM_ERROR を返します。
 *******************************************************************************
 */
int window_send_push(CommWindow *win, const CommPacket *packet)
{
    uint16_t idx;

    if (win == NULL || packet == NULL)
    {
        return COMM_ERROR;
    }

    if (window_send_full(win))
    {
        return COMM_ERROR;
    }

    idx             = win_index(win, win->next_seq);
    win->packets[idx] = *packet;
    win->valid[idx]   = 1;
    win->next_seq++;

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          ACK を処理して送信ウィンドウを前進させます。
 *  @param[in,out]  win     送信ウィンドウ構造体へのポインタ。
 *  @param[in]      ack_num 確認応答された通番。
 *  @return         成功時は COMM_SUCCESS、失敗時は COMM_ERROR を返します。
 *******************************************************************************
 */
int window_send_ack(CommWindow *win, uint32_t ack_num)
{
    uint16_t idx;

    if (win == NULL)
    {
        return COMM_ERROR;
    }

    while (seqnum_in_window(win->base_seq, win->base_seq,
                             (uint16_t)(ack_num - win->base_seq + 1U)) &&
           win->base_seq != ack_num + 1U)
    {
        idx               = win_index(win, win->base_seq);
        win->valid[idx]   = 0;
        win->base_seq++;
    }

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          送信ウィンドウが満杯かどうかを返します。
 *  @param[in]      win     送信ウィンドウ構造体へのポインタ。
 *  @return         満杯の場合は 1、空きがある場合は 0 を返します。
 *******************************************************************************
 */
int window_send_full(const CommWindow *win)
{
    if (win == NULL)
    {
        return 1;
    }
    return (uint32_t)(win->next_seq - win->base_seq) >= (uint32_t)win->window_size;
}

/* ---------- 受信側 ---------- */

/**
 *******************************************************************************
 *  @brief          受信ウィンドウにパケットを格納します。
 *  @param[in,out]  win     受信ウィンドウ構造体へのポインタ。
 *  @param[in]      packet  格納するパケットへのポインタ。
 *  @return         成功時は COMM_SUCCESS、ウィンドウ外の場合は COMM_ERROR を返します。
 *
 *  @details
 *  通番が受信ウィンドウ内であればバッファリングします。\n
 *  追い越し (順序外到着) にも対応します。
 *******************************************************************************
 */
int window_recv_push(CommWindow *win, const CommPacket *packet)
{
    uint16_t idx;

    if (win == NULL || packet == NULL)
    {
        return COMM_ERROR;
    }

    if (!seqnum_in_window(packet->seq_num, win->base_seq, win->window_size))
    {
        return COMM_ERROR;
    }

    idx               = win_index(win, packet->seq_num);
    win->packets[idx] = *packet;
    win->valid[idx]   = 1;

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          受信ウィンドウから順序整列済みパケットを取り出します。
 *  @param[in,out]  win     受信ウィンドウ構造体へのポインタ。
 *  @param[out]     packet  取り出したパケットを格納する構造体へのポインタ。
 *  @return         取り出せた場合は COMM_SUCCESS、次のパケットが未着の場合は COMM_ERROR を返します。
 *******************************************************************************
 */
int window_recv_pop(CommWindow *win, CommPacket *packet)
{
    uint16_t idx;

    if (win == NULL || packet == NULL)
    {
        return COMM_ERROR;
    }

    idx = win_index(win, win->next_seq);
    if (!win->valid[idx])
    {
        return COMM_ERROR;
    }

    *packet         = win->packets[idx];
    win->valid[idx] = 0;
    win->base_seq++;
    win->next_seq++;

    return COMM_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          受信ウィンドウで欠番が発生しているか確認し、NACK 番号を返します。
 *  @param[in]      win         受信ウィンドウ構造体へのポインタ。
 *  @param[out]     nack_num    欠番の通番を格納するポインタ。
 *  @return         欠番がある場合は 1、ない場合は 0 を返します。
 *******************************************************************************
 */
int window_recv_needs_nack(const CommWindow *win, uint32_t *nack_num)
{
    uint16_t idx;

    if (win == NULL || nack_num == NULL)
    {
        return 0;
    }

    idx = win_index(win, win->next_seq);
    if (!win->valid[idx])
    {
        /* next_seq が未着 = 欠番 */
        *nack_num = win->next_seq;
        return 1;
    }

    return 0;
}
