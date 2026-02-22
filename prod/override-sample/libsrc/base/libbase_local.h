/**
 *******************************************************************************
 *  @file           libbase_local.h
 *  @brief          libbase 内部共有ヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase 内の各ファイルと DllMain.c の間で共有する型定義および変数宣言を提供します。
 *  libbase の外部公開 API には含めません。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBBASE_LOCAL_H
#define LIBBASE_LOCAL_H

#ifndef _WIN32
    #include <dlfcn.h>
    #include <pthread.h>
#else /* _WIN32 */
    #include <windows.h>
#endif /* _WIN32 */

/** liboverride の関数ポインタ型 */
typedef int (*func_override_t)(const int, const int, const int, int *);

/** Linux/Windows 共通のモジュールハンドル型 */
#ifndef _WIN32
    #define MODULE_HANDLE void *
#else /* _WIN32 */
    #define MODULE_HANDLE HMODULE
#endif /* _WIN32 */

/**
 * 関数ポインタキャッシュエントリ。
 * ライブラリ名・関数名・ハンドル・関数ポインタおよび排他制御用ロックを 1 つにまとめる。
 * lib_name は拡張子なし (例: "liboverride")。
 * libbase_load_func がプラットフォームに応じた拡張子を付加してロードし、
 * 内部でロックを取得するためスレッドセーフ。
 *
 * 静的変数として定義する場合は LIBFUNCCACHE_INIT マクロで初期化すること。
 */
typedef struct
{
    const char   *lib_name;  /**< 拡張子なしライブラリ名 (例: "liboverride") */
    const char   *func_name; /**< 関数シンボル名 (例: "func_override") */
    MODULE_HANDLE handle;    /**< キャッシュ済みハンドル (NULL = 未ロード) */
    void         *func_ptr;  /**< キャッシュ済み関数ポインタ (NULL = 未取得) */
#ifndef _WIN32
    pthread_mutex_t mutex;   /**< ロード処理を保護する mutex (Linux) */
#else  /* _WIN32 */
    SRWLOCK         lock;    /**< ロード処理を保護する SRW ロック (Windows) */
#endif /* _WIN32 */
} LibFuncCache;

/**
 * LibFuncCache 静的変数の初期化マクロ。
 * @param lib   拡張子なしライブラリ名 (文字列リテラル)
 * @param func  関数シンボル名 (文字列リテラル)
 * @param type  格納する関数ポインタの型 (例: func_override_t)。
 *              libbase_load_func の第2引数と一致させること。
 */
#ifndef _WIN32
    #define LIBFUNCCACHE_INIT(lib, func, type) \
        { (lib), (func), NULL, NULL, PTHREAD_MUTEX_INITIALIZER }
#else  /* _WIN32 */
    #define LIBFUNCCACHE_INIT(lib, func, type) \
        { (lib), (func), NULL, NULL, SRWLOCK_INIT }
#endif /* _WIN32 */

/**
 * 未ロードなら lib_name に拡張子を付加して dlopen/dlsym し、cache に格納する。
 * 既にロード済みの場合は即座に格納済みの関数ポインタを返す。
 * 内部でロックを取得するためスレッドセーフ。
 * @return 成功時 void * (関数ポインタ)、失敗時 NULL
 */
void *libbase_load_func_impl(LibFuncCache *cache);

/**
 * libbase_load_func_impl を呼び出し、結果を type にキャストして返すマクロ。
 * @param cache  LibFuncCache へのポインタ
 * @param type   LIBFUNCCACHE_INIT で指定したものと同じ関数ポインタ型
 */
#define libbase_load_func(cache, type) ((type)libbase_load_func_impl(cache))

/**
 * 管理下にあるすべてのキャッシュエントリを解放する。
 * 必ず、destructor / DllMain コンテキストから呼ぶこと。
 */
void libbase_unload_all(void);

#endif /* LIBBASE_LOCAL_H */
