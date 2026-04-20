/**
 *******************************************************************************
 *  @file           symbol_loader_init.c
 *  @brief          設定テキストファイルから symbol_loader_entry_t エントリを読み込む。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  テキストファイルから func_key / lib_name / func_name を
 *  読み込み、_func_objects 配列の対応エントリに設定します。\n
 *
 *  ファイルフォーマット:\n
 *  @code
    # コメント行
    func_key  lib_name  func_name   # 行末コメント
 *  @endcode
 *
 *  - '#' で始まる行はコメント行として無視する。\n
 *  - 行中の '#' 以降を行末コメントとして切り捨てる。\n
 *  - sscanf で func_key / lib_name / func_name の 3 フィールドを解析する。\n
 *  - func_key が一致するキャッシュエントリの lib_name / func_name 配列に
 *    strncpy で書き込む。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <com_util/runtime/symbol_loader.h>
#include <com_util/fs/file_io.h>
#include <stdio.h>
#include <string.h>

/** fgets で読み込む行バッファの最大長 */
#define CONFIG_LINE_MAX 1024

/* doxygen コメントは、ヘッダに記載 */
COM_UTIL_EXPORT void COM_UTIL_API symbol_loader_init(symbol_loader_entry_t *const *fobj_array,
                                                               const size_t                  fobj_length,
                                                               const char                   *configpath)
{
    FILE *fp;
    char line[CONFIG_LINE_MAX];
    char func_key[SYMBOL_LOADER_NAME_MAX];
    char lib_name[SYMBOL_LOADER_NAME_MAX];
    char func_name[SYMBOL_LOADER_NAME_MAX];
    char *comment;
    size_t fobj_index;

    fp = com_util_fopen(configpath, "r", NULL);
    if (fp == NULL)
    {
        return;
    }

    while (com_util_fgets(line, sizeof(line), fp) != NULL)
    {
        /* '#' 以降をコメントとして切り捨てる (行頭 '#' も同様に処理される) */
        comment = strchr(line, '#');
        if (comment != NULL)
        {
            *comment = '\0';
        }

        /* func_key lib_name func_name の 3 フィールドを解析 */
#if defined(PLATFORM_LINUX)
        if (sscanf(line, "%255s %255s %255s", func_key, lib_name, func_name) != 3)
#elif defined(PLATFORM_WINDOWS)
        if (sscanf_s(line, "%255s %255s %255s",
                     func_key, (unsigned)sizeof(func_key),
                     lib_name, (unsigned)sizeof(lib_name),
                     func_name, (unsigned)sizeof(func_name)) != 3)
#endif /* PLATFORM_ */
        {
            /* 空行・コメント行・フィールドが不足している行はスキップ */
            continue;
        }

        /* func_key が一致するキャッシュを検索し、配列に書き込む */
        for (fobj_index = 0; fobj_index < fobj_length; fobj_index++)
        {
            symbol_loader_entry_t *cache = fobj_array[fobj_index];
            if (cache->func_key == NULL || strcmp(cache->func_key, func_key) != 0)
            {
                continue;
            }

#if defined(PLATFORM_LINUX)
            strncpy(cache->lib_name, lib_name, SYMBOL_LOADER_NAME_MAX - 1);
            cache->lib_name[SYMBOL_LOADER_NAME_MAX - 1] = '\0';
            strncpy(cache->func_name, func_name, SYMBOL_LOADER_NAME_MAX - 1);
            cache->func_name[SYMBOL_LOADER_NAME_MAX - 1] = '\0';
#elif defined(PLATFORM_WINDOWS)
            strncpy_s(cache->lib_name, SYMBOL_LOADER_NAME_MAX, lib_name, SYMBOL_LOADER_NAME_MAX - 1);
            strncpy_s(cache->func_name, SYMBOL_LOADER_NAME_MAX, func_name, SYMBOL_LOADER_NAME_MAX - 1);
#endif /* PLATFORM_ */
            break;
        }
    }

    com_util_fclose(fp);
    return;
}
