/**
 *******************************************************************************
 *  @file           override-sample.c
 *  @brief          関数の動的オーバーライドのサンプル コマンドを実装します。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  libbase の base_calc を呼び出し、オーバーライド機能を示すサンプル プログラムです。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <base.h>
#include <cplat/argparser/argparser.h>
#include <cplat/base/error.h>
#include <cplat/console/console.h>
#include <cplat/crt/path.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *  @brief          メイン エントリ ポイントです。
 *  @param[in]      argc コマンド ライン引数の数。
 *  @param[in]      argv コマンド ライン引数の配列。
 *  @return         正常終了時は 0 を返します。
 */
int main(int argc, char *argv[])
{
    cplat_console_init();

    int need_help = 0;

    cplat_argparser_init(argc, argv, "関数の動的オーバーライドのサンプル コマンド。");
    cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します。", &need_help);

    if (cplat_argparser_get_register_error_count() > 0)
    {
        cplat_argparser_print_register_error_messages(stderr);
        return EXIT_FAILURE;
    }

    int parse_result = cplat_argparser_parse();

    if (need_help != 0)
    {
        cplat_argparser_print_usage(stdout);
        return EXIT_SUCCESS;
    }

    if (parse_result != CPLAT_OK)
    {
        cplat_argparser_print_error_messages(stderr);
        cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    int result;
    int ret;
    char configpath[PLATFORM_PATH_MAX];

    {
        cplat_error error;
        char tmpdir[PLATFORM_PATH_MAX];
        if (cplat_get_temp_dir(tmpdir, sizeof(tmpdir), &error) == CPLAT_OK)
        {
            if (cplat_path_concat(configpath, sizeof(configpath), &error, tmpdir, PLATFORM_PATH_SEP,
                                     "libbase_extdef.json") != CPLAT_OK)
            {
                fprintf(stderr, "failed to build config path: exceeds PLATFORM_PATH_MAX\n");
                return EXIT_FAILURE;
            }
        }
        else if (cplat_error_is(&error, CPLAT_CAUSE_NAME_TOO_LONG) != 0)
        {
            fprintf(stderr, "failed to build config path: exceeds PLATFORM_PATH_MAX\n");
            return EXIT_FAILURE;
        }
        else
        {
            configpath[0] = '\0';
        }
    }

    printf("configpath: %s\n", configpath);
    printf("Processing will be extended if defines.\n");
    printf(" e.g.  printf '{\"base_calc\":{\"lib\":\"liboverride\",\"func\":\"override_calc\"}}\\n'"
           " > \"%s\"\n",
           configpath);
#if defined(PLATFORM_LINUX)
    printf("       rm \"%s\"\n\n", configpath);
#elif defined(PLATFORM_WINDOWS)
    printf("       del \"%s\"\n\n", configpath);
#endif /* PLATFORM_ */

    printf("--- sym_loader info ---\n");
    ret = base_sym_loader_info();
    printf("ret: %d\n\n", ret);

    ret = base_calc(1, 2, &result);
    base_console_output("ret: %d\n", ret);
    if (ret != BASE_OK)
    {
        fprintf(stderr, "func failed (base_calc(1, 2, &result))\n");
    }
    else
    {
        printf("result: %d\n", result);
    }

    return EXIT_SUCCESS;
}
