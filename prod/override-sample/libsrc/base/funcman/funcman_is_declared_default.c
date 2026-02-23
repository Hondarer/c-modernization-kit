/**
 *******************************************************************************
 *  @file           funcman_is_declared_default.c
 *  @brief          funcman_object が明示的デフォルトかどうかを返します。
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
int funcman_is_declared_default(const funcman_object *fobj)
{
    if (fobj->resolved == 0)
    {
        (void)_funcman_get_func(fobj);
    }

    if (fobj->resolved == 2)
    {
        return 1;
    }

    return 0;
}
