#include <process_orchestration/orchestrator.h>
#include <process_orchestration/orchestrator_protocol.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define POS_ORCH_STACK_ECHO_BUF_SIZE 4096U

static int call_parent_child_op(const pos_orch_config *config, uint32_t method_id, uint32_t child_id,
                                pos_child_status *status_out, int timeout_ms, cplat_error *detail_out);
static int parse_echo_response(const uint8_t *raw_resp, size_t raw_len, char *echo_buffer, size_t echo_buffer_size,
                               pos_child_echo_result *result_out);

int POS_API pos_orch_make_parent_endpoint(const pos_orch_config *config, pos_ipc_endpoint *endpoint_out)
{
    if ((config == NULL) || (endpoint_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    memset(endpoint_out, 0, sizeof(*endpoint_out));
    endpoint_out->scope = config->scope;
    endpoint_out->backend = config->backend;
    endpoint_out->namespace_name = config->namespace_name;
    endpoint_out->instance_name = config->instance_name;
    endpoint_out->target_name = "parent";
    endpoint_out->custom_dir = config->custom_dir;
    return CPLAT_OK;
}

int POS_API pos_orch_make_child_endpoint(const pos_orch_config *config, uint32_t child_id,
                                         pos_ipc_endpoint *endpoint_out)
{
    static const char *const k_child_names[POS_ORCH_CHILD_COUNT] = {"child-1", "child-2", "child-3"};

    if ((config == NULL) || (endpoint_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if ((child_id < POS_ORCH_CHILD_ID_MIN) || (child_id > POS_ORCH_CHILD_ID_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    memset(endpoint_out, 0, sizeof(*endpoint_out));
    endpoint_out->scope = config->scope;
    endpoint_out->backend = config->backend;
    endpoint_out->namespace_name = config->namespace_name;
    endpoint_out->instance_name = config->instance_name;
    endpoint_out->target_name = k_child_names[child_id - POS_ORCH_CHILD_ID_MIN];
    endpoint_out->custom_dir = config->custom_dir;
    return CPLAT_OK;
}

int POS_API pos_orch_parent_stop(const pos_orch_config *config, int timeout_ms, cplat_error *detail_out)
{
    pos_ipc_endpoint ep;
    size_t resp_size = 0U;
    int rc;

    cplat_error_clear(detail_out);
    rc = pos_orch_make_parent_endpoint(config, &ep);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    return pos_ipc_call(&ep, POS_ORCH_METHOD_PARENT_STOP, NULL, 0U, NULL, 0U, &resp_size, timeout_ms, detail_out);
}

int POS_API pos_orch_child_start(const pos_orch_config *config, uint32_t child_id, pos_child_status *status_out,
                                 int timeout_ms, cplat_error *detail_out)
{
    return call_parent_child_op(config, POS_ORCH_METHOD_CHILD_START, child_id, status_out, timeout_ms, detail_out);
}

int POS_API pos_orch_child_stop(const pos_orch_config *config, uint32_t child_id, pos_child_status *status_out,
                                int timeout_ms, cplat_error *detail_out)
{
    return call_parent_child_op(config, POS_ORCH_METHOD_CHILD_STOP, child_id, status_out, timeout_ms, detail_out);
}

int POS_API pos_orch_child_status(const pos_orch_config *config, uint32_t child_id, pos_child_status *status_out,
                                  int timeout_ms, cplat_error *detail_out)
{
    if (status_out == NULL)
    {
        cplat_error_clear(detail_out);
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    return call_parent_child_op(config, POS_ORCH_METHOD_CHILD_STATUS, child_id, status_out, timeout_ms, detail_out);
}

int POS_API pos_orch_child_echo(const pos_orch_config *config, uint32_t child_id, const char *message,
                                char *echo_buffer, size_t echo_buffer_size, pos_child_echo_result *result_out,
                                int timeout_ms, cplat_error *detail_out)
{
    pos_ipc_endpoint ep;
    pos_ipc_client *client = NULL;
    int rc;

    cplat_error_clear(detail_out);
    rc = pos_orch_make_child_endpoint(config, child_id, &ep);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = pos_ipc_client_open(&ep, timeout_ms, &client, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = pos_orch_child_echo_with_client(client, message, echo_buffer, echo_buffer_size, result_out, timeout_ms,
                                         detail_out);
    pos_ipc_client_close(client);
    return rc;
}

int POS_API pos_orch_child_echo_with_client(pos_ipc_client *client, const char *message, char *echo_buffer,
                                            size_t echo_buffer_size, pos_child_echo_result *result_out, int timeout_ms,
                                            cplat_error *detail_out)
{
    size_t msg_len;
    size_t need_raw_cap;
    uint8_t stack_buf[POS_ORCH_STACK_ECHO_BUF_SIZE + POS_ORCH_IDENTITY_PAYLOAD_SIZE];
    uint8_t *heap_buf = NULL;
    uint8_t *recv_buf = stack_buf;
    size_t recv_len = 0U;
    int rc;

    cplat_error_clear(detail_out);
    if (result_out != NULL)
    {
        memset(result_out, 0, sizeof(*result_out));
    }
    if ((client == NULL) || (message == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if ((echo_buffer_size > 0U) && (echo_buffer == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    msg_len = strlen(message);
    if (msg_len > (POS_IPC_DEFAULT_MAX_PAYLOAD_SIZE - POS_ORCH_IDENTITY_PAYLOAD_SIZE))
    {
        return CPLAT_ERR_LIMIT_EXCEEDED;
    }
    need_raw_cap = msg_len + POS_ORCH_IDENTITY_PAYLOAD_SIZE;

    if (need_raw_cap > sizeof(stack_buf))
    {
        heap_buf = (uint8_t *)malloc(need_raw_cap);
        if (heap_buf == NULL)
        {
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
        recv_buf = heap_buf;
    }

    rc = pos_ipc_client_call(client, POS_ORCH_METHOD_CHILD_ECHO, (msg_len > 0U) ? message : NULL, msg_len, recv_buf,
                             need_raw_cap, &recv_len, timeout_ms, detail_out);
    if (rc == CPLAT_OK)
    {
        rc = parse_echo_response(recv_buf, recv_len, echo_buffer, echo_buffer_size, result_out);
    }

    free(heap_buf);
    return rc;
}

const char *POS_API pos_orch_child_state_name(pos_child_state state)
{
    switch (state)
    {
    case POS_CHILD_STATE_STOPPED:
        return "STOPPED";
    case POS_CHILD_STATE_STARTING:
        return "STARTING";
    case POS_CHILD_STATE_RUNNING:
        return "RUNNING";
    case POS_CHILD_STATE_STOPPING:
        return "STOPPING";
    case POS_CHILD_STATE_CRASHED:
        return "CRASHED";
    default:
        return "UNKNOWN";
    }
}

static int call_parent_child_op(const pos_orch_config *config, uint32_t method_id, uint32_t child_id,
                                pos_child_status *status_out, int timeout_ms, cplat_error *detail_out)
{
    pos_ipc_endpoint ep;
    uint8_t req_buf[POS_ORCH_CHILD_ID_PAYLOAD_SIZE];
    uint8_t resp_buf[POS_ORCH_STATUS_PAYLOAD_SIZE];
    size_t resp_len = 0U;
    int rc;

    cplat_error_clear(detail_out);
    if ((child_id < POS_ORCH_CHILD_ID_MIN) || (child_id > POS_ORCH_CHILD_ID_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    rc = pos_orch_make_parent_endpoint(config, &ep);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    pos_orch_encode_u32(child_id, req_buf);
    rc = pos_ipc_call(&ep, method_id, req_buf, sizeof(req_buf), resp_buf, sizeof(resp_buf), &resp_len, timeout_ms,
                      detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    if (status_out != NULL)
    {
        rc = pos_orch_decode_status(resp_buf, resp_len, status_out);
        if (rc != CPLAT_OK)
        {
            return rc;
        }
    }

    return CPLAT_OK;
}

static int parse_echo_response(const uint8_t *raw_resp, size_t raw_len, char *echo_buffer, size_t echo_buffer_size,
                               pos_child_echo_result *result_out)
{
    uint32_t child_id = 0U;
    uint32_t child_pid = 0U;
    size_t msg_len;
    int rc;

    rc = pos_orch_decode_identity(raw_resp, raw_len, &child_id, &child_pid);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    msg_len = raw_len - POS_ORCH_IDENTITY_PAYLOAD_SIZE;
    if (result_out != NULL)
    {
        result_out->child_id = child_id;
        result_out->child_pid = child_pid;
        result_out->message_len = msg_len;
    }

    if (echo_buffer != NULL)
    {
        if (echo_buffer_size <= msg_len)
        {
            return CPLAT_ERR_BUFFER_TOO_SMALL;
        }
        if (msg_len > 0U)
        {
            memcpy(echo_buffer, raw_resp + POS_ORCH_IDENTITY_PAYLOAD_SIZE, msg_len);
        }
        echo_buffer[msg_len] = '\0';
    }

    return CPLAT_OK;
}
