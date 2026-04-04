/**
 *******************************************************************************
 *  @file           symbol_loader_is_default.c
 *  @brief          symbol_loader_entry_t が明示的デフォルトかどうかを返します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <util/runtime/symbol_loader.h>

/* doxygen コメントは、ヘッダに記載 */
int symbol_loader_is_default(symbol_loader_entry_t *fobj)
{
    if (fobj->resolved == 0)
    {
        (void)symbol_loader_resolve(fobj);
    }

    if (fobj->resolved == 2)
    {
        return 1;
    }

    return 0;
}
