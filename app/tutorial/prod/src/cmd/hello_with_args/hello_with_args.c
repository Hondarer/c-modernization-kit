/**
 *******************************************************************************
 *  @file           hello_with_args.c
 *  @brief          メッセージと指定された引数を表示するサンプル コマンドを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/04/23
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <cplat/argparser/argparser.h>
#include <cplat/console/console.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *  @brief          アプリケーションのエントリ ポイント。
 *
 *  @param[in]      argc コマンド ライン引数の個数。
 *  @param[in]      argv コマンド ライン引数の文字列の配列。
 *  @return         常に 0 を返します。
 */
int main(int argc, char *argv[])
{
    cplat_console_init();

    int need_help = 0;
    const char *message = NULL;

    cplat_argparser_init(argc, argv, "メッセージと指定された引数を表示します。");
    cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します。", &need_help);
    cplat_argparser_register_positional_string("message", "表示する文字列。", CPLAT_ARGPARSER_REQUIRED, &message);

    if (cplat_argparser_get_register_error_count() > 0)
    {
        cplat_argparser_print_register_error_messages(stderr);
        return EXIT_FAILURE;
    }

    int parse_result = cplat_argparser_parse();

    if (need_help != 0)
    {
        cplat_argparser_print_usage(stdout);
        return EXIT_SUCCESS;
    }

    if (parse_result != CPLAT_OK)
    {
        cplat_argparser_print_error_messages(stderr);
        cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    printf("✨ Hello, c-modernization-kit! ✨\n");

    printf("echo: %s\n", message);

    return EXIT_SUCCESS;
}
