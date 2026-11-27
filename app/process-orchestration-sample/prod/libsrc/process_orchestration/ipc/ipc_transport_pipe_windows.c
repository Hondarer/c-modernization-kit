#include <process_orchestration/ipc_transport.h>

#include <cplat/base/platform.h>

#if defined(PLATFORM_WINDOWS)

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>

    #include <cplat/clock/clock.h>
    #include <cplat/sync/sync.h>
    #include <cplat/win32/win32.h>
    #include <stddef.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>

    #define POS_IPC_PIPE_BUF_SIZE   65536U
    #define POS_IPC_IOCP_KEY_ACCEPT ((ULONG_PTR)1U)
    #define POS_IPC_IOCP_KEY_WAKEUP ((ULONG_PTR)2U)

struct pos_ipc_transport_conn
{
    OVERLAPPED read_ov;
    HANDLE handle;
    void *user_data;
    int iocp_bound;
    int read_armed;
    int has_peek_byte;
    uint8_t peek_byte;
    uint8_t pad[3];
};

struct pos_ipc_transport_listener
{
    char address[POS_IPC_RESOLVED_ADDR_MAX];
    HANDLE first_pipe;
};

struct pos_ipc_transport_reactor
{
    OVERLAPPED accept_ov;
    pos_ipc_transport_listener *listener;
    HANDLE iocp;
    HANDLE accept_pipe;
    cplat_local_lock *lock;
    int accept_pending;
    int pad;
};

static int pipe_win_client_connect(const pos_ipc_resolved_endpoint *resolved, int timeout_ms,
                                   pos_ipc_transport_conn **conn_out, cplat_error *detail_out);
static int pipe_win_conn_send_all(pos_ipc_transport_conn *conn, const void *buf, size_t len, int timeout_ms,
                                  cplat_error *detail_out);
static int pipe_win_conn_recv_all(pos_ipc_transport_conn *conn, void *buf, size_t len, int timeout_ms,
                                  cplat_error *detail_out);
static void pipe_win_conn_close(pos_ipc_transport_conn *conn);
static int pipe_win_listener_open(const pos_ipc_resolved_endpoint *resolved, const pos_ipc_access_policy *policy,
                                  pos_ipc_transport_listener **listener_out, cplat_error *detail_out);
static void pipe_win_listener_close(pos_ipc_transport_listener *listener);
static int pipe_win_reactor_create(pos_ipc_transport_listener *listener, uint32_t max_connections,
                                   pos_ipc_transport_reactor **reactor_out, cplat_error *detail_out);
static int pipe_win_reactor_add_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn, void *user_data,
                                     cplat_error *detail_out);
static void pipe_win_reactor_remove_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn);
static int pipe_win_reactor_wait(pos_ipc_transport_reactor *reactor, int timeout_ms, pos_ipc_reactor_event *events_out,
                                 size_t max_events, size_t *event_count_out, cplat_error *detail_out);
static void pipe_win_reactor_wakeup(pos_ipc_transport_reactor *reactor);
static void pipe_win_reactor_destroy(pos_ipc_transport_reactor *reactor);

static HANDLE create_pipe_instance(const char *address, int first_instance, cplat_error *detail_out);
static int start_async_accept(pos_ipc_transport_reactor *reactor, HANDLE existing_pipe, cplat_error *detail_out);
static int wait_overlapped_io(HANDLE handle, OVERLAPPED *ov, DWORD *transferred_out, int timeout_ms,
                              uint64_t deadline_ms, cplat_error *detail_out);
static int map_win32_result(DWORD err, cplat_error *detail_out);
static uint64_t now_monotonic_ms(void);

static const pos_ipc_transport_ops g_pipe_win_ops = {
    pipe_win_client_connect,      pipe_win_conn_send_all,  pipe_win_conn_recv_all,  pipe_win_conn_close,
    pipe_win_listener_open,       pipe_win_listener_close, pipe_win_reactor_create, pipe_win_reactor_add_conn,
    pipe_win_reactor_remove_conn, pipe_win_reactor_wait,   pipe_win_reactor_wakeup, pipe_win_reactor_destroy};

