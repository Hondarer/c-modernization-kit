/**
 *******************************************************************************
 *  @file           funcman.c
 *  @brief          拡張可能な関数の動的ロードを行い、関数アドレスを返却します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  funcman_get_func はスレッドセーフです。
 *  内部で mutex (Linux) または SRW ロック (Windows) を使用して排他制御します。
 * 
 *  @todo           一度探索に失敗した場合は、次回以降探索を省略するために
 *                  探索済みフラグを設ける必要がある。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <libbase.h>
#include <string.h>

/* doxygen コメントは、ヘッダに記載 */
void *_funcman_get_func(funcman_object *fobj)
{
#ifndef _WIN32
    const char *ext = ".so";
#else  /* _WIN32 */
    const char *ext = ".dll";
#endif /* _WIN32 */
    char lib_with_ext[FUNCMAN_NAME_MAX];

    /* ロード完了後は func_ptr が NULL 以外になる。
     * 初回ロード後の読み取りのみになるため早期リターンで判定する。 */
    if (fobj->func_ptr != NULL)
    {
        return fobj->func_ptr;
    }

    /* ロード処理を排他制御する。
     * ロック取得後に再度 func_ptr を確認し、他スレッドが先にロードを
     * 完了していた場合は処理をスキップする (double-checked locking)。 */
#ifndef _WIN32
    if (pthread_mutex_lock(&fobj->mutex) != 0)
    {
        return NULL;
    }
#else  /* _WIN32 */
    AcquireSRWLockExclusive(&fobj->lock);
#endif /* _WIN32 */

    if (fobj->func_ptr == NULL)
    {
        if (fobj->lib_name[0] == '\0' || fobj->func_name[0] == '\0')
        {
            goto unlock;
        }
        if (strlen(fobj->lib_name) + strlen(ext) >= FUNCMAN_NAME_MAX)
        {
            goto unlock;
        }
        strcpy(lib_with_ext, fobj->lib_name);
        strcat(lib_with_ext, ext);

#ifndef _WIN32
        fobj->handle = dlopen(lib_with_ext, RTLD_LAZY);
#else  /* _WIN32 */
        fobj->handle = LoadLibrary(lib_with_ext);
#endif /* _WIN32 */
        if (fobj->handle == NULL)
        {
            goto unlock;
        }

#ifndef _WIN32
        fobj->func_ptr = dlsym(fobj->handle, fobj->func_name);
#else  /* _WIN32 */
        fobj->func_ptr = (void *)GetProcAddress(fobj->handle, fobj->func_name);
#endif /* _WIN32 */
        if (fobj->func_ptr == NULL)
        {
#ifndef _WIN32
            dlclose(fobj->handle);
#else  /* _WIN32 */
            FreeLibrary(fobj->handle);
#endif /* _WIN32 */
            fobj->handle = NULL;
        }
    }

unlock:
#ifndef _WIN32
    pthread_mutex_unlock(&fobj->mutex);
#else  /* _WIN32 */
    ReleaseSRWLockExclusive(&fobj->lock);
#endif /* _WIN32 */

    return fobj->func_ptr;
}
