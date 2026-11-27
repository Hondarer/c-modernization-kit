/**
 *******************************************************************************
 *  @file           ipc_transport.h
 *  @brief          汎用 IPC 層の内部トランスポート抽象インターフェイスを定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/27
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PROCESS_ORCHESTRATION_IPC_TRANSPORT_H
#define PROCESS_ORCHESTRATION_IPC_TRANSPORT_H

#include <cplat/base/error.h>
#include <cplat/base/result.h>
#include <process_orchestration/ipc.h>
#include <stddef.h>
#include <stdint.h>

/** @brief AF_UNIX の sun_path 最大バイト数 (終端 NUL 含む。Linux・Windows 共通)。 */
#define POS_IPC_UNIX_PATH_MAX 108U

/** @brief 解決済みアドレス文字列の格納バッファー サイズ [byte]。 */
#define POS_IPC_RESOLVED_ADDR_MAX 512U

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** @brief 解決済みエンドポイント情報。 */
    typedef struct pos_ipc_resolved_endpoint
    {
        pos_ipc_backend backend;                    /**< 解決後の実バックエンド (AF_UNIX または NAMED_PIPE)。 */
        pos_ipc_scope scope;                        /**< 元の空間スコープ。 */
        char address[POS_IPC_RESOLVED_ADDR_MAX];    /**< ソケット パスまたは名前付きパイプ名 (UTF-8)。 */
        char parent_dir[POS_IPC_RESOLVED_ADDR_MAX]; /**< AF_UNIX 用の親ディレクトリ パス (不要時は空文字列)。 */
    } pos_ipc_resolved_endpoint;

    /** @brief トランスポート接続ハンドル (バックエンド固有の不透明型)。 */
    typedef struct pos_ipc_transport_conn pos_ipc_transport_conn;

    /** @brief トランスポート リスナー ハンドル (バックエンド固有の不透明型)。 */
    typedef struct pos_ipc_transport_listener pos_ipc_transport_listener;

    /** @brief トランスポート I/O 多重化リアクター ハンドル (バックエンド固有の不透明型)。 */
    typedef struct pos_ipc_transport_reactor pos_ipc_transport_reactor;

    /** @brief リアクターが通知するイベント種別。 */
    typedef enum pos_ipc_reactor_event_kind
    {
        POS_IPC_REACTOR_EVENT_ACCEPT = 1,   /**< 新規クライアント接続を受け付けた。 */
        POS_IPC_REACTOR_EVENT_READABLE = 2, /**< 監視中の接続が読み取り可能になった。 */
        POS_IPC_REACTOR_EVENT_CLOSED = 3    /**< 監視中の接続が切断またはエラー状態になった。 */
    } pos_ipc_reactor_event_kind;

    /** @brief リアクター待機で返却する 1 件のイベント。 */
    typedef struct pos_ipc_reactor_event
    {
        pos_ipc_reactor_event_kind kind;       /**< イベント種別。 */
        uint32_t pad;                          /**< 明示的アラインメント。 */
        void *user_data;                       /**< 接続登録時に渡した任意ポインター。 */
        pos_ipc_transport_conn *accepted_conn; /**< ACCEPT イベント時の新規接続ハンドル。 */
    } pos_ipc_reactor_event;

    /** @brief バックエンド別のトランスポート操作テーブル。 */
    typedef struct pos_ipc_transport_ops
    {
        int (*client_connect)(const pos_ipc_resolved_endpoint *resolved, int timeout_ms,
                              pos_ipc_transport_conn **conn_out, cplat_error *detail_out);
        int (*conn_send_all)(pos_ipc_transport_conn *conn, const void *buf, size_t len, int timeout_ms,
                             cplat_error *detail_out);
        int (*conn_recv_all)(pos_ipc_transport_conn *conn, void *buf, size_t len, int timeout_ms,
                             cplat_error *detail_out);
        void (*conn_close)(pos_ipc_transport_conn *conn);

        int (*listener_open)(const pos_ipc_resolved_endpoint *resolved, const pos_ipc_access_policy *policy,
                             pos_ipc_transport_listener **listener_out, cplat_error *detail_out);
        void (*listener_close)(pos_ipc_transport_listener *listener);

        int (*reactor_create)(pos_ipc_transport_listener *listener, uint32_t max_connections,
                              pos_ipc_transport_reactor **reactor_out, cplat_error *detail_out);
        int (*reactor_add_conn)(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn, void *user_data,
                                cplat_error *detail_out);
        void (*reactor_remove_conn)(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn);
        int (*reactor_wait)(pos_ipc_transport_reactor *reactor, int timeout_ms, pos_ipc_reactor_event *events_out,
                            size_t max_events, size_t *event_count_out, cplat_error *detail_out);
        void (*reactor_wakeup)(pos_ipc_transport_reactor *reactor);
        void (*reactor_destroy)(pos_ipc_transport_reactor *reactor);
    } pos_ipc_transport_ops;

    /** @brief 論理エンドポイントを検証し、バックエンド別の物理アドレスへ解決します。 */
    int pos_ipc_endpoint_resolve(const pos_ipc_endpoint *endpoint, pos_ipc_resolved_endpoint *resolved_out,
                                 cplat_error *detail_out);

    /** @brief AF_UNIX 用の親ディレクトリを必要に応じて再帰作成します (Linux では 0700)。 */
    int pos_ipc_endpoint_ensure_dir(const char *dir_path, cplat_error *detail_out);

    /** @brief 指定バックエンドに対応するトランスポート操作テーブルを取得します。 */
    int pos_ipc_transport_get_ops(pos_ipc_backend backend, const pos_ipc_transport_ops **ops_out,
                                  cplat_error *detail_out);

#if defined(PLATFORM_LINUX)
    const pos_ipc_transport_ops *pos_ipc_transport_unix_linux_ops(void);
#elif defined(PLATFORM_WINDOWS)
const pos_ipc_transport_ops *pos_ipc_transport_unix_windows_ops(void);
const pos_ipc_transport_ops *pos_ipc_transport_pipe_windows_ops(void);
#endif /* PLATFORM_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROCESS_ORCHESTRATION_IPC_TRANSPORT_H */
