#include <process_orchestration/ipc.h>
#include <process_orchestration/ipc_protocol.h>
#include <process_orchestration/ipc_transport.h>

#include <cplat/clock/clock.h>
#include <cplat/sync/sync.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define POS_IPC_IDLE_SWEEP_SLICE_MS 100
#define POS_IPC_MAX_EVENT_BATCH     32U

typedef struct pos_ipc_handler_entry
{
    uint32_t method_id;
    uint32_t pad;
    pos_ipc_handler_fn handler;
    void *user_context;
} pos_ipc_handler_entry;

typedef struct pos_ipc_conn_slot
{
    pos_ipc_transport_conn *conn;
    uint64_t last_activity_ms;
    int in_use;
    int in_worker;
} pos_ipc_conn_slot;

struct pos_ipc_server
{
    pos_ipc_resolved_endpoint resolved;
    pos_ipc_server_options options;
    const pos_ipc_transport_ops *ops;
    pos_ipc_transport_listener *listener;
    pos_ipc_transport_reactor *reactor;

    cplat_local_lock *lock;
    cplat_condvar *queue_cv;

    pos_ipc_handler_entry handlers[POS_IPC_MAX_HANDLERS];
    size_t handler_count;

    pos_ipc_conn_slot *slots;
    uint32_t active_connections;
    uint32_t queue_head;
    uint32_t queue_tail;
    uint32_t queue_count;
    pos_ipc_conn_slot **work_queue;

    cplat_thread *io_thread;
    cplat_thread **worker_threads;

    int started;
    int stopping;
};

static void io_loop_thread(void *arg);
static void worker_thread(void *arg);
static void handle_accept_event(pos_ipc_server *server, pos_ipc_transport_conn *accepted_conn);
static void enqueue_slot_locked(pos_ipc_server *server, pos_ipc_conn_slot *slot);
static void release_slot_locked(pos_ipc_server *server, pos_ipc_conn_slot *slot);
static void sweep_idle_connections(pos_ipc_server *server);
static int find_handler_locked(const pos_ipc_server *server, uint32_t method_id, pos_ipc_handler_fn *handler_out,
                               void **context_out);
static uint64_t now_monotonic_ms(void);