const pos_ipc_transport_ops *pos_ipc_transport_pipe_windows_ops(void)
{
    return &g_pipe_win_ops;
}

static int pipe_win_client_connect(const pos_ipc_resolved_endpoint *resolved, int timeout_ms,
                                   pos_ipc_transport_conn **conn_out, cplat_error *detail_out)
{
    uint64_t deadline_ms = 0U;
    HANDLE handle = INVALID_HANDLE_VALUE;
    pos_ipc_transport_conn *conn = NULL;

    *conn_out = NULL;
    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    for (;;)
    {
        handle = CreateFileU(resolved->address, GENERIC_READ | GENERIC_WRITE, 0U, NULL, OPEN_EXISTING,
                             FILE_FLAG_OVERLAPPED, NULL);
        if (handle != INVALID_HANDLE_VALUE)
        {
            break;
        }
        {
            DWORD err = GetLastError();
            if (err == ERROR_PIPE_BUSY)
            {
                if (timeout_ms == CPLAT_NO_WAIT)
                {
                    return CPLAT_ERR_TIMEOUT;
                }
                if (timeout_ms >= 0)
                {
                    uint64_t now = now_monotonic_ms();
                    if (now >= deadline_ms)
                    {
                        return CPLAT_ERR_TIMEOUT;
                    }
                }
                Sleep(5U);
                continue;
            }
            return map_win32_result(err, detail_out);
        }
    }

    conn = (pos_ipc_transport_conn *)calloc(1U, sizeof(*conn));
    if (conn == NULL)
    {
        CloseHandle(handle);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    conn->handle = handle;
    *conn_out = conn;
    return CPLAT_OK;
}

static int pipe_win_conn_send_all(pos_ipc_transport_conn *conn, const void *buf, size_t len, int timeout_ms,
                                  cplat_error *detail_out)
{
    const uint8_t *ptr = (const uint8_t *)buf;
    size_t sent = 0U;
    uint64_t deadline_ms = 0U;
    HANDLE evt = NULL;

    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    evt = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (evt == NULL)
    {
        return map_win32_result(GetLastError(), detail_out);
    }

    while (sent < len)
    {
        OVERLAPPED ov;
        DWORD written = 0U;
        size_t chunk = len - sent;
        DWORD write_len = (chunk > (size_t)INT32_MAX) ? (DWORD)INT32_MAX : (DWORD)chunk;

        memset(&ov, 0, sizeof(ov));
        /* IOCP 関連付け済みハンドルでも同期待機へ完了を閉じ込めるため下位ビット 1 を立てる。 */
        /* see: https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-getqueuedcompletionstatus */
        ov.hEvent = (HANDLE)((uintptr_t)evt | (uintptr_t)1U);
        (void)ResetEvent(evt);

        if (!WriteFile(conn->handle, ptr + sent, write_len, &written, &ov))
        {
            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING)
            {
                int rc = wait_overlapped_io(conn->handle, &ov, &written, timeout_ms, deadline_ms, detail_out);
                if (rc != CPLAT_OK)
                {
                    CloseHandle(evt);
                    return rc;
                }
            }
            else
            {
                CloseHandle(evt);
                return map_win32_result(err, detail_out);
            }
        }
        sent += (size_t)written;
    }

    CloseHandle(evt);
    return CPLAT_OK;
}

