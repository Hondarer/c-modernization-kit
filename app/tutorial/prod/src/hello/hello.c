/**
 *******************************************************************************
 *  @file           hello.c
 *  @brief          サンプルアプリケーション。
 *  @author         Tetsuo Honda
 *  @date           2026/04/23
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/console/console.h>
#include <stdio.h>

/**
 *  @brief          アプリケーションのエントリポイント。
 *
 *  @param[in]      argc コマンドライン引数の個数。
 *  @param[in]      argv コマンドライン引数の文字列の配列。
 *  @return         常に 0 を返します。
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    com_util_console_init();

    printf("✨ Hello, c-modernization-kit! ✨\n");

    return 0;
}
