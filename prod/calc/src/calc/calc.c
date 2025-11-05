/**
 *******************************************************************************
 *  @file           src/calc/calc.c
 *  @brief          calc 関数の呼び出しコマンド。
 *  @author         doxygen-sample team
 *  @date           2025/11/06
 *  @version        1.0.0
 *
 *  コマンドライン引数から 2 つの整数を受け取り、calc 関数を使用して
 *  加算結果を標準出力に出力します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalc.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *******************************************************************************
 *  @brief          プログラムのエントリーポイント。
 *  @param[in]      argc コマンドライン引数の数。
 *  @param[in]      argv コマンドライン引数の配列。
 *  @return         成功時は 0、失敗時は 1 を返します。
 *
 *  @details
 *  使用例:
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
    int result;

    switch (argv[2][0])
    {
    case '+':
        result = calcHandler(CALC_KIND_ADD, arg1, arg3);
        break;
    default:
        fprintf(stderr, "Usage: %s <arg1> <arg2> <arg3>\n", argv[0]);
        return 1;
        break;
    }

    printf("%d\n", result);

    return 0;
}
