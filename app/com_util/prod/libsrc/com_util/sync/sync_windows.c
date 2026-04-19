/**
 *******************************************************************************
 *  @file           sync_windows.c
 *  @brief          Windows 向けスレッド・同期プリミティブ実装。
 *  @author         Tetsuo Honda
 *  @date           2026/04/20
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/base/platform.h>

#if defined(PLATFORM_WINDOWS)

    #include <com_util/sync/sync.h>

/* doxygen コメントはヘッダに記載 */
int com_util_condvar_timedwait(com_util_condvar_t *cv, com_util_mutex_t *mtx,
                               uint32_t timeout_ms)
{
    return SleepConditionVariableCS(cv, mtx, (DWORD)timeout_ms) ? 0 : -1;
}

/* doxygen コメントはヘッダに記載 */
int com_util_thread_create(com_util_thread_t *thread,
                           com_util_thread_func_t func, void *arg)
{
    *thread = CreateThread(NULL, 0, func, arg, 0, NULL);
    return (*thread == NULL) ? -1 : 0;
}

/* doxygen コメントはヘッダに記載 */
void com_util_thread_join(com_util_thread_t *thread)
{
    if (*thread != NULL)
    {
        WaitForSingleObject(*thread, INFINITE);
        CloseHandle(*thread);
        *thread = NULL;
    }
}

#endif /* PLATFORM_WINDOWS */
