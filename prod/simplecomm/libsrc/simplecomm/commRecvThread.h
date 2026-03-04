/**
 *******************************************************************************
 *  @file           commRecvThread.h
 *  @brief          受信スレッド内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef COMM_RECV_THREAD_H
#define COMM_RECV_THREAD_H

#include "commContext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern int comm_recv_thread_start(struct CommContext_ *ctx);
    extern int comm_recv_thread_stop(struct CommContext_ *ctx);

#ifdef __cplusplus
}
#endif

#endif /* COMM_RECV_THREAD_H */
