#include <process_orchestration/ipc_transport.h>

#include <cplat/base/platform.h>

#if defined(PLATFORM_LINUX)

    #include <cplat/clock/clock.h>
    #include <cplat/sync/sync.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <stddef.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/eventfd.h>
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <sys/un.h>
    #include <unistd.h>

struct pos_ipc_transport_conn
{
    int fd;
};

struct pos_ipc_transport_listener
{
    char address[POS_IPC_RESOLVED_ADDR_MAX];
    int fd;
    int pad;
};

typedef struct pos_ipc_linux_entry
{
    pos_ipc_transport_conn *conn;
    void *user_data;
    int active;
    int pad;
} pos_ipc_linux_entry;

struct pos_ipc_transport_reactor
{
    pos_ipc_transport_listener *listener;
    cplat_local_lock *lock;
    pos_ipc_linux_entry *entries;
    struct pollfd *poll_fds;
    pos_ipc_linux_entry **poll_map;
    uint32_t max_connections;
    int wakeup_fd;
};

static int unix_linux_client_connect(const pos_ipc_resolved_endpoint *resolved, int timeout_ms,
                                     pos_ipc_transport_conn **conn_out, cplat_error *detail_out);
static int unix_linux_conn_send_all(pos_ipc_transport_conn *conn, const void *buf, size_t len, int timeout_ms,
                                    cplat_error *detail_out);
static int unix_linux_conn_recv_all(pos_ipc_transport_conn *conn, void *buf, size_t len, int timeout_ms,
                                    cplat_error *detail_out);
static void unix_linux_conn_close(pos_ipc_transport_conn *conn);
static int unix_linux_listener_open(const pos_ipc_resolved_endpoint *resolved, const pos_ipc_access_policy *policy,
                                    pos_ipc_transport_listener **listener_out, cplat_error *detail_out);
static void unix_linux_listener_close(pos_ipc_transport_listener *listener);
static int unix_linux_reactor_create(pos_ipc_transport_listener *listener, uint32_t max_connections,
                                     pos_ipc_transport_reactor **reactor_out, cplat_error *detail_out);
static int unix_linux_reactor_add_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn,
                                       void *user_data, cplat_error *detail_out);
static void unix_linux_reactor_remove_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn);
static int unix_linux_reactor_wait(pos_ipc_transport_reactor *reactor, int timeout_ms,
                                   pos_ipc_reactor_event *events_out, size_t max_events, size_t *event_count_out,
                                   cplat_error *detail_out);
static void unix_linux_reactor_wakeup(pos_ipc_transport_reactor *reactor);
static void unix_linux_reactor_destroy(pos_ipc_transport_reactor *reactor);

static int build_sockaddr_un(const char *path, struct sockaddr_un *addr_out, socklen_t *len_out);
static int probe_existing_socket(const char *path, cplat_error *detail_out);
static int wait_fd_ready(int fd, short events, int timeout_ms, uint64_t deadline_ms, cplat_error *detail_out);
static int map_errno_result(int err, cplat_error *detail_out);
static uint64_t now_monotonic_ms(void);

static const pos_ipc_transport_ops g_unix_linux_ops = {
    unix_linux_client_connect,      unix_linux_conn_send_all,  unix_linux_conn_recv_all,  unix_linux_conn_close,
    unix_linux_listener_open,       unix_linux_listener_close, unix_linux_reactor_create, unix_linux_reactor_add_conn,
    unix_linux_reactor_remove_conn, unix_linux_reactor_wait,   unix_linux_reactor_wakeup, unix_linux_reactor_destroy};

const pos_ipc_transport_ops *pos_ipc_transport_unix_linux_ops(void)
{
    return &g_unix_linux_ops;
}

