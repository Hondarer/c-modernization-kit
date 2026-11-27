/**
 *******************************************************************************
 *  @file           orchestrator.h
 *  @brief          親プロセスおよび子プロセスを制御する C API を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/27
 *  @version        1.0.0
 *
 *  第三者プロセスから親プロセスへの 4 操作 (親終了、子起動、子終了、子状態取得) と、
 *  稼働中の子プロセス (1〜3) へ親を経由せず直接送信するエコー バック操作を提供します。\n
 *  親プロセスは子が意図せず終了した場合に自動再起動せず、`POS_CHILD_STATE_CRASHED` 状態と
 *  終了コードを保持します。復帰は外部からの @ref pos_orch_child_start で明示的に指示します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

#ifndef PROCESS_ORCHESTRATION_ORCHESTRATOR_H
#define PROCESS_ORCHESTRATION_ORCHESTRATOR_H

#include <cplat/base/error.h>
#include <cplat/base/result.h>
#include <process_orchestration/export.h>
#include <process_orchestration/ipc.h>
#include <stddef.h>
#include <stdint.h>

/** @brief 子プロセス ID の最小値。 */
#define POS_ORCH_CHILD_ID_MIN 1U

/** @brief 子プロセス ID の最大値。 */
#define POS_ORCH_CHILD_ID_MAX 3U

