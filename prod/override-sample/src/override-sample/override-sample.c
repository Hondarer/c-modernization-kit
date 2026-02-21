/**
 *******************************************************************************
 *  @file           override-sample.c
 *  @brief          override-sample メインプログラム。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase の func を呼び出し、オーバーライド機能を示すサンプルプログラムです。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <libbase.h>
#include <stdio.h>

/**
 *******************************************************************************
 *  @brief          メインエントリポイント。
 *  @return         正常終了時は 0 を返します。
 *
 *  @details
 *  useOverride=0 と useOverride=1 の両パターンで func を呼び出し、\n
 *  動的ライブラリのオーバーライド機能を示します。
 *******************************************************************************
 */
int main(void)
{
    int result;
    int rtc;

    rtc = func(0, 1, 2, &result);
    console_output("rtc: %d\n", rtc);
    if (rtc != 0)
    {
        fprintf(stderr, "func failed (useOverride=0)\n");
    }
    else
    {
        console_output("result: %d\n", result);
    }

    rtc = func(1, 1, 2, &result);
    console_output("rtc: %d\n", rtc);
    if (rtc != 0)
    {
        fprintf(stderr, "func failed (useOverride=1)\n");
    }
    else
    {
        console_output("result: %d\n", result);
    }

    return 0;
}
