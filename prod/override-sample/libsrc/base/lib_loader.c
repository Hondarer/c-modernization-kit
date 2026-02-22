/**
 *******************************************************************************
 *  @file           lib_loader.c
 *  @brief          動的ライブラリロード Factory 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/22
 *  @version        1.0.0
 *
 *  ライブラリ名・関数名・ハンドル・関数ポインタを保持する LibFuncCache 構造体に対して、
 *  プラットフォーム共通のロード／アンロード処理を提供します。
 *
 *  libbase_load_func はスレッドセーフです。
 *  内部で mutex (Linux) または SRW ロック (Windows) を使用して排他制御します。
 *  呼び出し元での pthread_once / INIT_ONCE による保護は不要です。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "libbase_local.h"
#include <stddef.h>
#include <string.h>

/** ライブラリ名バッファの最大長 (ベース名 + 拡張子を含む) */
#define LIBBASE_MAX_LIB_NAME 256

/* liboverride の関数ポインタキャッシュ。
 * lib_name は拡張子なし (libbase_load_func が .so/.dll を付加する)。
 * 排他制御は libbase_load_func が内部で担当する。
 * アンロード時の解放は DllMain.c が担当する。 */
LibFuncCache s_cache_func_override =
    LIBFUNCCACHE_INIT("liboverride", "func_override", func_override_t);

/* クリーンアップ対象のキャッシュポインタ配列。
 * 関数が増えた場合はここにエントリを追加するだけでよい。 */
static LibFuncCache * const s_all_caches[] = {
    &s_cache_func_override,
    /* &s_cache_func2, */ /* 将来追加 */
};

/**
 * cache->handle を閉じ、handle と func_ptr を NULL にリセットする。
 * DllMain / destructor コンテキストからのみ呼ぶこと。
 */
static void libbase_unload_func(LibFuncCache *cache);

/* doxygen コメントは、ヘッダに記載 */
void *libbase_load_func_impl(LibFuncCache *cache)
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

/* doxygen コメントは、ヘッダに記載 */
void libbase_unload_all(void)
{
    size_t i;
    for (i = 0; i < sizeof(s_all_caches) / sizeof(s_all_caches[0]); i++)
    {
        libbase_unload_func(s_all_caches[i]);
    }
}

/* doxygen コメントは、ヘッダに記載 */
void libbase_unload_func(LibFuncCache *cache)
{
    /* DllMain / destructor コンテキストから呼ばれるため、
     * ローダーロック保持中にミューテックスを取得すると
     * デッドロックを引き起こす恐れがある。
     * このコンテキストではシングルスレッド動作が保証されるため、
     * ロックなしで解放する。 */
    if (cache->handle == NULL)
    {
        return;
    }
#ifndef _WIN32
    dlclose(cache->handle);
#else  /* _WIN32 */
    FreeLibrary(cache->handle);
#endif /* _WIN32 */
    cache->handle  = NULL;
    cache->func_ptr = NULL;
}
