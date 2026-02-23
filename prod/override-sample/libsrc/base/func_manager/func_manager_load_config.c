/**
 *******************************************************************************
 *  @file           func_manager_load_config.c
 *  @brief          設定テキストファイルから func_object エントリを読み込む。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  /tmp/libbase_ext.txt 等のテキストファイルから func_key / lib_name / func_name を
 *  読み込み、_func_objects 配列の対応エントリに設定します。\n
 *
 *  ファイルフォーマット:\n
 *  @code
 *  # コメント行
 *  func_key  lib_name  func_name   # 行末コメント
 *  @endcode
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

#include "func_manager_local.h"
#include <func_manager.h>
#include <stdio.h>
#include <string.h>

/** fgets で読み込む行バッファの最大長 */
#define CONFIG_LINE_MAX 1024

/* doxygen コメントは、ヘッダに記載 */
int func_manager_load_config(const char *configpath)
{
    FILE *fp;
    char line[CONFIG_LINE_MAX];
    char func_key[FUNC_MANAGER_NAME_MAX];
    char lib_name[FUNC_MANAGER_NAME_MAX];
    char func_name[FUNC_MANAGER_NAME_MAX];
    char *comment;
    size_t cache_index;
    int loaded = 0;

    fp = fopen(configpath, "r");
    if (fp == NULL)
    {
        return -1;
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
        for (cache_index = 0; cache_index < _func_objects_count; cache_index++)
        {
            func_object *cache = _func_objects[cache_index];
            if (cache->func_key == NULL || strcmp(cache->func_key, func_key) != 0)
            {
                continue;
            }

            strncpy(cache->lib_name, lib_name, FUNC_MANAGER_NAME_MAX - 1);
            cache->lib_name[FUNC_MANAGER_NAME_MAX - 1] = '\0';
            strncpy(cache->func_name, func_name, FUNC_MANAGER_NAME_MAX - 1);
            cache->func_name[FUNC_MANAGER_NAME_MAX - 1] = '\0';
            loaded++;
            break;
        }
    }

    fclose(fp);
    return loaded;
}
