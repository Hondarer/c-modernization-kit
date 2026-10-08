/**
 *******************************************************************************
 *  @file           shared-and-static-calc.c
 *  @brief          動的リンクと静的リンクの計算関数を呼び出すコマンドを実装します。
 *  @author         c-modernization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  コマンド ライン引数から 2 つの整数を受け取り、
 *  calc 関数、calcbase_add, calcbase_subtract, calcbase_multiply, calcbase_divide 関数を使用して
 *  加算結果を標準出力に出力します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <calc.h>
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
    ./calcbase_add 10 + 20
    // 出力: 30
    @endcode
 *
 *  @attention      引数は正確に 3 つ必要です。
 */
int main(int argc, char *argv[])
{
    cplat_console_init();

    int need_help = 0;
    int arg1 = 0;
    int arg3 = 0;
    const char *operator_value = NULL;
    cplat_argparser_init(argc, argv, "動的リンクと静的リンクの計算結果を比較します。");
    cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します。", &need_help);
    cplat_argparser_register_positional_int("num1", "第一オペランド。", CPLAT_ARGPARSER_REQUIRED, &arg1);
    cplat_argparser_register_positional_string("operator", "+、-、x、/ のいずれか。", CPLAT_ARGPARSER_REQUIRED,
                                               &operator_value);
    cplat_argparser_register_positional_int("num2", "第二オペランド。", CPLAT_ARGPARSER_REQUIRED, &arg3);
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
    if (operator_value[0] == 0x00 || operator_value[1] != 0x00)
    {
        fprintf(stderr, "Error: operator must be one of +, -, x, /.\n\n");
        cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    switch (operator_value[0])
    {
    case '+':
    {
        int result_shared;
        if (calc_handler(CALC_KIND_ADD, arg1, arg3, &result_shared) != CALC_OK)
        {
            fprintf(stderr, "Error: calc_handler failed\n");
            return EXIT_FAILURE;
        }
        printf("result_shared: %d\n", result_shared);

        int result_static;
        if (calcbase_add(arg1, arg3, &result_static) != CALC_OK)
        {
            fprintf(stderr, "Error: calcbase_add failed\n");
            return EXIT_FAILURE;
        }
        printf("result_static: %d\n", result_static);

        break;
    }
    case '-':
    {
        int result_shared;
        if (calc_handler(CALC_KIND_SUBTRACT, arg1, arg3, &result_shared) != CALC_OK)
        {
            fprintf(stderr, "Error: calc_handler failed\n");
            return EXIT_FAILURE;
        }
        printf("result_shared: %d\n", result_shared);

        int result_static;
        if (calcbase_subtract(arg1, arg3, &result_static) != CALC_OK)
        {
            fprintf(stderr, "Error: calcbase_subtract failed\n");
            return EXIT_FAILURE;
        }
        printf("result_static: %d\n", result_static);

        break;
    }
    case 'x':
    {
        int result_shared;
        if (calc_handler(CALC_KIND_MULTIPLY, arg1, arg3, &result_shared) != CALC_OK)
        {
            fprintf(stderr, "Error: calc_handler failed\n");
            return EXIT_FAILURE;
        }
        printf("result_shared: %d\n", result_shared);

        int result_static;
        if (calcbase_multiply(arg1, arg3, &result_static) != CALC_OK)
        {
            fprintf(stderr, "Error: calcbase_multiply failed\n");
            return EXIT_FAILURE;
        }
        printf("result_static: %d\n", result_static);

        break;
    }
    case '/':
    {
        int result_shared;
        if (calc_handler(CALC_KIND_DIVIDE, arg1, arg3, &result_shared) != CALC_OK)
        {
            fprintf(stderr, "Error: calc_handler failed\n");
            return EXIT_FAILURE;
        }
        printf("result_shared: %d\n", result_shared);

        int result_static;
        if (calcbase_divide(arg1, arg3, &result_static) != CALC_OK)
        {
            fprintf(stderr, "Error: calcbase_divide failed\n");
            return EXIT_FAILURE;
        }
        printf("result_static: %d\n", result_static);

        break;
    }
    default:
        fprintf(stderr, "Error: operator must be one of +, -, x, /.\n\n");
        cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
