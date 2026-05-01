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
#include <errno.h>
#include <stdio.h>

/**
 *******************************************************************************
 *  @brief          メインエントリポイント。
 *  @return         正常終了時は 0 を返します。
 *******************************************************************************
 */
int main(void)
{
    com_util_console_init();

    int err = 0;
    int result;
    int rtc;
    char configpath[PLATFORM_PATH_MAX];

    {
        char tmpdir[PLATFORM_PATH_MAX];
        if (com_util_get_temp_dir(tmpdir, sizeof(tmpdir), &err) == 0)
        {
            if (com_util_path_concat(configpath, sizeof(configpath), &err,
                                     tmpdir, PLATFORM_PATH_SEP, "libbase_extdef.txt") != 0)
            {
                fprintf(stderr, "failed to build config path: exceeds PLATFORM_PATH_MAX\n");
                return 1;
            }
        }
        else if (err == ENAMETOOLONG)
        {
            fprintf(stderr, "failed to build config path: exceeds PLATFORM_PATH_MAX\n");
            return 1;
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

    printf("--- sym_loader info ---\n");
    rtc = sym_loader_info_libbase();
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
