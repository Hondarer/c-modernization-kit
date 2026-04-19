/**
 *******************************************************************************
 *  @file           sync_linux.c
 *  @brief          Linux 向けスレッド・同期プリミティブ実装。
 *  @author         Tetsuo Honda
 *  @date           2026/04/20
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)

    #include <time.h>

    #include <com_util/sync/sync.h>

/* doxygen コメントはヘッダに記載 */
int com_util_condvar_timedwait(com_util_condvar_t *cv, com_util_mutex_t *mtx,
                               uint32_t timeout_ms)
{
    struct timespec abs_ts;
    clock_gettime(CLOCK_REALTIME, &abs_ts);
    abs_ts.tv_sec  += (time_t)(timeout_ms / 1000U);
    abs_ts.tv_nsec += (long)((timeout_ms % 1000U) * 1000000UL);
    if (abs_ts.tv_nsec >= 1000000000L)
    {
        abs_ts.tv_sec++;
        abs_ts.tv_nsec -= 1000000000L;
    }
    return pthread_cond_timedwait(cv, mtx, &abs_ts);
}

/* doxygen コメントはヘッダに記載 */
int com_util_thread_create(com_util_thread_t *thread,
                           com_util_thread_func_t func, void *arg)
{
    return pthread_create(thread, NULL, func, arg);
}

/* doxygen コメントはヘッダに記載 */
void com_util_thread_join(com_util_thread_t *thread)
{
    pthread_join(*thread, NULL);
}

#elif defined(PLATFORM_WINDOWS) && defined(COMPILER_MSVC)
    #pragma warning(disable : 4206)
#endif /* PLATFORM_ */
