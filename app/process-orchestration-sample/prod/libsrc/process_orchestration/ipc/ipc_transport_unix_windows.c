#include <process_orchestration/ipc_transport.h>

#include <cplat/base/platform.h>

#if defined(PLATFORM_WINDOWS)

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <afunix.h>
    #include <windows.h>

    #include <cplat/clock/clock.h>
    #include <cplat/sync/sync.h>
    #include <cplat/win32/win32.h>
    #include <stddef.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>

    #pragma comment(lib, "ws2_32.lib")

struct pos_ipc_transport_conn
{
    SOCKET sock;
};

struct pos_ipc_transport_listener
{
    SOCKET sock;
    char address[POS_IPC_RESOLVED_ADDR_MAX];
};

typedef struct pos_ipc_win_unix_entry
{
    pos_ipc_transport_conn *conn;
    void *user_data;
    int active;
    int pad;
} pos_ipc_win_unix_entry;

struct pos_ipc_transport_reactor
{
    pos_ipc_transport_listener *listener;
    cplat_local_lock *lock;
    pos_ipc_win_unix_entry *entries;
    WSAPOLLFD *poll_fds;
    pos_ipc_win_unix_entry **poll_map;
    SOCKET wakeup_read;
    SOCKET wakeup_write;
    uint32_t max_connections;
    uint32_t pad;
};

static cplat_once g_wsa_once = CPLAT_ONCE_INIT;
static int g_wsa_init_rc = CPLAT_ERR_UNKNOWN;

static int unix_win_client_connect(const pos_ipc_resolved_endpoint *resolved, int timeout_ms,
                                   pos_ipc_transport_conn **conn_out, cplat_error *detail_out);
static int unix_win_conn_send_all(pos_ipc_transport_conn *conn, const void *buf, size_t len, int timeout_ms,
                                  cplat_error *detail_out);
static int unix_win_conn_recv_all(pos_ipc_transport_conn *conn, void *buf, size_t len, int timeout_ms,
                                  cplat_error *detail_out);
static void unix_win_conn_close(pos_ipc_transport_conn *conn);
static int unix_win_listener_open(const pos_ipc_resolved_endpoint *resolved, const pos_ipc_access_policy *policy,
                                  pos_ipc_transport_listener **listener_out, cplat_error *detail_out);
static void unix_win_listener_close(pos_ipc_transport_listener *listener);
static int unix_win_reactor_create(pos_ipc_transport_listener *listener, uint32_t max_connections,
                                   pos_ipc_transport_reactor **reactor_out, cplat_error *detail_out);
static int unix_win_reactor_add_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn, void *user_data,
                                     cplat_error *detail_out);
static void unix_win_reactor_remove_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn);
static int unix_win_reactor_wait(pos_ipc_transport_reactor *reactor, int timeout_ms, pos_ipc_reactor_event *events_out,
                                 size_t max_events, size_t *event_count_out, cplat_error *detail_out);
static void unix_win_reactor_wakeup(pos_ipc_transport_reactor *reactor);
static void unix_win_reactor_destroy(pos_ipc_transport_reactor *reactor);

static void init_winsock_once(void);
static int ensure_winsock_ready(void);
static int set_nonblocking(SOCKET sock);
static int build_sockaddr_un(const char *path, SOCKADDR_UN *addr_out, int *len_out);
static int probe_existing_socket(const char *path, cplat_error *detail_out);
static int create_wakeup_pair(SOCKET *read_out, SOCKET *write_out, cplat_error *detail_out);
static int wait_socket_ready(SOCKET sock, SHORT events, int timeout_ms, uint64_t deadline_ms, cplat_error *detail_out);
static int map_wsa_result(int wsa_err, cplat_error *detail_out);
static uint64_t now_monotonic_ms(void);

static const pos_ipc_transport_ops g_unix_win_ops = {
    unix_win_client_connect,      unix_win_conn_send_all,  unix_win_conn_recv_all,  unix_win_conn_close,
    unix_win_listener_open,       unix_win_listener_close, unix_win_reactor_create, unix_win_reactor_add_conn,
    unix_win_reactor_remove_conn, unix_win_reactor_wait,   unix_win_reactor_wakeup, unix_win_reactor_destroy};

const pos_ipc_transport_ops *pos_ipc_transport_unix_windows_ops(void)
{
    return &g_unix_win_ops;
}