static int pipe_win_conn_recv_all(pos_ipc_transport_conn *conn, void *buf, size_t len, int timeout_ms,
                                  cplat_error *detail_out)
{
    uint8_t *ptr = (uint8_t *)buf;
    size_t received = 0U;
    uint64_t deadline_ms = 0U;
    HANDLE evt = NULL;

    if ((len > 0U) && (conn->has_peek_byte != 0))
    {
        ptr[0] = conn->peek_byte;
        conn->has_peek_byte = 0;
        received = 1U;
    }
    if (received >= len)
    {
        return CPLAT_OK;
    }

    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    evt = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (evt == NULL)
    {
        return map_win32_result(GetLastError(), detail_out);
    }

    while (received < len)
    {
        OVERLAPPED ov;
        DWORD nread = 0U;
        size_t chunk = len - received;
        DWORD read_len = (chunk > (size_t)INT32_MAX) ? (DWORD)INT32_MAX : (DWORD)chunk;

        memset(&ov, 0, sizeof(ov));
        ov.hEvent = (HANDLE)((uintptr_t)evt | (uintptr_t)1U);
        (void)ResetEvent(evt);

        if (!ReadFile(conn->handle, ptr + received, read_len, &nread, &ov))
        {
            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING)
            {
                int rc = wait_overlapped_io(conn->handle, &ov, &nread, timeout_ms, deadline_ms, detail_out);
                if (rc != CPLAT_OK)
                {
                    CloseHandle(evt);
                    return rc;
                }
            }
            else if ((err == ERROR_BROKEN_PIPE) || (err == ERROR_PIPE_NOT_CONNECTED))
            {
                CloseHandle(evt);
                return CPLAT_ERR_EOF;
            }
            else
            {
                CloseHandle(evt);
                return map_win32_result(err, detail_out);
            }
        }
        if (nread == 0U)
        {
            CloseHandle(evt);
            return CPLAT_ERR_EOF;
        }
        received += (size_t)nread;
    }

    CloseHandle(evt);
    return CPLAT_OK;
}

static void pipe_win_conn_close(pos_ipc_transport_conn *conn)
{
    if (conn == NULL)
    {
        return;
    }
    if (conn->handle != INVALID_HANDLE_VALUE)
    {
        (void)CancelIoEx(conn->handle, NULL);
        CloseHandle(conn->handle);
        conn->handle = INVALID_HANDLE_VALUE;
    }
    free(conn);
}

static int pipe_win_listener_open(const pos_ipc_resolved_endpoint *resolved, const pos_ipc_access_policy *policy,
                                  pos_ipc_transport_listener **listener_out, cplat_error *detail_out)
{
    pos_ipc_transport_listener *listener = NULL;
    HANDLE first_pipe;

    *listener_out = NULL;
    if ((policy != NULL) && (policy->mode != POS_IPC_ACCESS_CURRENT_USER))
    {
        return CPLAT_ERR_UNSUPPORTED;
    }

    first_pipe = create_pipe_instance(resolved->address, 1, detail_out);
    if (first_pipe == INVALID_HANDLE_VALUE)
    {
        return (GetLastError() == ERROR_ACCESS_DENIED) ? CPLAT_ERR_BUSY : map_win32_result(GetLastError(), detail_out);
    }

    listener = (pos_ipc_transport_listener *)calloc(1U, sizeof(*listener));
    if (listener == NULL)
    {
        CloseHandle(first_pipe);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    listener->first_pipe = first_pipe;
    strncpy(listener->address, resolved->address, sizeof(listener->address) - 1U);
    listener->address[sizeof(listener->address) - 1U] = '\0';

    *listener_out = listener;
    return CPLAT_OK;
}

static void pipe_win_listener_close(pos_ipc_transport_listener *listener)
{
    if (listener == NULL)
    {
        return;
    }
    if (listener->first_pipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(listener->first_pipe);
        listener->first_pipe = INVALID_HANDLE_VALUE;
    }
    free(listener);
}

static int pipe_win_reactor_create(pos_ipc_transport_listener *listener, uint32_t max_connections,
                                   pos_ipc_transport_reactor **reactor_out, cplat_error *detail_out)
{
    pos_ipc_transport_reactor *reactor = NULL;
    HANDLE first_pipe;
    int rc;

    (void)max_connections;
    *reactor_out = NULL;

    reactor = (pos_ipc_transport_reactor *)calloc(1U, sizeof(*reactor));
    if (reactor == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    reactor->listener = listener;
    reactor->accept_pipe = INVALID_HANDLE_VALUE;
    reactor->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0U, 1U);
    if (reactor->iocp == NULL)
    {
        DWORD err = GetLastError();
        free(reactor);
        return map_win32_result(err, detail_out);
    }

    rc = cplat_local_lock_create(&reactor->lock);
    if (rc != CPLAT_OK)
    {
        CloseHandle(reactor->iocp);
        free(reactor);
        return rc;
    }

    first_pipe = listener->first_pipe;
    listener->first_pipe = INVALID_HANDLE_VALUE;

    rc = start_async_accept(reactor, first_pipe, detail_out);
    if (rc != CPLAT_OK)
    {
        pipe_win_reactor_destroy(reactor);
        return rc;
    }

    *reactor_out = reactor;
    return CPLAT_OK;
}

static int pipe_win_reactor_add_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn, void *user_data,
                                     cplat_error *detail_out)
{
    DWORD nread = 0U;

    cplat_local_lock_lock(reactor->lock);
    conn->user_data = user_data;
    if (conn->iocp_bound == 0)
    {
        if (CreateIoCompletionPort(conn->handle, reactor->iocp, (ULONG_PTR)conn, 0U) == NULL)
        {
            DWORD err = GetLastError();
            cplat_local_lock_unlock(reactor->lock);
            return map_win32_result(err, detail_out);
        }
        conn->iocp_bound = 1;
    }

    memset(&conn->read_ov, 0, sizeof(conn->read_ov));
    conn->read_armed = 1;
    if (!ReadFile(conn->handle, &conn->peek_byte, 1U, &nread, &conn->read_ov))
    {
        DWORD err = GetLastError();
        if (err != ERROR_IO_PENDING)
        {
            conn->read_armed = 0;
            cplat_local_lock_unlock(reactor->lock);
            return map_win32_result(err, detail_out);
        }
    }
    cplat_local_lock_unlock(reactor->lock);
    return CPLAT_OK;
}

