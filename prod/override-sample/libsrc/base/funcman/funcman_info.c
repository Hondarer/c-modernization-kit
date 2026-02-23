/**
 *******************************************************************************
 *  @file           funcman_info.c
 *  @brief          funcman_object ポインタ配列の内容を標準出力に表示します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  各エントリを表示し、未解決のエントリがあれば解決を試みます。\n
 *  1 つでも解決失敗した場合は -1 を返します。\n
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <libbase.h>
#include <stdio.h>

/* doxygen コメントは、ヘッダに記載 */
int funcman_info(funcman_object *const *fobj_array, const size_t fobj_length)
{
    int rtc = 0;
    size_t fobj_index;

    for (fobj_index = 0; fobj_index < fobj_length; fobj_index++)
    {
        funcman_object *fobj = fobj_array[fobj_index];

        if (fobj->resolved == 0)
        {
            (void)_funcman_get_func(fobj);
        }
        printf("- [%zu] %s\n", fobj_index, fobj->func_key);
        printf("    - resolved : %d\n", fobj->resolved);
        printf("    - lib_name : %s\n", fobj->lib_name);
        printf("    - func_name: %s\n", fobj->func_name);
        printf("    - handle   : %p\n", (void *)fobj->handle);
        printf("    - func_ptr : %p\n", fobj->func_ptr);

        if (fobj->resolved < 0)
        {
            rtc = -1;
        }
    }

    return rtc;
}