static int unix_win_client_connect(const pos_ipc_resolved_endpoint *resolved, int timeout_ms,
                                   pos_ipc_transport_conn **conn_out, cplat_error *detail_out)
{
    SOCKADDR_UN addr;
    int addr_len = 0;
    SOCKET sock = INVALID_SOCKET;
    pos_ipc_transport_conn *conn = NULL;
    uint64_t deadline_ms = 0U;
    int rc;

    *conn_out = NULL;
    rc = ensure_winsock_ready();
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = build_sockaddr_un(resolved->address, &addr, &addr_len);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    /*
     * Windows 10 1803+ のネイティブ AF_UNIX ソケットを使用する。
     * see: https://devblogs.microsoft.com/commandline/af_unix-comes-to-windows/
     */
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        return map_wsa_result(WSAGetLastError(), detail_out);
    }

    if (set_nonblocking(sock) != CPLAT_OK)
    {
        int err = WSAGetLastError();
        closesocket(sock);
        return map_wsa_result(err, detail_out);
    }

    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    if (connect(sock, (const struct sockaddr *)&addr, addr_len) == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if ((err == WSAEWOULDBLOCK) || (err == WSAEINPROGRESS))
        {
            int so_err = 0;
            int so_len = (int)sizeof(so_err);

            rc = wait_socket_ready(sock, POLLWRNORM, timeout_ms, deadline_ms, detail_out);
            if (rc != CPLAT_OK)
            {
                closesocket(sock);
                return rc;
            }
            if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&so_err, &so_len) == SOCKET_ERROR)
            {
                int get_err = WSAGetLastError();
                closesocket(sock);
                return map_wsa_result(get_err, detail_out);
            }
            if (so_err != 0)
            {
                closesocket(sock);
                return map_wsa_result(so_err, detail_out);
            }
        }
        else
        {
            closesocket(sock);
            return map_wsa_result(err, detail_out);
        }
    }

    conn = (pos_ipc_transport_conn *)calloc(1U, sizeof(*conn));
    if (conn == NULL)
    {
        closesocket(sock);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    conn->sock = sock;
    *conn_out = conn;
    return CPLAT_OK;
}

static int unix_win_conn_send_all(pos_ipc_transport_conn *conn, const void *buf, size_t len, int timeout_ms,
                                  cplat_error *detail_out)
{
    const char *ptr = (const char *)buf;
    size_t sent = 0U;
    uint64_t deadline_ms = 0U;

    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    while (sent < len)
    {
        size_t chunk = len - sent;
        int send_len = (chunk > (size_t)INT32_MAX) ? INT32_MAX : (int)chunk;
        int n = send(conn->sock, ptr + sent, send_len, 0);
        if (n > 0)
        {
            sent += (size_t)n;
            continue;
        }
        if (n == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
            {
                int rc = wait_socket_ready(conn->sock, POLLWRNORM, timeout_ms, deadline_ms, detail_out);
                if (rc != CPLAT_OK)
                {
                    return rc;
                }
                continue;
            }
            if ((err == WSAECONNRESET) || (err == WSAECONNABORTED) || (err == WSAESHUTDOWN))
            {
                cplat_error_capture_windows_error(detail_out, (unsigned long)err);
                return CPLAT_ERR_EOF;
            }
            return map_wsa_result(err, detail_out);
        }
        return CPLAT_ERR_EOF;
    }

    return CPLAT_OK;
}

static int unix_win_conn_recv_all(pos_ipc_transport_conn *conn, void *buf, size_t len, int timeout_ms,
                                  cplat_error *detail_out)
{
    char *ptr = (char *)buf;
    size_t received = 0U;
    uint64_t deadline_ms = 0U;

    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    while (received < len)
    {
        size_t chunk = len - received;
        int recv_len = (chunk > (size_t)INT32_MAX) ? INT32_MAX : (int)chunk;
        int n = recv(conn->sock, ptr + received, recv_len, 0);
        if (n > 0)
        {
            received += (size_t)n;
            continue;
        }
        if (n == 0)
        {
            return CPLAT_ERR_EOF;
        }
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
            {
                int rc = wait_socket_ready(conn->sock, POLLRDNORM, timeout_ms, deadline_ms, detail_out);
                if (rc != CPLAT_OK)
                {
                    return rc;
                }
                continue;
            }
            if ((err == WSAECONNRESET) || (err == WSAECONNABORTED))
            {
                cplat_error_capture_windows_error(detail_out, (unsigned long)err);
                return CPLAT_ERR_EOF;
            }
            return map_wsa_result(err, detail_out);
        }
    }

    return CPLAT_OK;
}

