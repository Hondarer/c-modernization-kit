/**
 *******************************************************************************
 *  @file           libsrc/string_catalog/string_catalog_language.c
 *  @brief          プロセスが文字列を出力する言語の設定を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  出力言語はプロセス全体で一元管理され、文字列組み立ての都度指定する必要はありません。\n
 *  未設定のプロセスではニュートラル言語が使用されます。
 *
 *  言語設定はプロセス共有の状態であり、排他制御は行いません。\n
 *  プロセスの初期化時に設定し、文字列組み立ての実行中は変更しない運用を前提とします。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <string_catalog/string_catalog_spec.h>

/** プロセスが文字列を出力する言語です。 */
static string_catalog_language s_language = STRING_CATALOG_LANGUAGE_NEUTRAL;

/* Doxygen コメントは、ヘッダーに記載 */

int string_catalog_set_language(const string_catalog_language language)
{
    /* 列挙の基底型は処理系定義のため、符号なし整数へキャストして上限値のみを判定する */
    if ((unsigned int)language >= (unsigned int)STRING_CATALOG_LANGUAGE_COUNT)
    {
        return STRING_CATALOG_ERR_INVALID_ARGUMENT;
    }

    s_language = language;

    return STRING_CATALOG_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

string_catalog_language string_catalog_get_language(void)
{
    return s_language;
}
