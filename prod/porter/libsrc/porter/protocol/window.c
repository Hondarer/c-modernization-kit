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

#include <porter_const.h>

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
void window_init(PotrWindow *win, uint32_t initial_seq, uint16_t window_size)
{
    if (win == NULL)
    {
        return;
    }

    memset(win, 0, sizeof(*win));
    win->base_seq   = initial_seq;
    win->next_seq   = initial_seq;
    win->window_size = (window_size <= POTR_MAX_WINDOW_SIZE)
                           ? window_size
                           : (uint16_t)POTR_MAX_WINDOW_SIZE;
}

/* ウィンドウ内インデックス計算 */
static uint16_t win_index(const PotrWindow *win, uint32_t seq)
{
    return (uint16_t)((seq - win->base_seq) % win->window_size);
}

/* ---------- 送信側 ---------- */

/**
 *******************************************************************************
 *  @brief          送信ウィンドウにパケットを積みます。
 *  @param[in,out]  win     送信ウィンドウ構造体へのポインタ。
 *  @param[in]      packet  積むパケットへのポインタ。
 *  @return         成功時は POTR_SUCCESS、ウィンドウ満杯の場合は POTR_ERROR を返します。
 *******************************************************************************
 */
int window_send_push(PotrWindow *win, const PotrPacket *packet)
{
    uint16_t idx;

    if (win == NULL || packet == NULL)
    {
        return POTR_ERROR;
    }

    /* ACK なし設計のため、満杯の場合は最古エントリを evict して循環利用する。
       evict されたエントリへの NACK は REJECT で応答する。 */
    if (window_send_full(win))
    {
        idx               = win_index(win, win->base_seq);
        win->valid[idx]   = 0;
        win->base_seq++;
    }

    idx               = win_index(win, win->next_seq);
    win->packets[idx] = *packet;
    win->valid[idx]   = 1;
    win->next_seq++;

    return POTR_SUCCESS;
}


/**
 *******************************************************************************
 *  @brief          送信ウィンドウが満杯かどうかを返します。
 *  @param[in]      win     送信ウィンドウ構造体へのポインタ。
 *  @return         満杯の場合は 1、空きがある場合は 0 を返します。
 *******************************************************************************
 */
int window_send_full(const PotrWindow *win)
{
    if (win == NULL)
    {
        return 1;
    }
    return (uint32_t)(win->next_seq - win->base_seq) >= (uint32_t)win->window_size;
}

/**
 *******************************************************************************
 *  @brief          送信ウィンドウから指定通番のパケットを取得します (再送用)。
 *  @param[in]      win         送信ウィンドウ構造体へのポインタ。
 *  @param[in]      seq_num     取得する通番。
 *  @param[out]     packet_out  取得したパケットを格納する構造体へのポインタ。
 *  @return         成功時は POTR_SUCCESS、エントリが存在しない場合は POTR_ERROR を返します。
 *******************************************************************************
 */
int window_send_get(const PotrWindow *win, uint32_t seq_num, PotrPacket *packet_out)
{
    uint16_t idx;

    if (win == NULL || packet_out == NULL)
    {
        return POTR_ERROR;
    }

    /* 通番がウィンドウ範囲外 */
    if (!seqnum_in_window(seq_num, win->base_seq,
                          (uint16_t)(win->next_seq - win->base_seq)))
    {
        return POTR_ERROR;
    }

    idx = win_index(win, seq_num);
    if (!win->valid[idx])
    {
        return POTR_ERROR;
    }

    *packet_out = win->packets[idx];
    return POTR_SUCCESS;
}

/* ---------- 受信側 ---------- */

/**
 *******************************************************************************
 *  @brief          受信ウィンドウにパケットを格納します。
 *  @param[in,out]  win     受信ウィンドウ構造体へのポインタ。
 *  @param[in]      packet  格納するパケットへのポインタ。
 *  @return         成功時は POTR_SUCCESS、ウィンドウ外の場合は POTR_ERROR を返します。
 *
 *  @details
 *  通番が受信ウィンドウ内であればバッファリングします。\n
 *  追い越し (順序外到着) にも対応します。
 *******************************************************************************
 */
int window_recv_push(PotrWindow *win, const PotrPacket *packet)
{
    uint16_t idx;

    if (win == NULL || packet == NULL)
    {
        return POTR_ERROR;
    }

    if (!seqnum_in_window(packet->seq_num, win->base_seq, win->window_size))
    {
        return POTR_ERROR;
    }

    idx = win_index(win, packet->seq_num);
    if (!win->valid[idx])
    {
        win->packets[idx] = *packet;
        win->valid[idx]   = 1;
    }

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          受信ウィンドウから順序整列済みパケットを取り出します。
 *  @param[in,out]  win     受信ウィンドウ構造体へのポインタ。
 *  @param[out]     packet  取り出したパケットを格納する構造体へのポインタ。
 *  @return         取り出せた場合は POTR_SUCCESS、次のパケットが未着の場合は POTR_ERROR を返します。
 *******************************************************************************
 */
int window_recv_pop(PotrWindow *win, PotrPacket *packet)
{
    uint16_t idx;

    if (win == NULL || packet == NULL)
    {
        return POTR_ERROR;
    }

    idx = win_index(win, win->next_seq);
    if (!win->valid[idx])
    {
        return POTR_ERROR;
    }

    *packet         = win->packets[idx];
    win->valid[idx] = 0;
    win->base_seq++;
    win->next_seq++;

    return POTR_SUCCESS;
}

/**
 *******************************************************************************
 *  @brief          受信ウィンドウで指定通番をスキップして次の通番へ前進させます。
 *  @param[in,out]  win     受信ウィンドウ構造体へのポインタ。
 *  @param[in]      seq_num スキップする通番。next_seq と一致する場合のみ動作します。
 *
 *  @details
 *  REJECT 受信時に欠落パケットを「配信済み」として扱い、後続パケットの配信を
 *  継続するために使用します。seq_num が next_seq と一致しない場合は何もしません。
 *******************************************************************************
 */
void window_recv_skip(PotrWindow *win, uint32_t seq_num)
{
    uint16_t idx;

    if (win == NULL || win->next_seq != seq_num)
    {
        return;
    }

    idx               = win_index(win, seq_num);
    win->valid[idx]   = 0; /* 万一セットされていても無効化 */
    win->base_seq++;
    win->next_seq++;
}

/**
 *******************************************************************************
 *  @brief          受信ウィンドウで欠番が発生しているか確認し、NACK 番号を返します。
 *  @param[in]      win         受信ウィンドウ構造体へのポインタ。
 *  @param[out]     nack_num    欠番の通番を格納するポインタ。
 *  @return         欠番がある場合は 1、ない場合は 0 を返します。
 *******************************************************************************
 */
int window_recv_needs_nack(const PotrWindow *win, uint32_t *nack_num)
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
