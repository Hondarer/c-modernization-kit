/**
 *******************************************************************************
 *  @file           potrHealthThread.h
 *  @brief          ヘルスチェックスレッド内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/08
 *  @version        1.0.0
 *
 *  @details
 *  送信者が定周期で PING パケットを送信するスレッドの起動・停止 API。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef POTR_HEALTH_THREAD_H
#define POTR_HEALTH_THREAD_H

#include "potrContext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern int potr_health_thread_start(struct PotrContext_ *ctx);
    extern int potr_health_thread_stop(struct PotrContext_ *ctx);

#ifdef __cplusplus
}
#endif

#endif /* POTR_HEALTH_THREAD_H */
