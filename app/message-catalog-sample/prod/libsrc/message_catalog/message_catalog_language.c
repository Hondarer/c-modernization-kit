/**
 *******************************************************************************
 *  @file           libsrc/message_catalog/message_catalog_language.c
 *  @brief          プロセスがメッセージを出力する言語の設定を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  言語はプロセスで 1 つとし、メッセージを組み立てるたびに指定しません。\n
 *  設定していないプロセスはニュートラル言語を使用します。
 *
 *  設定はプロセス グローバルな状態であり、同期を持ちません。\n
 *  プロセスの初期化時に設定し、メッセージを組み立てている間は変更しない前提です。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <message_catalog/message_catalog_spec.h>

/** プロセスがメッセージを出力する言語です。 */
static message_catalog_language s_language = MESSAGE_CATALOG_LANGUAGE_NEUTRAL;

/* Doxygen コメントは、ヘッダーに記載 */

int message_catalog_set_language(const message_catalog_language language)
{
    /* 列挙の基底型は処理系定義のため、符号なしへ変換して上限だけを判定する */
    if ((unsigned int)language >= (unsigned int)MESSAGE_CATALOG_LANGUAGE_COUNT)
    {
        return MESSAGE_CATALOG_ERR_INVALID_ARGUMENT;
    }

    s_language = language;

    return MESSAGE_CATALOG_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

message_catalog_language message_catalog_get_language(void)
{
    return s_language;
}
