/**
 *******************************************************************************
 *  @file           libsrc/string_catalog/string_catalog_language.c
 *  @brief          プロセスが文字列を出力する言語の設定を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  言語はプロセスで 1 つとし、文字列を組み立てるたびに指定しません。\n
 *  設定していないプロセスはニュートラル言語を使用します。
 *
 *  設定はプロセス グローバルな状態であり、同期を持ちません。\n
 *  プロセスの初期化時に設定し、文字列を組み立てている間は変更しない前提です。
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
    /* 列挙の基底型は処理系定義のため、符号なしへ変換して上限だけを判定する */
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
