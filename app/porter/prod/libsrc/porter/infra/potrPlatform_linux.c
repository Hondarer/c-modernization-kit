/**
 *******************************************************************************
 *  @file           potrPlatform_linux.c
 *  @brief          Linux 向けプラットフォーム抽象レイヤー実装。
 *  @author         Tetsuo Honda
 *  @date           2026/04/18
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)

    #include <errno.h>
    #include <fcntl.h>

    #include "potrPlatform.h"

/* doxygen コメントはヘッダに記載 */
int potr_condvar_timedwait(PotrCondVar *cv, PotrMutex *mtx, uint32_t timeout_ms)
{
    struct timespec abs_ts;
    clock_gettime(CLOCK_REALTIME, &abs_ts);
    abs_ts.tv_sec += (time_t)(timeout_ms / 1000U);
    abs_ts.tv_nsec += (long)((timeout_ms % 1000U) * 1000000UL);
    if (abs_ts.tv_nsec >= 1000000000L)
    {
        abs_ts.tv_sec++;
        abs_ts.tv_nsec -= 1000000000L;
    }
    return pthread_cond_timedwait(cv, mtx, &abs_ts);
}

/* doxygen コメントはヘッダに記載 */
int potr_sendto(PotrSocket sock, const uint8_t *buf, size_t len, int flags, const struct sockaddr *dest, int dest_len)
{
    return (int)sendto(sock, buf, len, flags, dest, (socklen_t)dest_len);
}

/* doxygen コメントはヘッダに記載 */
int potr_recvfrom(PotrSocket sock, uint8_t *buf, size_t len, int flags, struct sockaddr *src, int *src_len)
{
    socklen_t sl = (socklen_t)*src_len;
    int n = (int)recvfrom(sock, buf, len, flags, src, &sl);
    *src_len = (int)sl;
    return n;
}

/* doxygen コメントはヘッダに記載 */
int potr_poll_writable(PotrSocket fd, int timeout_ms)
{
    struct pollfd pfd;
    int r;
    pfd.fd = fd;
    pfd.events = POLLOUT;
    pfd.revents = 0;
    r = poll(&pfd, 1, timeout_ms);
    if (r > 0 && (pfd.revents & POLLOUT))
        return 1;
    if (r == 0)
        return 0;
    return -1;
}

/* doxygen コメントはヘッダに記載 */
int potr_poll_readable(PotrSocket fd, int timeout_ms)
{
    struct pollfd pfd;
    int r;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    r = poll(&pfd, 1, timeout_ms);
    if (r > 0 && (pfd.revents & POLLIN))
        return 1;
    if (r == 0 || (r < 0 && errno == EINTR))
        return 0;
    return -1;
}

/* doxygen コメントはヘッダに記載 */
int potr_thread_create(PotrThread *thread, PotrThreadFunc func, void *arg)
{
    return pthread_create(thread, NULL, func, arg);
}

/* doxygen コメントはヘッダに記載 */
void potr_thread_join(PotrThread *thread)
{
    pthread_join(*thread, NULL);
}

/* doxygen コメントはヘッダに記載 */
int potr_tcp_send(PotrSocket fd, const uint8_t *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len)
    {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n <= 0)
            return -1;
        sent += (size_t)n;
    }
    return 0;
}

/* doxygen コメントはヘッダに記載 */
int potr_tcp_recv_all(PotrSocket fd, uint8_t *buf, size_t n)
{
    size_t received = 0;
    while (received < n)
    {
        ssize_t r = recv(fd, buf + received, n - received, 0);
        if (r < 0)
            return -1;
        if (r == 0)
            return 0;
        received += (size_t)r;
    }
    return 1;
}

/* doxygen コメントはヘッダに記載 */
int potr_socket_lib_init(void)
{
    return 0;
}
void potr_socket_lib_cleanup(void) {}

/* doxygen コメントはヘッダに記載 */
int potr_set_nonblocking(PotrSocket fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0 ? -1 : 0;
}

/* doxygen コメントはヘッダに記載 */
int potr_set_blocking(PotrSocket fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return -1;
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) < 0 ? -1 : 0;
}

#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)
    #pragma warning(disable : 4206)
#endif /* PLATFORM_ */
