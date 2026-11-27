#include <process_orchestration/ipc_protocol.h>

#include <cplat/net/byteorder.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static void write_u16_be(uint8_t *dst, uint16_t value);
static void write_u32_be(uint8_t *dst, uint32_t value);
static uint16_t read_u16_be(const uint8_t *src);
static uint32_t read_u32_be(const uint8_t *src);
static int drain_bytes(const pos_ipc_transport_ops *ops, pos_ipc_transport_conn *conn, size_t remaining, int timeout_ms,
                       cplat_error *detail_out);

void pos_ipc_frame_encode_header(const pos_ipc_frame_header *header, uint8_t out_bytes[POS_IPC_FRAME_HEADER_SIZE])
{
    write_u32_be(&out_bytes[0], header->magic);
    write_u16_be(&out_bytes[4], header->version);
    write_u16_be(&out_bytes[6], header->flags);
    write_u32_be(&out_bytes[8], header->method_id);
    write_u32_be(&out_bytes[12], (uint32_t)header->status_code);
    write_u32_be(&out_bytes[16], header->payload_size);
    write_u32_be(&out_bytes[20], header->reserved);
}

int pos_ipc_frame_decode_header(const uint8_t in_bytes[POS_IPC_FRAME_HEADER_SIZE], uint16_t expected_flags,
                                uint32_t max_payload_size, pos_ipc_frame_header *header_out)
{
    pos_ipc_frame_header hdr;

    if (header_out == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    hdr.magic = read_u32_be(&in_bytes[0]);
    hdr.version = read_u16_be(&in_bytes[4]);
    hdr.flags = read_u16_be(&in_bytes[6]);
    hdr.method_id = read_u32_be(&in_bytes[8]);
    hdr.status_code = (int32_t)read_u32_be(&in_bytes[12]);
    hdr.payload_size = read_u32_be(&in_bytes[16]);
    hdr.reserved = read_u32_be(&in_bytes[20]);

    if (hdr.magic != POS_IPC_FRAME_MAGIC)
    {
        return CPLAT_ERR_CORRUPT_DATA;
    }
    if (hdr.version != POS_IPC_FRAME_VERSION)
    {
        return CPLAT_ERR_VERSION_MISMATCH;
    }
    if ((hdr.flags != expected_flags) || (hdr.payload_size > max_payload_size))
    {
        return CPLAT_ERR_CORRUPT_DATA;
    }

    *header_out = hdr;
    return CPLAT_OK;
}

int pos_ipc_frame_send(const pos_ipc_transport_ops *ops, pos_ipc_transport_conn *conn,
                       const pos_ipc_frame_header *header, const void *payload, int timeout_ms, cplat_error *detail_out)
{
    uint8_t raw_header[POS_IPC_FRAME_HEADER_SIZE];
    int rc;

    cplat_error_clear(detail_out);
    if ((ops == NULL) || (conn == NULL) || (header == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if ((header->payload_size > 0U) && (payload == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    pos_ipc_frame_encode_header(header, raw_header);

    rc = ops->conn_send_all(conn, raw_header, sizeof(raw_header), timeout_ms, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    if (header->payload_size > 0U)
    {
        rc = ops->conn_send_all(conn, payload, (size_t)header->payload_size, timeout_ms, detail_out);
        if (rc != CPLAT_OK)
        {
            return rc;
        }
    }

    return CPLAT_OK;
}

int pos_ipc_frame_recv(const pos_ipc_transport_ops *ops, pos_ipc_transport_conn *conn, uint16_t expected_flags,
                       uint32_t max_payload_size, pos_ipc_frame_header *header_out, void *payload_buffer,
                       size_t buffer_capacity, int timeout_ms, cplat_error *detail_out)
{
    uint8_t raw_header[POS_IPC_FRAME_HEADER_SIZE];
    pos_ipc_frame_header hdr;
    int rc;

    cplat_error_clear(detail_out);
    if ((ops == NULL) || (conn == NULL) || (header_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    rc = ops->conn_recv_all(conn, raw_header, sizeof(raw_header), timeout_ms, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = pos_ipc_frame_decode_header(raw_header, expected_flags, max_payload_size, &hdr);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    *header_out = hdr;

    if (hdr.payload_size == 0U)
    {
        return CPLAT_OK;
    }

    if ((payload_buffer == NULL) || ((size_t)hdr.payload_size > buffer_capacity))
    {
        /* 持続接続のストリーム境界を保つため、上限内の残余ペイロードを読み捨ててから通知する。 */
        int drain_rc = drain_bytes(ops, conn, (size_t)hdr.payload_size, timeout_ms, detail_out);
        if (drain_rc != CPLAT_OK)
        {
            return drain_rc;
        }
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }

    return ops->conn_recv_all(conn, payload_buffer, (size_t)hdr.payload_size, timeout_ms, detail_out);
}

static void write_u16_be(uint8_t *dst, uint16_t value)
{
    uint16_t net_val = cplat_hton16(value);
    memcpy(dst, &net_val, sizeof(net_val));
}

static void write_u32_be(uint8_t *dst, uint32_t value)
{
    uint32_t net_val = cplat_hton32(value);
    memcpy(dst, &net_val, sizeof(net_val));
}

static uint16_t read_u16_be(const uint8_t *src)
{
    uint16_t net_val = 0U;
    memcpy(&net_val, src, sizeof(net_val));
    return cplat_ntoh16(net_val);
}

static uint32_t read_u32_be(const uint8_t *src)
{
    uint32_t net_val = 0U;
    memcpy(&net_val, src, sizeof(net_val));
    return cplat_ntoh32(net_val);
}

static int drain_bytes(const pos_ipc_transport_ops *ops, pos_ipc_transport_conn *conn, size_t remaining, int timeout_ms,
                       cplat_error *detail_out)
{
    uint8_t scratch[4096];

    while (remaining > 0U)
    {
        size_t chunk = (remaining < sizeof(scratch)) ? remaining : sizeof(scratch);
        int rc = ops->conn_recv_all(conn, scratch, chunk, timeout_ms, detail_out);
        if (rc != CPLAT_OK)
        {
            return rc;
        }
        remaining -= chunk;
    }

    return CPLAT_OK;
}
