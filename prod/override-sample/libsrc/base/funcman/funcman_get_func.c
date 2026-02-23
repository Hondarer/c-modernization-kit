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

    /* ロード完了後は resolved が 0 以外になる。早期リターンで判定する。
     * もし解決に失敗している場合は、NULL が返却される。 */
    if (fobj->resolved != 0)
    {
        return fobj->func_ptr;
    }

    /* ロード処理を排他制御する。
     * ロック取得後に再度 resolved を確認し、他スレッドが先にロードを
     * 完了していた場合は処理をスキップする (double-checked locking)。 */
#ifndef _WIN32
    if (pthread_mutex_lock(&fobj->mutex) != 0)
    {
        return NULL;
    }
#else  /* _WIN32 */
    AcquireSRWLockExclusive(&fobj->lock);
#endif /* _WIN32 */

    if (fobj->resolved == 0)
    {
        if (fobj->lib_name[0] == '\0' || fobj->func_name[0] == '\0')
        {
            fobj->resolved = -1; /* resolved=-1: 定義なし */
            goto unlock;
        }
        if (strlen(fobj->lib_name) + strlen(ext) >= FUNCMAN_NAME_MAX)
        {
            fobj->resolved = -2; /* resolved=-2: 名称長さオーバー */
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
            fobj->resolved = -3; /* resolved=-3: ライブラリオープンエラー */
            goto unlock;
        }

#ifndef _WIN32
        fobj->func_ptr = dlsym(fobj->handle, fobj->func_name);
#else  /* _WIN32 */
        fobj->func_ptr = (void *)GetProcAddress(fobj->handle, fobj->func_name);
#endif /* _WIN32 */
        if (fobj->func_ptr == NULL)
        {
            fobj->resolved = -4; /* resolved=-4: 関数探索エラー */
#ifndef _WIN32
            dlclose(fobj->handle);
#else  /* _WIN32 */
            FreeLibrary(fobj->handle);
#endif /* _WIN32 */
            fobj->handle = NULL;
        }
        fobj->resolved = 1; /* resolved=1: 解決済 */
    }

unlock:
#ifndef _WIN32
    pthread_mutex_unlock(&fobj->mutex);
#else  /* _WIN32 */
    ReleaseSRWLockExclusive(&fobj->lock);
#endif /* _WIN32 */

    return fobj->func_ptr;
}
