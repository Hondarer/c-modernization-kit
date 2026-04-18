/**
 *******************************************************************************
 *  @file           potrPlatform.h
 *  @brief          プラットフォーム抽象レイヤー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/18
 *  @version        1.0.0
 *
 *  @details
 *  OS ごとに異なるスレッド・同期・ソケット・時刻 API を共通インターフェースで抽象化します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef POTR_PLATFORM_H
#define POTR_PLATFORM_H

#include <stddef.h>
#include <stdint.h>

#include <com_util/base/platform.h>

/* ============================================================
 * 型定義 (PotrSocket / PotrThread / PotrMutex / PotrCondVar)
 * ============================================================ */
#if defined(PLATFORM_LINUX)
    #include <pthread.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <poll.h>
    #include <time.h>
    typedef int                PotrSocket;
    typedef pthread_t          PotrThread;
    typedef pthread_mutex_t    PotrMutex;
    typedef pthread_cond_t     PotrCondVar;
    #define POTR_INVALID_SOCKET (-1)
#elif defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    typedef SOCKET             PotrSocket;
    typedef HANDLE             PotrThread;
    typedef CRITICAL_SECTION   PotrMutex;
    typedef CONDITION_VARIABLE PotrCondVar;
    #define POTR_INVALID_SOCKET INVALID_SOCKET
#endif /* PLATFORM_ */

/* ============================================================
 * ミューテックス / 条件変数 マクロ
 * ============================================================ */
#if defined(PLATFORM_LINUX)
    #define POTR_MUTEX_INIT(m)     pthread_mutex_init((m), NULL)
    #define POTR_MUTEX_LOCK(m)     pthread_mutex_lock(m)
    #define POTR_MUTEX_UNLOCK(m)   pthread_mutex_unlock(m)
    #define POTR_MUTEX_DESTROY(m)  pthread_mutex_destroy(m)
    #define POTR_COND_INIT(c)      pthread_cond_init((c), NULL)
    #define POTR_COND_WAIT(c, m)   pthread_cond_wait((c), (m))
    #define POTR_COND_SIGNAL(c)    pthread_cond_signal(c)
    #define POTR_COND_BROADCAST(c) pthread_cond_broadcast(c)
    #define POTR_COND_DESTROY(c)   pthread_cond_destroy(c)
#elif defined(PLATFORM_WINDOWS)
    #define POTR_MUTEX_INIT(m)     InitializeCriticalSection(m)
    #define POTR_MUTEX_LOCK(m)     EnterCriticalSection(m)
    #define POTR_MUTEX_UNLOCK(m)   LeaveCriticalSection(m)
    #define POTR_MUTEX_DESTROY(m)  DeleteCriticalSection(m)
    #define POTR_COND_INIT(c)      InitializeConditionVariable(c)
    #define POTR_COND_WAIT(c, m)   SleepConditionVariableCS((c), (m), INFINITE)
    #define POTR_COND_SIGNAL(c)    WakeConditionVariable(c)
    #define POTR_COND_BROADCAST(c) WakeAllConditionVariable(c)
    #define POTR_COND_DESTROY(c)   ((void)0) /* Windows は破棄不要 */
#endif /* PLATFORM_ */

/* ============================================================
 * スレッド関数マクロ / PotrThreadFunc 型
 * ============================================================ */
#if defined(PLATFORM_LINUX)
    /** スレッド関数定義マクロ (static void *name(void *arg) に展開)。 */
    #define POTR_THREAD_FUNC(name) static void *name(void *arg)
    /** スレッド関数末尾の return 文マクロ。 */
    #define POTR_THREAD_RETURN     return NULL
    /** スレッド関数ポインタ型。 */
    typedef void *(*PotrThreadFunc)(void *);
#elif defined(PLATFORM_WINDOWS)
    /** スレッド関数定義マクロ (static DWORD WINAPI name(LPVOID arg) に展開)。 */
    #define POTR_THREAD_FUNC(name) static DWORD WINAPI name(LPVOID arg)
    /** スレッド関数末尾の return 文マクロ。 */
    #define POTR_THREAD_RETURN     return 0
    /** スレッド関数ポインタ型。 */
    typedef DWORD (WINAPI *PotrThreadFunc)(LPVOID);
#endif /* PLATFORM_ */

/* ============================================================
 * static inline 関数
 * ============================================================ */

/**
 *  @brief  単調増加クロックの現在時刻をミリ秒単位で返す。
 *          タイムアウト判定や差分計算など ms 精度で十分な用途に使用する。
 *  @return ミリ秒値 (起動時からの経過)。
 */
static inline uint64_t potr_get_monotonic_ms(void)
{
#if defined(PLATFORM_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
#elif defined(PLATFORM_WINDOWS)
    return (uint64_t)GetTickCount64();
#endif /* PLATFORM_ */
}

/**
 *  @brief  ソケットを閉じる。
 *  @param[in]  fd  閉じるソケット。
 */
static inline void potr_close_socket(PotrSocket fd)
{
#if defined(PLATFORM_LINUX)
    close(fd);
#elif defined(PLATFORM_WINDOWS)
    closesocket(fd);
#endif /* PLATFORM_ */
}

