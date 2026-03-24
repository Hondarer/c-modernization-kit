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

#include <stdint.h>

#include <porter_type.h>

#include "protocol/window.h"
#include "infra/compress/compress.h"
#include "infra/crypto/crypto.h"
#include "infra/potrSendQueue.h"

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

/** TCP 通信種別 (POTR_TYPE_TCP / POTR_TYPE_TCP_BIDIR) か判定する。 */
static inline int potr_is_tcp_type(PotrType t)
{
    return t == POTR_TYPE_TCP || t == POTR_TYPE_TCP_BIDIR;
}

/** RAW 系通信種別 (POTR_TYPE_*_RAW) か判定する。 */
static inline int potr_is_raw_type(PotrType t)
{
    return t == POTR_TYPE_UNICAST_RAW   ||
           t == POTR_TYPE_MULTICAST_RAW ||
           t == POTR_TYPE_BROADCAST_RAW;
}

/** RAW 系通信種別をベース通信種別に変換する (非 RAW 型はそのまま返す)。 */
static inline PotrType potr_raw_base_type(PotrType t)
{
    switch (t)
    {
        case POTR_TYPE_UNICAST_RAW:   return POTR_TYPE_UNICAST;
        case POTR_TYPE_MULTICAST_RAW: return POTR_TYPE_MULTICAST;
        case POTR_TYPE_BROADCAST_RAW: return POTR_TYPE_BROADCAST;
        case POTR_TYPE_UNICAST:
        case POTR_TYPE_MULTICAST:
        case POTR_TYPE_BROADCAST:
        case POTR_TYPE_TCP:
        case POTR_TYPE_TCP_BIDIR:
        case POTR_TYPE_UNICAST_BIDIR:
        default:                      return t;
    }
}

/** NACK 重複抑制リングバッファのスロット数 (POTR_MAX_PATH × 2)。 */
#define POTR_NACK_DEDUP_SLOTS 8U

/** NACK 重複抑制バッファの 1 エントリ。 */
typedef struct
{
    uint32_t ack_num; /**< 再送または REJECT した ack_num。 */
    uint32_t _pad;    /**< パディング (time_ms を 8 バイト境界に揃える)。 */
    uint64_t time_ms; /**< 処理時刻 (ms、単調増加)。0 = 未使用スロット。 */
} PotrNackDedupEntry;

/**
 *  @brief  N:1 モードにおける個別ピアのコンテキスト。
 *
 *  @details
 *  is_multi_peer == 1 のとき有効。ピアごとに独立した送受信状態を保持する。\n
 *  ピアテーブル (PotrContext_::peers[]) に配置される。
 */