/** @brief 管理対象の子プロセス総数。 */
#define POS_ORCH_CHILD_COUNT 3U

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** @brief 子プロセスのライフサイクル状態。 */
    typedef enum pos_child_state
    {
        POS_CHILD_STATE_STOPPED = 0,  /**< 未起動、または「子の終了」指示による正常停止。 */
        POS_CHILD_STATE_STARTING = 1, /**< 起動処理中 (子 IPC 疎通確認待ちを含む)。 */
        POS_CHILD_STATE_RUNNING = 2,  /**< 稼働中 (子 IPC 受付可能)。 */
        POS_CHILD_STATE_STOPPING = 3, /**< 停止処理中。 */
        POS_CHILD_STATE_CRASHED = 4   /**< 意図的停止以外で予期せず終了した状態 (自動再起動なし)。 */
    } pos_child_state;

    /** @brief 親プロセスが保持する子プロセスの状態情報。 */
    typedef struct pos_child_status
    {
        uint32_t child_id;           /**< 子 ID (1..3)。 */
        pos_child_state state;       /**< 現在のライフサイクル状態。 */
        uint32_t pid;                /**< 稼働中または起動中の PID。STOPPED / CRASHED 時は 0。 */
        uint32_t start_count;        /**< 累計起動回数 (初期起動 + 外部「子の起動」指示)。 */
        int32_t last_exit_code;      /**< 直近の終了コード (`has_last_exit_code == 1` の時有効)。 */
        uint32_t has_last_exit_code; /**< 終了コード取得済みなら 1、未終了・未取得なら 0。 */
    } pos_child_status;

    /** @brief 子プロセスへの直接エコー バック応答のメタ情報。 */
    typedef struct pos_child_echo_result
    {
        uint32_t child_id;  /**< 応答した子 ID (1..3)。 */
        uint32_t child_pid; /**< 応答した子プロセスの PID。 */
        size_t message_len; /**< 返却メッセージ長 [byte] (終端 NUL 除く)。 */
    } pos_child_echo_result;

    /** @brief 親・子プロセス群の空間および接続先共通設定。 */
    typedef struct pos_orch_config
    {
        pos_ipc_scope scope;        /**< 空間スコープ (`POS_IPC_SCOPE_USER` または `POS_IPC_SCOPE_CUSTOM`)。 */
        pos_ipc_backend backend;    /**< トランスポート バックエンド。 */
        const char *namespace_name; /**< 名前空間 (NULL または空文字列時は既定値)。 */
        const char *instance_name;  /**< インスタンス識別子 (NULL または空文字列時は既定値)。 */
        const char *custom_dir;     /**< `POS_IPC_SCOPE_CUSTOM` 時のベース ディレクトリ (UTF-8)。 */
    } pos_orch_config;

    /**
     *  @brief          親プロセス用の IPC エンドポイントを構築します。
     *  @param[in]      config       空間設定。NULL 不可。
     *  @param[out]     endpoint_out 構築先エンドポイント。NULL 不可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_orch_make_parent_endpoint(const pos_orch_config *config, pos_ipc_endpoint *endpoint_out);

    /**
     *  @brief          指定した子プロセス用の IPC エンドポイントを構築します。
     *  @param[in]      config       空間設定。NULL 不可。
     *  @param[in]      child_id     子 ID (1〜3)。
     *  @param[out]     endpoint_out 構築先エンドポイント。NULL 不可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_INVALID_ARGUMENT` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_orch_make_child_endpoint(const pos_orch_config *config, uint32_t child_id,
                                                        pos_ipc_endpoint *endpoint_out);

    /**
     *  @brief          親プロセスへ終了指示を送ります。
     *  @param[in]      config     空間設定。NULL 不可。
     *  @param[in]      timeout_ms タイムアウト [ms]。
     *  @param[out]     detail_out 詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。
     *
     *  親プロセスは稼働中の全子プロセスを停止・回収したうえで成功応答を返し、終了します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_orch_parent_stop(const pos_orch_config *config, int timeout_ms, cplat_error *detail_out);

    /**
     *  @brief          親プロセスへ指定した子の起動指示を送ります。
     *  @param[in]      config     空間設定。NULL 不可。
     *  @param[in]      child_id   起動対象の子 ID (1〜3)。
     *  @param[out]     status_out 起動後の子状態の格納先。NULL 可。
     *  @param[in]      timeout_ms タイムアウト [ms]。
     *  @param[out]     detail_out 詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK`、`CPLAT_ERR_BUSY` (すでに稼働中)、またはその他の `CPLAT_ERR_*` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_orch_child_start(const pos_orch_config *config, uint32_t child_id,
                                                pos_child_status *status_out, int timeout_ms, cplat_error *detail_out);

    /**
     *  @brief          親プロセスへ指定した子の終了指示を送ります。
     *  @param[in]      config     空間設定。NULL 不可。
     *  @param[in]      child_id   終了対象の子 ID (1〜3)。
     *  @param[out]     status_out 終了後の子状態の格納先。NULL 可。
     *  @param[in]      timeout_ms タイムアウト [ms]。
     *  @param[out]     detail_out 詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_orch_child_stop(const pos_orch_config *config, uint32_t child_id,
                                               pos_child_status *status_out, int timeout_ms, cplat_error *detail_out);

    /**
     *  @brief          親プロセスから指定した子の状態を取得します。
     *  @param[in]      config     空間設定。NULL 不可。
     *  @param[in]      child_id   取得対象の子 ID (1〜3)。
     *  @param[out]     status_out 子状態の格納先。NULL 不可。
     *  @param[in]      timeout_ms タイムアウト [ms]。
     *  @param[out]     detail_out 詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK` または `CPLAT_ERR_*` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_orch_child_status(const pos_orch_config *config, uint32_t child_id,
                                                 pos_child_status *status_out, int timeout_ms, cplat_error *detail_out);

    /**
     *  @brief          指定した子プロセスへ親を経由せず直接エコー バック要求を送ります (単発接続)。
     *  @param[in]      config           空間設定。NULL 不可。
     *  @param[in]      child_id         宛先の子 ID (1〜3)。
     *  @param[in]      message          送信する UTF-8 文字列。NULL 不可 (空文字列可)。
     *  @param[out]     echo_buffer      応答文字列格納先 (NUL 終端)。`echo_buffer_size == 0` 時は NULL 可。
     *  @param[in]      echo_buffer_size @p echo_buffer の容量 [byte] (終端 NUL 分を含む)。
     *  @param[out]     result_out       応答メタ情報 (子 ID、子 PID、メッセージ長) の格納先。NULL 可。
     *  @param[in]      timeout_ms       タイムアウト [ms]。
     *  @param[out]     detail_out       詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK`、`CPLAT_ERR_BUFFER_TOO_SMALL`、またはその他の `CPLAT_ERR_*` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT int POS_API pos_orch_child_echo(const pos_orch_config *config, uint32_t child_id, const char *message,
                                               char *echo_buffer, size_t echo_buffer_size,
                                               pos_child_echo_result *result_out, int timeout_ms,
                                               cplat_error *detail_out);

    /**
     *  @brief          開済みの持続接続クライアント経由で子プロセスへ直接エコー バック要求を送ります。
     *  @param[in]      client           子エンドポイントへ接続済みの持続接続ハンドル。NULL 不可。
     *  @param[in]      message          送信する UTF-8 文字列。NULL 不可 (空文字列可)。
     *  @param[out]     echo_buffer      応答文字列格納先 (NUL 終端)。`echo_buffer_size == 0` 時は NULL 可。
     *  @param[in]      echo_buffer_size @p echo_buffer の容量 [byte] (終端 NUL 分を含む)。
     *  @param[out]     result_out       応答メタ情報 (子 ID、子 PID、メッセージ長) の格納先。NULL 可。
     *  @param[in]      timeout_ms       タイムアウト [ms]。
     *  @param[out]     detail_out       詳細エラーの格納先。NULL 可。
     *  @return         `CPLAT_OK`、`CPLAT_ERR_BUFFER_TOO_SMALL`、またはその他の `CPLAT_ERR_*` を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。同一 @p client への同時呼び出しは内部で直列化されます。
     */
    POS_EXPORT int POS_API pos_orch_child_echo_with_client(pos_ipc_client *client, const char *message,
                                                           char *echo_buffer, size_t echo_buffer_size,
                                                           pos_child_echo_result *result_out, int timeout_ms,
                                                           cplat_error *detail_out);

    /**
     *  @brief          @ref pos_child_state の表示用 ASCII 文字列を返します。
     *  @param[in]      state 状態値。
     *  @return         `"STOPPED"` などの固定文字列 (不正値は `"UNKNOWN"`) を返します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。
     */
    POS_EXPORT const char *POS_API pos_orch_child_state_name(pos_child_state state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROCESS_ORCHESTRATION_ORCHESTRATOR_H */
