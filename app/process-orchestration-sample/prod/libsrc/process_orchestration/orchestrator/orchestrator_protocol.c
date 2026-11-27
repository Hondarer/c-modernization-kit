#include <process_orchestration/orchestrator_protocol.h>

#include <cplat/net/byteorder.h>
#include <string.h>

void pos_orch_encode_u32(uint32_t value, uint8_t out[4])
{
    uint32_t be = cplat_hton32(value);
    memcpy(out, &be, sizeof(be));
}

uint32_t pos_orch_decode_u32(const uint8_t in[4])
{
    uint32_t be = 0U;
    memcpy(&be, in, sizeof(be));
    return cplat_ntoh32(be);
}

void pos_orch_encode_status(const pos_child_status *status, uint8_t out[POS_ORCH_STATUS_PAYLOAD_SIZE])
{
    pos_orch_encode_u32(status->child_id, out + 0U);
    pos_orch_encode_u32((uint32_t)status->state, out + 4U);
    pos_orch_encode_u32(status->pid, out + 8U);
    pos_orch_encode_u32(status->start_count, out + 12U);
    pos_orch_encode_u32((uint32_t)status->last_exit_code, out + 16U);
    pos_orch_encode_u32(status->has_last_exit_code, out + 20U);
}

int pos_orch_decode_status(const uint8_t *in, size_t len, pos_child_status *status_out)
{
    uint32_t raw_state;

    if ((in == NULL) || (status_out == NULL) || (len != POS_ORCH_STATUS_PAYLOAD_SIZE))
    {
        return CPLAT_ERR_CORRUPT_DATA;
    }

    raw_state = pos_orch_decode_u32(in + 4U);
    if (raw_state > (uint32_t)POS_CHILD_STATE_CRASHED)
    {
        return CPLAT_ERR_CORRUPT_DATA;
    }

    status_out->child_id = pos_orch_decode_u32(in + 0U);
    status_out->state = (pos_child_state)raw_state;
    status_out->pid = pos_orch_decode_u32(in + 8U);
    status_out->start_count = pos_orch_decode_u32(in + 12U);
    status_out->last_exit_code = (int32_t)pos_orch_decode_u32(in + 16U);
    status_out->has_last_exit_code = pos_orch_decode_u32(in + 20U);
    return CPLAT_OK;
}

void pos_orch_encode_identity(uint32_t child_id, uint32_t child_pid, uint8_t out[POS_ORCH_IDENTITY_PAYLOAD_SIZE])
{
    pos_orch_encode_u32(child_id, out + 0U);
    pos_orch_encode_u32(child_pid, out + 4U);
}

int pos_orch_decode_identity(const uint8_t *in, size_t len, uint32_t *child_id_out, uint32_t *child_pid_out)
{
    if ((in == NULL) || (len < POS_ORCH_IDENTITY_PAYLOAD_SIZE))
    {
        return CPLAT_ERR_CORRUPT_DATA;
    }

    if (child_id_out != NULL)
    {
        *child_id_out = pos_orch_decode_u32(in + 0U);
    }
    if (child_pid_out != NULL)
    {
        *child_pid_out = pos_orch_decode_u32(in + 4U);
    }
    return CPLAT_OK;
}
