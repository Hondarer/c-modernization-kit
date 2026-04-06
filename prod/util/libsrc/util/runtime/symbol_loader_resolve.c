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
 *  GCC/Clang では __atomic_load_n / __atomic_store_n を使用します。\n
 *  MSVC (x86_64) では TSO により load が acquire-ordered のため変更不要です。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <util/runtime/symbol_loader.h>
#include <string.h>

/* doxygen コメントは、ヘッダに記載 */
void *symbol_loader_resolve(symbol_loader_entry_t *fobj)
{
#ifndef _WIN32
    const char *ext = ".so";
#else  /* _WIN32 */
    const char *ext = ".dll";
#endif /* _WIN32 */
    char lib_with_ext[SYMBOL_LOADER_NAME_MAX];

    /* ロード完了後は resolved が 0 以外になる。早期リターンで判定する。
     * ARM64 など weakly-ordered アーキテクチャでのメモリ可視性を保証するため
     * GCC/Clang では acquire ロードを使用する。
     * MSVC (x86_64/TSO) では plain load で acquire 相当の保証が得られる。 */
#if defined(__GNUC__) || defined(__clang__)
    if (__atomic_load_n(&fobj->resolved, __ATOMIC_ACQUIRE) != 0)
    {
        return __atomic_load_n(&fobj->func_ptr, __ATOMIC_RELAXED);
    }
#else
    if (fobj->resolved != 0)
    {
        return fobj->func_ptr;
    }
#endif

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
        if (strcmp(fobj->lib_name, "default") == 0 && strcmp(fobj->func_name, "default") == 0)
        {
            /* resolved=2: 明示的デフォルト。func_ptr は NULL のまま。
             * func_ptr への書き込みなし → release store のみ必要。 */
#if defined(__GNUC__) || defined(__clang__)
            __atomic_store_n(&fobj->resolved, 2, __ATOMIC_RELEASE);
#else
            fobj->resolved = 2;
#endif
            goto unlock;
        }
        if (fobj->lib_name[0] == '\0' || fobj->func_name[0] == '\0')
        {
#if defined(__GNUC__) || defined(__clang__)
            __atomic_store_n(&fobj->resolved, -1, __ATOMIC_RELEASE);
#else
            fobj->resolved = -1; /* resolved=-1: 定義なし (定義ファイル不存在、定義行が不存在) */
#endif
            goto unlock;
        }
        if (strlen(fobj->lib_name) + strlen(ext) >= SYMBOL_LOADER_NAME_MAX)
        {
#if defined(__GNUC__) || defined(__clang__)
            __atomic_store_n(&fobj->resolved, -2, __ATOMIC_RELEASE);
#else
            fobj->resolved = -2; /* resolved=-2: 名称長さオーバー */
#endif
            goto unlock;
        }
#ifndef _WIN32
        strcpy(lib_with_ext, fobj->lib_name);
        strcat(lib_with_ext, ext);
#else  /* _WIN32 */
        strcpy_s(lib_with_ext, sizeof(lib_with_ext), fobj->lib_name);
        strcat_s(lib_with_ext, sizeof(lib_with_ext), ext);
#endif /* _WIN32 */

#ifndef _WIN32
        fobj->handle = dlopen(lib_with_ext, RTLD_LAZY);
#else  /* _WIN32 */
        fobj->handle = LoadLibrary(lib_with_ext);
#endif /* _WIN32 */
        if (fobj->handle == NULL)
        {
#if defined(__GNUC__) || defined(__clang__)
            __atomic_store_n(&fobj->resolved, -3, __ATOMIC_RELEASE);
#else
            fobj->resolved = -3; /* resolved=-3: ライブラリオープンエラー */
#endif
            goto unlock;
        }

        /* func_ptr を書き込んでから resolved を release-store する。
         * fast path の acquire ロードと対になり、func_ptr の可視性を保証する。 */
#ifndef _WIN32
        {
            void *ptr = dlsym(fobj->handle, fobj->func_name);
#if defined(__GNUC__) || defined(__clang__)
            __atomic_store_n(&fobj->func_ptr, ptr, __ATOMIC_RELAXED);
#else
            fobj->func_ptr = ptr;
#endif
            if (ptr == NULL)
            {
                dlclose(fobj->handle);
                fobj->handle = NULL;
            }
        }
#else  /* _WIN32 */
        {
            void *ptr = (void *)GetProcAddress(fobj->handle, fobj->func_name);
            fobj->func_ptr = ptr; /* MSVC x86_64: TSO により release-ordered */
            if (ptr == NULL)
            {
                FreeLibrary(fobj->handle);
                fobj->handle = NULL;
            }
        }
#endif /* _WIN32 */

#if defined(__GNUC__) || defined(__clang__)
        __atomic_store_n(&fobj->resolved, 1, __ATOMIC_RELEASE); /* resolved=1: 解決済 */
#else
        fobj->resolved = 1; /* resolved=1: 解決済 */
#endif
    }

unlock:
#ifndef _WIN32
    pthread_mutex_unlock(&fobj->mutex);
#else  /* _WIN32 */
    ReleaseSRWLockExclusive(&fobj->lock);
#endif /* _WIN32 */

    return fobj->func_ptr;
}
