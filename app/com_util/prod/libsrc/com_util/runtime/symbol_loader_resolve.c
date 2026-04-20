/**
 *******************************************************************************
 *  @file           symbol_loader_resolve.c
 *  @brief          拡張可能な関数の動的ロードを行い、関数アドレスを返却します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  symbol_loader_resolve_as はスレッドセーフです。
 *  内部で mutex (Linux) または SRW ロック (Windows) を使用して排他制御します。
 *
 *  @par            double-checked locking とメモリ順序
 *  fast path (ロックなし) の resolved 読み取りには acquire セマンティクスを使用します。
 *  これにより、weakly-ordered アーキテクチャ (ARM64 等) においても
 *  resolved != 0 が見えた時点で func_ptr の書き込みも可視であることが保証されます。\n
 *  GCC では __atomic_load_n / __atomic_store_n を使用します。\n
 *  MSVC (x86_64) では TSO により load が acquire-ordered のため変更不要です。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/runtime/symbol_loader.h>
#include <string.h>

/* doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT void *COM_UTIL_API symbol_loader_resolve(symbol_loader_entry_t *fobj)
{
#if defined(PLATFORM_LINUX)
    const char *ext = ".so";
#elif defined(PLATFORM_WINDOWS)
    const char *ext = ".dll";
#endif /* PLATFORM_ */
    char lib_with_ext[SYMBOL_LOADER_NAME_MAX];

    /* ロード完了後は resolved が 0 以外になる。早期リターンで判定する。
     * ARM64 など weakly-ordered アーキテクチャでのメモリ可視性を保証するため
     * GCC では acquire ロードを使用する。
     * MSVC (x86_64/TSO) では plain load で acquire 相当の保証が得られる。 */
#if defined(COMPILER_GCC)
    if (__atomic_load_n(&fobj->resolved, __ATOMIC_ACQUIRE) != 0)
    {
        return __atomic_load_n(&fobj->func_ptr, __ATOMIC_RELAXED);
    }
#else /* !COMPILER_GCC */
    if (fobj->resolved != 0)
    {
        return fobj->func_ptr;
    }
#endif /* COMPILER_GCC */

    /* ロード処理を排他制御する。
     * ロック取得後に再度 resolved を確認し、他スレッドが先にロードを
     * 完了していた場合は処理をスキップする (double-checked locking)。 */
#if defined(PLATFORM_LINUX)
    if (pthread_mutex_lock(&fobj->mutex) != 0)
    {
        return NULL;
    }
#elif defined(PLATFORM_WINDOWS)
    AcquireSRWLockExclusive(&fobj->lock);
#endif /* PLATFORM_ */

    if (fobj->resolved == 0)
    {
        if (strcmp(fobj->lib_name, "default") == 0 && strcmp(fobj->func_name, "default") == 0)
        {
            /* resolved=2: 明示的デフォルト。func_ptr は NULL のまま。
             * func_ptr への書き込みなし → release store のみ必要。 */
#if defined(COMPILER_GCC)
            __atomic_store_n(&fobj->resolved, 2, __ATOMIC_RELEASE);
#else /* !COMPILER_GCC */
            fobj->resolved = 2;
#endif /* COMPILER_GCC */
            goto unlock;
        }
        if (fobj->lib_name[0] == '\0' || fobj->func_name[0] == '\0')
        {
#if defined(COMPILER_GCC)
            __atomic_store_n(&fobj->resolved, -1, __ATOMIC_RELEASE);
#else /* !COMPILER_GCC */
            fobj->resolved = -1; /* resolved=-1: 定義なし (定義ファイル不存在、定義行が不存在) */
#endif /* COMPILER_GCC */
            goto unlock;
        }
        if (strlen(fobj->lib_name) + strlen(ext) >= SYMBOL_LOADER_NAME_MAX)
        {
#if defined(COMPILER_GCC)
            __atomic_store_n(&fobj->resolved, -2, __ATOMIC_RELEASE);
#else /* !COMPILER_GCC */
            fobj->resolved = -2; /* resolved=-2: 名称長さオーバー */
#endif /* COMPILER_GCC */
            goto unlock;
        }
#if defined(PLATFORM_LINUX)
        strcpy(lib_with_ext, fobj->lib_name);
        strcat(lib_with_ext, ext);
#elif defined(PLATFORM_WINDOWS)
        strcpy_s(lib_with_ext, sizeof(lib_with_ext), fobj->lib_name);
        strcat_s(lib_with_ext, sizeof(lib_with_ext), ext);
#endif /* PLATFORM_ */

#if defined(PLATFORM_LINUX)
        fobj->handle = dlopen(lib_with_ext, RTLD_LAZY);
#elif defined(PLATFORM_WINDOWS)
        fobj->handle = LoadLibrary(lib_with_ext);
#endif /* PLATFORM_ */
        if (fobj->handle == NULL)
        {
#if defined(COMPILER_GCC)
            __atomic_store_n(&fobj->resolved, -3, __ATOMIC_RELEASE);
#else /* !COMPILER_GCC */
            fobj->resolved = -3; /* resolved=-3: ライブラリオープンエラー */
#endif /* COMPILER_GCC */
            goto unlock;
        }

        /* func_ptr を書き込んでから resolved を release-store する。
         * fast path の acquire ロードと対になり、func_ptr の可視性を保証する。 */
#if defined(PLATFORM_LINUX)
        {
            void *ptr = dlsym(fobj->handle, fobj->func_name);
#if defined(COMPILER_GCC)
            __atomic_store_n(&fobj->func_ptr, ptr, __ATOMIC_RELAXED);
#else /* !COMPILER_GCC */
            fobj->func_ptr = ptr;
#endif /* COMPILER_GCC */
            if (ptr == NULL)
            {
                dlclose(fobj->handle);
                fobj->handle = NULL;
            }
        }
#elif defined(PLATFORM_WINDOWS)
        {
            void *ptr = (void *)GetProcAddress(fobj->handle, fobj->func_name);
            fobj->func_ptr = ptr; /* MSVC x86_64: TSO により release-ordered */
            if (ptr == NULL)
            {
                FreeLibrary(fobj->handle);
                fobj->handle = NULL;
            }
        }
#endif /* PLATFORM_ */

#if defined(COMPILER_GCC)
        __atomic_store_n(&fobj->resolved, 1, __ATOMIC_RELEASE); /* resolved=1: 解決済 */
#else /* !COMPILER_GCC */
        fobj->resolved = 1; /* resolved=1: 解決済 */
#endif /* COMPILER_GCC */
    }

unlock:
#if defined(PLATFORM_LINUX)
    pthread_mutex_unlock(&fobj->mutex);
#elif defined(PLATFORM_WINDOWS)
    ReleaseSRWLockExclusive(&fobj->lock);
#endif /* PLATFORM_ */

    return fobj->func_ptr;
}