static int unix_linux_client_connect(const pos_ipc_resolved_endpoint *resolved, int timeout_ms,
                                     pos_ipc_transport_conn **conn_out, cplat_error *detail_out)
{
    struct sockaddr_un addr;
    socklen_t addr_len = 0;
    pos_ipc_transport_conn *conn = NULL;
    uint64_t deadline_ms = 0U;
    int fd = -1;
    int rc;

    *conn_out = NULL;
    rc = build_sockaddr_un(resolved->address, &addr, &addr_len);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    /*
     * SOCK_NONBLOCK | SOCK_CLOEXEC により接続タイムアウト制御と子プロセスへの FD 漏洩防止を行う。
     * see: https://man7.org/linux/man-pages/man2/socket.2.html
     */
    fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0)
    {
        return map_errno_result(errno, detail_out);
    }

    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    if (connect(fd, (const struct sockaddr *)&addr, addr_len) < 0)
    {
        int err = errno;
        if ((err == EINPROGRESS) || (err == EAGAIN))
        {
            int so_err = 0;
            socklen_t so_len = (socklen_t)sizeof(so_err);

            rc = wait_fd_ready(fd, POLLOUT, timeout_ms, deadline_ms, detail_out);
            if (rc != CPLAT_OK)
            {
                close(fd);
                return rc;
            }
            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_err, &so_len) < 0)
            {
                int get_err = errno;
                close(fd);
                return map_errno_result(get_err, detail_out);
            }
            if (so_err != 0)
            {
                close(fd);
                return map_errno_result(so_err, detail_out);
            }
        }
        else
        {
            close(fd);
            return map_errno_result(err, detail_out);
        }
    }

    conn = (pos_ipc_transport_conn *)calloc(1U, sizeof(*conn));
    if (conn == NULL)
    {
        close(fd);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    conn->fd = fd;
    *conn_out = conn;
    return CPLAT_OK;
}

static int unix_linux_conn_send_all(pos_ipc_transport_conn *conn, const void *buf, size_t len, int timeout_ms,
                                    cplat_error *detail_out)
{
    const uint8_t *ptr = (const uint8_t *)buf;
    size_t sent = 0U;
    uint64_t deadline_ms = 0U;

    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    while (sent < len)
    {
        /*
         * 相手プロセス終了時に SIGPIPE で呼び出し元プロセスが落ちないよう MSG_NOSIGNAL を付与する。
         * see: https://man7.org/linux/man-pages/man2/send.2.html
         */
        ssize_t n = send(conn->fd, ptr + sent, len - sent, MSG_NOSIGNAL);
        if (n > 0)
        {
            sent += (size_t)n;
            continue;
        }
        if (n < 0)
        {
            int err = errno;
            if (err == EINTR)
            {
                continue;
            }
            if ((err == EAGAIN) || (err == EWOULDBLOCK))
            {
                int rc = wait_fd_ready(conn->fd, POLLOUT, timeout_ms, deadline_ms, detail_out);
                if (rc != CPLAT_OK)
                {
                    return rc;
                }
                continue;
            }
            if ((err == EPIPE) || (err == ECONNRESET))
            {
                cplat_error_capture_errno(detail_out, err);
                return CPLAT_ERR_EOF;
            }
            return map_errno_result(err, detail_out);
        }
        return CPLAT_ERR_EOF;
    }

    return CPLAT_OK;
}

static int unix_linux_conn_recv_all(pos_ipc_transport_conn *conn, void *buf, size_t len, int timeout_ms,
                                    cplat_error *detail_out)
{
    uint8_t *ptr = (uint8_t *)buf;
    size_t received = 0U;
    uint64_t deadline_ms = 0U;

    if (timeout_ms >= 0)
    {
        deadline_ms = now_monotonic_ms() + (uint64_t)timeout_ms;
    }

    while (received < len)
    {
        ssize_t n = recv(conn->fd, ptr + received, len - received, 0);
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
            int err = errno;
            if (err == EINTR)
            {
                continue;
            }
            if ((err == EAGAIN) || (err == EWOULDBLOCK))
            {
                int rc = wait_fd_ready(conn->fd, POLLIN, timeout_ms, deadline_ms, detail_out);
                if (rc != CPLAT_OK)
                {
                    return rc;
                }
                continue;
            }
            if (err == ECONNRESET)
            {
                cplat_error_capture_errno(detail_out, err);
                return CPLAT_ERR_EOF;
            }
            return map_errno_result(err, detail_out);
        }
    }

    return CPLAT_OK;
}

