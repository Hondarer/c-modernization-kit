/**
 *******************************************************************************
 *  @file           libsrc/string_catalog/string_catalog_render.c
 *  @brief          位置指定書式の展開と構文確認を提供します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  書式が解釈するのは `{0}` から `{9}` までの位置指定と、`{{` と `}}` のエスケープだけです。\n
 *  値の文字列表現は引数種別が決めるため、書式側には書式指定を書けません。\n
 *  展開と構文確認は同じ走査で行い、書き込み先を持たない呼び出しを構文確認として扱います。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "format_engine.h"

#include <string_catalog/string_catalog_const.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/** 1 個の値を文字列化する一時領域のバイト数です。最長は 16 進 16 桁の表現です。 */
#define VALUE_TEXT_MAX 64

/** 文字列引数が NULL のときに出力する表現です。 */
#define NULL_STRING_TEXT "(null)"

/** 1 文字として出力する ASCII の下限です。 */
#define CHAR_PRINTABLE_MIN 0x20U

/** 1 文字として出力する ASCII の上限です。 */
#define CHAR_PRINTABLE_MAX 0x7EU

/**
 *  @brief          書き込み先と、書き込みの経過を保持します。
 *
 *  @ref render_buffer::dest が NULL のときは、書き込みを行わず構文だけを確認します。
 */
typedef struct render_buffer
{
    char *dest;           /**< 書き込み先です。NULL のときは構文確認だけを行います。 */
    size_t dest_size;     /**< dest のバイト数です。dest が NULL のときは 0 です。 */
    size_t length;        /**< dest へ書き込んだバイト数です。NUL 終端を含みません。 */
    bool is_truncated;    /**< 容量不足で切り詰めた場合に true になります。 */
    unsigned char pad[7]; /**< 明示的アラインメントです。0 を指定します。 */
} render_buffer;

/**
 *  @brief          指定した長さの文字列を書き込み先へ追加します。
 *  @param[in,out]  buffer 書き込み先と経過。NULL を渡してはなりません。
 *  @param[in]      text   追加する文字列。NULL を渡してはなりません。
 *  @param[in]      length @p text から追加するバイト数。
 *
 *  容量が不足する場合は入るところまで書き込み、切り詰めを記録します。\n
 *  書き込みを行った場合は常に NUL 終端します。
 */
static void render_buffer_append(render_buffer *buffer, const char *text, const size_t length)
{
    size_t capacity;
    size_t copy_length;

    if (buffer->dest == NULL)
    {
        return;
    }

    /* NUL 終端の 1 バイトを除いた、本文を書き込める最大バイト数 */
    capacity = buffer->dest_size - 1U;
    copy_length = length;

    if (copy_length > (capacity - buffer->length))
    {
        copy_length = capacity - buffer->length;
        buffer->is_truncated = true;
    }

    memcpy(&buffer->dest[buffer->length], text, copy_length);
    buffer->length += copy_length;
    buffer->dest[buffer->length] = '\0';
}

/**
 *  @brief          NUL 終端の文字列を書き込み先へ追加します。
 *  @param[in,out]  buffer 書き込み先と経過。NULL を渡してはなりません。
 *  @param[in]      text   追加する文字列。NULL を渡してはなりません。
 */
static void render_buffer_append_string(render_buffer *buffer, const char *text)
{
    render_buffer_append(buffer, text, strlen(text));
}

/**
 *  @brief          引数 1 個の文字列表現を書き込み先へ追加します。
 *  @param[in,out]  buffer 書き込み先と経過。NULL を渡してはなりません。
 *  @param[in]      value  追加する値。NULL を渡してはなりません。
 *  @return         成功時は @ref STRING_CATALOG_OK を返します。
 *  @return         引数種別が未知の場合は @ref STRING_CATALOG_ERR_INVALID_DEFINITION を返します。
 */