static void unix_win_conn_close(pos_ipc_transport_conn *conn)
{
    if (conn == NULL)
    {
        return;
    }
    if (conn->sock != INVALID_SOCKET)
    {
        closesocket(conn->sock);
        conn->sock = INVALID_SOCKET;
    }
    free(conn);
}

static int unix_win_listener_open(const pos_ipc_resolved_endpoint *resolved, const pos_ipc_access_policy *policy,
                                  pos_ipc_transport_listener **listener_out, cplat_error *detail_out)
{
    SOCKADDR_UN addr;
    int addr_len = 0;
    SOCKET sock = INVALID_SOCKET;
    pos_ipc_transport_listener *listener = NULL;
    int rc;

    *listener_out = NULL;
    if ((policy != NULL) && (policy->mode != POS_IPC_ACCESS_CURRENT_USER))
    {
        return CPLAT_ERR_UNSUPPORTED;
    }

    rc = ensure_winsock_ready();
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = build_sockaddr_un(resolved->address, &addr, &addr_len);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = pos_ipc_endpoint_ensure_dir(resolved->parent_dir, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    rc = probe_existing_socket(resolved->address, detail_out);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        return map_wsa_result(WSAGetLastError(), detail_out);
    }

    if (set_nonblocking(sock) != CPLAT_OK)
    {
        int err = WSAGetLastError();
        closesocket(sock);
        return map_wsa_result(err, detail_out);
    }

    if (bind(sock, (const struct sockaddr *)&addr, addr_len) == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        closesocket(sock);
        return map_wsa_result(err, detail_out);
    }

    if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        closesocket(sock);
        (void)DeleteFileU(resolved->address);
        return map_wsa_result(err, detail_out);
    }

    listener = (pos_ipc_transport_listener *)calloc(1U, sizeof(*listener));
    if (listener == NULL)
    {
        closesocket(sock);
        (void)DeleteFileU(resolved->address);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    listener->sock = sock;
    strncpy(listener->address, resolved->address, sizeof(listener->address) - 1U);
    listener->address[sizeof(listener->address) - 1U] = '\0';

    *listener_out = listener;
    return CPLAT_OK;
}

static void unix_win_listener_close(pos_ipc_transport_listener *listener)
{
    if (listener == NULL)
    {
        return;
    }
    if (listener->sock != INVALID_SOCKET)
    {
        closesocket(listener->sock);
        listener->sock = INVALID_SOCKET;
    }
    if (listener->address[0] != '\0')
    {
        (void)DeleteFileU(listener->address);
        listener->address[0] = '\0';
    }
    free(listener);
}

static int unix_win_reactor_create(pos_ipc_transport_listener *listener, uint32_t max_connections,
                                   pos_ipc_transport_reactor **reactor_out, cplat_error *detail_out)
{
    pos_ipc_transport_reactor *reactor = NULL;
    int rc;

    *reactor_out = NULL;
    reactor = (pos_ipc_transport_reactor *)calloc(1U, sizeof(*reactor));
    if (reactor == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    reactor->listener = listener;
    reactor->max_connections = max_connections;
    reactor->wakeup_read = INVALID_SOCKET;
    reactor->wakeup_write = INVALID_SOCKET;

    rc = create_wakeup_pair(&reactor->wakeup_read, &reactor->wakeup_write, detail_out);
    if (rc != CPLAT_OK)
    {
        free(reactor);
        return rc;
    }

    rc = cplat_local_lock_create(&reactor->lock);
    if (rc != CPLAT_OK)
    {
        unix_win_reactor_destroy(reactor);
        return rc;
    }

    reactor->entries = (pos_ipc_win_unix_entry *)calloc((size_t)max_connections, sizeof(*reactor->entries));
    reactor->poll_fds = (WSAPOLLFD *)calloc((size_t)max_connections + 2U, sizeof(*reactor->poll_fds));
    reactor->poll_map = (pos_ipc_win_unix_entry **)calloc((size_t)max_connections + 2U, sizeof(*reactor->poll_map));
    if ((reactor->entries == NULL) || (reactor->poll_fds == NULL) || (reactor->poll_map == NULL))
    {
        unix_win_reactor_destroy(reactor);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    *reactor_out = reactor;
    return CPLAT_OK;
}

static int unix_win_reactor_add_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn, void *user_data,
                                     cplat_error *detail_out)
{
    uint32_t i;

    (void)detail_out;
    cplat_local_lock_lock(reactor->lock);
    for (i = 0U; i < reactor->max_connections; ++i)
    {
        if (reactor->entries[i].active == 0)
        {
            reactor->entries[i].conn = conn;
            reactor->entries[i].user_data = user_data;
            reactor->entries[i].active = 1;
            cplat_local_lock_unlock(reactor->lock);
            return CPLAT_OK;
        }
    }
    cplat_local_lock_unlock(reactor->lock);
    return CPLAT_ERR_LIMIT_EXCEEDED;
}

static void unix_win_reactor_remove_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn)
{
    uint32_t i;

    cplat_local_lock_lock(reactor->lock);
    for (i = 0U; i < reactor->max_connections; ++i)
    {
        if ((reactor->entries[i].active != 0) && (reactor->entries[i].conn == conn))
        {
            reactor->entries[i].conn = NULL;
            reactor->entries[i].user_data = NULL;
            reactor->entries[i].active = 0;
            break;
        }
    }
    cplat_local_lock_unlock(reactor->lock);
}