/**
 *  @brief  ソケットの送受信を停止する (shutdown)。
 *  @param[in]  fd  停止するソケット。
 */
static inline void potr_shutdown_socket(PotrSocket fd)
{
#if defined(PLATFORM_LINUX)
    shutdown(fd, SHUT_RDWR);
#elif defined(PLATFORM_WINDOWS)
    shutdown(fd, SD_BOTH);
#endif /* PLATFORM_ */
}

/* ============================================================
 * extern 関数宣言 (.c で実装)
 * ============================================================ */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *  @brief  単調増加クロックの現在時刻を秒・ナノ秒で返す。
 *          高精度な経過時間測定や受信タイムスタンプの記録に使用する。
 *  @param[out]  tv_sec   秒部。
 *  @param[out]  tv_nsec  ナノ秒部。
 */
extern void potr_get_monotonic(int64_t *tv_sec, int32_t *tv_nsec);

/**
 *  @brief  現在時刻を秒・ナノ秒で返す。
 *          セッション開始時刻など実時刻の刻印が必要な用途に使用する。
 *  @param[out]  tv_sec   秒部。
 *  @param[out]  tv_nsec  ナノ秒部。
 */
extern void potr_get_realtime(int64_t *tv_sec, int32_t *tv_nsec);

/**
 *  @brief  タイムアウト付き条件変数待機。
 *          呼び出し前にミューテックスを取得しておく必要がある。
 *  @param[in]  cv         条件変数。
 *  @param[in]  mtx        保護ミューテックス (取得済みであること)。
 *  @param[in]  timeout_ms タイムアウト (ミリ秒)。
 *  @return     0 (シグナル受信またはタイムアウト)。
 */
extern int potr_condvar_timedwait(PotrCondVar *cv, PotrMutex *mtx,
                                  uint32_t timeout_ms);

/**
 *  @brief  UDP データグラムを送信する。
 *  @param[in]  sock      送信ソケット。
 *  @param[in]  buf       送信データ。
 *  @param[in]  len       送信バイト数。
 *  @param[in]  flags     送信フラグ。
 *  @param[in]  dest      送信先アドレス。
 *  @param[in]  dest_len  送信先アドレス長。
 *  @return 送信バイト数 (負値: エラー)。
 */
extern int potr_sendto(PotrSocket sock, const uint8_t *buf, size_t len,
                       int flags,
                       const struct sockaddr *dest, int dest_len);

/**
 *  @brief  UDP データグラムを受信する。
 *  @param[in]  sock     受信ソケット。
 *  @param[out] buf      受信バッファ。
 *  @param[in]  len      バッファサイズ。
 *  @param[in]  flags    受信フラグ。
 *  @param[out] src      送信元アドレス格納先。
 *  @param[in,out] src_len  送信元アドレス長。
 *  @return 受信バイト数 (負値: エラー)。
 */
extern int potr_recvfrom(PotrSocket sock, uint8_t *buf, size_t len,
                         int flags,
                         struct sockaddr *src, int *src_len);

/**
 *  @brief  ソケットが書き込み可能か確認する (poll/WSAPoll)。
 *  @param[in]  fd         対象ソケット。
 *  @param[in]  timeout_ms タイムアウト (ミリ秒、0 = 即時)。
 *  @return  1: 書き込み可能、0: タイムアウト、-1: エラー。
 */
extern int potr_poll_writable(PotrSocket fd, int timeout_ms);

/**
 *  @brief  スレッドを生成する。
 *  @param[out]  thread  生成したスレッドハンドルの格納先。
 *  @param[in]   func    スレッド関数。
 *  @param[in]   arg     スレッド関数に渡す引数。
 *  @return  0: 成功、非 0: 失敗。
 */
extern int potr_thread_create(PotrThread *thread, PotrThreadFunc func, void *arg);

/**
 *  @brief  スレッドの終了を待機し、ハンドルを解放する。
 *  @param[in,out]  thread  スレッドハンドルへのポインタ。
 *                          Windows では解放後に NULL を書き込む。
 */
extern void potr_thread_join(PotrThread *thread);

/**
 *  @brief  TCP ソケットへ全バイトを確実に送信する。
 *  @param[in]  fd   送信ソケット。
 *  @param[in]  buf  送信データ。
 *  @param[in]  len  送信バイト数。
 *  @return  0: 成功 (全バイト送信)、-1: 切断またはエラー。
 *  @note    呼び出し前に送信ミューテックスを取得しておく必要がある。
 */
extern int potr_tcp_send(PotrSocket fd, const uint8_t *buf, size_t len);

/**
 *  @brief  ソケットをノンブロッキングモードに設定する。
 *  @param[in]  fd  対象ソケット。
 *  @return  0: 成功、-1: 失敗。
 */
extern int potr_set_nonblocking(PotrSocket fd);

/**
 *  @brief  ソケットをブロッキングモードに戻す。
 *  @param[in]  fd  対象ソケット。
 *  @return  0: 成功、-1: 失敗。
 */
extern int potr_set_blocking(PotrSocket fd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POTR_PLATFORM_H */
