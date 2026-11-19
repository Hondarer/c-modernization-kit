/**
 *******************************************************************************
 *  @file           file.c
 *  @brief          構造体インスタンスと JSON テキスト ファイルを相互変換します。
 *  @author         Tetsuo Honda
 *  @date           2026/08/16
 *  @version        1.0.0
 *
 *  ファイル I/O は `cplat/crt/stdio.h` の stdio ラッパーを使用します。\n
 *  JSON 設定ファイルの読み書きは単発の逐次アクセスが支配的な用途であるため、
 *  `cplat_file_*` (低レベル API) や mmap ではなく stdio ラッパーを選択します
 *  (`app/cplat/docs/fileio-api-selection-guideline.md` の結論 4)。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/json/file.h>
#include <struct_meta/json/json.h>

#include <cplat/base/result.h>
#include <cplat/crt/stdio.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_json_file_save(const struct_meta_descriptor *desc, const void *instance, const char *path)
{
    if ((desc == NULL) || (instance == NULL) || (path == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    cJSON *json = NULL;
    int ret = struct_meta_json_encode(desc, instance, &json);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    char *text = cjson_app_cJSON_Print(json);
    cjson_app_cJSON_Delete(json);

    if (text == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    FILE *stream = cplat_fopen(path, "wb", NULL);
    if (stream == NULL)
    {
        cjson_app_cJSON_free(text);
        return CPLAT_ERR_NOT_FOUND;
    }

    size_t text_len = strlen(text);
    int needs_newline = (text_len == 0U) || (text[text_len - 1U] != '\n');
    if ((needs_newline != 0) && (text_len == SIZE_MAX))
    {
        /*
         * すでに CPLAT_ERR_OUT_OF_RANGE を返す経路の後始末であり、閉じる処理の失敗で結果は変わらないため、
         * 戻り値は明示的に破棄する。
         */
        (void)cplat_fclose(stream, NULL);
        cjson_app_cJSON_free(text);
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    size_t expected_len = text_len;

    if (needs_newline != 0)
    {
        expected_len++;
    }

    size_t written = cplat_fwrite(text, 1, text_len, stream, NULL);
    if ((written == text_len) && (needs_newline != 0))
    {
        static const char newline[] = "\n";

        if (cplat_fwrite(newline, 1U, 1U, stream, NULL) == 1U)
        {
            written++;
        }
    }

    /* 閉じる処理は残りの書き出しを含むため、失敗した場合は書き込みの失敗として扱う。 */
    int close_ret = cplat_fclose(stream, NULL);
    cjson_app_cJSON_free(text);

    if ((written != expected_len) || (close_ret != 0))
    {
        return CPLAT_ERR_UNKNOWN;
    }

    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_json_file_load(const struct_meta_descriptor *desc, const char *path, void *instance)
{
    if ((desc == NULL) || (instance == NULL) || (path == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    FILE *stream = cplat_fopen(path, "rb", NULL);
    if (stream == NULL)
    {
        return CPLAT_ERR_NOT_FOUND;
    }

    if (cplat_fseek(stream, 0, SEEK_END) != 0)
    {
        /* 移動の失敗を返す経路の後始末であり、閉じる失敗で結果は変わらないため、戻り値は明示的に破棄する。 */
        (void)cplat_fclose(stream, NULL);
        return CPLAT_ERR_UNKNOWN;
    }

    int64_t file_size = cplat_ftell(stream);
    if ((file_size < 0) || (cplat_fseek(stream, 0, SEEK_SET) != 0))
    {
        /* 大きさの取得の失敗を返す経路の後始末であり、閉じる失敗で結果は変わらないため、戻り値は明示的に破棄する。 */
        (void)cplat_fclose(stream, NULL);
        return CPLAT_ERR_UNKNOWN;
    }

    /* JSON テキストのバイト列バッファーである。要素型に対応しない生バイト確保のため sizeof(*p) は使わない。 */
    char *text = (char *)malloc((size_t)file_size + 1U);
    if (text == NULL)
    {
        /* 確保の失敗を返す経路の後始末であり、閉じる失敗で結果は変わらないため、戻り値は明示的に破棄する。 */
        (void)cplat_fclose(stream, NULL);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    size_t read_count = cplat_fread(text, 1, (size_t)file_size, stream, NULL);

    /* 読み込み用のストリームであり、読み込みの結果は直後に read_count で検査するため、戻り値は明示的に破棄する。 */
    (void)cplat_fclose(stream, NULL);

    if (read_count != (size_t)file_size)
    {
        free(text);
        return CPLAT_ERR_UNKNOWN;
    }

    text[file_size] = '\0';

    cJSON *json = cjson_app_cJSON_Parse(text);
    free(text);

    if (json == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    int ret = struct_meta_json_decode(desc, json, instance);
    cjson_app_cJSON_Delete(json);

    return ret;
}
