/**
 *******************************************************************************
 *  @file           funcman_dispose.c
 *  @brief          funcman_object ポインタ配列を解放します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <libbase.h>

/* doxygen コメントは、ヘッダに記載 */
void funcman_dispose(funcman_object *const *fobj_array, const size_t fobj_length)
{
    size_t fobj_index;

    /* DllMain / destructor コンテキストから呼ばれるため、
     * ローダーロック保持中にミューテックスを取得すると
     * デッドロックを引き起こす恐れがある。
     * このコンテキストではシングルスレッド動作が保証されるため、
     * ロックなしで解放する。 */

    for (fobj_index = 0; fobj_index < fobj_length; fobj_index++)
    {
        funcman_object *cache = fobj_array[fobj_index];

        if (cache->handle == NULL)
        {
            continue;
        }

#ifndef _WIN32
        dlclose(cache->handle);
#else
        FreeLibrary(cache->handle);
#endif

        cache->handle = NULL;
        cache->func_ptr = NULL;
    }
}
