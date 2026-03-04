/**
 *******************************************************************************
 *  @file           potrRecvThread.h
 *  @brief          受信スレッド内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef POTR_RECV_THREAD_H
#define POTR_RECV_THREAD_H

#include "potrContext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern int comm_recv_thread_start(struct PotrContext_ *ctx);
    extern int comm_recv_thread_stop(struct PotrContext_ *ctx);

#ifdef __cplusplus
}
#endif

#endif /* POTR_RECV_THREAD_H */
