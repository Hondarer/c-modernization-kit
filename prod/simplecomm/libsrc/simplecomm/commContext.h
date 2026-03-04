/**
 *******************************************************************************
 *  @file           commContext.h
 *  @brief          セッションコンテキスト内部定義ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @details
 *  CommHandle の実体定義。ライブラリ外部には公開しない。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef COMM_CONTEXT_H
#define COMM_CONTEXT_H

#include <simplecomm_type.h>

#include "protocol/retransmit.h"
#include "protocol/window.h"

#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET     CommSocket;
    typedef HANDLE     CommThread;
    #define COMM_INVALID_SOCKET INVALID_SOCKET
#else
    #include <pthread.h>
    typedef int        CommSocket;
    typedef pthread_t  CommThread;
    #define COMM_INVALID_SOCKET (-1)
#endif

/**
 *  @brief  セッションコンテキスト構造体。CommHandle の実体。
 */
struct CommContext_
{
    CommRecvCallback callback;     /**< 受信コールバック。 */
    CommThread       recv_thread;  /**< 受信スレッドハンドル。 */
    CommServiceDef   service;      /**< サービス定義。 */
    CommGlobalConfig global;       /**< グローバル設定。 */
    CommWindow       send_window;  /**< 送信ウィンドウ。 */
    CommWindow       recv_window;  /**< 受信ウィンドウ。 */
    CommRetransmit   retransmit;   /**< 再送制御。 */
    CommSocket       sock;         /**< UDP ソケット。 */
    volatile int     running;      /**< 受信スレッド実行フラグ (1: 実行中, 0: 停止)。 */
    uint8_t          _pad[4];      /**< パディング。 */
    size_t           frag_buf_len; /**< フラグメント結合バッファの現在のデータ長 (バイト)。 */
    uint8_t          frag_buf[COMM_MAX_MESSAGE_SIZE]; /**< フラグメント結合バッファ。 */
    uint8_t          _frag_pad[1]; /**< パディング (frag_buf を 8 バイト境界に揃える)。 */
};

#endif /* COMM_CONTEXT_H */