static void unix_linux_conn_close(pos_ipc_transport_conn *conn)
{
    if (conn == NULL)
    {
        return;
    }
    if (conn->fd >= 0)
    {
        close(conn->fd);
        conn->fd = -1;
    }
    free(conn);
}

static int unix_linux_listener_open(const pos_ipc_resolved_endpoint *resolved, const pos_ipc_access_policy *policy,
                                    pos_ipc_transport_listener **listener_out, cplat_error *detail_out)
{
    struct sockaddr_un addr;
    socklen_t addr_len = 0;
    pos_ipc_transport_listener *listener = NULL;
    int fd = -1;
    int rc;

    *listener_out = NULL;
    if ((policy != NULL) && (policy->mode != POS_IPC_ACCESS_CURRENT_USER))
    {
        return CPLAT_ERR_UNSUPPORTED;
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

    fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0)
    {
        return map_errno_result(errno, detail_out);
    }

    if (bind(fd, (const struct sockaddr *)&addr, addr_len) < 0)
    {
        int err = errno;
        close(fd);
        return map_errno_result(err, detail_out);
    }

    /*
     * 同一ユーザー空間のソケット ファイルは所有者のみアクセス可能 (0600) に制限する。
     * see: https://man7.org/linux/man-pages/man7/unix.7.html
     */
    if (chmod(resolved->address, (mode_t)0600) < 0)
    {
        int err = errno;
        close(fd);
        (void)unlink(resolved->address);
        return map_errno_result(err, detail_out);
    }

    if (listen(fd, SOMAXCONN) < 0)
    {
        int err = errno;
        close(fd);
        (void)unlink(resolved->address);
        return map_errno_result(err, detail_out);
    }

    listener = (pos_ipc_transport_listener *)calloc(1U, sizeof(*listener));
    if (listener == NULL)
    {
        close(fd);
        (void)unlink(resolved->address);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    listener->fd = fd;
    strncpy(listener->address, resolved->address, sizeof(listener->address) - 1U);
    listener->address[sizeof(listener->address) - 1U] = '\0';

    *listener_out = listener;
    return CPLAT_OK;
}

static void unix_linux_listener_close(pos_ipc_transport_listener *listener)
{
    if (listener == NULL)
    {
        return;
    }
    if (listener->fd >= 0)
    {
        close(listener->fd);
        listener->fd = -1;
    }
    if (listener->address[0] != '\0')
    {
        (void)unlink(listener->address);
        listener->address[0] = '\0';
    }
    free(listener);
}

static int unix_linux_reactor_create(pos_ipc_transport_listener *listener, uint32_t max_connections,
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
    reactor->wakeup_fd = eventfd(0U, EFD_NONBLOCK | EFD_CLOEXEC);
    if (reactor->wakeup_fd < 0)
    {
        int err = errno;
        free(reactor);
        return map_errno_result(err, detail_out);
    }

    rc = cplat_local_lock_create(&reactor->lock);
    if (rc != CPLAT_OK)
    {
        close(reactor->wakeup_fd);
        free(reactor);
        return rc;
    }

    reactor->entries = (pos_ipc_linux_entry *)calloc((size_t)max_connections, sizeof(*reactor->entries));
    reactor->poll_fds = (struct pollfd *)calloc((size_t)max_connections + 2U, sizeof(*reactor->poll_fds));
    reactor->poll_map = (pos_ipc_linux_entry **)calloc((size_t)max_connections + 2U, sizeof(*reactor->poll_map));
    if ((reactor->entries == NULL) || (reactor->poll_fds == NULL) || (reactor->poll_map == NULL))
    {
        unix_linux_reactor_destroy(reactor);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    *reactor_out = reactor;
    return CPLAT_OK;
}

static int unix_linux_reactor_add_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn,
                                       void *user_data, cplat_error *detail_out)
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

static void unix_linux_reactor_remove_conn(pos_ipc_transport_reactor *reactor, pos_ipc_transport_conn *conn)
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

static int unix_linux_reactor_wait(pos_ipc_transport_reactor *reactor, int timeout_ms,
                                   pos_ipc_reactor_event *events_out, size_t max_events, size_t *event_count_out,
                                   cplat_error *detail_out)
{
    nfds_t nfds = 0U;
    uint32_t i;
    size_t count = 0U;
    int ready;

    *event_count_out = 0U;

    reactor->poll_fds[nfds].fd = reactor->wakeup_fd;
    reactor->poll_fds[nfds].events = POLLIN;
    reactor->poll_fds[nfds].revents = 0;
    reactor->poll_map[nfds] = NULL;
    nfds++;

    reactor->poll_fds[nfds].fd = reactor->listener->fd;
    reactor->poll_fds[nfds].events = POLLIN;
    reactor->poll_fds[nfds].revents = 0;
    reactor->poll_map[nfds] = NULL;
    nfds++;

    cplat_local_lock_lock(reactor->lock);
    for (i = 0U; i < reactor->max_connections; ++i)
    {
        if (reactor->entries[i].active != 0)
        {
            reactor->poll_fds[nfds].fd = reactor->entries[i].conn->fd;
            reactor->poll_fds[nfds].events = POLLIN;
            reactor->poll_fds[nfds].revents = 0;
            reactor->poll_map[nfds] = &reactor->entries[i];
            nfds++;
        }
    }
    cplat_local_lock_unlock(reactor->lock);

    ready = poll(reactor->poll_fds, nfds, timeout_ms);
    if (ready < 0)
    {
        int err = errno;
        if (err == EINTR)
        {
            return CPLAT_OK;
        }
        return map_errno_result(err, detail_out);
    }
    if (ready == 0)
    {
        return CPLAT_OK;
    }

    if ((reactor->poll_fds[0].revents & POLLIN) != 0)
    {
        uint64_t val = 0U;
        while (read(reactor->wakeup_fd, &val, sizeof(val)) > 0)
        {
        }
    }

    if ((reactor->poll_fds[1].revents & POLLIN) != 0)
    {
        while (count < max_events)
        {
            int client_fd = accept(reactor->listener->fd, NULL, NULL);
            if (client_fd < 0)
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR))
                {
                    break;
                }
                break;
            }
            {
                int fl = fcntl(client_fd, F_GETFL, 0);
                if (fl >= 0)
                {
                    (void)fcntl(client_fd, F_SETFL, fl | O_NONBLOCK);
                }
                (void)fcntl(client_fd, F_SETFD, FD_CLOEXEC);
            }
            {
                pos_ipc_transport_conn *accepted = (pos_ipc_transport_conn *)calloc(1U, sizeof(*accepted));
                if (accepted == NULL)
                {
                    close(client_fd);
                    break;
                }
                accepted->fd = client_fd;
                memset(&events_out[count], 0, sizeof(events_out[count]));
                events_out[count].kind = POS_IPC_REACTOR_EVENT_ACCEPT;
                events_out[count].accepted_conn = accepted;
                count++;
            }
        }
    }

    for (nfds_t idx = 2U; (idx < nfds) && (count < max_events); ++idx)
    {
        short revents = reactor->poll_fds[idx].revents;
        pos_ipc_linux_entry *entry = reactor->poll_map[idx];
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
        if (((revents & (POLLERR | POLLNVAL)) != 0) || (((revents & POLLHUP) != 0) && ((revents & POLLIN) == 0)))
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

static void unix_linux_reactor_wakeup(pos_ipc_transport_reactor *reactor)
{
    uint64_t one = 1U;
    if ((reactor != NULL) && (reactor->wakeup_fd >= 0))
    {
        ssize_t ignored = write(reactor->wakeup_fd, &one, sizeof(one));
        (void)ignored;
    }
}

static void unix_linux_reactor_destroy(pos_ipc_transport_reactor *reactor)
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
    if (reactor->wakeup_fd >= 0)
    {
        close(reactor->wakeup_fd);
    }
    free(reactor);
}

