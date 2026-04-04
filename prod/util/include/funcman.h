/**
 *******************************************************************************
 *  @file           funcman.h
 *  @brief          関数動的呼び出し機構 (funcman) の公開 API ヘッダー。
 *  @author         c-modenization-kit sample team
 *  @date           2026/03/17
 *  @version        1.0.0
 *
 *  funcman は、テキスト設定ファイルから関数シンボルとライブラリ名を読み込み、
 *  実行時に動的リンクで関数を解決するキャッシュ機構です。
 *
 *  使用方法:
 *  1. funcman_object を NEW_FUNCMAN_OBJECT マクロで静的初期化する。
 *  2. funcman_init() でテキスト設定ファイルを読み込む (DllMain/constructor から呼ぶ)。
 *  3. funcman_get_func() で関数ポインタを取得して呼び出す。
 *  4. funcman_dispose() でリソースを解放する (DllMain/destructor から呼ぶ)。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef FUNCMAN_H
#define FUNCMAN_H

#ifndef _WIN32
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif /* _GNU_SOURCE */
#endif /* _WIN32 */

#include <stddef.h>

#ifndef _WIN32
    #include <dlfcn.h>
    #include <pthread.h>
#else /* _WIN32 */
    #include <windows.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 *  @def            FUNCMAN_API
 *  @brief          Windows DLL エクスポート/インポート修飾子。
 *
 *  libutil をビルドする側 (FUNCMAN_UTIL_EXPORTS が定義された状態) では dllexport、
 *  利用する側では dllimport として解決されます。Linux では空になります。
 */
#if defined(_WIN32)
    #ifdef FUNCMAN_UTIL_EXPORTS
        #define FUNCMAN_API __declspec(dllexport)
    #else
        #define FUNCMAN_API __declspec(dllimport)
    #endif
#else
    #define FUNCMAN_API
#endif

/**
 *  @def            MODULE_HANDLE
 *  @brief          Linux/Windows 共通のモジュールハンドル型。
 */
#ifndef _WIN32
    #define MODULE_HANDLE void *
#else /* _WIN32 */
    #define MODULE_HANDLE HMODULE
#endif /* _WIN32 */

#define FUNCMAN_NAME_MAX 256 /**< lib_name / func_name 配列の最大長 (終端 '\0' を含む)。 */

    /**
     *******************************************************************************
     *  @brief          関数ポインタキャッシュエントリ。
     *
     *  @details        ライブラリ名・関数名・ハンドル・関数ポインタおよび排他制御用ロックを管理します。\n
     *                  静的変数として定義する場合は NEW_FUNCMAN_OBJECT マクロで初期化してください。\n
     *******************************************************************************
     */
    typedef struct
    {
        const char *func_key;             /**< この関数インスタンスの識別キー。 */
        char lib_name[FUNCMAN_NAME_MAX];  /**< 拡張子なしライブラリ名。[0]=='\0' = 未設定。 */
        char func_name[FUNCMAN_NAME_MAX]; /**< 関数シンボル名。[0]=='\0' = 未設定。 */
        MODULE_HANDLE handle;             /**< キャッシュ済みハンドル (NULL = 未ロード)。 */
        void *func_ptr;                   /**< キャッシュ済み関数ポインタ (NULL = 未取得)。 */
        int resolved;                     /**< 解決済フラグ (0 = 未解決)。 */
        int padding;                      /**< パディング。 */
#ifndef _WIN32
        pthread_mutex_t mutex; /**< ロード処理を保護する mutex (Linux)。 */
#else                          /* _WIN32 */
    SRWLOCK lock; /**< ロード処理を保護する SRW ロック (Windows)。 */
#endif                         /* _WIN32 */
    } funcman_object;

/**
 *  @def            NEW_FUNCMAN_OBJECT
 *  @brief          funcman_object 静的変数の初期化マクロ。
 *
 *  @param[in]      key     この関数インスタンスの識別キー (文字列リテラル)。
 *  @param[in]      type    格納する関数ポインタの型 (例: sample_func_t)。
 */
#ifndef _WIN32
    #define NEW_FUNCMAN_OBJECT(key, type) {(key), {0}, {0}, NULL, NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER}
#else /* _WIN32 */
    #define NEW_FUNCMAN_OBJECT(key, type) {(key), {0}, {0}, NULL, NULL, 0, 0, SRWLOCK_INIT}
