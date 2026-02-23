/**
 *******************************************************************************
 *  @file           func_manager.c
 *  @brief          拡張可能な関数の動的ロードを行い、関数アドレスを返却します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  func_manager_get_func はスレッドセーフです。
 *  内部で mutex (Linux) または SRW ロック (Windows) を使用して排他制御します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <func_manager.h>
#include "func_manager_local.h"
#include <string.h>

/** ライブラリ名バッファの最大長 (ベース名 + 拡張子を含む) */
#define LIBBASE_MAX_LIB_NAME 256

/* doxygen コメントは、ヘッダに記載 */
void *_func_manager_get_func(func_object *cache)
{
#ifndef _WIN32
    const char *ext = ".so";
#else  /* _WIN32 */
    const char *ext = ".dll";
#endif /* _WIN32 */
    char lib_with_ext[LIBBASE_MAX_LIB_NAME];

    /* ロード完了後は func_ptr が NULL 以外になる。
     * 初回ロード後の読み取りのみになるため早期リターンで判定する。 */
    if (cache->func_ptr != NULL)
    {
        return cache->func_ptr;
    }

    /* ロード処理を排他制御する。
     * ロック取得後に再度 func_ptr を確認し、他スレッドが先にロードを
     * 完了していた場合は処理をスキップする (double-checked locking)。 */
#ifndef _WIN32
    if (pthread_mutex_lock(&cache->mutex) != 0)
    {
        return NULL;
    }
#else  /* _WIN32 */
    AcquireSRWLockExclusive(&cache->lock);
#endif /* _WIN32 */

    if (cache->func_ptr == NULL)
    {
        if (cache->lib_name[0] == '\0' || cache->func_name[0] == '\0')
        {
            goto unlock;
        }
        if (strlen(cache->lib_name) + strlen(ext) >= LIBBASE_MAX_LIB_NAME)
        {
            goto unlock;
        }
        strcpy(lib_with_ext, cache->lib_name);
        strcat(lib_with_ext, ext);

#ifndef _WIN32
        cache->handle = dlopen(lib_with_ext, RTLD_LAZY);
#else  /* _WIN32 */
        cache->handle = LoadLibrary(lib_with_ext);
#endif /* _WIN32 */
        if (cache->handle == NULL)
        {
            goto unlock;
        }

#ifndef _WIN32
        cache->func_ptr = dlsym(cache->handle, cache->func_name);
#else  /* _WIN32 */
        cache->func_ptr = (void *)GetProcAddress(cache->handle, cache->func_name);
#endif /* _WIN32 */
        if (cache->func_ptr == NULL)
        {
#ifndef _WIN32
            dlclose(cache->handle);
#else  /* _WIN32 */
            FreeLibrary(cache->handle);
#endif /* _WIN32 */
            cache->handle = NULL;
        }
    }

unlock:
#ifndef _WIN32
    pthread_mutex_unlock(&cache->mutex);
#else  /* _WIN32 */
    ReleaseSRWLockExclusive(&cache->lock);
#endif /* _WIN32 */

    return cache->func_ptr;
}