static int render_buffer_append_argument(render_buffer *buffer, const format_engine_argument_value *value)
{
    /* 各書式は固定の桁数に収まるため、切り詰めは発生しない */
    char value_text[VALUE_TEXT_MAX];

    switch (value->kind)
    {
    case STRING_CATALOG_ARGUMENT_KIND_STRING:
        if (value->value.string_value == NULL)
        {
            render_buffer_append_string(buffer, NULL_STRING_TEXT);
        }
        else
        {
            render_buffer_append_string(buffer, value->value.string_value);
        }
        return STRING_CATALOG_OK;

    case STRING_CATALOG_ARGUMENT_KIND_CHAR:
    {
        /* isprint はロケールに依存するため、ASCII の印字可能範囲を直接判定する */
        const unsigned int code = (unsigned int)(unsigned char)value->value.char_value;

        if ((code >= CHAR_PRINTABLE_MIN) && (code <= CHAR_PRINTABLE_MAX))
        {
            (void)snprintf(value_text, sizeof(value_text), "'%c'", (char)code);
        }
        else
        {
            (void)snprintf(value_text, sizeof(value_text), "%u (0x%02x)", code, code);
        }
    }
    break;

    case STRING_CATALOG_ARGUMENT_KIND_INT8:
        (void)snprintf(value_text, sizeof(value_text), "%" PRId8, value->value.int8_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_UINT8:
        (void)snprintf(value_text, sizeof(value_text), "%" PRIu8, value->value.uint8_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_INT16:
        (void)snprintf(value_text, sizeof(value_text), "%" PRId16, value->value.int16_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_UINT16:
        (void)snprintf(value_text, sizeof(value_text), "%" PRIu16, value->value.uint16_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_INT32:
        (void)snprintf(value_text, sizeof(value_text), "%" PRId32, value->value.int32_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_UINT32:
        (void)snprintf(value_text, sizeof(value_text), "%" PRIu32, value->value.uint32_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_INT64:
        (void)snprintf(value_text, sizeof(value_text), "%" PRId64, value->value.int64_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_UINT64:
        (void)snprintf(value_text, sizeof(value_text), "%" PRIu64, value->value.uint64_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_HEX8:
        (void)snprintf(value_text, sizeof(value_text), "0x%02" PRIx8, value->value.uint8_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_HEX16:
        (void)snprintf(value_text, sizeof(value_text), "0x%04" PRIx16, value->value.uint16_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_HEX32:
        (void)snprintf(value_text, sizeof(value_text), "0x%08" PRIx32, value->value.uint32_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_HEX64:
        (void)snprintf(value_text, sizeof(value_text), "0x%016" PRIx64, value->value.uint64_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_SIZE:
        (void)snprintf(value_text, sizeof(value_text), "%zu", value->value.size_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_SSIZE:
        (void)snprintf(value_text, sizeof(value_text), "%" PRId64, value->value.int64_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_POINTER:
        /* %p の表現はプラットフォームで異なるため、ポインター幅の 16 進表現へ揃える */
        (void)snprintf(value_text, sizeof(value_text), "0x%016" PRIxPTR, (uintptr_t)value->value.pointer_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_DOUBLE:
        (void)snprintf(value_text, sizeof(value_text), "%g", value->value.double_value);
        break;

    case STRING_CATALOG_ARGUMENT_KIND_ERROR_CODE:
        (void)snprintf(value_text, sizeof(value_text), "%d (0x%08x)", value->value.error_code_value,
                       (unsigned int)value->value.error_code_value);
        break;

    default:
        return STRING_CATALOG_ERR_INVALID_DEFINITION;
    }

    render_buffer_append_string(buffer, value_text);

    return STRING_CATALOG_OK;
}

/**
 *  @brief          書式を走査し、位置指定を展開します。
 *  @param[in,out]  buffer      書き込み先と経過。NULL を渡してはなりません。
 *  @param[in]      text        走査する書式。NULL を渡してはなりません。
 *  @param[in]      values      展開に使用する値の配列。NULL のときは構文確認だけを行います。
 *  @param[in]      value_count 位置指定が指してよい引数の個数。
 *  @return         成功時は @ref STRING_CATALOG_OK を返します。
 *  @return         構文が不正な場合、または位置指定が @p value_count 以上の添字を指す場合は
 *                  @ref STRING_CATALOG_ERR_INVALID_DEFINITION を返します。
 */
static int render_scan_text(render_buffer *buffer, const char *text, const format_engine_argument_value *values,
                            const int value_count)
{
    size_t position = 0U;

    while (text[position] != '\0')
    {
        size_t plain_length;
        int index;
        int ret;

        /* 位置指定とエスケープ以外は、連続する範囲をまとめて追加する */
        plain_length = strcspn(&text[position], "{}");
        if (plain_length > 0U)
        {
            render_buffer_append(buffer, &text[position], plain_length);
            position += plain_length;
            continue;
        }

        if (text[position] == '}')
        {
            if (text[position + 1U] != '}')
            {
                return STRING_CATALOG_ERR_INVALID_DEFINITION;
            }
            render_buffer_append(buffer, "}", 1U);
            position += 2U;
            continue;
        }

        if (text[position + 1U] == '{')
        {
            render_buffer_append(buffer, "{", 1U);
            position += 2U;
            continue;
        }

        /* 位置指定は 1 桁の添字だけを受け付ける */
        if ((text[position + 1U] < '0') || (text[position + 1U] > '9') || (text[position + 2U] != '}'))
        {
            return STRING_CATALOG_ERR_INVALID_DEFINITION;
        }

        index = text[position + 1U] - '0';
        if (index >= value_count)
        {
            return STRING_CATALOG_ERR_INVALID_DEFINITION;
        }

        if (values != NULL)
        {
            ret = render_buffer_append_argument(buffer, &values[index]);
            if (ret != STRING_CATALOG_OK)
            {
                return ret;
            }
        }

        position += 3U;
    }

    return STRING_CATALOG_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int format_engine_render_text(char *dest, const size_t dest_size, const char *text,
                              const format_engine_argument_value *values, const int value_count)
{
    render_buffer buffer = {0};
    int ret;

    buffer.dest = dest;
    buffer.dest_size = dest_size;
    dest[0] = '\0';

    ret = render_scan_text(&buffer, text, values, value_count);
    if (ret != STRING_CATALOG_OK)
    {
        return ret;
    }

    if (buffer.is_truncated)
    {
        return STRING_CATALOG_ERR_TRUNCATED;
    }

    return STRING_CATALOG_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int format_engine_validate_text(const char *text, const int value_count)
{
    render_buffer buffer = {0};

    return render_scan_text(&buffer, text, NULL, value_count);
}