static int build_sockaddr_un(const char *path, struct sockaddr_un *addr_out, socklen_t *len_out)
{
    size_t path_len = strlen(path);
    if ((path_len == 0U) || (path_len >= sizeof(addr_out->sun_path)))
    {
        return CPLAT_ERR_LIMIT_EXCEEDED;
    }

    memset(addr_out, 0, sizeof(*addr_out));
    addr_out->sun_family = AF_UNIX;
    memcpy(addr_out->sun_path, path, path_len + 1U);
    *len_out = (socklen_t)(offsetof(struct sockaddr_un, sun_path) + path_len + 1U);
    return CPLAT_OK;
}

static int probe_existing_socket(const char *path, cplat_error *detail_out)
{
    struct stat st;
    struct sockaddr_un addr;
    socklen_t addr_len = 0;
    int probe_fd;
    int rc;

    if (lstat(path, &st) < 0)
    {
        if (errno == ENOENT)
        {
            return CPLAT_OK;
        }
        return map_errno_result(errno, detail_out);
    }

    if (!S_ISSOCK(st.st_mode))
    {
        return CPLAT_ERR_BUSY;
    }

    rc = build_sockaddr_un(path, &addr, &addr_len);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    probe_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (probe_fd < 0)
    {
        return map_errno_result(errno, detail_out);
    }

    if (connect(probe_fd, (const struct sockaddr *)&addr, addr_len) == 0)
    {
        close(probe_fd);
        return CPLAT_ERR_BUSY;
    }

    {
        int err = errno;
        close(probe_fd);
        if ((err == EINPROGRESS) || (err == EAGAIN) || (err == EWOULDBLOCK))
        {
            return CPLAT_ERR_BUSY;
        }
        if (err == ECONNREFUSED)
        {
            if (unlink(path) < 0)
            {
                if (errno != ENOENT)
                {
                    return map_errno_result(errno, detail_out);
                }
            }
            return CPLAT_OK;
        }
        if (err == ENOENT)
        {
            return CPLAT_OK;
        }
        return map_errno_result(err, detail_out);
    }
}

