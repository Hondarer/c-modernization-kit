/**
 *******************************************************************************
 *  @file           funcman_init.c
 *  @brief          設定テキストファイルから funcman_object エントリを読み込む。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  テキストファイルから func_key / lib_name / func_name を
 *  読み込み、_func_objects 配列の対応エントリに設定します。\n
 *
 *  ファイルフォーマット:\n
    @code
    # コメント行
    func_key  lib_name  func_name   # 行末コメント
    @endcode
 *
 *  - '#' で始まる行はコメント行として無視する。\n
 *  - 行中の '#' 以降を行末コメントとして切り捨てる。\n
 *  - sscanf で func_key / lib_name / func_name の 3 フィールドを解析する。\n
 *  - func_key が一致するキャッシュエントリの lib_name / func_name 配列に
 *    strncpy で書き込む。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <libbase.h>
#include <stdio.h>
#include <string.h>

/** fgets で読み込む行バッファの最大長 */
#define CONFIG_LINE_MAX 1024

/* doxygen コメントは、ヘッダに記載 */
void funcman_init(funcman_object *const *fobj_array, const size_t fobj_length, const char *configpath)
{
    FILE *fp;
    char line[CONFIG_LINE_MAX];
    char func_key[FUNCMAN_NAME_MAX];
    char lib_name[FUNCMAN_NAME_MAX];
    char func_name[FUNCMAN_NAME_MAX];
    char *comment;
    size_t fobj_index;

    fp = fopen(configpath, "r");
    if (fp == NULL)
    {
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        /* '#' 以降をコメントとして切り捨てる (行頭 '#' も同様に処理される) */
        comment = strchr(line, '#');
        if (comment != NULL)
        {
            *comment = '\0';
        }

        /* func_key lib_name func_name の 3 フィールドを解析 */
        if (sscanf(line, "%255s %255s %255s", func_key, lib_name, func_name) != 3)
        {
            /* 空行・コメント行・フィールドが不足している行はスキップ */
            continue;
        }

        /* func_key が一致するキャッシュを検索し、配列に書き込む */
        for (fobj_index = 0; fobj_index < fobj_length; fobj_index++)
        {
            funcman_object *cache = fobj_array[fobj_index];
            if (cache->func_key == NULL || strcmp(cache->func_key, func_key) != 0)
            {
                continue;
            }

            strncpy(cache->lib_name, lib_name, FUNCMAN_NAME_MAX - 1);
            cache->lib_name[FUNCMAN_NAME_MAX - 1] = '\0';
            strncpy(cache->func_name, func_name, FUNCMAN_NAME_MAX - 1);
            cache->func_name[FUNCMAN_NAME_MAX - 1] = '\0';
            break;
        }
    }

    fclose(fp);
    return;
}