#endif /* _WIN32 */

    /**
     *******************************************************************************
     *  @brief          拡張関数ポインタを返します。この関数は内部用です。
     *
     *  @details        既にロード済みの場合は即座に格納済みの関数ポインタを返します。
     *
     *  @param[in,out]  fobj funcman_object へのポインタ。
     *  @return         成功時 void * (関数ポインタ)、失敗時 NULL。
     *******************************************************************************
     */
    extern FUNCMAN_API void *_funcman_get_func(funcman_object *fobj);

/**
 *  @def            funcman_get_func
 *  @brief          拡張関数ポインタを返します。
 *
 *  @param[in]      fobj funcman_object へのポインタ。
 *  @param[in]      type NEW_FUNCMAN_OBJECT で指定したものと同じ関数ポインタ型。
 */
#define funcman_get_func(fobj, type) ((type)_funcman_get_func(fobj))

    /**
     *******************************************************************************
     *  @brief          funcman_object が明示的デフォルトかどうかを返します。
     *
     *  @details        lib_name と func_name がともに定義ファイルで default に設定されている場合に 1 を返します。\n
     *                  それ以外の場合は 0 を返します。
     *
     *  @param[in]      fobj funcman_object へのポインタ。
     *  @return         明示的デフォルトの場合は 1、それ以外は 0。
     *******************************************************************************
     */
    extern FUNCMAN_API int funcman_is_declared_default(funcman_object *fobj);

    /**
     *******************************************************************************
     *  @brief          funcman_object ポインタ配列を初期化します。
     *
     *  @details        必ず、constructor / DllMain コンテキストから呼ぶようにしてください。
     *
     *  @param[in]      fobj_array  funcman_object ポインタ配列。
     *  @param[in]      fobj_length 配列の要素数。
     *  @param[in]      configpath  定義ファイルのパス。
     *******************************************************************************
     */
    extern FUNCMAN_API void funcman_init(funcman_object *const *fobj_array, const size_t fobj_length,
                             const char *configpath);

    /**
     *******************************************************************************
     *  @brief          funcman_object ポインタ配列を解放します。
     *
     *  @details        必ず、destructor / DllMain コンテキストから呼ぶようにしてください。
     *
     *  @param[in]      fobj_array  funcman_object ポインタ配列。
     *  @param[in]      fobj_length 配列の要素数。
     *******************************************************************************
     */
    extern FUNCMAN_API void funcman_dispose(funcman_object *const *fobj_array, const size_t fobj_length);

    /**
     *******************************************************************************
     *  @brief          funcman_object ポインタ配列の内容を標準出力に表示します。
     *
     *  @param[in]      fobj_array      funcman_object ポインタ配列。
     *  @param[in]      fobj_length     配列の要素数。
     *  @return         すべてのエントリが正常に解決されている場合は 0、1 つでも失敗している場合は -1 を返します。
     *******************************************************************************
     */
    extern FUNCMAN_API int funcman_info(funcman_object *const *fobj_array, const size_t fobj_length);

    /**
     *******************************************************************************
     *  @brief          指定した関数が所属する共有ライブラリ (.so/.dll) の絶対パスを取得します。
     *
     *  @param[out]     out_path    出力バッファ。
     *  @param[in]      out_path_sz 出力バッファサイズ[byte]。
     *  @param[in]      func_addr   所属モジュールを特定するための関数アドレス。
     *  @return         関数が成功した場合、0 を返します。失敗した場合は 0 以外を返します。
     *******************************************************************************
     */
    extern FUNCMAN_API int get_lib_path(char *out_path, const size_t out_path_sz, const void *func_addr);

    /**
     *******************************************************************************
     *  @brief          指定した関数が所属する共有ライブラリ (.so/.dll) の basename (パスなし・拡張子なし)
     *                  を取得します。
     *
     *  Linux の .so.1.2.3 のようなバージョン付きは ".so." より前を basename とみなします。
     *
     *  @param[out]     out_basename    出力バッファ。
     *  @param[in]      out_basename_sz 出力バッファサイズ[byte]。
     *  @param[in]      func_addr       所属モジュールを特定するための関数アドレス。
     *  @return         関数が成功した場合、0 を返します。失敗した場合は 0 以外を返します。
     *******************************************************************************
     */
    extern FUNCMAN_API int get_lib_basename(char *out_basename, const size_t out_basename_sz,
                                const void *func_addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FUNCMAN_H */
