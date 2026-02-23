/**
 *******************************************************************************
 *  @file           libbase.h
 *  @brief          ベースライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  このライブラリは動的ライブラリのオーバーライド機能を示すサンプルです。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBBASE_H
#define LIBBASE_H

#ifndef _WIN32
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif
#endif

#include <stddef.h>

#ifndef _WIN32
    #include <dlfcn.h>
    #include <pthread.h>
#else /* _WIN32 */
    #include <windows.h>
#endif /* _WIN32 */

/**
 *  @def            BASE_API
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                              | 値                       |
 *  | ------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時               | (空)                     |
 *  | Windows / `BASE_STATIC` 定義時 (静的リンク)       | (空)                     |
 *  | Windows / `BASE_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
 *  | Windows / `BASE_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
 */

/**
 *  @def            WINAPI
 *  @brief          Windows 呼び出し規約マクロ。
 *
 *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
 *                  Linux (非 Windows) 環境では空に展開されます。\n
 *                  既に定義済みの場合は再定義されません。
 */
#ifndef _WIN32
    #define BASE_API
    #define WINAPI
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef BASE_STATIC
            #ifdef BASE_EXPORTS
                #define BASE_API __declspec(dllexport)
            #else /* BASE_EXPORTS */
                #define BASE_API __declspec(dllimport)
            #endif /* BASE_EXPORTS */
        #else      /* BASE_STATIC */
            #define BASE_API
        #endif /* BASE_STATIC */
    #else      /* __INTELLISENSE__ */
        #define BASE_API
    #endif /* __INTELLISENSE__ */
    #ifndef WINAPI
        #define WINAPI __stdcall
    #endif /* WINAPI */
#endif     /* _WIN32 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *******************************************************************************
     *  @brief          計算処理を行います。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @warning        result が NULL の場合は -1 を返します。
     *******************************************************************************
     */
    BASE_API extern int WINAPI sample_func(const int a, const int b, int *result);

    /**
     *******************************************************************************
     *  @brief          printf と同じ書式でコンソールに出力します。
     *  @param[in]      format printf 互換の書式文字列。
     *  @param[in]      ... 書式文字列に対応する引数。
     *
     *  @details        この関数は printf のラッパーです。\n
     *                  動的ライブラリ内から呼び出し元プロセスのコンソールに出力します。
     *
     *  @par            使用例
     *  @code{.c}
     *  console_output("result: %d\n", 42);  // 出力: result: 42
     *  @endcode
     *******************************************************************************
     */
    BASE_API extern void WINAPI console_output(const char *format, ...);

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
    BASE_API extern int WINAPI get_lib_path(char *out_path, const size_t out_path_sz, const void *func_addr);

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
    BASE_API extern int WINAPI get_lib_basename(char *out_basename, const size_t out_basename_sz,
                                                const void *func_addr);

    /* --- funcman START ---*/

/**
 *  @def            MODULE_HANDLE
 *  @brief          Linux/Windows 共通のモジュールハンドル型。
 */
#ifndef _WIN32
    #define MODULE_HANDLE void *
#else /* _WIN32 */
    #define MODULE_HANDLE HMODULE
#endif /* _WIN32 */

/**
 *  @def            FUNCMAN_NAME_MAX
 *  @brief          lib_name / func_name 配列の最大長 (終端 '\0' を含む)。
 */
#define FUNCMAN_NAME_MAX 256

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
        const char *func_key;             /**< この関数インスタンスの識別キー */
        char lib_name[FUNCMAN_NAME_MAX];  /**< 拡張子なしライブラリ名。[0]=='\0' = 未設定 */
        char func_name[FUNCMAN_NAME_MAX]; /**< 関数シンボル名。[0]=='\0' = 未設定 */
        MODULE_HANDLE handle;             /**< キャッシュ済みハンドル (NULL = 未ロード) */
        void *func_ptr;                   /**< キャッシュ済み関数ポインタ (NULL = 未取得) */
        int resolved;                     /**< 解決済フラグ (0 = 未解決) */
        int padding;                      /**< パディング */
#ifndef _WIN32
        pthread_mutex_t mutex; /**< ロード処理を保護する mutex (Linux) */
#else                          /* _WIN32 */
    SRWLOCK lock; /**< ロード処理を保護する SRW ロック (Windows) */
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
    BASE_API extern void *WINAPI _funcman_get_func(funcman_object *fobj);

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
     *  @brief          funcman_object ポインタ配列を初期化します。
     *
     *  @details        必ず、constructor / DllMain コンテキストから呼ぶようにしてください。
     *
     *  @param[in]      fobj_array  funcman_object ポインタ配列。
     *  @param[in]      fobj_length 配列の要素数。
     *  @param[in]      configpath  定義ファイルのパス。
     *******************************************************************************
     */
    BASE_API extern void WINAPI funcman_init(funcman_object *const *fobj_array, const size_t fobj_length,
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
    BASE_API extern void WINAPI funcman_dispose(funcman_object *const *fobj_array, const size_t fobj_length);

    /**
     *******************************************************************************
     *  @brief          funcman_object ポインタ配列の内容を標準出力に表示します。
     *
     *  @param[in]      fobj_array      funcman_object ポインタ配列。
     *  @param[in]      fobj_length     配列の要素数。
     *  @return         すべてのエントリが正常に解決されている場合は 0、1 つでも失敗している場合は -1 を返します。
     *******************************************************************************
     */
    BASE_API extern int WINAPI funcman_info(funcman_object *const *fobj_array, const size_t fobj_length);

    /**
     *******************************************************************************
     *  @brief          libbase が管理する funcman_object ポインタ配列の内容を標準出力に表示します。
     *  @return         すべてのエントリが正常に解決されている場合は 0、1 つでも失敗している場合は -1 を返します。
     *******************************************************************************
     */
    BASE_API extern int WINAPI funcman_info_libbase(void);

    /* --- funcman END ---*/

#ifdef __cplusplus
}
#endif

#endif /* LIBBASE_H */
