/**
 * @file main.c
 * @brief サブフォルダ make 検証用メインプログラム
 */

#include <stdio.h>

#include "sample-app.h"

/**
 * @brief メイン関数
 * @return 0: 正常終了
 */
int main(void)
{
    int a = 10;
    int b = 20;

    printf("Testing subfolder make for src\n");
    printf("helper_a(%d) = %d\n", a, helper_a(a));
    printf("helper_b(%d) = %d\n", b, helper_b(b));
    printf("helper_a(%d) + helper_b(%d) = %d\n", a, b, helper_a(a) + helper_b(b));

    return 0;
}
