/**
 *******************************************************************************
 *  @file           add.c
 *  @brief          2 つの整数を加算するコマンドを実装します。
 *  @author         c-modernization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  コマンド ライン引数から 2 つの整数を受け取り、calcbase_add 関数を使用して
 *  加算結果を標準出力に出力します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <calcbase.h>
#include <cplat/argparser/argparser.h>
#include <cplat/console/console.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *  @brief          プログラムのエントリ ポイント。
 *  @param[in]      argc コマンド ライン引数の数。
 *  @param[in]      argv コマンド ライン引数の配列。
 *  @return         成功時は 0、失敗時は 0 以外の値を返します。
 *
 *  使用例:
 *
    @code{.c}
    ./calcbase_add 10 20
    // 出力: 30
    @endcode
 *
 *  @attention      引数は正確に 2 つ必要です。
 */
int main(int argc, char *argv[])
{
    cplat_console_init();

    int need_help = 0;
    int arg1 = 0;
    int arg2 = 0;
    cplat_argparser_init(argc, argv, "2 つの整数を加算します。");
    cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します。", &need_help);
    cplat_argparser_register_positional_int("arg1", "第一オペランド。", CPLAT_ARGPARSER_REQUIRED, &arg1);
    cplat_argparser_register_positional_int("arg2", "第二オペランド。", CPLAT_ARGPARSER_REQUIRED, &arg2);
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

    int result;

    if (calcbase_add(arg1, arg2, &result) != CALC_OK)
    {
        fprintf(stderr, "Error: calcbase_add failed\n");
        return EXIT_FAILURE;
    }

    printf("%d\n", result);

    return EXIT_SUCCESS;
}
