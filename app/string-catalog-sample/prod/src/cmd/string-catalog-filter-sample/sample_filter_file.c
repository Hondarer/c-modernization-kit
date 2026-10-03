/**
 *******************************************************************************
 *  @file           sample_filter_file.c
 *  @brief          条件式リストのファイルの読み込みと書き出しを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/10/03
 *  @version        0.1.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_filter_file.h"

#include <cplat/base/result.h>
#include <cplat/crt/stdio.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** UTF-8 の BOM です。 */
static const unsigned char s_utf8_bom[] = {0xEFU, 0xBBU, 0xBFU};

/** 復元した 1 行を格納する大きさです。括弧を補うため、行幅の上限より大きく取ります。 */
#define DECOMPILE_BUFFER_SIZE ((size_t)CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX * 2U)

/** 行末の改行 (LF または CRLF) を取り除きます。 */
static void trim_newline(char *line)
{
    size_t length = strlen(line);

    while ((length > 0U) && ((line[length - 1U] == '\n') || (line[length - 1U] == '\r')))
    {
        length--;
        line[length] = '\0';
    }
}

/**
 *  @brief          ファイルの行を、行幅ごとの条件式リストの配列へ読み込みます。
 *  @param[in]      stream          読み込むストリーム。
 *  @param[in]      line_width      行幅。
 *  @param[out]     rows            条件式リストの配列。@ref SAMPLE_FILTER_FILE_LINE_MAX 行分を 0 で埋めておきます。
 *  @param[out]     too_long        行幅を超えた行の印。@ref SAMPLE_FILTER_FILE_LINE_MAX 要素。
 *  @param[out]     line_count_out  読み込んだ行数。
 *  @return         成功時は `CPLAT_OK`、行数の上限を超えた場合は `CPLAT_ERR_LIMIT_EXCEEDED`、
 *                  読み取りに失敗した場合は `CPLAT_ERR_UNKNOWN` を返します。
 */
