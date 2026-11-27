/**
 *******************************************************************************
 *  @file           ipc.h
 *  @brief          C 関数から同期的に特定プロセスへ要求を送り応答を受け取る汎用ローカル IPC API を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/27
 *  @version        1.0.0
 *
 *  将来の `cplat` 統合を見据え、結果コード (`cplat/base/result.h`)、詳細エラー (`cplat/base/error.h`)、
 *  および待機時間規約 (`cplat/base/wait.h`) に準拠した汎用同期要求応答 IPC 層です。\n
 *  固定長バイナリ ヘッダー + 不透明バイト列ペイロードにより、上位層は任意のペイロード形式を利用できます。\n
 *  呼び出し元は用途に応じて単発接続 (@ref pos_ipc_call) と持続接続 (@ref pos_ipc_client_open /
 *  @ref pos_ipc_client_call / @ref pos_ipc_client_close) を選択できます。\n
 *  サーバー側は I/O 多重化 (`poll` / `WSAPoll` / Windows 名前付きパイプ IOCP) と固定数ワーカー スレッド プールにより、
 *  待機中の持続接続がスレッドを占有しない構成とします。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

#ifndef PROCESS_ORCHESTRATION_IPC_H
#define PROCESS_ORCHESTRATION_IPC_H

#include <cplat/base/error.h>
#include <cplat/base/result.h>
#include <cplat/base/wait.h>
#include <process_orchestration/export.h>
#include <stddef.h>
#include <stdint.h>

/** @brief 汎用 IPC が既定で許可する最大ペイロード サイズ [byte] (1 MiB)。 */
#define POS_IPC_DEFAULT_MAX_PAYLOAD_SIZE (1024U * 1024U)

/** @brief サーバーの既定ワーカー スレッド数。 */
#define POS_IPC_DEFAULT_WORKER_COUNT 4U

/** @brief サーバーの既定最大同時接続数。 */
#define POS_IPC_DEFAULT_MAX_CONNECTIONS 128U

/** @brief 持続接続の既定アイドル切断タイムアウト [ms]。 */
#define POS_IPC_DEFAULT_IDLE_TIMEOUT_MS 30000

/** @brief サーバー側 1 要求あたりの既定 I/O タイムアウト [ms]。 */
#define POS_IPC_DEFAULT_IO_TIMEOUT_MS 5000