static int wait_fd_ready(int fd, short events, int timeout_ms, uint64_t deadline_ms, cplat_error *detail_out)
{
    for (;;)
    {
        struct pollfd pfd;
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

        pfd.fd = fd;
        pfd.events = events;
        pfd.revents = 0;

        rc = poll(&pfd, 1U, wait_ms);
        if (rc > 0)
        {
            return CPLAT_OK;
        }
        if (rc == 0)
        {
            return CPLAT_ERR_TIMEOUT;
        }
        if (errno == EINTR)
        {
            continue;
        }
        return map_errno_result(errno, detail_out);
    }
}

static int map_errno_result(int err, cplat_error *detail_out)
{
    cplat_error_capture_errno(detail_out, err);
    if ((err == ENOENT) || (err == ECONNREFUSED))
    {
        return CPLAT_ERR_NOT_FOUND;
    }
    if ((err == EACCES) || (err == EPERM))
    {
        return CPLAT_ERR_PERMISSION_DENIED;
    }
    if (err == ETIMEDOUT)
    {
        return CPLAT_ERR_TIMEOUT;
    }
    if ((err == EADDRINUSE) || (err == EBUSY))
    {
        return CPLAT_ERR_BUSY;
    }
    if (err == ENOMEM)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
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

typedef int pos_ipc_transport_unix_linux_unused_t;

#endif /* PLATFORM_LINUX */