static int unix_win_reactor_wait(pos_ipc_transport_reactor *reactor, int timeout_ms, pos_ipc_reactor_event *events_out,
                                 size_t max_events, size_t *event_count_out, cplat_error *detail_out)
{
    ULONG nfds = 0U;
    uint32_t i;
    size_t count = 0U;
    int ready;

    *event_count_out = 0U;

    reactor->poll_fds[nfds].fd = reactor->wakeup_read;
    reactor->poll_fds[nfds].events = POLLRDNORM;
    reactor->poll_fds[nfds].revents = 0;
    reactor->poll_map[nfds] = NULL;
    nfds++;

    reactor->poll_fds[nfds].fd = reactor->listener->sock;
    reactor->poll_fds[nfds].events = POLLRDNORM;
    reactor->poll_fds[nfds].revents = 0;
    reactor->poll_map[nfds] = NULL;
    nfds++;

    cplat_local_lock_lock(reactor->lock);
    for (i = 0U; i < reactor->max_connections; ++i)
    {
        if (reactor->entries[i].active != 0)
        {
            reactor->poll_fds[nfds].fd = reactor->entries[i].conn->sock;
            reactor->poll_fds[nfds].events = POLLRDNORM;
            reactor->poll_fds[nfds].revents = 0;
            reactor->poll_map[nfds] = &reactor->entries[i];
            nfds++;
        }
    }
    cplat_local_lock_unlock(reactor->lock);

    ready = WSAPoll(reactor->poll_fds, nfds, timeout_ms);
    if (ready == SOCKET_ERROR)
    {
        return map_wsa_result(WSAGetLastError(), detail_out);
    }
    if (ready == 0)
    {
        return CPLAT_OK;
    }

    if ((reactor->poll_fds[0].revents & POLLRDNORM) != 0)
    {
        char drain[64];
        while (recv(reactor->wakeup_read, drain, (int)sizeof(drain), 0) > 0)
        {
        }
    }

    if ((reactor->poll_fds[1].revents & POLLRDNORM) != 0)
    {
        while (count < max_events)
        {
            SOCKET client_sock = accept(reactor->listener->sock, NULL, NULL);
            if (client_sock == INVALID_SOCKET)
            {
                break;
            }
            if (set_nonblocking(client_sock) != CPLAT_OK)
            {
                closesocket(client_sock);
                break;
            }
            {
                pos_ipc_transport_conn *accepted = (pos_ipc_transport_conn *)calloc(1U, sizeof(*accepted));
                if (accepted == NULL)
                {
                    closesocket(client_sock);
                    break;
                }
                accepted->sock = client_sock;
                memset(&events_out[count], 0, sizeof(events_out[count]));
                events_out[count].kind = POS_IPC_REACTOR_EVENT_ACCEPT;
                events_out[count].accepted_conn = accepted;
                count++;
            }
        }
    }

    for (ULONG idx = 2U; (idx < nfds) && (count < max_events); ++idx)
    {
        SHORT revents = reactor->poll_fds[idx].revents;
        pos_ipc_win_unix_entry *entry = reactor->poll_map[idx];
        void *user_data = NULL;
        int still_active = 0;

        if ((revents == 0) || (entry == NULL))
        {
            continue;
        }

        cplat_local_lock_lock(reactor->lock);
        if (entry->active != 0)
        {
            user_data = entry->user_data;
            entry->conn = NULL;
            entry->user_data = NULL;
            entry->active = 0;
            still_active = 1;
        }
        cplat_local_lock_unlock(reactor->lock);

        if (still_active == 0)
        {
            continue;
        }

        memset(&events_out[count], 0, sizeof(events_out[count]));
        events_out[count].user_data = user_data;
        if (((revents & (POLLERR | POLLNVAL)) != 0) || (((revents & POLLHUP) != 0) && ((revents & POLLRDNORM) == 0)))
        {
            events_out[count].kind = POS_IPC_REACTOR_EVENT_CLOSED;
        }
        else
        {
            events_out[count].kind = POS_IPC_REACTOR_EVENT_READABLE;
        }
        count++;
    }

    *event_count_out = count;
    return CPLAT_OK;
}