int POS_API pos_ipc_server_create(const pos_ipc_endpoint *endpoint, const pos_ipc_server_options *options,
                                  pos_ipc_server **server_out, cplat_error *detail_out)
{
    pos_ipc_server *server = NULL;
    pos_ipc_resolved_endpoint resolved;
    const pos_ipc_transport_ops *ops = NULL;
    int rc;

    cplat_error_clear(detail_out);
    if (server_out == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *server_out = NULL;

    if (endpoint == NULL)
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

    server = (pos_ipc_server *)calloc(1U, sizeof(*server));
    if (server == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    server->resolved = resolved;
    server->ops = ops;

    if (options != NULL)
    {
        server->options = *options;
    }
    if (server->options.worker_count == 0U)
    {
        server->options.worker_count = POS_IPC_DEFAULT_WORKER_COUNT;
    }
    if (server->options.max_connections == 0U)
    {
        server->options.max_connections = POS_IPC_DEFAULT_MAX_CONNECTIONS;
    }
    if (server->options.max_payload_size == 0U)
    {
        server->options.max_payload_size = POS_IPC_DEFAULT_MAX_PAYLOAD_SIZE;
    }
    if (server->options.idle_timeout_ms == 0)
    {
        server->options.idle_timeout_ms = POS_IPC_DEFAULT_IDLE_TIMEOUT_MS;
    }
    if (server->options.io_timeout_ms == 0)
    {
        server->options.io_timeout_ms = POS_IPC_DEFAULT_IO_TIMEOUT_MS;
    }

    if ((server->options.idle_timeout_ms < 0) || (server->options.io_timeout_ms < 0))
    {
        free(server);
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    rc = cplat_local_lock_create(&server->lock);
    if (rc != CPLAT_OK)
    {
        free(server);
        return rc;
    }

    rc = cplat_condvar_create(&server->queue_cv);
    if (rc != CPLAT_OK)
    {
        cplat_local_lock_dispose(server->lock);
        free(server);
        return rc;
    }

    server->slots = (pos_ipc_conn_slot *)calloc((size_t)server->options.max_connections, sizeof(*server->slots));
    server->work_queue =
        (pos_ipc_conn_slot **)calloc((size_t)server->options.max_connections, sizeof(*server->work_queue));
    server->worker_threads =
        (cplat_thread **)calloc((size_t)server->options.worker_count, sizeof(*server->worker_threads));

    if ((server->slots == NULL) || (server->work_queue == NULL) || (server->worker_threads == NULL))
    {
        pos_ipc_server_dispose(server);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    *server_out = server;
    return CPLAT_OK;
}

int POS_API pos_ipc_server_register_handler(pos_ipc_server *server, uint32_t method_id, pos_ipc_handler_fn handler,
                                            void *user_context)
{
    size_t i;

    if ((server == NULL) || (handler == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    cplat_local_lock_lock(server->lock);
    if (server->started != 0)
    {
        cplat_local_lock_unlock(server->lock);
        return CPLAT_ERR_BUSY;
    }

    for (i = 0U; i < server->handler_count; ++i)
    {
        if (server->handlers[i].method_id == method_id)
        {
            cplat_local_lock_unlock(server->lock);
            return CPLAT_ERR_DUPLICATE_DEFINITION;
        }
    }

    if (server->handler_count >= POS_IPC_MAX_HANDLERS)
    {
        cplat_local_lock_unlock(server->lock);
        return CPLAT_ERR_LIMIT_EXCEEDED;
    }

    server->handlers[server->handler_count].method_id = method_id;
    server->handlers[server->handler_count].handler = handler;
    server->handlers[server->handler_count].user_context = user_context;
    server->handler_count++;
    cplat_local_lock_unlock(server->lock);
    return CPLAT_OK;
}

int POS_API pos_ipc_server_start(pos_ipc_server *server, cplat_error *detail_out)
{
    uint32_t i;
    int rc;

    cplat_error_clear(detail_out);
    if (server == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    cplat_local_lock_lock(server->lock);
    if (server->started != 0)
    {
        cplat_local_lock_unlock(server->lock);
        return CPLAT_ERR_BUSY;
    }
    server->stopping = 0;
    cplat_local_lock_unlock(server->lock);

    rc = server->ops->listener_open(&server->resolved, &server->options.access_policy, &server->listener, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = server->ops->reactor_create(server->listener, server->options.max_connections, &server->reactor, detail_out);
    if (rc != CPLAT_OK)
    {
        server->ops->listener_close(server->listener);
        server->listener = NULL;
        return rc;
    }

    cplat_local_lock_lock(server->lock);
    server->started = 1;
    cplat_local_lock_unlock(server->lock);

    for (i = 0U; i < server->options.worker_count; ++i)
    {
        rc = cplat_thread_create(&server->worker_threads[i], worker_thread, server);
        if (rc != CPLAT_OK)
        {
            (void)pos_ipc_server_stop(server, CPLAT_WAIT_FOREVER, NULL);
            return rc;
        }
    }

    rc = cplat_thread_create(&server->io_thread, io_loop_thread, server);
    if (rc != CPLAT_OK)
    {
        (void)pos_ipc_server_stop(server, CPLAT_WAIT_FOREVER, NULL);
        return rc;
    }

    return CPLAT_OK;
}

int POS_API pos_ipc_server_stop(pos_ipc_server *server, int timeout_ms, cplat_error *detail_out)
{
    uint32_t i;

    cplat_error_clear(detail_out);
    if (server == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if ((timeout_ms != CPLAT_WAIT_FOREVER) && (timeout_ms != CPLAT_NO_WAIT) && (timeout_ms < 0))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    cplat_local_lock_lock(server->lock);
    if (server->started == 0)
    {
        cplat_local_lock_unlock(server->lock);
        return CPLAT_OK;
    }
    server->stopping = 1;
    cplat_condvar_broadcast(server->queue_cv);
    cplat_local_lock_unlock(server->lock);

    if (server->reactor != NULL)
    {
        server->ops->reactor_wakeup(server->reactor);
    }

    if (server->io_thread != NULL)
    {
        cplat_thread_join(server->io_thread);
        server->io_thread = NULL;
    }

    for (i = 0U; i < server->options.worker_count; ++i)
    {
        if (server->worker_threads[i] != NULL)
        {
            cplat_thread_join(server->worker_threads[i]);
            server->worker_threads[i] = NULL;
        }
    }

    cplat_local_lock_lock(server->lock);
    for (i = 0U; i < server->options.max_connections; ++i)
    {
        if (server->slots[i].in_use != 0)
        {
            if (server->reactor != NULL)
            {
                server->ops->reactor_remove_conn(server->reactor, server->slots[i].conn);
            }
            server->ops->conn_close(server->slots[i].conn);
            server->slots[i].conn = NULL;
            server->slots[i].in_use = 0;
            server->slots[i].in_worker = 0;
        }
    }
    server->active_connections = 0U;
    server->queue_head = 0U;
    server->queue_tail = 0U;
    server->queue_count = 0U;
    server->started = 0;
    cplat_local_lock_unlock(server->lock);

    if (server->reactor != NULL)
    {
        server->ops->reactor_destroy(server->reactor);
        server->reactor = NULL;
    }
    if (server->listener != NULL)
    {
        server->ops->listener_close(server->listener);
        server->listener = NULL;
    }

    return CPLAT_OK;
}

void POS_API pos_ipc_server_dispose(pos_ipc_server *server)
{
    if (server == NULL)
    {
        return;
    }
    if (server->started != 0)
    {
        (void)pos_ipc_server_stop(server, CPLAT_WAIT_FOREVER, NULL);
    }
    free(server->worker_threads);
    free(server->work_queue);
    free(server->slots);
    if (server->queue_cv != NULL)
    {
        cplat_condvar_dispose(server->queue_cv);
    }
    if (server->lock != NULL)
    {
        cplat_local_lock_dispose(server->lock);
    }
    free(server);
}

static void io_loop_thread(void *arg)
{
    pos_ipc_server *server = (pos_ipc_server *)arg;
    pos_ipc_reactor_event events[POS_IPC_MAX_EVENT_BATCH];

    for (;;)
    {
        size_t event_count = 0U;
        size_t i;
        int rc;

        cplat_local_lock_lock(server->lock);
        if (server->stopping != 0)
        {
            cplat_local_lock_unlock(server->lock);
            break;
        }
        cplat_local_lock_unlock(server->lock);

        rc = server->ops->reactor_wait(server->reactor, POS_IPC_IDLE_SWEEP_SLICE_MS, events, POS_IPC_MAX_EVENT_BATCH,
                                       &event_count, NULL);
        if ((rc == CPLAT_OK) && (event_count > 0U))
        {
            for (i = 0U; i < event_count; ++i)
            {
                if (events[i].kind == POS_IPC_REACTOR_EVENT_ACCEPT)
                {
                    handle_accept_event(server, events[i].accepted_conn);
                }
                else if (events[i].kind == POS_IPC_REACTOR_EVENT_READABLE)
                {
                    pos_ipc_conn_slot *slot = (pos_ipc_conn_slot *)events[i].user_data;
                    cplat_local_lock_lock(server->lock);
                    if ((slot != NULL) && (slot->in_use != 0) && (slot->in_worker == 0))
                    {
                        slot->in_worker = 1;
                        enqueue_slot_locked(server, slot);
                    }
                    cplat_local_lock_unlock(server->lock);
                }
                else if (events[i].kind == POS_IPC_REACTOR_EVENT_CLOSED)
                {
                    pos_ipc_conn_slot *slot = (pos_ipc_conn_slot *)events[i].user_data;
                    cplat_local_lock_lock(server->lock);
                    if ((slot != NULL) && (slot->in_use != 0) && (slot->in_worker == 0))
                    {
                        release_slot_locked(server, slot);
                    }
                    cplat_local_lock_unlock(server->lock);
                }
            }
        }

        sweep_idle_connections(server);
    }
}

static void worker_thread(void *arg)
{
    pos_ipc_server *server = (pos_ipc_server *)arg;
    size_t buf_size = (size_t)server->options.max_payload_size;
    void *req_buf = malloc(buf_size);
    void *resp_buf = malloc(buf_size);

    if ((req_buf == NULL) || (resp_buf == NULL))
    {
        free(req_buf);
        free(resp_buf);
        return;
    }

    for (;;)
    {
        pos_ipc_conn_slot *slot = NULL;
        pos_ipc_frame_header req_hdr;
        pos_ipc_frame_header resp_hdr;
        pos_ipc_handler_fn handler = NULL;
        void *user_ctx = NULL;
        size_t resp_size = 0U;
        int rc;
        int handler_rc;
        int keep_alive = 0;

        cplat_local_lock_lock(server->lock);
        while ((server->queue_count == 0U) && (server->stopping == 0))
        {
            cplat_condvar_wait(server->queue_cv, server->lock);
        }
        if ((server->queue_count == 0U) && (server->stopping != 0))
        {
            cplat_local_lock_unlock(server->lock);
            break;
        }
        slot = server->work_queue[server->queue_head];
        server->queue_head = (server->queue_head + 1U) % server->options.max_connections;
        server->queue_count--;
        cplat_local_lock_unlock(server->lock);

        rc = pos_ipc_frame_recv(server->ops, slot->conn, POS_IPC_FRAME_FLAG_REQUEST, server->options.max_payload_size,
                                &req_hdr, req_buf, buf_size, server->options.io_timeout_ms, NULL);
        if (rc == CPLAT_OK)
        {
            cplat_local_lock_lock(server->lock);
            rc = find_handler_locked(server, req_hdr.method_id, &handler, &user_ctx);
            cplat_local_lock_unlock(server->lock);

            if (rc != CPLAT_OK)
            {
                handler_rc = CPLAT_ERR_NOT_FOUND;
                resp_size = 0U;
            }
            else
            {
                const void *req_ptr = (req_hdr.payload_size > 0U) ? req_buf : NULL;
                handler_rc = handler(req_hdr.method_id, req_ptr, (size_t)req_hdr.payload_size, resp_buf, buf_size,
                                     &resp_size, user_ctx);
                if (resp_size > buf_size)
                {
                    handler_rc = CPLAT_ERR_LIMIT_EXCEEDED;
                    resp_size = 0U;
                }
            }

            resp_hdr.magic = POS_IPC_FRAME_MAGIC;
            resp_hdr.version = POS_IPC_FRAME_VERSION;
            resp_hdr.flags = POS_IPC_FRAME_FLAG_RESPONSE;
            resp_hdr.method_id = req_hdr.method_id;
            resp_hdr.status_code = (int32_t)handler_rc;
            resp_hdr.payload_size = (uint32_t)resp_size;
            resp_hdr.reserved = 0U;

            rc = pos_ipc_frame_send(server->ops, slot->conn, &resp_hdr, (resp_size > 0U) ? resp_buf : NULL,
                                    server->options.io_timeout_ms, NULL);
            if (rc == CPLAT_OK)
            {
                keep_alive = 1;
            }
        }

        cplat_local_lock_lock(server->lock);
        if ((keep_alive != 0) && (server->stopping == 0))
        {
            slot->last_activity_ms = now_monotonic_ms();
            slot->in_worker = 0;
            if (server->ops->reactor_add_conn(server->reactor, slot->conn, slot, NULL) != CPLAT_OK)
            {
                release_slot_locked(server, slot);
            }
            else
            {
                server->ops->reactor_wakeup(server->reactor);
            }
        }
        else
        {
            release_slot_locked(server, slot);
        }
        cplat_local_lock_unlock(server->lock);
    }

    free(req_buf);
    free(resp_buf);
}

static void handle_accept_event(pos_ipc_server *server, pos_ipc_transport_conn *accepted_conn)
{
    uint32_t i;
    pos_ipc_conn_slot *free_slot = NULL;

    if (accepted_conn == NULL)
    {
        return;
    }

    cplat_local_lock_lock(server->lock);
    if ((server->stopping != 0) || (server->active_connections >= server->options.max_connections))
    {
        cplat_local_lock_unlock(server->lock);
        server->ops->conn_close(accepted_conn);
        return;
    }

    for (i = 0U; i < server->options.max_connections; ++i)
    {
        if (server->slots[i].in_use == 0)
        {
            free_slot = &server->slots[i];
            break;
        }
    }

    if (free_slot == NULL)
    {
        cplat_local_lock_unlock(server->lock);
        server->ops->conn_close(accepted_conn);
        return;
    }

    free_slot->conn = accepted_conn;
    free_slot->last_activity_ms = now_monotonic_ms();
    free_slot->in_use = 1;
    free_slot->in_worker = 0;
    server->active_connections++;

    if (server->ops->reactor_add_conn(server->reactor, accepted_conn, free_slot, NULL) != CPLAT_OK)
    {
        release_slot_locked(server, free_slot);
    }
    cplat_local_lock_unlock(server->lock);
}

static void enqueue_slot_locked(pos_ipc_server *server, pos_ipc_conn_slot *slot)
{
    server->work_queue[server->queue_tail] = slot;
    server->queue_tail = (server->queue_tail + 1U) % server->options.max_connections;
    server->queue_count++;
    cplat_condvar_signal(server->queue_cv);
}

static void release_slot_locked(pos_ipc_server *server, pos_ipc_conn_slot *slot)
{
    if (slot->conn != NULL)
    {
        server->ops->conn_close(slot->conn);
        slot->conn = NULL;
    }
    slot->in_use = 0;
    slot->in_worker = 0;
    if (server->active_connections > 0U)
    {
        server->active_connections--;
    }
}

static void sweep_idle_connections(pos_ipc_server *server)
{
    uint64_t now_ms = now_monotonic_ms();
    uint64_t limit_ms = (uint64_t)server->options.idle_timeout_ms;
    uint32_t i;

    cplat_local_lock_lock(server->lock);
    for (i = 0U; i < server->options.max_connections; ++i)
    {
        pos_ipc_conn_slot *slot = &server->slots[i];
        if ((slot->in_use != 0) && (slot->in_worker == 0) && (now_ms >= slot->last_activity_ms) &&
            ((now_ms - slot->last_activity_ms) >= limit_ms))
        {
            server->ops->reactor_remove_conn(server->reactor, slot->conn);
            release_slot_locked(server, slot);
        }
    }
    cplat_local_lock_unlock(server->lock);
}

static int find_handler_locked(const pos_ipc_server *server, uint32_t method_id, pos_ipc_handler_fn *handler_out,
                               void **context_out)
{
    size_t i;

    for (i = 0U; i < server->handler_count; ++i)
    {
        if (server->handlers[i].method_id == method_id)
        {
            *handler_out = server->handlers[i].handler;
            *context_out = server->handlers[i].user_context;
            return CPLAT_OK;
        }
    }

    return CPLAT_ERR_NOT_FOUND;
}

static uint64_t now_monotonic_ms(void)
{
    uint64_t ms = 0U;
    (void)cplat_clock_get_monotonic_ms(&ms);
    return ms;
}
