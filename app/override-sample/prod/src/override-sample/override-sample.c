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

    {
        char tmpdir[PLATFORM_PATH_MAX];
        if (com_util_get_temp_dir(tmpdir, sizeof(tmpdir), NULL) == 0)
        {
            snprintf(configpath, sizeof(configpath),
                     "%s" PLATFORM_PATH_SEP "libbase_extdef.txt", tmpdir);
        }
        else
        {
            configpath[0] = '\0';
        }
    }

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
