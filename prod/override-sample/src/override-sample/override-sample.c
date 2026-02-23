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
#ifdef _WIN32
    #include <windows.h>
#endif

/**
 *******************************************************************************
 *  @brief          メインエントリポイント。
 *  @return         正常終了時は 0 を返します。
 *******************************************************************************
 */
int main(void)
{
    int result;
    int rtc;
    char configpath[4096];

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); /* コンソールの出力コードページを utf-8 に設定する */
#endif

#ifndef _WIN32
    snprintf(configpath, sizeof(configpath), "/tmp/libbase_extdef.txt");
#else
    {
        wchar_t tmpw[MAX_PATH] = L"";
        DWORD n = GetTempPathW((DWORD)(sizeof(tmpw) / sizeof(tmpw[0])), tmpw);
        configpath[0] = '\0';
        if (n > 0 && n < (DWORD)(sizeof(tmpw) / sizeof(tmpw[0])))
        {
            char tmpu8[MAX_PATH * 4] = {0};
            int m = WideCharToMultiByte(CP_UTF8, 0, tmpw, -1, tmpu8, (int)sizeof(tmpu8), NULL, NULL);
            if (m > 0)
                snprintf(configpath, sizeof(configpath), "%slibbase_extdef.txt", tmpu8);
        }
    }
#endif

    printf("configpath: %s\n", configpath);
    printf("Processing will be extended if configpath defines sample_func, liboverride, or override_func.\n");
    printf(" e.g.  echo \"sample_func liboverride override_func\" > \"%s\"\n", configpath);
#ifndef _WIN32
    printf("       rm \"%s\"\n\n", configpath);
#else
    printf("       del \"%s\"\n\n", configpath);
#endif

    printf("--- funcman info ---\n");
    funcman_info_libbase();
    printf("\n");

    rtc = sample_func(1, 2, &result);
    console_output("rtc: %d\n", rtc);
    if (rtc != 0)
    {
        fprintf(stderr, "func failed (sample_func(1, 2, &result))\n");
    }
    else
    {
        printf("result: %d\n", result);
    }

    return 0;
}
