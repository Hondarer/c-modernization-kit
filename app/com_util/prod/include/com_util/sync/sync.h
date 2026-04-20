/**
 *******************************************************************************
 *  @file           sync.h
 *  @brief          スレッド・同期プリミティブ抽象レイヤーのヘッダー。
 *  @author         Tetsuo Honda
 *  @date           2026/04/20
 *  @version        1.0.0
 *
 *  @details
 *  OS ごとに異なるミューテックス・条件変数・スレッド API を共通インターフェースで
 *  抽象化します。\n
 *
 *  | OS      | ミューテックス           | 条件変数             | スレッド       |
 *  | ------- | ------------------------ | -------------------- | -------------- |
 *  | Linux   | pthread_mutex_t          | pthread_cond_t       | pthread_t      |
 *  | Windows | CRITICAL_SECTION         | CONDITION_VARIABLE   | HANDLE         |
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef COM_UTIL_SYNC_H
#define COM_UTIL_SYNC_H

#include <stdint.h>

#include <com_util/base/platform.h>
#include <com_util_export.h>

/* ============================================================
 * 型定義
 * ============================================================ */
#if defined(PLATFORM_LINUX)
    #include <pthread.h>
    /** ミューテックス型。 */
    typedef pthread_mutex_t com_util_mutex_t;
    /** 条件変数型。 */
    typedef pthread_cond_t  com_util_condvar_t;
    /** スレッドハンドル型。 */
    typedef pthread_t       com_util_thread_t;
    /** スレッド関数ポインタ型。 */
    typedef void *(*com_util_thread_func_t)(void *);
#elif defined(PLATFORM_WINDOWS)
    #include <com_util/base/windows_sdk.h>
    /** ミューテックス型。 */
    typedef CRITICAL_SECTION   com_util_mutex_t;
    /** 条件変数型。 */
    typedef CONDITION_VARIABLE com_util_condvar_t;
    /** スレッドハンドル型。 */
    typedef HANDLE             com_util_thread_t;
    /** スレッド関数ポインタ型。 */
    typedef DWORD (WINAPI *com_util_thread_func_t)(LPVOID);
#endif /* PLATFORM_ */

/* ============================================================
 * ミューテックス マクロ
 * ============================================================ */
#if defined(PLATFORM_LINUX)
    /** ミューテックスを初期化する。 */
    #define COM_UTIL_MUTEX_INIT(m)     pthread_mutex_init((m), NULL)
    /** ミューテックスをロックする。 */
    #define COM_UTIL_MUTEX_LOCK(m)     pthread_mutex_lock(m)
    /** ミューテックスをアンロックする。 */
    #define COM_UTIL_MUTEX_UNLOCK(m)   pthread_mutex_unlock(m)
    /** ミューテックスを破棄する。 */
    #define COM_UTIL_MUTEX_DESTROY(m)  pthread_mutex_destroy(m)
#elif defined(PLATFORM_WINDOWS)
    #define COM_UTIL_MUTEX_INIT(m)     InitializeCriticalSection(m)
    #define COM_UTIL_MUTEX_LOCK(m)     EnterCriticalSection(m)
    #define COM_UTIL_MUTEX_UNLOCK(m)   LeaveCriticalSection(m)
    #define COM_UTIL_MUTEX_DESTROY(m)  DeleteCriticalSection(m)
#endif /* PLATFORM_ */

/* ============================================================
 * 条件変数 マクロ
 * ============================================================ */
#if defined(PLATFORM_LINUX)
    /** 条件変数を初期化する。 */
    #define COM_UTIL_COND_INIT(c)      pthread_cond_init((c), NULL)
    /** 条件変数を待機する。呼び出し前にミューテックスを取得しておく必要がある。 */
    #define COM_UTIL_COND_WAIT(c, m)   pthread_cond_wait((c), (m))
    /** 条件変数に1スレッドへシグナルを送る。 */
    #define COM_UTIL_COND_SIGNAL(c)    pthread_cond_signal(c)
    /** 条件変数に全スレッドへシグナルを送る。 */
    #define COM_UTIL_COND_BROADCAST(c) pthread_cond_broadcast(c)
    /** 条件変数を破棄する。 */
    #define COM_UTIL_COND_DESTROY(c)   pthread_cond_destroy(c)
#elif defined(PLATFORM_WINDOWS)
    #define COM_UTIL_COND_INIT(c)      InitializeConditionVariable(c)
    #define COM_UTIL_COND_WAIT(c, m)   SleepConditionVariableCS((c), (m), INFINITE)
    #define COM_UTIL_COND_SIGNAL(c)    WakeConditionVariable(c)
    #define COM_UTIL_COND_BROADCAST(c) WakeAllConditionVariable(c)
    #define COM_UTIL_COND_DESTROY(c)   ((void)0) /* Windows は破棄不要 */
#endif /* PLATFORM_ */

/* ============================================================
 * スレッド関数マクロ
 * ============================================================ */
#if defined(PLATFORM_LINUX)
    /** スレッド関数定義マクロ (static void *name(void *arg) に展開)。 */
    #define COM_UTIL_THREAD_FUNC(name) static void *name(void *arg)
    /** スレッド関数末尾の return 文マクロ。 */
    #define COM_UTIL_THREAD_RETURN     return NULL
#elif defined(PLATFORM_WINDOWS)
    /** スレッド関数定義マクロ (static DWORD WINAPI name(LPVOID arg) に展開)。 */
    #define COM_UTIL_THREAD_FUNC(name) static DWORD WINAPI name(LPVOID arg)
    /** スレッド関数末尾の return 文マクロ。 */
    #define COM_UTIL_THREAD_RETURN     return 0
#endif /* PLATFORM_ */

/* ============================================================
 * extern 関数宣言 (.c で実装)
 * ============================================================ */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *  @brief  タイムアウト付き条件変数待機。
 *          呼び出し前にミューテックスを取得しておく必要がある。
 *  @param[in]  cv         条件変数。
 *  @param[in]  mtx        保護ミューテックス (取得済みであること)。
 *  @param[in]  timeout_ms タイムアウト (ミリ秒)。
 *  @return     0 (シグナル受信またはタイムアウト)。
 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_condvar_timedwait(com_util_condvar_t *cv,
                                                            com_util_mutex_t   *mtx,
                                                            uint32_t            timeout_ms);

/**
 *  @brief  スレッドを生成する。
 *  @param[out]  thread  生成したスレッドハンドルの格納先。
 *  @param[in]   func    スレッド関数。
 *  @param[in]   arg     スレッド関数に渡す引数。
 *  @return  0: 成功、非 0: 失敗。
 */
COM_UTIL_EXPORT int COM_UTIL_API com_util_thread_create(com_util_thread_t     *thread,
                                                        com_util_thread_func_t func,
                                                        void                  *arg);

/**
 *  @brief  スレッドの終了を待機し、ハンドルを解放する。
 *  @param[in,out]  thread  スレッドハンドルへのポインタ。
 *                          Windows では解放後に NULL を書き込む。
 */
COM_UTIL_EXPORT void COM_UTIL_API com_util_thread_join(com_util_thread_t *thread);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COM_UTIL_SYNC_H */