static void pipe_win_reactor_remove_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn)
{
    cplat_local_lock_lock(reactor->lock);
    if ((conn != NULL) && (conn->read_armed != 0))
    {
        (void)CancelIoEx(conn->handle, &conn->read_ov);
        conn->read_armed = 0;
    }
    cplat_local_lock_unlock(reactor->lock);
}

static int pipe_win_reactor_wait(pos_ipc_transport_reactor *reactor, int timeout_ms, pos_ipc_reactor_event *events_out,
                                 size_t max_events, size_t *event_count_out, cplat_error *detail_out)
{
    DWORD bytes = 0U;
    ULONG_PTR key = 0U;
    LPOVERLAPPED ov = NULL;
    DWORD wait_ms = (timeout_ms < 0) ? INFINITE : (DWORD)timeout_ms;
    BOOL ok;

    (void)detail_out;
    *event_count_out = 0U;
    if (max_events == 0U)
    {
        return CPLAT_OK;
    }

    ok = GetQueuedCompletionStatus(reactor->iocp, &bytes, &key, &ov, wait_ms);
    if ((!ok) && (ov == NULL))
    {
        return CPLAT_OK;
    }

    if (key == POS_IPC_IOCP_KEY_WAKEUP)
    {
        return CPLAT_OK;
    }

    if (key == POS_IPC_IOCP_KEY_ACCEPT)
    {
        HANDLE accepted_handle = reactor->accept_pipe;
        reactor->accept_pipe = INVALID_HANDLE_VALUE;
        reactor->accept_pending = 0;

        if (ok && (accepted_handle != INVALID_HANDLE_VALUE))
        {
            pos_ipc_transport_conn *conn = (pos_ipc_transport_conn *)calloc(1U, sizeof(*conn));
            if (conn != NULL)
            {
                conn->handle = accepted_handle;
                memset(&events_out[0], 0, sizeof(events_out[0]));
                events_out[0].kind = POS_IPC_REACTOR_EVENT_ACCEPT;
                events_out[0].accepted_conn = conn;
                *event_count_out = 1U;
            }
            else
            {
                CloseHandle(accepted_handle);
            }
        }
        else if (accepted_handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(accepted_handle);
        }

        (void)start_async_accept(reactor, INVALID_HANDLE_VALUE, NULL);
        return CPLAT_OK;
    }

    {
        pos_ipc_transport_conn *conn = (pos_ipc_transport_conn *)key;
        int emit = 0;
        void *user_data = NULL;

        cplat_local_lock_lock(reactor->lock);
        if ((conn != NULL) && (conn->read_armed != 0))
        {
            conn->read_armed = 0;
            user_data = conn->user_data;
            emit = 1;
        }
        cplat_local_lock_unlock(reactor->lock);

        if (emit != 0)
        {
            memset(&events_out[0], 0, sizeof(events_out[0]));
            events_out[0].user_data = user_data;
            if (ok && (bytes == 1U))
            {
                conn->has_peek_byte = 1;
                events_out[0].kind = POS_IPC_REACTOR_EVENT_READABLE;
            }
            else
            {
                events_out[0].kind = POS_IPC_REACTOR_EVENT_CLOSED;
            }
            *event_count_out = 1U;
        }
    }

    return CPLAT_OK;
}

