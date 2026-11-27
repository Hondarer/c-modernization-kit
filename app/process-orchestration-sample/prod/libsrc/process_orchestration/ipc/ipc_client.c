#include <process_orchestration/ipc.h>
#include <process_orchestration/ipc_protocol.h>
#include <process_orchestration/ipc_transport.h>

#include <cplat/sync/sync.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct pos_ipc_client
{
    const pos_ipc_transport_ops *ops;
    pos_ipc_transport_conn *conn;
    cplat_local_lock *lock;
};

static int validate_timeout(int timeout_ms);
static int perform_call_locked(const pos_ipc_transport_ops *ops, pos_ipc_transport_conn *conn, uint32_t method_id,
                               const void *request_data, size_t request_size, void *response_buffer,
                               size_t response_capacity, size_t *response_size_out, int timeout_ms,
                               cplat_error *detail_out);

int POS_API pos_ipc_call(const pos_ipc_endpoint *endpoint, uint32_t method_id, const void *request_data,
                         size_t request_size, void *response_buffer, size_t response_capacity,
                         size_t *response_size_out, int timeout_ms, cplat_error *detail_out)
{
    pos_ipc_resolved_endpoint resolved;
    const pos_ipc_transport_ops *ops = NULL;
    pos_ipc_transport_conn *conn = NULL;
    int rc;

    cplat_error_clear(detail_out);
    if (response_size_out != NULL)
    {
        *response_size_out = 0U;
    }
    if ((endpoint == NULL) || (validate_timeout(timeout_ms) != CPLAT_OK))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if ((request_size > 0U) && (request_data == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if ((response_capacity > 0U) && (response_buffer == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if (request_size > POS_IPC_DEFAULT_MAX_PAYLOAD_SIZE)
    {
        return CPLAT_ERR_LIMIT_EXCEEDED;
    }

    rc = pos_ipc_endpoint_resolve(endpoint, &resolved, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = pos_ipc_transport_get_ops(resolved.backend, &ops, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = ops->client_connect(&resolved, timeout_ms, &conn, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = perform_call_locked(ops, conn, method_id, request_data, request_size, response_buffer, response_capacity,
                             response_size_out, timeout_ms, detail_out);
    ops->conn_close(conn);
    return rc;
}

int POS_API pos_ipc_client_open(const pos_ipc_endpoint *endpoint, int timeout_ms, pos_ipc_client **client_out,
                                cplat_error *detail_out)
{
    pos_ipc_resolved_endpoint resolved;
    const pos_ipc_transport_ops *ops = NULL;
    pos_ipc_transport_conn *conn = NULL;
    pos_ipc_client *client = NULL;
    int rc;

    cplat_error_clear(detail_out);
    if (client_out == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *client_out = NULL;

    if ((endpoint == NULL) || (validate_timeout(timeout_ms) != CPLAT_OK))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    rc = pos_ipc_endpoint_resolve(endpoint, &resolved, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = pos_ipc_transport_get_ops(resolved.backend, &ops, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    client = (pos_ipc_client *)calloc(1U, sizeof(*client));
    if (client == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    rc = cplat_local_lock_create(&client->lock);
    if (rc != CPLAT_OK)
    {
        free(client);
        return rc;
    }

    rc = ops->client_connect(&resolved, timeout_ms, &conn, detail_out);
    if (rc != CPLAT_OK)
    {
        cplat_local_lock_dispose(client->lock);
        free(client);
        return rc;
    }

    client->ops = ops;
    client->conn = conn;
    *client_out = client;
    return CPLAT_OK;
}

int POS_API pos_ipc_client_call(pos_ipc_client *client, uint32_t method_id, const void *request_data,
                                size_t request_size, void *response_buffer, size_t response_capacity,
                                size_t *response_size_out, int timeout_ms, cplat_error *detail_out)
{
    int rc;

    cplat_error_clear(detail_out);
    if (response_size_out != NULL)
    {
        *response_size_out = 0U;
    }
    if ((client == NULL) || (validate_timeout(timeout_ms) != CPLAT_OK))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if ((request_size > 0U) && (request_data == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if ((response_capacity > 0U) && (response_buffer == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if (request_size > POS_IPC_DEFAULT_MAX_PAYLOAD_SIZE)
    {
        return CPLAT_ERR_LIMIT_EXCEEDED;
    }

    cplat_local_lock_lock(client->lock);
    rc = perform_call_locked(client->ops, client->conn, method_id, request_data, request_size, response_buffer,
                             response_capacity, response_size_out, timeout_ms, detail_out);
    cplat_local_lock_unlock(client->lock);
    return rc;
}

void POS_API pos_ipc_client_close(pos_ipc_client *client)
{
    if (client == NULL)
    {
        return;
    }
    if ((client->ops != NULL) && (client->conn != NULL))
    {
        client->ops->conn_close(client->conn);
    }
    if (client->lock != NULL)
    {
        cplat_local_lock_dispose(client->lock);
    }
    free(client);
}

static int validate_timeout(int timeout_ms)
{
    if ((timeout_ms == CPLAT_WAIT_FOREVER) || (timeout_ms == CPLAT_NO_WAIT) || (timeout_ms >= 0))
    {
        return CPLAT_OK;
    }
    return CPLAT_ERR_INVALID_ARGUMENT;
}

static int perform_call_locked(const pos_ipc_transport_ops *ops, pos_ipc_transport_conn *conn, uint32_t method_id,
                               const void *request_data, size_t request_size, void *response_buffer,
                               size_t response_capacity, size_t *response_size_out, int timeout_ms,
                               cplat_error *detail_out)
{
    pos_ipc_frame_header req_hdr;
    pos_ipc_frame_header resp_hdr;
    int rc;

    req_hdr.magic = POS_IPC_FRAME_MAGIC;
    req_hdr.version = POS_IPC_FRAME_VERSION;
    req_hdr.flags = POS_IPC_FRAME_FLAG_REQUEST;
    req_hdr.method_id = method_id;
    req_hdr.status_code = CPLAT_OK;
    req_hdr.payload_size = (uint32_t)request_size;
    req_hdr.reserved = 0U;

    rc = pos_ipc_frame_send(ops, conn, &req_hdr, request_data, timeout_ms, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = pos_ipc_frame_recv(ops, conn, POS_IPC_FRAME_FLAG_RESPONSE, POS_IPC_DEFAULT_MAX_PAYLOAD_SIZE, &resp_hdr,
                            response_buffer, response_capacity, timeout_ms, detail_out);
    if (rc == CPLAT_ERR_BUFFER_TOO_SMALL)
    {
        if (response_size_out != NULL)
        {
            *response_size_out = (size_t)resp_hdr.payload_size;
        }
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    if (response_size_out != NULL)
    {
        *response_size_out = (size_t)resp_hdr.payload_size;
    }

    return (int)resp_hdr.status_code;
}
