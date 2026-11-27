/**
 *******************************************************************************
 *  @file           ipc_protocol.h
 *  @brief          汎用 IPC 層の固定長バイナリ ヘッダー定義とフレーム送受信ヘルパーを宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/27
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PROCESS_ORCHESTRATION_IPC_PROTOCOL_H
#define PROCESS_ORCHESTRATION_IPC_PROTOCOL_H

#include <cplat/base/error.h>
#include <process_orchestration/ipc_transport.h>
#include <stddef.h>
#include <stdint.h>

/** @brief フレーム ヘッダーの固定マジック値 ("POSI")。 */
#define POS_IPC_FRAME_MAGIC 0x504F5349U

/** @brief 現在のプロトコル バージョン。 */
#define POS_IPC_FRAME_VERSION 1U

/** @brief 固定長フレーム ヘッダーのバイト数。 */
#define POS_IPC_FRAME_HEADER_SIZE 24U

/** @brief 要求フレームを示すフラグ。 */
#define POS_IPC_FRAME_FLAG_REQUEST 0x0001U

/** @brief 応答フレームを示すフラグ。 */
#define POS_IPC_FRAME_FLAG_RESPONSE 0x0002U

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** @brief ホスト表現の IPC フレーム ヘッダー。 */
    typedef struct pos_ipc_frame_header
    {
        uint32_t magic;        /**< マジック値 (@ref POS_IPC_FRAME_MAGIC)。 */
        uint16_t version;      /**< プロトコル版 (@ref POS_IPC_FRAME_VERSION)。 */
        uint16_t flags;        /**< 要求/応答フラグ。 */
        uint32_t method_id;    /**< 操作識別子。 */
        int32_t status_code;   /**< 要求時は 0、応答時は CPLAT_OK / CPLAT_ERR_*。 */
        uint32_t payload_size; /**< 後続ペイロードのバイト数。 */
        uint32_t reserved;     /**< 予約領域 (0 固定)。 */
    } pos_ipc_frame_header;

    /** @brief 固定長ヘッダーをネットワーク バイト オーダー (24 バイト) へシリアライズします。 */
    void pos_ipc_frame_encode_header(const pos_ipc_frame_header *header, uint8_t out_bytes[POS_IPC_FRAME_HEADER_SIZE]);

    /** @brief 24 バイト列からヘッダーをデコードし、マジック・版・フラグ・サイズ上限を検証します。 */
    int pos_ipc_frame_decode_header(const uint8_t in_bytes[POS_IPC_FRAME_HEADER_SIZE], uint16_t expected_flags,
                                    uint32_t max_payload_size, pos_ipc_frame_header *header_out);

    /** @brief 接続へ 1 フレーム (ヘッダー + ペイロード) を送信します。 */
    int pos_ipc_frame_send(const pos_ipc_transport_ops *ops, pos_ipc_transport_conn *conn,
                           const pos_ipc_frame_header *header, const void *payload, int timeout_ms,
                           cplat_error *detail_out);

    /**
     *  @brief          接続から 1 フレーム (ヘッダー + ペイロード) を受信します。
     *
     *  ペイロード長が @p buffer_capacity を超える場合は、ストリーム整合を保つため残余バイトを読み捨てたうえで
     *  `CPLAT_ERR_BUFFER_TOO_SMALL` を返します (`header_out->payload_size` に必要長が格納されます)。
     */
    int pos_ipc_frame_recv(const pos_ipc_transport_ops *ops, pos_ipc_transport_conn *conn, uint16_t expected_flags,
                           uint32_t max_payload_size, pos_ipc_frame_header *header_out, void *payload_buffer,
                           size_t buffer_capacity, int timeout_ms, cplat_error *detail_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROCESS_ORCHESTRATION_IPC_PROTOCOL_H */
