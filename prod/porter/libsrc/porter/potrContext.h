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

#include "protocol/window.h"
#include "compress/compress.h"
#include "potrSendQueue.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET     PotrSocket;
    typedef HANDLE     PotrThread;
    #define POTR_INVALID_SOCKET INVALID_SOCKET
#else
    #include <pthread.h>
    #include <netinet/in.h>
    typedef int        PotrSocket;
    typedef pthread_t  PotrThread;
    #define POTR_INVALID_SOCKET (-1)
#endif

/**
 *  @brief  セッションコンテキスト構造体。PotrHandle の実体。
 */
struct PotrContext_
{
    PotrRecvCallback callback;        /**< 受信コールバック。 */
    PotrThread       recv_thread;     /**< 受信スレッドハンドル。 */
    PotrThread       health_thread;   /**< ヘルスチェックスレッドハンドル (送信者のみ)。 */
    PotrServiceDef   service;      /**< サービス定義。 */
    PotrGlobalConfig global;       /**< グローバル設定。 */
    PotrWindow       send_window;  /**< 送信バッファ (過去 N パケット保持。NACK 再送・REJECT 判定に使用)。 */
    PotrWindow       recv_window;  /**< 受信ウィンドウ (順序整列・欠番検出)。 */
    PotrSocket       sock;            /**< UDP ソケット。 */
    volatile int     running;         /**< 受信スレッド実行フラグ (1: 実行中, 0: 停止)。 */
    volatile int     health_running;  /**< ヘルスチェックスレッド実行フラグ (1: 実行中, 0: 停止)。 */
    volatile int     health_alive;    /**< 疎通状態 (1: alive, 0: dead/未接続)。受信者が管理。 */
    PotrRole         role;            /**< 役割 (POTR_ROLE_SENDER / POTR_ROLE_RECEIVER)。 */

    /* 解決済みアドレス */
    struct in_addr     dst_addr_resolved; /**< 解決済み宛先 IPv4 アドレス。送信先 (送信者) または bind アドレス (受信者)。(unicast のみ) */
    struct in_addr     src_addr_resolved; /**< 解決済み送信元 IPv4 アドレス。bind / 送信インターフェース (送信者) または送信元フィルタ (受信者)。src_addr が設定されている場合のみ有効。 */
    struct sockaddr_in dest_addr;         /**< 送信先ソケットアドレス (送信者が sendto に使用)。 */

    /* 自セッション識別子 (potrOpenService 時に決定) */
    uint32_t         session_id;        /**< 自セッション識別子 (乱数)。 */
    int64_t          session_tv_sec;    /**< 自セッション開始時刻 秒部。 */
    int32_t          session_tv_nsec;   /**< 自セッション開始時刻 ナノ秒部。 */

    /* 相手セッション追跡 (受信者が使用) */
    uint32_t         peer_session_id;      /**< 追跡中の相手セッション識別子。 */
    int64_t          peer_session_tv_sec;  /**< 追跡中の相手セッション開始時刻 秒部。 */
    int32_t          peer_session_tv_nsec; /**< 追跡中の相手セッション開始時刻 ナノ秒部。 */
    int              peer_session_known;   /**< 相手セッションが初期化済みか (0: 未初期化)。 */

    /* ヘルスチェック: 最終受信時刻 (受信者が使用。CLOCK_MONOTONIC 基準)。 */
    int32_t          last_recv_tv_nsec;   /**< 最終受信時刻 ナノ秒部。 */
    uint32_t         _pad_lastrecv;       /**< パディング (last_recv_tv_sec を 8 バイト境界に揃える)。 */
    int64_t          last_recv_tv_sec;    /**< 最終受信時刻 秒部。0 = 未受信。 */

    size_t           frag_buf_len;       /**< フラグメント結合バッファの現在のデータ長 (バイト)。 */
    int              frag_compressed;   /**< フラグメント受信中の圧縮フラグ (非 0: 圧縮あり)。 */
    uint8_t          _frag_pad2[4];     /**< パディング。 */
    uint8_t          frag_buf[POTR_MAX_MESSAGE_SIZE]; /**< フラグメント結合バッファ。 */
    uint8_t          _frag_pad[1];      /**< パディング (frag_buf を 8 バイト境界に揃える)。 */
    uint8_t          compress_buf[POTR_COMPRESS_BUF_SIZE]; /**< 圧縮・解凍用一時バッファ。 */
    uint8_t          _cmp_pad[5];       /**< パディング (compress_buf を 8 バイト境界に揃える)。 */

    /* 非同期送信 (POTR_ROLE_SENDER のみ使用) */
    PotrThread     send_thread;         /**< 送信スレッドハンドル。 */
    volatile int   send_thread_running; /**< 送信スレッド実行フラグ (1: 実行中, 0: 停止)。 */
    uint32_t       _send_pad;           /**< パディング (send_queue を 8 バイト境界に揃える)。 */
    PotrSendQueue  send_queue;          /**< 非同期送信キュー。 */
};

#endif /* POTR_CONTEXT_H */
