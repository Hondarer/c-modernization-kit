/**
 *******************************************************************************
 *  @file           orchestrator_protocol.h
 *  @brief          親・子プロセス間の IPC メソッド ID とバイナリ ペイロード変換を定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/27
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef PROCESS_ORCHESTRATION_ORCHESTRATOR_PROTOCOL_H
#define PROCESS_ORCHESTRATION_ORCHESTRATOR_PROTOCOL_H

#include <process_orchestration/orchestrator.h>
#include <stddef.h>
#include <stdint.h>

/** @brief 親プロセス向け IPC メソッド ID。 */
#define POS_ORCH_METHOD_PARENT_STOP  0x00010001U
#define POS_ORCH_METHOD_CHILD_START  0x00010002U
#define POS_ORCH_METHOD_CHILD_STOP   0x00010003U
#define POS_ORCH_METHOD_CHILD_STATUS 0x00010004U

/** @brief 子プロセス向け IPC メソッド ID。 */
#define POS_ORCH_METHOD_CHILD_ECHO      0x00020001U
#define POS_ORCH_METHOD_CHILD_SELF_STOP 0x00020002U
#define POS_ORCH_METHOD_CHILD_PING      0x00020003U

/** @brief 各バイナリ ペイロードの固定サイズ [byte]。 */
#define POS_ORCH_CHILD_ID_PAYLOAD_SIZE 4U
#define POS_ORCH_STATUS_PAYLOAD_SIZE   24U
#define POS_ORCH_IDENTITY_PAYLOAD_SIZE 8U

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    void pos_orch_encode_u32(uint32_t value, uint8_t out[4]);
    uint32_t pos_orch_decode_u32(const uint8_t in[4]);

    void pos_orch_encode_status(const pos_child_status *status, uint8_t out[POS_ORCH_STATUS_PAYLOAD_SIZE]);
    int pos_orch_decode_status(const uint8_t *in, size_t len, pos_child_status *status_out);

    void pos_orch_encode_identity(uint32_t child_id, uint32_t child_pid, uint8_t out[POS_ORCH_IDENTITY_PAYLOAD_SIZE]);
    int pos_orch_decode_identity(const uint8_t *in, size_t len, uint32_t *child_id_out, uint32_t *child_pid_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROCESS_ORCHESTRATION_ORCHESTRATOR_PROTOCOL_H */
