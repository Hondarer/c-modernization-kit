/**
 *******************************************************************************
 *  @file           symbol_loader.h
 *  @brief          関数動的解決機構 (symbol loader) の公開 API ヘッダー。
 *  @author         c-modenization-kit sample team
 *  @date           2026/03/17
 *  @version        1.0.0
 *
 *  symbol loader は、テキスト設定ファイルから関数シンボルとライブラリ名を読み込み、
 *  実行時に動的リンクで関数を解決するキャッシュ機構です。
 *
 *  使用方法:
 *  1. symbol_loader_entry_t を SYMBOL_LOADER_ENTRY_INIT マクロで静的初期化する。
 *  2. symbol_loader_init() でテキスト設定ファイルを読み込む (DllMain/constructor から呼ぶ)。
 *  3. symbol_loader_resolve_as() で関数ポインタを取得して呼び出す。
 *  4. symbol_loader_dispose() でリソースを解放する (DllMain/destructor から呼ぶ)。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SYMBOL_LOADER_H
#define SYMBOL_LOADER_H

#include <com_util/base/platform.h>
#include <com_util_export.h>

#if defined(PLATFORM_LINUX)
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif /* _GNU_SOURCE */
#endif /* PLATFORM_LINUX */

#include <stddef.h>

#if defined(PLATFORM_LINUX)
    #include <dlfcn.h>
    #include <pthread.h>
#elif defined(PLATFORM_WINDOWS)
    #include <com_util/base/windows_sdk.h>
#endif /* PLATFORM_ */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *  @def            MODULE_HANDLE
 *  @brief          Linux/Windows 共通のモジュールハンドル型。
 *  @details        symbol loader が内部で保持する動的ロード済みモジュールの不透明ハンドルです。\n
 *                  Linux では `dlopen()` が返す `void *`、Windows では `LoadLibrary()` 系が返す
 *                  `HMODULE` を表します。\n
 *                  Doxygen ではプラットフォーム非依存の説明のため、不透明ハンドルとして `void *`
 *                  で表記します。
 */
#ifdef DOXYGEN
    #define MODULE_HANDLE void *
#elif defined(PLATFORM_LINUX)
    #define MODULE_HANDLE void *
#elif defined(PLATFORM_WINDOWS)
    #define MODULE_HANDLE HMODULE
#endif

#define SYMBOL_LOADER_NAME_MAX 256 /**< lib_name / func_name 配列の最大長 (終端 '\0' を含む)。 */

    /**
     *******************************************************************************
     *  @brief          関数ポインタキャッシュエントリ。
     *
     *  @details        ライブラリ名・関数名・ハンドル・関数ポインタおよび排他制御用ロックを管理します。\n
     *                  静的変数として定義する場合は SYMBOL_LOADER_ENTRY_INIT マクロで初期化してください。\n
     *                  排他制御オブジェクトは Linux では `pthread_mutex_t`、Windows では `SRWLOCK`
     *                  を使用します。Doxygen では共通説明のため不透明ポインタとして表記します。\n
     *******************************************************************************
     */
    typedef struct
    {
        const char *func_key;             /**< この関数インスタンスの識別キー。 */
        char lib_name[SYMBOL_LOADER_NAME_MAX];  /**< 拡張子なしライブラリ名。[0]=='\0' = 未設定。 */
        char func_name[SYMBOL_LOADER_NAME_MAX]; /**< 関数シンボル名。[0]=='\0' = 未設定。 */
        MODULE_HANDLE handle;             /**< キャッシュ済みハンドル (NULL = 未ロード)。 */
        void *func_ptr;                   /**< キャッシュ済み関数ポインタ (NULL = 未取得)。 */
        int resolved;                     /**< 解決済フラグ (0 = 未解決)。 */
        int padding;                      /**< パディング。 */
#ifdef DOXYGEN
        void *lock_obj; /**< ロード処理を保護する排他制御オブジェクト。Linux は mutex、Windows は SRW ロック。 */
#elif defined(PLATFORM_LINUX)
        pthread_mutex_t mutex; /**< ロード処理を保護する mutex (Linux)。 */
#elif defined(PLATFORM_WINDOWS)
        SRWLOCK lock; /**< ロード処理を保護する SRW ロック (Windows)。 */
#endif /* PLATFORM_ */
    } symbol_loader_entry_t;

