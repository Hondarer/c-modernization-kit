/**
 *******************************************************************************
 *  @file           func.c
 *  @brief          func 関数の実装ファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  useOverride フラグに従い自身で処理するか外部ライブラリに委譲する関数を提供します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "libbase_local.h"
#include <libbase.h>
#include <stddef.h>

#ifndef _WIN32
    #include <pthread.h>
#endif /* _WIN32 */

/* ハンドルと関数ポインタのキャッシュ (初回ロード時のみ取得)
 * アンロード時の解放は DllMain.c が担当する。 */
MODULE_HANDLE s_handle = NULL;
func_override_t s_func_override = NULL;

/* 一回限りの初期化制御変数 (スレッドセーフな初回ロードに使用) */

static void load_liboverride_onceImpl(void);

void load_liboverride_onceImpl(void)
{
#ifndef _WIN32
    s_handle = dlopen("liboverride.so", RTLD_LAZY);
    if (s_handle == NULL)
    {
        return;
    }
    s_func_override = (func_override_t)dlsym(s_handle, "func_override");
    if (s_func_override == NULL)
    {
        dlclose(s_handle);
        s_handle = NULL;
    }
#else  /* _WIN32 */
    s_handle = LoadLibrary("liboverride.dll");
    if (s_handle == NULL)
    {
        return TRUE;
    }
    s_func_override = (func_override_t)GetProcAddress(s_handle, "func_override");
    if (s_func_override == NULL)
    {
        FreeLibrary(s_handle);
        s_handle = NULL;
    }
#endif /* _WIN32 */
}

#ifndef _WIN32

static pthread_once_t s_once = PTHREAD_ONCE_INIT;

static void load_liboverride_once(void)
{
    load_liboverride_onceImpl();
}

#else /* _WIN32 */

static INIT_ONCE s_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK load_liboverride_once(PINIT_ONCE pOnce, PVOID param, PVOID *ctx)
{
    (void)pOnce;
    (void)param;
    (void)ctx;
    load_liboverride_onceImpl();
    return TRUE;
}

#endif /* _WIN32 */

/* doxygen コメントは、ヘッダに記載 */
int WINAPI func(const int useOverride, const int a, const int b, int *result)
{
    if (result == NULL)
    {
        return -1;
    }

    if (useOverride == 0)
    {
        console_output("func: a=%d, b=%d の処理 (*result = a + b;) を行います\n", a, b);
        *result = a + b;
        return 0;
    }

    /* useOverride != 0: liboverride に委譲 */
    {
#ifndef _WIN32
        if (pthread_once(&s_once, load_liboverride_once) != 0)
        {
            return -1;
        }
#else  /* _WIN32 */
        if (!InitOnceExecuteOnce(&s_once, load_liboverride_once, NULL, NULL))
        {
            return -1;
        }
#endif /* _WIN32 */
        if (s_func_override == NULL)
        {
            return -1;
        }
        console_output("func: func_override に移譲します\n");
        return s_func_override(useOverride, a, b, result);
    }
}