static int read_rows(FILE *stream, const size_t line_width, char *rows, bool *too_long, size_t *line_count_out)
{
    /* 行幅いっぱいの行に、BOM (3 バイト)、改行 (最大 2 バイト)、NUL を加えても収まる大きさ */
    char buffer[CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX + 8U];
    size_t line_count = 0U;

    for (;;)
    {
        char *row;
        int ret = cplat_fgets(buffer, line_width + 6U, stream, NULL);
        bool is_overflowed = false;

        if (ret == CPLAT_ERR_EOF)
        {
            break;
        }
        if (line_count >= SAMPLE_FILTER_FILE_LINE_MAX)
        {
            return CPLAT_ERR_LIMIT_EXCEEDED;
        }

        /* 収まらない行は、行の残りを読み捨てて、行幅を超えた行として扱う */
        while (ret == CPLAT_ERR_BUFFER_TOO_SMALL)
        {
            char rest[64];

            is_overflowed = true;
            ret = cplat_fgets(rest, sizeof(rest), stream, NULL);
        }
        if ((ret != CPLAT_OK) && (ret != CPLAT_ERR_EOF))
        {
            return CPLAT_ERR_UNKNOWN;
        }

        trim_newline(buffer);
        if ((line_count == 0U) && (memcmp(buffer, s_utf8_bom, sizeof(s_utf8_bom)) == 0))
        {
            memmove(buffer, buffer + sizeof(s_utf8_bom), strlen(buffer) - sizeof(s_utf8_bom) + 1U);
        }

        row = rows + (line_count * line_width);
        if (is_overflowed || (strlen(buffer) > line_width))
        {
            too_long[line_count] = true;
        }
        else
        {
            memcpy(row, buffer, strlen(buffer));
        }
        line_count++;
    }

    *line_count_out = line_count;
    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_file_load(const char *path, const size_t line_capacity, const size_t line_width, void *image,
                            const size_t image_size, cplat_string_catalog_filter_diagnostic *diagnostics,
                            const size_t diagnostic_capacity, sample_filter_file_result *result_out)
{
    cplat_string_catalog_filter_diagnostic *compiled_diagnostics = NULL;
    cplat_string_catalog_filter_info info;
    unsigned char *compiled = NULL;
    bool *too_long = NULL;
    char *rows = NULL;
    FILE *stream;
    size_t line_count = 0U;
    size_t compiled_invalid = 0U;
    size_t merged = 0U;
    size_t compiled_index = 0U;
    int ret;

    if ((path == NULL) || (image == NULL) || (line_width < CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MIN) ||
        (line_width > CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    stream = cplat_fopen(path, "rb", NULL);
    if (stream == NULL)
    {
        return CPLAT_ERR_UNKNOWN;
    }

    rows = (char *)calloc(SAMPLE_FILTER_FILE_LINE_MAX, line_width);
    too_long = (bool *)calloc(SAMPLE_FILTER_FILE_LINE_MAX, sizeof(*too_long));
    compiled_diagnostics =
        (cplat_string_catalog_filter_diagnostic *)calloc(SAMPLE_FILTER_FILE_LINE_MAX, sizeof(*compiled_diagnostics));
    compiled = (unsigned char *)malloc(image_size);
    if ((rows == NULL) || (too_long == NULL) || (compiled_diagnostics == NULL) || (compiled == NULL))
    {
        ret = CPLAT_ERR_OUT_OF_MEMORY;
    }
    else
    {
        ret = read_rows(stream, line_width, rows, too_long, &line_count);
    }
    (void)cplat_fclose(stream, NULL);

    /* 失敗した場合に格納先を変えないよう、手元の領域へコンパイルしてから複製する */
    if (ret == CPLAT_OK)
    {
        ret = cplat_string_catalog_filter_compile(rows, line_count, line_width, line_capacity, compiled, image_size,
                                                  compiled_diagnostics, SAMPLE_FILTER_FILE_LINE_MAX, &compiled_invalid);
    }

    if (ret == CPLAT_OK)
    {
        /* 行幅を超えた行と、コンパイルで無効にした行を、ファイルの行の順に並べる。
           行幅を超えた行は空行としてコンパイルしたため、両者の行は重ならない */
        for (size_t line_index = 0; line_index < line_count; line_index++)
        {
            cplat_string_catalog_filter_diagnostic entry;

            if (too_long[line_index])
            {
                entry.line_index = (uint32_t)line_index;
                entry.column = (uint32_t)line_width;
                entry.error = CPLAT_STRING_CATALOG_FILTER_LINE_ERROR_LIMIT_EXCEEDED;
            }
            else if ((compiled_index < compiled_invalid) &&
                     (compiled_diagnostics[compiled_index].line_index == (uint32_t)line_index))
            {
                entry = compiled_diagnostics[compiled_index];
                compiled_index++;
            }
            else
            {
                continue;
            }

            if ((diagnostics != NULL) && (merged < diagnostic_capacity))
            {
                diagnostics[merged] = entry;
            }
            merged++;
        }

        memcpy(image, compiled, image_size);
        if (result_out != NULL)
        {
            (void)cplat_string_catalog_filter_get_info(image, image_size, &info);
            result_out->line_count = line_count;
            result_out->condition_count = info.line_count;
            result_out->invalid_count = merged;
        }
    }

    free(compiled);
    free(compiled_diagnostics);
    free(too_long);
    free(rows);
    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int sample_filter_file_save(const char *path, const void *image, const size_t image_size)
{
    cplat_string_catalog_filter_info info;
    char text[DECOMPILE_BUFFER_SIZE];
    FILE *stream;
    int ret;

    if ((path == NULL) || (image == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    ret = cplat_string_catalog_filter_get_info(image, image_size, &info);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    stream = cplat_fopen(path, "wb", NULL);
    if (stream == NULL)
    {
        return CPLAT_ERR_UNKNOWN;
    }

    for (uint32_t line_index = 0; (ret == CPLAT_OK) && (line_index < info.line_count); line_index++)
    {
        ret = cplat_string_catalog_filter_decompile_line(image, image_size, line_index, text, sizeof(text));
        if ((ret == CPLAT_OK) && ((fputs(text, stream) < 0) || (fputc('\n', stream) == EOF)))
        {
            ret = CPLAT_ERR_UNKNOWN;
        }
    }

    if ((cplat_fclose(stream, NULL) != 0) && (ret == CPLAT_OK))
    {
        ret = CPLAT_ERR_UNKNOWN;
    }
    return ret;
}
