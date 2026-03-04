/**
 *******************************************************************************
 *  @file           potrContext.h
 *  @brief          セッションコンテキスト内部定義ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @details
 *  PotrHandle の実体定義。ライブラリ外部には公開しない。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef POTR_CONTEXT_H
#define POTR_CONTEXT_H

#include <porter_type.h>

#include "protocol/retransmit.h"
#include "protocol/window.h"
#include "compress/compress.h"

#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET     PotrSocket;
    typedef HANDLE     PotrThread;
    #define POTR_INVALID_SOCKET INVALID_SOCKET
#else
    #include <pthread.h>
    typedef int        PotrSocket;
    typedef pthread_t  PotrThread;
    #define POTR_INVALID_SOCKET (-1)
#endif

/**
 *  @brief  セッションコンテキスト構造体。PotrHandle の実体。
 */
struct PotrContext_
{
    PotrRecvCallback callback;     /**< 受信コールバック。 */
    PotrThread       recv_thread;  /**< 受信スレッドハンドル。 */
    PotrServiceDef   service;      /**< サービス定義。 */
    PotrGlobalConfig global;       /**< グローバル設定。 */
    PotrWindow       send_window;  /**< 送信ウィンドウ。 */
    PotrWindow       recv_window;  /**< 受信ウィンドウ。 */
    PotrRetransmit   retransmit;   /**< 再送制御。 */
    PotrSocket       sock;         /**< UDP ソケット。 */
    volatile int     running;      /**< 受信スレッド実行フラグ (1: 実行中, 0: 停止)。 */
    uint8_t          _pad[4];      /**< パディング。 */
    size_t           frag_buf_len;       /**< フラグメント結合バッファの現在のデータ長 (バイト)。 */
    int              frag_compressed;   /**< フラグメント受信中の圧縮フラグ (非 0: 圧縮あり)。 */
    uint8_t          _frag_pad2[4];     /**< パディング。 */
    uint8_t          frag_buf[POTR_MAX_MESSAGE_SIZE]; /**< フラグメント結合バッファ。 */
    uint8_t          _frag_pad[1];      /**< パディング (frag_buf を 8 バイト境界に揃える)。 */
    uint8_t          compress_buf[POTR_COMPRESS_BUF_SIZE]; /**< 圧縮・解凍用一時バッファ。 */
    uint8_t          _cmp_pad[5];       /**< パディング (compress_buf を 8 バイト境界に揃える)。 */
};

#endif /* POTR_CONTEXT_H */