/** @brief サーバーへ登録できる最大メソッド ハンドラー数。 */
#define POS_IPC_MAX_HANDLERS 64U

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** @brief エンドポイントの空間スコープ。 */
    typedef enum pos_ipc_scope
    {
        POS_IPC_SCOPE_USER = 0,   /**< 同一ユーザー空間。 */
        POS_IPC_SCOPE_SYSTEM = 1, /**< 共通サービス空間 (daemon / Windows Service。本 PoC では型定義のみ)。 */
        POS_IPC_SCOPE_CUSTOM = 2  /**< 呼び出し側指定の任意ベース ディレクトリ (テスト隔離向け)。 */
    } pos_ipc_scope;

    /** @brief 内部トランスポート バックエンド種別。 */
    typedef enum pos_ipc_backend
    {
        POS_IPC_BACKEND_DEFAULT = 0,   /**< 各 OS の既定 (Linux / Windows ともに AF_UNIX)。 */
        POS_IPC_BACKEND_AF_UNIX = 1,   /**< Unix ドメイン ソケット (SOCK_STREAM)。 */
        POS_IPC_BACKEND_NAMED_PIPE = 2 /**< Windows 名前付きパイプ (Linux では CPLAT_ERR_UNSUPPORTED)。 */
    } pos_ipc_backend;

    /** @brief サーバー側エンドポイントのアクセス権ポリシー種別。 */
    typedef enum pos_ipc_access_mode
    {
        POS_IPC_ACCESS_CURRENT_USER = 0,       /**< 作成ユーザーのみ許可 (USER / CUSTOM 既定)。 */
        POS_IPC_ACCESS_GROUP = 1,              /**< 指定グループへ許可 (SYSTEM スコープ向け予約)。 */
        POS_IPC_ACCESS_AUTHENTICATED_USERS = 2 /**< 認証済みローカル ユーザーへ許可 (SYSTEM スコープ向け予約)。 */
    } pos_ipc_access_mode;

    /** @brief サーバー側エンドポイントのアクセス権ポリシー。 */
    typedef struct pos_ipc_access_policy
    {
        pos_ipc_access_mode mode; /**< アクセス権ポリシー種別。 */
        uint32_t pad;             /**< 明示的アラインメント。 */
        const char *group_name;   /**< POS_IPC_ACCESS_GROUP 指定時のグループ名 (NULL 可)。 */
    } pos_ipc_access_policy;

    /**
     *  @brief          IPC 接続先エンドポイントの論理指定。
     *
     *  `namespace_name`、`instance_name`、`target_name` には `[A-Za-z0-9_-]` のみ使用できます。\n
     *  `AF_UNIX` では解決後の UTF-8 パス長が 107 バイト以下である必要があります (`UNIX_PATH_MAX` = 108 バイト制約)。
     */
    typedef struct pos_ipc_endpoint
    {
        pos_ipc_scope scope;        /**< 空間スコープ。 */
        pos_ipc_backend backend;    /**< トランスポート バックエンド。 */
        const char *namespace_name; /**< 名前空間 (NULL または空文字列時は既定 "pos")。 */
        const char *instance_name;  /**< インスタンス識別子 (NULL または空文字列時は既定 "default")。 */
        const char *target_name;    /**< 宛先プロセス識別名 (NULL・空文字列不可)。 */
        const char *custom_dir;     /**< POS_IPC_SCOPE_CUSTOM 時のベース ディレクトリ (UTF-8)。 */
    } pos_ipc_endpoint;

    /** @brief 持続接続クライアント ハンドル (不透明型)。 */
    typedef struct pos_ipc_client pos_ipc_client;

    /** @brief 汎用 IPC サーバー ハンドル (不透明型)。 */
    typedef struct pos_ipc_server pos_ipc_server;

    /**
     *  @brief          サーバー側メソッド ハンドラー関数型。
     *  @param[in]      method_id         呼び出されたメソッド ID。
     *  @param[in]      request_data      要求ペイロード。`request_size == 0` の場合は NULL 可。
     *  @param[in]      request_size      要求ペイロード長 [byte]。
     *  @param[out]     response_buffer   応答ペイロードの格納先。
     *  @param[in]      response_capacity @p response_buffer の容量 [byte]。
     *  @param[out]     response_size_out 書き込んだ応答ペイロード長 [byte] の格納先。
     *  @param[in]      user_context      登録時に渡した任意コンテキスト。
     *  @return         処理結果コード (`CPLAT_OK` または `CPLAT_ERR_*`)。クライアントへ応答ステータスとして返却されます。
     *
     *  @par            スレッド セーフ
     *  ハンドラーは複数のワーカー スレッドから同時に呼び出されるため、共有状態は呼び出し側で同期してください。
     */
    typedef int (*pos_ipc_handler_fn)(uint32_t method_id, const void *request_data, size_t request_size,
                                      void *response_buffer, size_t response_capacity, size_t *response_size_out,
                                      void *user_context);

    /** @brief サーバー生成オプション。0 指定の各フィールドには既定値が適用されます。 */
    typedef struct pos_ipc_server_options
    {
        pos_ipc_access_policy access_policy; /**< アクセス権ポリシー。 */
        uint32_t worker_count;               /**< 固定ワーカー スレッド数 (0 時は既定値)。 */
        uint32_t max_connections;            /**< 最大同時接続数上限 (0 時は既定値)。 */
        uint32_t max_payload_size;           /**< 最大ペイロード長 [byte] (0 時は既定値)。 */
        int idle_timeout_ms;                 /**< 持続接続アイドル切断期限 [ms] (0 時は既定値)。 */
        int io_timeout_ms;                   /**< 1 要求の送受信タイムアウト [ms] (0 時は既定値)。 */
        int pad;                             /**< 明示的アラインメント。 */
    } pos_ipc_server_options;

    /**
     *  @brief          単発接続で同期要求を送信し応答を受け取ります。
     *  @param[in]      endpoint          接続先エンドポイント。NULL 不可。
     *  @param[in]      method_id         呼び出すメソッド ID。
     *  @param[in]      request_data      要求ペイロード。`request_size == 0` の場合のみ NULL 可。
     *  @param[in]      request_size      要求ペイロード長 [byte]。
     *  @param[out]     response_buffer   応答ペイロード格納先。`response_capacity == 0` の場合のみ NULL 可。
     *  @param[in]      response_capacity @p response_buffer の容量 [byte]。
     *  @param[out]     response_size_out 応答ペイロード長 [byte] の格納先。NULL 可。
     *                                    `CPLAT_ERR_BUFFER_TOO_SMALL` 時には必要なバイト数を格納します。
     *  @param[in]      timeout_ms        タイムアウト [ms] (`CPLAT_WAIT_FOREVER`、`CPLAT_NO_WAIT`、または 0 以上)。
     *  @param[out]     detail_out        詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_ipc_call(const pos_ipc_endpoint *endpoint, uint32_t method_id, const void *request_data,
                                        size_t request_size, void *response_buffer, size_t response_capacity,
                                        size_t *response_size_out, int timeout_ms, cplat_error *detail_out);

    /**
     *  @brief          持続接続クライアントを開きます。
     *  @param[in]      endpoint   接続先エンドポイント。NULL 不可。
     *  @param[in]      timeout_ms 接続タイムアウト [ms]。
     *  @param[out]     client_out 生成したクライアント ハンドルの格納先。NULL 不可。失敗時は NULL。
     *  @param[out]     detail_out 詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_ipc_client_open(const pos_ipc_endpoint *endpoint, int timeout_ms,
                                               pos_ipc_client **client_out, cplat_error *detail_out);

    /**
     *  @brief          持続接続上で同期要求を送信し応答を受け取ります。
     *  @param[in]      client            開いた持続接続ハンドル。NULL 不可。
     *  @param[in]      method_id         呼び出すメソッド ID。
     *  @param[in]      request_data      要求ペイロード。`request_size == 0` の場合のみ NULL 可。
     *  @param[in]      request_size      要求ペイロード長 [byte]。
     *  @param[out]     response_buffer   応答ペイロード格納先。`response_capacity == 0` の場合のみ NULL 可。
     *  @param[in]      response_capacity @p response_buffer の容量 [byte]。
     *  @param[out]     response_size_out 応答ペイロード長 [byte] の格納先。NULL 可。
     *                                    `CPLAT_ERR_BUFFER_TOO_SMALL` 時には必要なバイト数を格納します。
     *  @param[in]      timeout_ms        送受信タイムアウト [ms]。
     *  @param[out]     detail_out        詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。
     *
     *  相手側による切断 (アイドル タイムアウトやプロセス終了) を検出した場合は暗黙の再接続を行わず、
     *  `CPLAT_ERR_EOF` 等のエラーを返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。同一 @p client に対する複数スレッドからの呼び出しは内部ロックで直列化します。
     *  ただし @ref pos_ipc_client_close との同時呼び出しはできません。
     */
    POS_EXPORT int POS_API pos_ipc_client_call(pos_ipc_client *client, uint32_t method_id, const void *request_data,
                                               size_t request_size, void *response_buffer, size_t response_capacity,
                                               size_t *response_size_out, int timeout_ms, cplat_error *detail_out);

    /**
     *  @brief          持続接続クライアントを閉じて破棄します。
     *  @param[in]      client 破棄するハンドル。NULL 可。
     *
     *  @par            スレッド セーフ
     *  異なるハンドルに対する呼び出しは同時に実行できます。同一ハンドルに対する他操作との同時呼び出しはできません。
     */
    POS_EXPORT void POS_API pos_ipc_client_close(pos_ipc_client *client);

    /**
     *  @brief          汎用 IPC サーバーを生成します (まだ待ち受けは開始しません)。
     *  @param[in]      endpoint   待ち受けエンドポイント。NULL 不可。
     *  @param[in]      options    サーバー オプション。NULL の場合はすべて既定値を使用します。
     *  @param[out]     server_out 生成したサーバー ハンドルの格納先。NULL 不可。失敗時は NULL。
     *  @param[out]     detail_out 詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_ipc_server_create(const pos_ipc_endpoint *endpoint,
                                                 const pos_ipc_server_options *options, pos_ipc_server **server_out,
                                                 cplat_error *detail_out);

    /**
     *  @brief          サーバーへメソッド ハンドラーを登録します。
     *  @param[in]      server       対象のサーバー。NULL 不可。@ref pos_ipc_server_start 前に呼び出してください。
     *  @param[in]      method_id    登録するメソッド ID。
     *  @param[in]      handler      ハンドラー関数。NULL 不可。
     *  @param[in]      user_context ハンドラーへ渡す任意コンテキスト。NULL 可。
     *  @return         `CPLAT_OK`、`CPLAT_ERR_INVALID_ARGUMENT`、`CPLAT_ERR_BUSY` (起動済み)、
     *                  `CPLAT_ERR_DUPLICATE_DEFINITION`、`CPLAT_ERR_LIMIT_EXCEEDED` のいずれかを返します。
     *
     *  @par            スレッド セーフ
     *  同一サーバーに対する操作は呼び出し側で直列化してください。
     */
    POS_EXPORT int POS_API pos_ipc_server_register_handler(pos_ipc_server *server, uint32_t method_id,
                                                           pos_ipc_handler_fn handler, void *user_context);

    /**
     *  @brief          サーバーの待ち受けとワーカー スレッド プールを開始します。
     *  @param[in]      server     対象のサーバー。NULL 不可。
     *  @param[out]     detail_out 詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。
     *                  同一エンドポイントで別サーバーが稼働中の場合は `CPLAT_ERR_BUSY` を返します。
     *
     *  @par            スレッド セーフ
     *  同一サーバーに対する操作は呼び出し側で直列化してください。
     */
    POS_EXPORT int POS_API pos_ipc_server_start(pos_ipc_server *server, cplat_error *detail_out);

    /**
     *  @brief          サーバーを停止し、稼働中ワーカーの完了を待機します。
     *  @param[in]      server     対象のサーバー。NULL 不可。
     *  @param[in]      timeout_ms 停止待機タイムアウト [ms]。
     *  @param[out]     detail_out 詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。未起動または停止済みなら `CPLAT_OK` を返します。
     *
     *  @par            スレッド セーフ
     *  同一サーバーに対する操作は呼び出し側で直列化してください。
     */
    POS_EXPORT int POS_API pos_ipc_server_stop(pos_ipc_server *server, int timeout_ms, cplat_error *detail_out);

    /**
     *  @brief          サーバー ハンドルを破棄します。稼働中の場合は停止してから解放します。
     *  @param[in]      server 破棄するサーバー。NULL 可。
     *
     *  @par            スレッド セーフ
     *  異なるサーバーに対する呼び出しは同時に実行できます。同一サーバーに対する他操作との同時呼び出しはできません。
     */
    POS_EXPORT void POS_API pos_ipc_server_dispose(pos_ipc_server *server);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROCESS_ORCHESTRATION_IPC_H */
