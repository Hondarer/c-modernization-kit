/**
 *******************************************************************************
 *  @file           struct_meta_gen_emit_array.c
 *  @brief          バイト列を C の静的配列リテラルとして出力します。
 *  @author         Tetsuo Honda
 *  @date           2026/08/30
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "struct_meta_gen_emit_array.h"

#include <inttypes.h>
#include <stdint.h>
#include <string.h>

/** 配列リテラルを出力する 1 行あたりのワード数です。 */
#define STRUCT_META_GEN_EMIT_ARRAY_WORDS_PER_LINE 4U

/* Doxygen コメントは、ヘッダーに記載 */

void struct_meta_gen_emit_uint64_array(FILE *out, const char *array_name, const void *data, size_t data_size)
{
    size_t words = (data_size + 7U) / 8U;

    fprintf(out, "static const uint64_t %s[%zu] = {\n", array_name, words);
    for (size_t i = 0; i < words; i++)
    {
        uint64_t word = 0;
        size_t offset = i * 8U;
        size_t remain = data_size - offset;
        size_t copy_size;
        if (remain < 8U)
        {
            copy_size = remain;
        }
        else
        {
            copy_size = 8U;
        }

        memcpy(&word, (const unsigned char *)data + offset, copy_size);

        if ((i % STRUCT_META_GEN_EMIT_ARRAY_WORDS_PER_LINE) == 0U)
        {
            fprintf(out, "    ");
        }
        fprintf(out, "UINT64_C(0x%016" PRIx64 ")", word);
        if ((i + 1U) < words)
        {
            fprintf(out, ",");
        }
        if ((((i + 1U) % STRUCT_META_GEN_EMIT_ARRAY_WORDS_PER_LINE) == 0U) || ((i + 1U) == words))
        {
            fprintf(out, "\n");
        }
        else
        {
            fprintf(out, " ");
        }
    }
    fprintf(out, "};\n\n");
}
