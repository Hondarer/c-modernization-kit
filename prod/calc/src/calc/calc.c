/**
 *******************************************************************************
 *  @file           src/calc/calc.c
 *  @brief          calc 関数の呼び出しコマンド。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  コマンドライン引数から 2 つの整数と演算子を受け取り、calcHandler 関数を使用して
 *  計算結果を標準出力に出力します。
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
 *  @return         成功時は 0、失敗時は 0 以外の値を返します。
 *
 *  @details        以下に、calc コマンドの処理フローを示します。
 *
 *  @image          html calc-flow.png "calc コマンドの処理フロー"
 *
 *  使用例:
 *
    @code{.c}
    ./add 10 + 20
    // 出力: 30
    @endcode
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

    int kind;
    switch (argv[2][0])
    {
    case '+':
        kind = CALC_KIND_ADD;
        break;
    case '-':
        kind = CALC_KIND_SUBTRACT;
        break;
    case 'x':
        kind = CALC_KIND_MULTIPLY;
        break;
    case '/':
        kind = CALC_KIND_DIVIDE;
        break;
    default:
        fprintf(stderr, "Usage: %s <num1> <+|-|x|/> <num2>\n", argv[0]);
        return 1;
        break;
    }

    if (calcHandler(kind, arg1, arg3, &result) != 0)
    {
        fprintf(stderr, "Error: calcHandler failed\n");
        return 1;
    }

    printf("%d\n", result);

    return 0;
}
