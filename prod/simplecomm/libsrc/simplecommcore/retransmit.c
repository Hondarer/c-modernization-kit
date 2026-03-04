/**
 *******************************************************************************
 *  @file           retransmit.c
 *  @brief          再送制御モジュール。
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

#include "retransmit.h"

/**
 *******************************************************************************
 *  @brief          再送制御管理構造体を初期化します。
 *  @param[out]     rt          初期化する構造体へのポインタ。
 *  @param[in]      timeout_ms  再送タイムアウト (ミリ秒)。
 *  @param[in]      max_retry   最大再送回数。
 *******************************************************************************
 */
void retransmit_init(CommRetransmit *rt, uint32_t timeout_ms, uint8_t max_retry)
{
    if (rt == NULL)
    {
        return;
    }
    memset(rt, 0, sizeof(*rt));
    rt->timeout_ms = timeout_ms;
    rt->max_retry  = max_retry;
}

/**
 *******************************************************************************
 *  @brief          パケットを再送管理に登録します。
 *  @param[in,out]  rt          再送制御管理構造体へのポインタ。
 *  @param[in]      seq_num     登録する通番。
 *  @param[in]      now_ms      現在時刻 (ミリ秒)。
 *  @return         成功時は COMM_SUCCESS、エントリが満杯の場合は COMM_ERROR を返します。
 *******************************************************************************
 */
int retransmit_register(CommRetransmit *rt, uint32_t seq_num, uint32_t now_ms)
{
    uint32_t i;

    if (rt == NULL)
    {
        return COMM_ERROR;
    }

    for (i = 0; i < COMM_MAX_WINDOW_SIZE; i++)
    {
        if (!rt->entries[i].active)
        {
            rt->entries[i].seq_num      = seq_num;
            rt->entries[i].send_time_ms = now_ms;
            rt->entries[i].retry_count  = 0;
            rt->entries[i].active       = 1;
            return COMM_SUCCESS;
        }
    }

    return COMM_ERROR;
}

/**
 *******************************************************************************
 *  @brief          ACK 受信により再送管理から登録を解除します。
 *  @param[in,out]  rt          再送制御管理構造体へのポインタ。
 *  @param[in]      seq_num     解除する通番。
 *  @return         成功時は COMM_SUCCESS、エントリが見つからない場合は COMM_ERROR を返します。
 *******************************************************************************
 */
int retransmit_ack(CommRetransmit *rt, uint32_t seq_num)
{
    uint32_t i;

    if (rt == NULL)
    {
        return COMM_ERROR;
    }

    for (i = 0; i < COMM_MAX_WINDOW_SIZE; i++)
    {
        if (rt->entries[i].active && rt->entries[i].seq_num == seq_num)
        {
            rt->entries[i].active = 0;
            return COMM_SUCCESS;
        }
    }

    return COMM_ERROR;
}

/**
 *******************************************************************************
 *  @brief          タイムアウト発生パケットを検索して再送対象の通番を返します。
 *  @param[in,out]  rt              再送制御管理構造体へのポインタ。
 *  @param[in]      now_ms          現在時刻 (ミリ秒)。
 *  @param[out]     seq_num_out     再送対象の通番を格納するポインタ。
 *  @return         再送対象がある場合は COMM_SUCCESS、ない場合は COMM_ERROR を返します。
 *
 *  @details
 *  タイムアウトが発生したエントリを最も古い順に 1 件返します。\n
 *  最大再送回数を超えたエントリは管理から削除します。
 *******************************************************************************
 */
int retransmit_check(CommRetransmit *rt, uint32_t now_ms, uint32_t *seq_num_out)
{
    uint32_t i;

    if (rt == NULL || seq_num_out == NULL)
    {
        return COMM_ERROR;
    }

    for (i = 0; i < COMM_MAX_WINDOW_SIZE; i++)
    {
        if (!rt->entries[i].active)
        {
            continue;
        }

        if ((now_ms - rt->entries[i].send_time_ms) >= rt->timeout_ms)
        {
            if (rt->entries[i].retry_count >= rt->max_retry)
            {
                /* 最大再送回数超過 → 管理解除 */
                rt->entries[i].active = 0;
                continue;
            }

            rt->entries[i].retry_count++;
            rt->entries[i].send_time_ms = now_ms;
            *seq_num_out = rt->entries[i].seq_num;
            return COMM_SUCCESS;
        }
    }

    return COMM_ERROR;
}