/**
 *  @def            SYMBOL_LOADER_ENTRY_INIT
 *  @brief          symbol_loader_entry_t 静的変数の初期化マクロ。
 *  @details        `func_key` を設定し、ライブラリ名・関数名・ハンドル・関数ポインタ・解決状態を
 *                  未初期化状態にそろえます。\n
 *                  Linux では末尾に `PTHREAD_MUTEX_INITIALIZER`、Windows では `SRWLOCK_INIT`
 *                  を設定します。\n
 *                  Doxygen ではプラットフォーム共通の表現として、末尾の排他制御オブジェクトを
 *                  `NULL` 相当の未初期化値で表記します。
 *
 *  @param[in]      key     この関数インスタンスの識別キー (文字列リテラル)。
 *  @param[in]      type    格納する関数ポインタの型 (例: sample_func_t)。
 */
#ifdef DOXYGEN
    #define SYMBOL_LOADER_ENTRY_INIT(key, type) {(key), {0}, {0}, NULL, NULL, 0, 0, NULL}
#elif defined(PLATFORM_LINUX)
    #define SYMBOL_LOADER_ENTRY_INIT(key, type) {(key), {0}, {0}, NULL, NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER}
#elif defined(PLATFORM_WINDOWS)
    #define SYMBOL_LOADER_ENTRY_INIT(key, type) {(key), {0}, {0}, NULL, NULL, 0, 0, SRWLOCK_INIT}
#endif

    /**
     *******************************************************************************
     *  @brief          拡張関数ポインタを返します。この関数は内部用です。
     *
     *  @details        すでにロード済みの場合は即座に格納済みの関数ポインタを返します。
     *
     *  @param[in,out]  fobj symbol_loader_entry_t へのポインタ。
     *  @return         成功時 void * (関数ポインタ)、失敗時 NULL。
     *******************************************************************************
     */
    COM_UTIL_EXPORT void *COM_UTIL_API symbol_loader_resolve(symbol_loader_entry_t *fobj);

/**
 *  @def            symbol_loader_resolve_as
 *  @brief          拡張関数ポインタを返します。
 *
 *  @param[in]      fobj symbol_loader_entry_t へのポインタ。
 *  @param[in]      type SYMBOL_LOADER_ENTRY_INIT で指定したものと同じ関数ポインタ型。
 */
#define symbol_loader_resolve_as(fobj, type) ((type)symbol_loader_resolve(fobj))

    /**
     *******************************************************************************
     *  @brief          symbol_loader_entry_t が明示的デフォルトかどうかを返します。
     *
     *  @details        lib_name と func_name がともに定義ファイルで default に設定されている場合に 1 を返します。\n
     *                  それ以外の場合は 0 を返します。
     *
     *  @param[in]      fobj symbol_loader_entry_t へのポインタ。
     *  @return         明示的デフォルトの場合は 1、それ以外は 0。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API symbol_loader_is_default(symbol_loader_entry_t *fobj);

    /**
     *******************************************************************************
     *  @brief          symbol_loader_entry_t ポインタ配列を初期化します。
     *
     *  @details        必ず、constructor / DllMain コンテキストから呼ぶようにしてください。
     *
     *  @param[in]      fobj_array  symbol_loader_entry_t ポインタ配列。
     *  @param[in]      fobj_length 配列の要素数。
     *  @param[in]      configpath  定義ファイルのパス。
     *******************************************************************************
     */
    COM_UTIL_EXPORT void COM_UTIL_API symbol_loader_init(symbol_loader_entry_t *const *fobj_array,
                                                         const size_t fobj_length,
                                                         const char *configpath);

    /**
     *******************************************************************************
     *  @brief          symbol_loader_entry_t ポインタ配列を解放します。
     *
     *  @details        必ず、destructor / DllMain コンテキストから呼ぶようにしてください。
     *
     *  @param[in]      fobj_array  symbol_loader_entry_t ポインタ配列。
     *  @param[in]      fobj_length 配列の要素数。
     *******************************************************************************
     */
    COM_UTIL_EXPORT void COM_UTIL_API
        symbol_loader_dispose(symbol_loader_entry_t *const *fobj_array, const size_t fobj_length);

    /**
     *******************************************************************************
     *  @brief          symbol_loader_entry_t ポインタ配列の内容を標準出力に表示します。
     *
     *  @param[in]      fobj_array      symbol_loader_entry_t ポインタ配列。
     *  @param[in]      fobj_length     配列の要素数。
     *  @return         すべてのエントリが正常に解決されている場合は 0、1 つでも失敗している場合は -1 を返します。
     *******************************************************************************
     */
    COM_UTIL_EXPORT int COM_UTIL_API
        symbol_loader_info(symbol_loader_entry_t *const *fobj_array, const size_t fobj_length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYMBOL_LOADER_H */