static void unix_win_reactor_wakeup(pos_ipc_transport_reactor *reactor)
{
    char byte = 1;
    if ((reactor != NULL) && (reactor->wakeup_write != INVALID_SOCKET))
    {
        (void)send(reactor->wakeup_write, &byte, 1, 0);
    }
}

static void unix_win_reactor_destroy(pos_ipc_transport_reactor *reactor)
{
    if (reactor == NULL)
    {
        return;
    }
    free(reactor->poll_map);
    free(reactor->poll_fds);
    free(reactor->entries);
    if (reactor->lock != NULL)
    {
        cplat_local_lock_dispose(reactor->lock);
    }
    if (reactor->wakeup_read != INVALID_SOCKET)
    {
        closesocket(reactor->wakeup_read);
    }
    if (reactor->wakeup_write != INVALID_SOCKET)
    {
        closesocket(reactor->wakeup_write);
    }
    free(reactor);
}

static void init_winsock_once(void)
{
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0)
    {
        g_wsa_init_rc = CPLAT_OK;
    }
    else
    {
        g_wsa_init_rc = CPLAT_ERR_UNKNOWN;
    }
}

static int ensure_winsock_ready(void)
{
    cplat_call_once(&g_wsa_once, init_winsock_once);
    return g_wsa_init_rc;
}

static int set_nonblocking(SOCKET sock)
{
    u_long mode = 1U;
    if (ioctlsocket(sock, FIONBIO, &mode) == SOCKET_ERROR)
    {
        return CPLAT_ERR_UNKNOWN;
    }
    return CPLAT_OK;
}

static int build_sockaddr_un(const char *path, SOCKADDR_UN *addr_out, int *len_out)
{
    size_t path_len = strlen(path);
    if ((path_len == 0U) || (path_len >= sizeof(addr_out->sun_path)))
    {
        return CPLAT_ERR_LIMIT_EXCEEDED;
    }

    memset(addr_out, 0, sizeof(*addr_out));
    addr_out->sun_family = AF_UNIX;
    memcpy(addr_out->sun_path, path, path_len + 1U);
    *len_out = (int)sizeof(*addr_out);
    return CPLAT_OK;
}

static int probe_existing_socket(const char *path, cplat_error *detail_out)
{
    DWORD attrs = GetFileAttributesU(path);
    SOCKADDR_UN addr;
    int addr_len = 0;
    SOCKET probe_sock = INVALID_SOCKET;
    int rc;

    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        return CPLAT_OK;
    }

    rc = build_sockaddr_un(path, &addr, &addr_len);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    probe_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (probe_sock == INVALID_SOCKET)
    {
        return map_wsa_result(WSAGetLastError(), detail_out);
    }
    (void)set_nonblocking(probe_sock);

    if (connect(probe_sock, (const struct sockaddr *)&addr, addr_len) == 0)
    {
        closesocket(probe_sock);
        return CPLAT_ERR_BUSY;
    }
    {
        int err = WSAGetLastError();
        if ((err == WSAEWOULDBLOCK) || (err == WSAEINPROGRESS))
        {
            WSAPOLLFD pfd;
            pfd.fd = probe_sock;
            pfd.events = POLLWRNORM;
            pfd.revents = 0;
            if ((WSAPoll(&pfd, 1U, 100) > 0) && ((pfd.revents & POLLWRNORM) != 0) && ((pfd.revents & POLLERR) == 0))
            {
                closesocket(probe_sock);
                return CPLAT_ERR_BUSY;
            }
        }
        closesocket(probe_sock);
    }

    (void)DeleteFileU(path);
    return CPLAT_OK;
}

