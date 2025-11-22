/**
 *******************************************************************************
 *  @file           src/add/add.c
 *  @brief          add 関数の呼び出しコマンド。
 *  @author         c-modenization-kit sample team
 *  @date           2025/11/22
 *  @version        1.0.0
 *
 *  コマンドライン引数から 2 つの整数を受け取り、add 関数を使用して
 *  加算結果を標準出力に出力します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2025. All rights reserved.
 *
 *******************************************************************************
 */

#include <libcalcbase.h>
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
 *  @code{.c}
 *  ./add 10 20
 *  // 出力: 30
 *  @endcode
 *
 *  @attention      引数は正確に 2 つ必要です。
 *******************************************************************************
 */
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <arg1> <arg2>\n", argv[0]);
        return 1;
    }

    int arg1 = atoi(argv[1]);
    int arg2 = atoi(argv[2]);
    int result;

    if (add(arg1, arg2, &result) != 0)
    {
        fprintf(stderr, "Error: add failed\n");
        return 1;
    }

    printf("%d\n", result);

    return 0;
}
