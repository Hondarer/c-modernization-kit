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
    typedef SOCKET             PotrSocket;
    typedef HANDLE             PotrThread;
    typedef CRITICAL_SECTION   PotrMutex;
    typedef CONDITION_VARIABLE PotrCondVar;
    #define POTR_INVALID_SOCKET INVALID_SOCKET
#else
    #include <pthread.h>
    #include <netinet/in.h>
    typedef int                PotrSocket;
    typedef pthread_t          PotrThread;
    typedef pthread_mutex_t    PotrMutex;
    typedef pthread_cond_t     PotrCondVar;
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
    PotrMutex        health_mutex;    /**< ヘルスチェックスレッド停止用ミューテックス。 */
    PotrCondVar      health_wakeup;   /**< ヘルスチェックスレッドを即時起床させる条件変数。 */
    PotrServiceDef   service;      /**< サービス定義。 */
    PotrGlobalConfig global;       /**< グローバル設定。 */
    PotrWindow       send_window;  /**< 送信バッファ (過去 N パケット保持。NACK 再送・REJECT 判定に使用)。 */
    PotrWindow       recv_window;  /**< 受信ウィンドウ (順序整列・欠番検出)。 */

    /* マルチパス: ソケット配列 */
    PotrSocket         sock[POTR_MAX_PATH];              /**< 各パスの UDP ソケット。 */
    int                n_path;                               /**< 有効パス数。 */

    volatile int     running;         /**< 受信スレッド実行フラグ (1: 実行中, 0: 停止)。 */
    volatile int     health_running;  /**< ヘルスチェックスレッド実行フラグ (1: 実行中, 0: 停止)。 */
    volatile int     health_alive;    /**< 疎通状態 (1: alive, 0: dead/未接続)。受信者が管理。 */
    PotrRole         role;            /**< 役割 (POTR_ROLE_SENDER / POTR_ROLE_RECEIVER)。 */

    /* 解決済みアドレス (各パス分) */
    struct in_addr     src_addr_resolved[POTR_MAX_PATH]; /**< 解決済み送信元 IPv4 アドレス。 */
    struct in_addr     dst_addr_resolved[POTR_MAX_PATH]; /**< 解決済み宛先 IPv4 アドレス (unicast のみ)。 */
    struct sockaddr_in dest_addr[POTR_MAX_PATH];         /**< 送信先ソケットアドレス (送信者が sendto に使用)。 */

    /* 自セッション識別子 (potrOpenService 時に決定) */
    uint32_t         session_id;        /**< 自セッション識別子 (乱数)。 */
    int64_t          session_tv_sec;    /**< 自セッション開始時刻 秒部。 */
    int32_t          session_tv_nsec;   /**< 自セッション開始時刻 ナノ秒部。 */

    /* 相手セッション追跡 (受信者が使用) */
    uint32_t         peer_session_id;      /**< 追跡中の相手セッション識別子。 */
    int64_t          peer_session_tv_sec;  /**< 追跡中の相手セッション開始時刻 秒部。 */
    int32_t          peer_session_tv_nsec; /**< 追跡中の相手セッション開始時刻 ナノ秒部。 */
    int              peer_session_known;   /**< 相手セッションが初期化済みか (0: 未初期化)。 */

    /* 受信者: パスごとの送信者ポートキャッシュ (src_port=0 対応) */
    uint16_t           peer_port[POTR_MAX_PATH]; /**< 各パスで観測した送信者ポート (NBO)。0 = 未観測。 */

    /* ヘルスチェック: 最終受信時刻 (受信者が使用。CLOCK_MONOTONIC 基準)。 */
    int32_t          last_recv_tv_nsec;   /**< 最終受信時刻 ナノ秒部。 */
    uint32_t         _pad_lastrecv;       /**< パディング (last_recv_tv_sec を 8 バイト境界に揃える)。 */
    int64_t          last_recv_tv_sec;    /**< 最終受信時刻 秒部。0 = 未受信。 */

    /* 受信者: パスごとの最終受信時刻 (パス単位の peer_port クリア用。CLOCK_MONOTONIC 基準)。 */
    int64_t            path_last_recv_sec[POTR_MAX_PATH];  /**< パスごとの最終受信時刻 秒部。0 = 未受信。 */
    int32_t            path_last_recv_nsec[POTR_MAX_PATH]; /**< パスごとの最終受信時刻 ナノ秒部。 */

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

    /* 送信者: NACK 重複抑制キャッシュ */
    uint32_t           last_nack_ack_num; /**< 直近に再送または REJECT した ack_num。 */
    uint64_t           last_nack_time_ms; /**< last_nack_ack_num を処理した時刻 (ms、単調増加)。0 = 未処理。 */

    PotrSendQueue  send_queue;          /**< 非同期送信キュー。 */
};

#endif /* POTR_CONTEXT_H */