typedef struct PotrPeerContext_
{
    PotrPeerId  peer_id; /**< 外部公開用ピア識別子 (単調増加カウンタから付与)。 */
    int         active;  /**< 1: 有効スロット, 0: 空き。 */

    /* 自セッション (このピア宛の送信に使用) */
    uint32_t session_id;        /**< 自セッション識別子 (乱数)。 */
    uint32_t _pad_session;      /**< パディング (session_tv_sec を 8 バイト境界に揃える)。 */
    int64_t  session_tv_sec;    /**< 自セッション開始時刻 秒部。 */
    int32_t  session_tv_nsec;   /**< 自セッション開始時刻 ナノ秒部。 */

    /* ピアセッション追跡 */
    uint32_t peer_session_id;       /**< 追跡中のピアセッション識別子。 */
    int64_t  peer_session_tv_sec;   /**< 追跡中のピアセッション開始時刻 秒部。 */
    int32_t  peer_session_tv_nsec;  /**< 追跡中のピアセッション開始時刻 ナノ秒部。 */
    int      peer_session_known;    /**< ピアセッションが初期化済みか (0: 未初期化)。 */

    /* 送受信ウィンドウ (ピアごと独立) */
    PotrWindow send_window;        /**< 送信ウィンドウ (NACK 再送用)。 */
    PotrMutex  send_window_mutex;  /**< send_window 保護 (送信・受信・ヘルスチェックスレッド競合)。 */
    PotrWindow recv_window;        /**< 受信ウィンドウ (順序整列)。 */

    /* フラグメント結合 (ピアごと独立) */
    uint8_t *frag_buf;       /**< フラグメント結合バッファ (動的確保)。 */
    size_t   frag_buf_len;   /**< 現在のデータ長。 */
    int      frag_compressed;/**< 圧縮フラグ (非 0: 圧縮あり)。 */

    /* ヘルスチェック */
    volatile int health_alive;      /**< 疎通状態 (1: alive, 0: dead/未接続)。 */
    int64_t      last_recv_tv_sec;  /**< 最終受信時刻 秒部 (CLOCK_MONOTONIC)。0 = 未受信。 */
    int32_t      last_recv_tv_nsec; /**< 最終受信時刻 ナノ秒部。 */
    uint32_t     _pad_nack_dedup;   /**< パディング (nack_dedup_buf を 8 バイト境界に揃える)。 */

    /* NACK 重複抑制 */
    PotrNackDedupEntry nack_dedup_buf[POTR_NACK_DEDUP_SLOTS]; /**< NACK 重複抑制バッファ。 */
    uint8_t            nack_dedup_next;                        /**< 次に書き込むスロット。 */
    uint8_t            _pad_reorder[3];                        /**< パディング (reorder_pending を 4 バイト境界に揃える)。 */

    /* リオーダーバッファタイムアウト管理 */
    int      reorder_pending;       /**< リオーダー待機中 (1: 待機中, 0: 待機なし)。 */
    uint32_t reorder_nack_num;      /**< 待機中の欠番通番。 */
    uint32_t _pad_reorder_dl;       /**< パディング (reorder_deadline_sec を 8 バイト境界に揃える)。 */
    int64_t  reorder_deadline_sec;  /**< タイムアウト期限 秒部 (CLOCK_MONOTONIC)。 */
    int32_t  reorder_deadline_nsec; /**< タイムアウト期限 ナノ秒部。 */

    /* マルチパス: ピアごとの送信先 (recvfrom で学習) */
    struct sockaddr_in dest_addr[POTR_MAX_PATH];         /**< 送信先ソケットアドレス。 */
    int                n_paths;                          /**< 学習済みパス数。 */
    int64_t            path_last_recv_sec[POTR_MAX_PATH]; /**< パスごとの最終受信時刻 秒部。 */
    int32_t            path_last_recv_nsec[POTR_MAX_PATH];/**< パスごとの最終受信時刻 ナノ秒部。 */
} PotrPeerContext;

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
    uint32_t         _pad_global;  /**< パディング (send_window を 8 バイト境界に揃える)。 */
    PotrWindow       send_window;       /**< 送信バッファ (過去 N パケット保持。NACK 再送・REJECT 判定に使用)。 */
    PotrMutex        send_window_mutex; /**< send_window 保護用ミューテックス (送信スレッド・ヘルスチェックスレッド・受信スレッドが競合するため)。 */
    PotrWindow       recv_window;       /**< 受信ウィンドウ (順序整列・欠番検出)。 */

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
    uint32_t         _pad_frag;         /**< パディング (frag_buf を 8 バイト境界に揃える)。 */
    uint8_t         *frag_buf;          /**< フラグメント結合バッファ (動的確保。max_message_size バイト)。 */
    uint8_t         *compress_buf;      /**< 圧縮・解凍用一時バッファ (動的確保)。 */
    size_t           compress_buf_size; /**< compress_buf のサイズ (バイト)。 */
    uint8_t         *crypto_buf;        /**< 暗号化・復号用一時バッファ (動的確保)。 */
    size_t           crypto_buf_size;   /**< crypto_buf のサイズ (バイト)。 */
    uint8_t         *recv_buf;          /**< 受信バッファ / 再送 wire 組立バッファ (動的確保。PACKET_HEADER_SIZE + max_payload バイト)。 */
    uint8_t         *send_wire_buf;     /**< 送信 wire 組立バッファ (動的確保。PACKET_HEADER_SIZE + max_payload バイト)。送信スレッドのみ使用。 */

    /* 非同期送信 (POTR_ROLE_SENDER のみ使用) */
    PotrThread        send_thread;          /**< 送信スレッドハンドル。 */
    volatile int      send_thread_running;  /**< 送信スレッド実行フラグ (1: 実行中, 0: 停止)。 */
    uint32_t          _pad_send_thread;     /**< パディング (last_send_ms を 8 バイト境界に揃える)。 */
    volatile uint64_t last_send_ms;         /**< 最終パケット送信時刻 (データ or PING、ms、単調増加)。0 = 未送信。 */

    /* 送信者: NACK 重複抑制リングバッファ */
    PotrNackDedupEntry nack_dedup_buf[POTR_NACK_DEDUP_SLOTS]; /**< NACK 重複抑制エントリ配列。 */
    uint8_t            nack_dedup_next;                        /**< 次に書き込むスロットインデックス。 */
    uint8_t            _pad_nack_dedup[7];                     /**< パディング (reorder フィールドを 4 バイト境界に揃える)。 */

    /* 受信者: リオーダーバッファタイムアウト管理 (reorder_timeout_ms > 0 のときのみ使用) */
    int      reorder_pending;        /**< リオーダー待機中か (1: 待機中、0: 待機なし)。 */
    uint32_t reorder_nack_num;       /**< 待機中の欠番通番。 */
    int64_t  reorder_deadline_sec;   /**< タイムアウト期限 秒部 (CLOCK_MONOTONIC)。 */
    int32_t  reorder_deadline_nsec;  /**< タイムアウト期限 ナノ秒部。 */
    uint32_t _pad_reorder;           /**< パディング (send_queue を 8 バイト境界に揃える)。 */

    PotrSendQueue  send_queue;          /**< 非同期送信キュー。 */

    /* N:1 マルチピアモード専用フィールド (is_multi_peer == 1 のときのみ有効) */
    int              is_multi_peer;    /**< 1: N:1 モード (src_addr/src_port 省略), 0: 1:1 モード。 */
    uint32_t         _pad_multi_peer; /**< パディング (peers を 8 バイト境界に揃える)。 */
    PotrPeerContext *peers;            /**< ピアテーブル (動的確保。max_peers エントリ)。 */
    int              max_peers;      /**< ピアテーブルサイズ (service.max_peers から取得)。 */
    int              n_peers;        /**< 現在の接続ピア数。 */
    PotrMutex        peers_mutex;    /**< ピアテーブル保護用ミューテックス。 */
    uint32_t         next_peer_id;   /**< 次に発行するピア ID (単調増加、初期値 1)。 */

    /* --- TCP 接続管理 (POTR_TYPE_TCP / POTR_TYPE_TCP_BIDIR のみ有効) --- */
    PotrSocket         tcp_listen_sock;            /**< RECEIVER: listen ソケット。 */
    PotrSocket         tcp_conn_fd[POTR_MAX_PATH]; /**< アクティブ TCP 接続 fd。v1 は [0] のみ使用。v2 以降でマルチパス対応。 */
    volatile int       tcp_connected;              /**< 1 = TCP 接続確立済み。 */
    uint32_t           _pad_tcp_connected;         /**< パディング (tcp_send_mutex を 8 バイト境界に揃える)。 */
    PotrMutex          tcp_send_mutex;             /**< TCP send() 排他制御 (送信スレッド・ヘルスチェックスレッド・recv スレッド競合防止)。 */

    /* connect/accept スレッド */
    PotrThread         connect_thread;             /**< SENDER: connect スレッド。RECEIVER: accept スレッド。 */
    volatile int       connect_thread_running;     /**< connect スレッド実行フラグ (1: 実行中, 0: 停止)。 */
    uint32_t           _pad_connect_thread;        /**< パディング (tcp_state_mutex を 8 バイト境界に揃える)。 */

    /* 切断通知 (recv/health スレッド → connect スレッドへの通知) */
    PotrMutex          tcp_state_mutex;            /**< tcp_state_cv 保護用ミューテックス。 */
    PotrCondVar        tcp_state_cv;               /**< 切断通知・reconnect sleep の中断用条件変数。 */

    /* PING 応答追跡 (SENDER health スレッドが参照、TCP recv スレッドが更新) */
    volatile uint64_t  tcp_last_ping_recv_ms;      /**< TCP PING 応答最終受信時刻 (ms, CLOCK_MONOTONIC 基準)。接続確立時に現在時刻で初期化。 */

    /* PING 要求到着追跡 (RECEIVER recv スレッドが参照・更新。RECEIVER 側 PING 到着タイムアウト監視に使用) */
    volatile uint64_t  tcp_last_ping_req_recv_ms;  /**< TCP PING 要求最終受信時刻 (ms, CLOCK_MONOTONIC 基準)。接続確立時に現在時刻で初期化。 */
};

#endif /* POTR_CONTEXT_H */