static void pipe_win_reactor_wakeup(pos_ipc_transport_reactor *reactor)
{
    if ((reactor != NULL) && (reactor->iocp != NULL))
    {
        (void)PostQueuedCompletionStatus(reactor->iocp, 0U, POS_IPC_IOCP_KEY_WAKEUP, NULL);
    }
}

static void pipe_win_reactor_destroy(pos_ipc_transport_reactor *reactor)
{
    if (reactor == NULL)
    {
        return;
    }
    if (reactor->accept_pipe != INVALID_HANDLE_VALUE)
    {
        (void)CancelIoEx(reactor->accept_pipe, &reactor->accept_ov);
        CloseHandle(reactor->accept_pipe);
        reactor->accept_pipe = INVALID_HANDLE_VALUE;
    }
    if (reactor->iocp != NULL)
    {
        CloseHandle(reactor->iocp);
        reactor->iocp = NULL;
    }
    if (reactor->lock != NULL)
    {
        cplat_local_lock_dispose(reactor->lock);
    }
    free(reactor);
}

static HANDLE create_pipe_instance(const char *address, int first_instance, cplat_error *detail_out)
{
    DWORD open_mode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;
    HANDLE pipe;

    /*
     * 初回インスタンスに FILE_FLAG_FIRST_PIPE_INSTANCE を付与し、同一エンドポイントの二重起動を即時検出する。
     * see: https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createnamedpipea
     */
    if (first_instance != 0)
    {
        open_mode |= FILE_FLAG_FIRST_PIPE_INSTANCE;
    }

    pipe = CreateNamedPipeU(address, open_mode, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                            PIPE_UNLIMITED_INSTANCES, POS_IPC_PIPE_BUF_SIZE, POS_IPC_PIPE_BUF_SIZE, 0U, NULL);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        cplat_error_capture_current_windows_error(detail_out);
    }
    return pipe;
}

