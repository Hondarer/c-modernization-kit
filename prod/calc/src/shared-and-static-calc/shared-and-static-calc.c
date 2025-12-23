/**
 *******************************************************************************
 *  @file           src/shared-and-static-calc/shared-and-static-calc.c
 *  @brief          動的リンク、静的リンクを使った関数の呼び出しコマンド。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  コマンドライン引数から 2 つの整数を受け取り、
 *  calc 関数、add, subtract, multiply, divide 関数を使用して
 *  加算結果を標準出力に出力します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalcbase.h>
#include <libcalc.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *******************************************************************************
 *  @brief          プログラムのエントリーポイント。
 *  @param[in]      argc コマンドライン引数の数。
 *  @param[in]      argv コマンドライン引数の配列。
 *  @return         成功時は 0、失敗時は 0 以外の値を返します。
 *
 *  @details
 *  使用例:
 *
 *  @code{.c}
 *  ./add 10 + 20
 *  // 出力: 30
 *  @endcode
 *
 *  @attention      引数は正確に 3 つ必要です。
 *******************************************************************************
 */
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <arg1> <arg2> <arg3>\n", argv[0]);
        return 1;
    }

    if (argv[2][0] == 0x00 || argv[2][1] != 0x00)
    {
        fprintf(stderr, "Usage: %s <arg1> <arg2> <arg3>\n", argv[0]);
        return 1;
    }

    int arg1 = atoi(argv[1]);
    int arg3 = atoi(argv[3]);

    switch (argv[2][0])
    {
    case '+':
    {
        int result_shared;
        if (calcHandler(CALC_KIND_ADD, arg1, arg3, &result_shared) != 0)
        {
            fprintf(stderr, "Error: calcHandler failed\n");
            return 1;
        }
        printf("result_shared: %d\n", result_shared);

        int result_static;
        if (add(arg1, arg3, &result_static) != 0)
        {
            fprintf(stderr, "Error: add failed\n");
            return 1;
        }
        printf("result_static: %d\n", result_static);

        break;
    }
    case '-':
    {
        int result_shared;
        if (calcHandler(CALC_KIND_SUBTRACT, arg1, arg3, &result_shared) != 0)
        {
            fprintf(stderr, "Error: calcHandler failed\n");
            return 1;
        }
        printf("result_shared: %d\n", result_shared);

        int result_static;
        if (subtract(arg1, arg3, &result_static) != 0)
        {
            fprintf(stderr, "Error: subtract failed\n");
            return 1;
        }
        printf("result_static: %d\n", result_static);

        break;
    }
    case 'x':
    {
        int result_shared;
        if (calcHandler(CALC_KIND_MULTIPLY, arg1, arg3, &result_shared) != 0)
        {
            fprintf(stderr, "Error: calcHandler failed\n");
            return 1;
        }
        printf("result_shared: %d\n", result_shared);

        int result_static;
        if (multiply(arg1, arg3, &result_static) != 0)
        {
            fprintf(stderr, "Error: multiply failed\n");
            return 1;
        }
        printf("result_static: %d\n", result_static);

        break;
    }
    case '/':
    {
        int result_shared;
        if (calcHandler(CALC_KIND_DIVIDE, arg1, arg3, &result_shared) != 0)
        {
            fprintf(stderr, "Error: calcHandler failed\n");
            return 1;
        }
        printf("result_shared: %d\n", result_shared);

        int result_static;
        if (divide(arg1, arg3, &result_static) != 0)
        {
            fprintf(stderr, "Error: divide failed\n");
            return 1;
        }
        printf("result_static: %d\n", result_static);

        break;
    }
    default:
        fprintf(stderr, "Usage: %s <arg1> <arg2> <arg3>\n", argv[0]);
        return 1;
        break;
    }

    return 0;
}
