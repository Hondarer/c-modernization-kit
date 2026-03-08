/**
 *******************************************************************************
 *  @file           potrSendThread.h
 *  @brief          非同期送信スレッドの内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/08
 *  @version        1.0.0
 *
 *  @details
 *  送信者 (POTR_ROLE_SENDER) が非ブロッキング送信を使用する場合に起動する
 *  送信スレッドの起動・停止 API。\n
 *  送信スレッドは送信キュー (PotrSendQueue) からペイロードエレメントを取り出して
 *  外側パケットを構築し sendto を呼び出します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef POTR_SEND_THREAD_H
#define POTR_SEND_THREAD_H

#include "potrContext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern int  potr_send_thread_start(struct PotrContext_ *ctx);
    extern void potr_send_thread_stop(struct PotrContext_ *ctx);

#ifdef __cplusplus
}
#endif

#endif /* POTR_SEND_THREAD_H */