static int start_async_accept(pos_ipc_transport_reactor *reactor, HANDLE existing_pipe, cplat_error *detail_out)
{
    HANDLE pipe = existing_pipe;

    if (pipe == INVALID_HANDLE_VALUE)
    {
        pipe = create_pipe_instance(reactor->listener->address, 0, detail_out);
        if (pipe == INVALID_HANDLE_VALUE)
        {
            return map_win32_result(GetLastError(), detail_out);
        }
    }

    if (CreateIoCompletionPort(pipe, reactor->iocp, POS_IPC_IOCP_KEY_ACCEPT, 0U) == NULL)
    {
        DWORD err = GetLastError();
        CloseHandle(pipe);
        return map_win32_result(err, detail_out);
    }

    memset(&reactor->accept_ov, 0, sizeof(reactor->accept_ov));
    reactor->accept_pipe = pipe;
    reactor->accept_pending = 1;

    if (!ConnectNamedPipe(pipe, &reactor->accept_ov))
    {
        DWORD err = GetLastError();
        if (err == ERROR_IO_PENDING)
        {
            return CPLAT_OK;
        }
        if (err == ERROR_PIPE_CONNECTED)
        {
            (void)PostQueuedCompletionStatus(reactor->iocp, 0U, POS_IPC_IOCP_KEY_ACCEPT, &reactor->accept_ov);
            return CPLAT_OK;
        }
        CloseHandle(pipe);
        reactor->accept_pipe = INVALID_HANDLE_VALUE;
        reactor->accept_pending = 0;
        return map_win32_result(err, detail_out);
    }

    return CPLAT_OK;
}

static int wait_overlapped_io(HANDLE handle, OVERLAPPED *ov, DWORD *transferred_out, int timeout_ms,
                              uint64_t deadline_ms, cplat_error *detail_out)
{
    HANDLE evt = (HANDLE)((uintptr_t)ov->hEvent & ~(uintptr_t)1U);
    DWORD wait_ms;
    DWORD wait_rc;

    if (timeout_ms == CPLAT_WAIT_FOREVER)
    {
        wait_ms = INFINITE;
    }
    else if (timeout_ms == CPLAT_NO_WAIT)
    {
        wait_ms = 0U;
    }
    else
    {
        uint64_t now = now_monotonic_ms();
        if (now >= deadline_ms)
        {
            (void)CancelIoEx(handle, ov);
            return CPLAT_ERR_TIMEOUT;
        }
        {
            uint64_t rem = deadline_ms - now;
            wait_ms = (rem > (uint64_t)INT32_MAX) ? (DWORD)INT32_MAX : (DWORD)rem;
        }
    }

    wait_rc = WaitForSingleObject(evt, wait_ms);
    if (wait_rc == WAIT_TIMEOUT)
    {
        (void)CancelIoEx(handle, ov);
        (void)GetOverlappedResult(handle, ov, transferred_out, TRUE);
        return CPLAT_ERR_TIMEOUT;
    }
    if (wait_rc != WAIT_OBJECT_0)
    {
        return map_win32_result(GetLastError(), detail_out);
    }

    if (!GetOverlappedResult(handle, ov, transferred_out, FALSE))
    {
        DWORD err = GetLastError();
        if ((err == ERROR_BROKEN_PIPE) || (err == ERROR_PIPE_NOT_CONNECTED))
        {
            return CPLAT_ERR_EOF;
        }
        return map_win32_result(err, detail_out);
    }

    return CPLAT_OK;
}

static int map_win32_result(DWORD err, cplat_error *detail_out)
{
    cplat_error_capture_windows_error(detail_out, (unsigned long)err);
    if ((err == ERROR_FILE_NOT_FOUND) || (err == ERROR_PATH_NOT_FOUND))
    {
        return CPLAT_ERR_NOT_FOUND;
    }
    if (err == ERROR_ACCESS_DENIED)
    {
        return CPLAT_ERR_PERMISSION_DENIED;
    }
    if ((err == ERROR_TIMEOUT) || (err == WAIT_TIMEOUT))
    {
        return CPLAT_ERR_TIMEOUT;
    }
    if (err == ERROR_PIPE_BUSY)
    {
        return CPLAT_ERR_BUSY;
    }
    if ((err == ERROR_BROKEN_PIPE) || (err == ERROR_PIPE_NOT_CONNECTED))
    {
        return CPLAT_ERR_EOF;
    }
    return cplat_error_to_result(detail_out);
}

static uint64_t now_monotonic_ms(void)
{
    uint64_t ms = 0U;
    (void)cplat_clock_get_monotonic_ms(&ms);
    return ms;
}

#else

typedef int pos_ipc_transport_pipe_windows_unused_t;

#endif /* PLATFORM_WINDOWS */
