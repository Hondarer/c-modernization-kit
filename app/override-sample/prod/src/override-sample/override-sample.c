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
#include <com_util/console/console.h>
#include <com_util/crt/path.h>
#include <stdio.h>

/**
 *******************************************************************************
 *  @brief          メインエントリポイント。
 *  @return         正常終了時は 0 を返します。
 *******************************************************************************
 */
int main(void)
{
    console_init();

    int result;
    int rtc;
    char configpath[4096];

#if defined(PLATFORM_LINUX)
    snprintf(configpath, sizeof(configpath), "/tmp/libbase_extdef.txt");
#elif defined(PLATFORM_WINDOWS)
    {
        wchar_t tmpw[PLATFORM_PATH_MAX] = L"";
        DWORD n = GetTempPathW((DWORD)(sizeof(tmpw) / sizeof(tmpw[0])), tmpw);
        configpath[0] = '\0';
        if (n > 0 && n < (DWORD)(sizeof(tmpw) / sizeof(tmpw[0])))
        {
            char tmpu8[PLATFORM_PATH_MAX * 4] = {0};
            int m = WideCharToMultiByte(CP_UTF8, 0, tmpw, -1, tmpu8, (int)sizeof(tmpu8), NULL, NULL);
            if (m > 0)
                snprintf(configpath, sizeof(configpath), "%slibbase_extdef.txt", tmpu8);
        }
    }
#endif /* PLATFORM_ */

    printf("configpath: %s\n", configpath);
    printf("Processing will be extended if defines.\n");
    printf(" e.g.  echo \"sample_func liboverride override_func\" > \"%s\"\n", configpath);
#if defined(PLATFORM_LINUX)
    printf("       rm \"%s\"\n\n", configpath);
#elif defined(PLATFORM_WINDOWS)
    printf("       del \"%s\"\n\n", configpath);
#endif /* PLATFORM_ */

    printf("--- symbol_loader info ---\n");
    rtc = symbol_loader_info_libbase();
    printf("rtc: %d\n\n", rtc);

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
