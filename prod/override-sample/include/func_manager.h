
#ifndef FUNC_MANAGER_H
#define FUNC_MANAGER_H

#include <stddef.h>

#ifndef _WIN32
    #include <dlfcn.h>
    #include <pthread.h>
#else /* _WIN32 */
    #include <windows.h>
#endif /* _WIN32 */

/** Linux/Windows 共通のモジュールハンドル型 */
#ifndef _WIN32
    #define MODULE_HANDLE void *
#else /* _WIN32 */
    #define MODULE_HANDLE HMODULE
#endif /* _WIN32 */

/** lib_name / func_name 配列の最大長 (終端 '\0' を含む) */
#define FUNC_MANAGER_NAME_MAX 256

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * 関数ポインタキャッシュエントリ。
     * ライブラリ名・関数名・ハンドル・関数ポインタおよび排他制御用ロックを管理する。
     *
     * 静的変数として定義する場合は NEW_FUNC_OBJECT マクロで初期化すること。
     * lib_name / func_name は _func_manager_load_config によって設定される。
     */
    typedef struct
    {
        const char *func_key;                  /**< この関数インスタンスの識別キー */
        char lib_name[FUNC_MANAGER_NAME_MAX];  /**< 拡張子なしライブラリ名。[0]=='\0' = 未設定 */
        char func_name[FUNC_MANAGER_NAME_MAX]; /**< 関数シンボル名。[0]=='\0' = 未設定 */
        MODULE_HANDLE handle;                  /**< キャッシュ済みハンドル (NULL = 未ロード) */
        void *func_ptr;                        /**< キャッシュ済み関数ポインタ (NULL = 未取得) */
#ifndef _WIN32
        pthread_mutex_t mutex; /**< ロード処理を保護する mutex (Linux) */
#else                          /* _WIN32 */
    SRWLOCK lock; /**< ロード処理を保護する SRW ロック (Windows) */
#endif                         /* _WIN32 */
    } func_object;

/**
 * func_object 静的変数の初期化マクロ。
 * lib_name / func_name は _func_manager_load_config で設定される。
 * @param[in] key   この関数インスタンスの識別キー (文字列リテラル)
 * @param[in] type  格納する関数ポインタの型 (例: sample_func_t)。
 *                  func_manager_get_func の第2引数と一致させること。
 */
#ifndef _WIN32
    #define NEW_FUNC_OBJECT(key, type) {(key), {0}, {0}, NULL, NULL, PTHREAD_MUTEX_INITIALIZER}
#else /* _WIN32 */
    #define NEW_FUNC_OBJECT(key, type) {(key), {0}, {0}, NULL, NULL, SRWLOCK_INIT}
#endif /* _WIN32 */

    /**
     * 未ロードなら lib_name に拡張子を付加して dlopen/dlsym し、cache に格納する。
     * 既にロード済みの場合は即座に格納済みの関数ポインタを返す。
     * @return 成功時 void * (関数ポインタ)、失敗時 NULL
     */
    extern void *_func_manager_get_func(func_object *cache);

/**
 * _func_manager_get_func を呼び出し、結果を type にキャストして返すマクロ。
 * @param cache  func_object へのポインタ
 * @param type   NEW_FUNC_OBJECT で指定したものと同じ関数ポインタ型
 */
#define func_manager_get_func(cache, type) ((type)_func_manager_get_func(cache))

    /**
     * テキストファイルから func_key / lib_name / func_name を読み込み、
     * 全キャッシュエントリの lib_name / func_name を設定する。
     * onLoad / constructor コンテキストで func_manager_init の後に呼ぶこと。
     * ファイルフォーマット: "func_key lib_name func_name [# コメント]"
     * @param configpath[in]  設定テキストファイルのパス (例: "/tmp/libbase_ext.txt")
     * @return 設定されたエントリ数。ファイルが開けない場合は -1。
     */
    extern int _func_manager_load_config(const char *configpath);

    /**
     * ロード時の初期化フック。管理するキャッシュポインタ配列を登録する。
     * 必ず、constructor / DllMain コンテキストから呼ぶこと。
     * @param caches[in]     管理対象の func_object ポインタ配列
     * @param count[in]      配列の要素数
     * @param configpath[in] 定義ファイルのパス
     */
    extern void func_manager_init(func_object *const *caches, const size_t count, const char *configpath);

    /**
     * 管理下にあるすべてのキャッシュエントリを解放する。
     * 必ず、destructor / DllMain コンテキストから呼ぶこと。
     */
    extern void func_manager_dispose(void);

#ifdef __cplusplus
}
#endif

#endif /* FUNC_MANAGER_H */