static int create_wakeup_pair(SOCKET *read_out, SOCKET *write_out, cplat_error *detail_out)
{
    SOCKET listen_sock = INVALID_SOCKET;
    SOCKET write_sock = INVALID_SOCKET;
    SOCKET read_sock = INVALID_SOCKET;
    struct sockaddr_in addr;
    int addr_len = (int)sizeof(addr);

    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET)
    {
        return map_wsa_result(WSAGetLastError(), detail_out);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0U;

    if ((bind(listen_sock, (const struct sockaddr *)&addr, addr_len) == SOCKET_ERROR) ||
        (listen(listen_sock, 1) == SOCKET_ERROR) ||
        (getsockname(listen_sock, (struct sockaddr *)&addr, &addr_len) == SOCKET_ERROR))
    {
        int err = WSAGetLastError();
        closesocket(listen_sock);
        return map_wsa_result(err, detail_out);
    }

    write_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (write_sock == INVALID_SOCKET)
    {
        int err = WSAGetLastError();
        closesocket(listen_sock);
        return map_wsa_result(err, detail_out);
    }

    if (connect(write_sock, (const struct sockaddr *)&addr, addr_len) == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        closesocket(write_sock);
        closesocket(listen_sock);
        return map_wsa_result(err, detail_out);
    }

    read_sock = accept(listen_sock, NULL, NULL);
    closesocket(listen_sock);
    if (read_sock == INVALID_SOCKET)
    {
        int err = WSAGetLastError();
        closesocket(write_sock);
        return map_wsa_result(err, detail_out);
    }

    (void)set_nonblocking(read_sock);
    (void)set_nonblocking(write_sock);
    *read_out = read_sock;
    *write_out = write_sock;
    return CPLAT_OK;
}

static int wait_socket_ready(SOCKET sock, SHORT events, int timeout_ms, uint64_t deadline_ms, cplat_error *detail_out)
{
    WSAPOLLFD pfd;
    int wait_ms;
    int rc;

    if (timeout_ms == CPLAT_WAIT_FOREVER)
    {
        wait_ms = -1;
    }
    else if (timeout_ms == CPLAT_NO_WAIT)
    {
        wait_ms = 0;
    }
    else
    {
        uint64_t now = now_monotonic_ms();
        if (now >= deadline_ms)
        {
            return CPLAT_ERR_TIMEOUT;
        }
        {
            uint64_t rem = deadline_ms - now;
            wait_ms = (rem > (uint64_t)INT32_MAX) ? INT32_MAX : (int)rem;
        }
    }

    pfd.fd = sock;
    pfd.events = events;
    pfd.revents = 0;

    rc = WSAPoll(&pfd, 1U, wait_ms);
    if (rc > 0)
    {
        return CPLAT_OK;
    }
    if (rc == 0)
    {
        return CPLAT_ERR_TIMEOUT;
    }
    return map_wsa_result(WSAGetLastError(), detail_out);
}

static int map_wsa_result(int wsa_err, cplat_error *detail_out)
{
    cplat_error_capture_windows_error(detail_out, (unsigned long)wsa_err);
    if ((wsa_err == WSAECONNREFUSED) || (wsa_err == WSAENOENT) || (wsa_err == ERROR_FILE_NOT_FOUND))
    {
        return CPLAT_ERR_NOT_FOUND;
    }
    if (wsa_err == WSAEACCES)
    {
        return CPLAT_ERR_PERMISSION_DENIED;
    }
    if (wsa_err == WSAETIMEDOUT)
    {
        return CPLAT_ERR_TIMEOUT;
    }
    if (wsa_err == WSAEADDRINUSE)
    {
        return CPLAT_ERR_BUSY;
    }
    if (wsa_err == WSAENOBUFS)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    return CPLAT_ERR_UNKNOWN;
}

static uint64_t now_monotonic_ms(void)
{
    uint64_t ms = 0U;
    (void)cplat_clock_get_monotonic_ms(&ms);
    return ms;
}

#else

typedef int pos_ipc_transport_unix_windows_unused_t;

#endif /* PLATFORM_WINDOWS */
