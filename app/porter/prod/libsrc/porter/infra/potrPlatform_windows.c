/**
 *******************************************************************************
 *  @file           potrPlatform_windows.c
 *  @brief          Windows 向けプラットフォーム抽象レイヤー実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/18
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_WINDOWS)

    #pragma comment(lib, "ws2_32.lib")

    #include "potrPlatform.h"

/* doxygen コメントはヘッダに記載 */
void potr_get_monotonic(int64_t *tv_sec, int32_t *tv_nsec)
{
    ULONGLONG ms = GetTickCount64();
    *tv_sec  = (int64_t)(ms / 1000ULL);
    *tv_nsec = (int32_t)((ms % 1000ULL) * 1000000UL);
}

/* doxygen コメントはヘッダに記載 */
void potr_get_realtime(int64_t *tv_sec, int32_t *tv_nsec)
{
    FILETIME       ft;
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(&ft);
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    /* FILETIME は 100ns 単位、1601-01-01 起算。Unix epoch (1970-01-01) との差は 11644473600 秒。 */
    *tv_sec  = (int64_t)(uli.QuadPart / 10000000ULL) - 11644473600LL;
    *tv_nsec = (int32_t)((uli.QuadPart % 10000000ULL) * 100ULL);
}

/* doxygen コメントはヘッダに記載 */
int potr_condvar_timedwait(PotrCondVar *cv, PotrMutex *mtx, uint32_t timeout_ms)
{
    return SleepConditionVariableCS(cv, mtx, (DWORD)timeout_ms) ? 0 : -1;
}

/* doxygen コメントはヘッダに記載 */
int potr_sendto(PotrSocket sock, const uint8_t *buf, size_t len,
                int flags, const struct sockaddr *dest, int dest_len)
{
    return sendto(sock, (const char *)buf, (int)len, flags, dest, dest_len);
}

/* doxygen コメントはヘッダに記載 */
int potr_recvfrom(PotrSocket sock, uint8_t *buf, size_t len,
                  int flags, struct sockaddr *src, int *src_len)
{
    return recvfrom(sock, (char *)buf, (int)len, flags, src, src_len);
}

/* doxygen コメントはヘッダに記載 */
int potr_poll_writable(PotrSocket fd, int timeout_ms)
{
    WSAPOLLFD pfd;
    int       r;
    pfd.fd      = fd;
    pfd.events  = POLLOUT;
    pfd.revents = 0;
    r = WSAPoll(&pfd, 1, timeout_ms);
    if (r > 0 && (pfd.revents & POLLOUT)) return 1;
    if (r == 0) return 0;
    return -1;
}

/* doxygen コメントはヘッダに記載 */
int potr_thread_create(PotrThread *thread, PotrThreadFunc func, void *arg)
{
    *thread = CreateThread(NULL, 0, func, arg, 0, NULL);
    return (*thread == NULL) ? -1 : 0;
}

/* doxygen コメントはヘッダに記載 */
void potr_thread_join(PotrThread *thread)
{
    if (*thread != NULL)
    {
        WaitForSingleObject(*thread, INFINITE);
        CloseHandle(*thread);
        *thread = NULL;
    }
}

/* doxygen コメントはヘッダに記載 */
int potr_tcp_send(PotrSocket fd, const uint8_t *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len)
    {
        int n = send(fd, (const char *)(buf + sent), (int)(len - sent), 0);
        if (n <= 0) return -1;
        sent += (size_t)n;
    }
    return 0;
}

/* doxygen コメントはヘッダに記載 */
int potr_set_nonblocking(PotrSocket fd)
{
    u_long mode = 1;
    return ioctlsocket(fd, FIONBIO, &mode) == 0 ? 0 : -1;
}

/* doxygen コメントはヘッダに記載 */
int potr_set_blocking(PotrSocket fd)
{
    u_long mode = 0;
    return ioctlsocket(fd, FIONBIO, &mode) == 0 ? 0 : -1;
}

#elif defined(PLATFORM_LINUX)
    /* Linux では potrPlatform_linux.c が使用される */
    typedef int potr_platform_windows_dummy_t;
#endif /* PLATFORM_ */
