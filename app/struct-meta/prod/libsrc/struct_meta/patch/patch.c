/**
 *******************************************************************************
 *  @file           patch.c
 *  @brief          記述子の階層をメニュー形式で辿り、対話形式で構造体インスタンスの値を編集します。
 *  @author         Tetsuo Honda
 *  @date           2026/08/16
 *  @version        1.0.0
 *
 *  `cplat_prompt` (`cplat/prompt/prompt.h`) を使用します。TTY でない場合
 *  (パイプ・リダイレクト等) は `cplat_prompt` 自身が `fgets` にフォールバック
 *  するため、本ファイルは対話端末かどうかを意識しません。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/patch/patch.h>

#include <struct_meta/access/access.h>
#include <struct_meta/meta/bytes.h>
#include <struct_meta/meta/integer.h>

#include <cplat/base/result.h>
#include <cplat/prompt/prompt.h>

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** 1 回の入力行を受けるバッファーのバイト数です。 */
#define STRUCT_META_PATCH_LINE_BYTES 256

static int patch_struct(cplat_prompt *prompt, const struct_meta_descriptor *desc, unsigned char *base,
                        const char *path);

/**
 *  @brief          現在位置へ区切り文字と末尾文字列を追加したパスを確保します。
 */
static int append_path(const char *path, const char *separator, const char *suffix, char **path_out)
{
    size_t path_length = strlen(path);
    size_t separator_length = strlen(separator);
    size_t suffix_length = strlen(suffix);

    if ((path_length > (SIZE_MAX - separator_length)) ||
        ((path_length + separator_length) > (SIZE_MAX - suffix_length)) ||
        ((path_length + separator_length + suffix_length) == SIZE_MAX))
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    size_t dest_size = path_length + separator_length + suffix_length + 1U;
    char *dest = (char *)malloc(dest_size);
    if (dest == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    memcpy(dest, path, path_length);
    memcpy(dest + path_length, separator, separator_length);
    memcpy(dest + path_length + separator_length, suffix, suffix_length + 1U);
    *path_out = dest;
    return CPLAT_OK;
}

/**
 *  @brief          現在位置へフィールド名を追加したパスを確保します。
 */
static int append_field_path(const char *path, const char *field_name, char **path_out)
{
    const char *separator = (path[0] == '\0') ? "" : ".";
    return append_path(path, separator, field_name, path_out);
}

/**
 *  @brief          現在位置へ配列インデックスを追加したパスを確保します。
 */
static int append_index_path(const char *path, size_t index, char **path_out)
{
    char suffix[(sizeof(size_t) * 3U) + 3U];
    int length = snprintf(suffix, sizeof(suffix), "[%zu]", index);
    if ((length < 0) || ((size_t)length >= sizeof(suffix)))
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }
    return append_path(path, "", suffix, path_out);
}

/**
 *  @brief          フィールド 1 個分の現在値を、一覧表示用に短く整形します。
 */
static void format_scalar_value(struct_meta_field_kind kind, const unsigned char *field_ptr, size_t element_size,
                                size_t char_buffer_size, char *dest, size_t dest_size)
{
    switch (kind)
    {
    case STRUCT_META_FIELD_SIGNED_INTEGER:
    {
        int64_t value;
        if (struct_meta_internal_integer_load_signed(field_ptr, element_size, &value) != CPLAT_OK)
        {
            snprintf(dest, dest_size, "?");
            break;
        }
        snprintf(dest, dest_size, "%" PRId64, value);
        break;
    }
    case STRUCT_META_FIELD_UNSIGNED_INTEGER:
    {
        uint64_t value;
        if (struct_meta_internal_integer_load_unsigned(field_ptr, element_size, &value) != CPLAT_OK)
        {
            snprintf(dest, dest_size, "?");
            break;
        }
        snprintf(dest, dest_size, "%" PRIu64, value);
        break;
    }
    case STRUCT_META_FIELD_FLOAT:
    {
        float value;
        memcpy(&value, field_ptr, sizeof(value));
        snprintf(dest, dest_size, "%g", (double)value);
        break;
    }
    case STRUCT_META_FIELD_DOUBLE:
    {
        double value;
        memcpy(&value, field_ptr, sizeof(value));
        snprintf(dest, dest_size, "%g", value);
        break;
    }
    case STRUCT_META_FIELD_CHAR_ARRAY:
        if ((char_buffer_size == 0U) || (memchr(field_ptr, '\0', char_buffer_size) == NULL))
        {
            snprintf(dest, dest_size, "<NUL 終端なし>");
        }
        else
        {
            snprintf(dest, dest_size, "\"%s\"", (const char *)field_ptr);
        }
        break;
    case STRUCT_META_FIELD_STRUCT:
    default:
        snprintf(dest, dest_size, "{...}");
        break;
    }
}

/**
 *  @brief          入力行を int へ変換します。空行は「変更なし」を表す @c CPLAT_ERR_EOF を返します。
 */
static int parse_int(const char *text, int *value_out)
{
    if (text[0] == '\0')
    {
        return CPLAT_ERR_EOF;
    }
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if ((end == text) || (*end != '\0') || (errno != 0) || (value < INT_MIN) || (value > INT_MAX))
    {
        return CPLAT_ERR_INVALID_INTEGER;
    }
    *value_out = (int)value;
    return CPLAT_OK;
}

/**
 *  @brief          現在位置のメンバー名からフィールドを一意に検索します。
 *
 *  同名フィールドを含む不正な記述子では、最初のフィールドを暗黙に選ばず、
 *  @c CPLAT_ERR_DUPLICATE_KEY を返します。
 */
static int find_field_by_name(const struct_meta_descriptor *descriptor, const char *name,
                              const struct_meta_field **field_out)
{
    if ((descriptor == NULL) || (name == NULL) || (field_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *field_out = NULL;

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (strcmp(descriptor->fields[i].name, name) == 0)
        {
            if (*field_out != NULL)
            {
                return CPLAT_ERR_DUPLICATE_KEY;
            }
            *field_out = &descriptor->fields[i];
        }
    }

    return (*field_out == NULL) ? CPLAT_ERR_NOT_FOUND : CPLAT_OK;
}

/**
 *  @brief          符号付き整数フィールド用に、入力行を @c int64_t として解釈します。
 *
 *  幅ごとの範囲検査は @ref struct_meta_internal_integer_store_signed が行うため、
 *  ここでは @c int64_t に収まるかだけを判定します。
 */
static int parse_signed_integer(const char *text, int64_t *value_out)
{
    if (text[0] == '\0')
    {
        return CPLAT_ERR_EOF;
    }
    char *end = NULL;
    errno = 0;
    long long value = strtoll(text, &end, 10);
    if ((end == text) || (*end != '\0') || (errno != 0))
    {
        return CPLAT_ERR_INVALID_INTEGER;
    }
    *value_out = (int64_t)value;
    return CPLAT_OK;
}

/**
 *  @brief          符号なし整数フィールド用に、入力行を @c uint64_t として解釈します。
 *
 *  幅ごとの範囲検査は @ref struct_meta_internal_integer_store_unsigned が行うため、
 *  ここでは @c uint64_t に収まるかだけを判定します。
 */
static int parse_unsigned_integer(const char *text, uint64_t *value_out)
{
    if (text[0] == '\0')
    {
        return CPLAT_ERR_EOF;
    }
    if (text[0] == '-')
    {
        return CPLAT_ERR_INVALID_INTEGER;
    }
    char *end = NULL;
    errno = 0;
    unsigned long long value = strtoull(text, &end, 10);
    if ((end == text) || (*end != '\0') || (errno != 0))
    {
        return CPLAT_ERR_INVALID_INTEGER;
    }
    *value_out = (uint64_t)value;
    return CPLAT_OK;
}

static int parse_double(const char *text, double *value_out)
{
    if (text[0] == '\0')
    {
        return CPLAT_ERR_EOF;
    }
    char *end = NULL;
    errno = 0;
    double value = strtod(text, &end);
    if ((end == text) || (*end != '\0') || (errno != 0))
    {
        return CPLAT_ERR_INVALID_INTEGER;
    }
    *value_out = value;
    return CPLAT_OK;
}

/**
 *  @brief          スカラー フィールド 1 個分の値を対話入力で書き換えます。
 *  @return         @c CPLAT_OK (書き換えた)、@c CPLAT_ERR_EOF (空行、変更なしで戻る)、
 *                  それ以外は @p prompt からのエラー。
 */
static int patch_scalar(cplat_prompt *prompt, const struct_meta_field *field, unsigned char *field_ptr,
                        const char *path)
{
    char line[STRUCT_META_PATCH_LINE_BYTES];
    char current[64];

    format_scalar_value(field->kind, field_ptr, field->element_size, field->char_buffer_size, current, sizeof(current));

    for (;;)
    {
        int ret =
            cplat_prompt_readline_fmt(prompt, line, sizeof(line), "%s (現在値 %s、空行で変更なし)> ", path, current);
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        if (line[0] == '\0')
        {
            return CPLAT_ERR_EOF;
        }

        switch (field->kind)
        {
        case STRUCT_META_FIELD_SIGNED_INTEGER:
        {
            int64_t value;
            int parse_ret = parse_signed_integer(line, &value);
            if (parse_ret != CPLAT_OK)
            {
                printf("整数として解釈できません: %s\n", line);
                break;
            }
            parse_ret = struct_meta_internal_integer_store_signed(field_ptr, field->element_size, value);
            if (parse_ret == CPLAT_OK)
            {
                return CPLAT_OK;
            }
            printf("%zu バイトの符号付き整数で表せません: %s\n", field->element_size, line);
            break;
        }
        case STRUCT_META_FIELD_UNSIGNED_INTEGER:
        {
            uint64_t value;
            int parse_ret = parse_unsigned_integer(line, &value);
            if (parse_ret != CPLAT_OK)
            {
                printf("符号なし整数として解釈できません: %s\n", line);
                break;
            }
            parse_ret = struct_meta_internal_integer_store_unsigned(field_ptr, field->element_size, value);
            if (parse_ret == CPLAT_OK)
            {
                return CPLAT_OK;
            }
            printf("%zu バイトの符号なし整数で表せません: %s\n", field->element_size, line);
            break;
        }
        case STRUCT_META_FIELD_FLOAT:
        {
            double parsed;
            int parse_ret = parse_double(line, &parsed);
            if (parse_ret == CPLAT_OK)
            {
                float value = (float)parsed;
                memcpy(field_ptr, &value, sizeof(value));
                return CPLAT_OK;
            }
            printf("数値として解釈できません: %s\n", line);
            break;
        }
        case STRUCT_META_FIELD_DOUBLE:
        {
            double value;
            int parse_ret = parse_double(line, &value);
            if (parse_ret == CPLAT_OK)
            {
                memcpy(field_ptr, &value, sizeof(value));
                return CPLAT_OK;
            }
            printf("数値として解釈できません: %s\n", line);
            break;
        }
        case STRUCT_META_FIELD_CHAR_ARRAY:
        {
            size_t text_len = strlen(line);
            if ((field->char_buffer_size == 0U) || (text_len >= field->char_buffer_size))
            {
                printf("文字列がバッファー サイズ (%zu バイト) を超えています。\n", field->char_buffer_size);
                break;
            }
            memcpy(field_ptr, line, text_len + 1U);
            return CPLAT_OK;
        }
        case STRUCT_META_FIELD_STRUCT:
        default:
            /* ここには到達しない (呼び出し元がスカラーだけを渡す)。 */
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
    }
}

static int patch_hex_array(cplat_prompt *prompt, const struct_meta_field *field, unsigned char *field_ptr,
                           const char *path)
{
    size_t text_size;
    int ret = struct_meta_internal_bytes_hex_text_size(field->element_count, &text_size);
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    if (text_size > (SIZE_MAX / 2U))
    {
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    char *current = (char *)malloc(text_size);
    char *line = (char *)malloc(text_size * 2U);
    if ((current == NULL) || (line == NULL))
    {
        free(current);
        free(line);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    ret = struct_meta_internal_bytes_to_hex(field_ptr, field->element_count, current, text_size);
    while (ret == CPLAT_OK)
    {
        ret =
            cplat_prompt_readline_fmt(prompt, line, text_size * 2U, "%s (現在値 %s、空行で変更なし)> ", path, current);
        if ((ret != CPLAT_OK) || (line[0] == '\0'))
        {
            break;
        }
        int parse_ret = struct_meta_internal_bytes_from_hex(field_ptr, field->element_count, line);
        if (parse_ret == CPLAT_OK)
        {
            break;
        }
        else
        {
            printf("%zu 個のバイトを2桁の16進数で空白区切りにしてください: %s\n", field->element_count, line);
        }
    }
    free(current);
    free(line);
    return ret;
}

/**
 *  @brief          フィールド 1 要素分 (配列要素、またはネスト構造体) を編集します。
 */
static int patch_element(cplat_prompt *prompt, const struct_meta_field *field, unsigned char *elem_ptr,
                         const char *path)
{
    if (field->kind == STRUCT_META_FIELD_STRUCT)
    {
        return patch_struct(prompt, field->nested, elem_ptr, path);
    }
    int ret = patch_scalar(prompt, field, elem_ptr, path);
    if (ret == CPLAT_ERR_EOF)
    {
        /* 空行 (変更なし) は呼び出し元にとってはエラーではない。 */
        return CPLAT_OK;
    }
    return ret;
}

/**
 *  @brief          配列フィールドの要素インデックスをメニュー形式で選択させ、選択した要素を編集します。
 */
static int patch_array_field(cplat_prompt *prompt, const struct_meta_field *field, unsigned char *field_ptr,
                             const char *path)
{
    char line[STRUCT_META_PATCH_LINE_BYTES];

    for (;;)
    {
        printf("-- %s (現在位置: %s、配列、要素数 %zu) --\n", field->name, path, field->element_count);
        for (size_t i = 0; i < field->element_count; i++)
        {
            char *element_path;
            int path_ret = append_index_path(path, i, &element_path);
            if (path_ret != CPLAT_OK)
            {
                return path_ret;
            }
            printf("  %zu) %s\n", i, element_path);
            free(element_path);
        }

        int ret = cplat_prompt_readline_fmt(prompt, line, sizeof(line), "要素番号を選択 (空行で戻る)> ");
        if (ret != CPLAT_OK)
        {
            return ret;
        }
        if (line[0] == '\0')
        {
            return CPLAT_OK;
        }

        int index;
        if ((parse_int(line, &index) != CPLAT_OK) || (index < 0) || ((size_t)index >= field->element_count))
        {
            printf("0 から %zu の範囲で入力してください。\n", field->element_count - 1U);
            continue;
        }

        unsigned char *element = field_ptr + ((size_t)index * field->element_size);
        char *element_path;
        ret = append_index_path(path, (size_t)index, &element_path);
        if (ret == CPLAT_OK)
        {
            ret = patch_element(prompt, field, element, element_path);
            free(element_path);
        }
        if (ret != CPLAT_OK)
        {
            return ret;
        }
    }
}

/**
 *  @brief          構造体インスタンス 1 個分のフィールド一覧をメニュー形式で辿ります。
 *
 *  フィールドは番号または現在位置のメンバー名の完全一致で選択できます。
 */
static int patch_struct(cplat_prompt *prompt, const struct_meta_descriptor *desc, unsigned char *base, const char *path)
{
    char line[STRUCT_META_PATCH_LINE_BYTES];

    for (;;)
    {
        if ((desc->brief != NULL) && (desc->brief[0] != '\0'))
        {
            printf("-- %s (現在位置: %s) --  %s\n", desc->name, (path[0] == '\0') ? "<root>" : path, desc->brief);
        }
        else
        {
            printf("-- %s (現在位置: %s) --\n", desc->name, (path[0] == '\0') ? "<root>" : path);
        }
        for (size_t i = 0; i < desc->field_count; i++)
        {
            const struct_meta_field *field = &desc->fields[i];
            char *field_path;
            int path_ret = append_field_path(path, field->name, &field_path);
            if (path_ret != CPLAT_OK)
            {
                return path_ret;
            }
            void *element;
            int access_ret = struct_meta_field_get_element(field, base, 0U, &element);
            if (access_ret != CPLAT_OK)
            {
                free(field_path);
                return access_ret;
            }
            unsigned char *field_ptr = (unsigned char *)element;
            const char *brief = "";
            const char *brief_sep = "";

            if ((field->brief != NULL) && (field->brief[0] != '\0'))
            {
                brief_sep = "  ";
                brief = field->brief;
            }

            if (field->kind == STRUCT_META_FIELD_STRUCT)
            {
                if (field->element_count > 1U)
                {
                    printf("  %zu) %s [配列 %zu 件]%s%s\n", i + 1U, field_path, field->element_count, brief_sep, brief);
                }
                else
                {
                    printf("  %zu) %s {...}%s%s\n", i + 1U, field_path, brief_sep, brief);
                }
            }
            else if ((field->kind != STRUCT_META_FIELD_CHAR_ARRAY) && (field->element_count > 1U))
            {
                struct_meta_internal_byte_format byte_format;
                int format_ret = struct_meta_internal_field_byte_format(field, &byte_format);
                if (format_ret != CPLAT_OK)
                {
                    free(field_path);
                    return format_ret;
                }
                if (byte_format == STRUCT_META_INTERNAL_BYTE_FORMAT_HEX)
                {
                    size_t text_size;
                    format_ret = struct_meta_internal_bytes_hex_text_size(field->element_count, &text_size);
                    char *text = NULL;
                    if (format_ret == CPLAT_OK)
                    {
                        text = (char *)malloc(text_size);
                        format_ret = (text == NULL) ? CPLAT_ERR_OUT_OF_MEMORY : CPLAT_OK;
                    }
                    if (format_ret == CPLAT_OK)
                    {
                        format_ret =
                            struct_meta_internal_bytes_to_hex(field_ptr, field->element_count, text, text_size);
                    }
                    if (format_ret != CPLAT_OK)
                    {
                        free(text);
                        free(field_path);
                        return format_ret;
                    }
                    printf("  %zu) %s = %s%s%s\n", i + 1U, field_path, text, brief_sep, brief);
                    free(text);
                }
                else
                {
                    printf("  %zu) %s [配列 %zu 件]%s%s\n", i + 1U, field_path, field->element_count, brief_sep, brief);
                }
            }
            else
            {
                char current[64];
                format_scalar_value(field->kind, field_ptr, field->element_size, field->char_buffer_size, current,
                                    sizeof(current));
                printf("  %zu) %s = %s%s%s\n", i + 1U, field_path, current, brief_sep, brief);
            }
            free(field_path);
        }

        int ret = cplat_prompt_readline_fmt(prompt, line, sizeof(line),
                                            "フィールド番号またはメンバー名を選択 (空行で戻る)> ");
        if (ret != CPLAT_OK)
        {
            return ret;
        }
        if (line[0] == '\0')
        {
            return CPLAT_OK;
        }

        int index;
        const struct_meta_field *field = NULL;
        const int parse_result = parse_int(line, &index);
        if (parse_result == CPLAT_OK)
        {
            if ((index < 1) || ((size_t)index > desc->field_count))
            {
                printf("1 から %zu の範囲で入力してください。\n", desc->field_count);
                continue;
            }
            field = &desc->fields[(size_t)index - 1U];
        }
        else
        {
            const int find_result = find_field_by_name(desc, line, &field);
            if (find_result == CPLAT_ERR_DUPLICATE_KEY)
            {
                printf("メンバー名が重複しているため選択できません: %s\n", line);
                continue;
            }
            if (find_result != CPLAT_OK)
            {
                printf("1 から %zu の番号、またはメンバー名を入力してください。\n", desc->field_count);
                continue;
            }
        }

        char *field_path;
        ret = append_field_path(path, field->name, &field_path);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
        struct_meta_internal_byte_format byte_format;
        ret = struct_meta_internal_field_byte_format(field, &byte_format);
        if (ret != CPLAT_OK)
        {
            free(field_path);
            return ret;
        }
        if (byte_format == STRUCT_META_INTERNAL_BYTE_FORMAT_HEX)
        {
            void *element;
            ret = struct_meta_field_get_element(field, base, 0U, &element);
            if (ret == CPLAT_OK)
            {
                ret = patch_hex_array(prompt, field, (unsigned char *)element, field_path);
            }
        }
        else if ((field->kind != STRUCT_META_FIELD_CHAR_ARRAY) && (field->element_count > 1U))
        {
            void *element;
            ret = struct_meta_field_get_element(field, base, 0U, &element);
            if (ret == CPLAT_OK)
            {
                ret = patch_array_field(prompt, field, (unsigned char *)element, field_path);
            }
        }
        else
        {
            void *element;
            ret = struct_meta_field_get_element(field, base, 0U, &element);
            if (ret == CPLAT_OK)
            {
                ret = patch_element(prompt, field, (unsigned char *)element, field_path);
            }
        }
        free(field_path);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
    }
}

/**
 *  @brief          検査済みのパスで、終端フィールドの配列インデックスが指定されているかを返します。
 */
static int path_has_terminal_index(const char *path)
{
    const char *terminal = strrchr(path, '.');
    if (terminal == NULL)
    {
        terminal = path;
    }
    else
    {
        terminal++;
    }
    return strchr(terminal, '[') != NULL;
}

/**
 *  @brief          プロンプトを作成し、指定されたフィールドまたは配列を編集します。
 */
static int patch_target(const struct_meta_field *field, unsigned char *value, const char *path, int edit_array,
                        int edit_hex)
{
    cplat_prompt *prompt = cplat_prompt_create(NULL);
    if (prompt == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    int ret;
    if (edit_hex != 0)
    {
        ret = patch_hex_array(prompt, field, value, path);
    }
    else if (edit_array != 0)
    {
        ret = patch_array_field(prompt, field, value, path);
    }
    else
    {
        ret = patch_element(prompt, field, value, path);
    }
    cplat_prompt_dispose(prompt);

    if (ret == CPLAT_ERR_EOF)
    {
        return CPLAT_OK;
    }
    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_patch_interactive(const struct_meta_descriptor *desc, void *instance)
{
    if ((desc == NULL) || (instance == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    int ret = struct_meta_descriptor_validate(desc);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    cplat_prompt *prompt = cplat_prompt_create(NULL);
    if (prompt == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    ret = patch_struct(prompt, desc, (unsigned char *)instance, "");
    cplat_prompt_dispose(prompt);

    if (ret == CPLAT_ERR_EOF)
    {
        return CPLAT_OK;
    }
    return ret;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_patch_path_interactive(const struct_meta_descriptor *desc, void *instance, const char *path)
{
    if ((desc == NULL) || (instance == NULL) || (path == NULL) || (path[0] == '\0'))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    const struct_meta_field *field;
    void *value;
    int ret = struct_meta_path_resolve(desc, instance, path, &field, &value);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    int edit_array = (field->kind != STRUCT_META_FIELD_CHAR_ARRAY) && (field->element_count > 1U) &&
                     (path_has_terminal_index(path) == 0);
    struct_meta_internal_byte_format byte_format;
    ret = struct_meta_internal_field_byte_format(field, &byte_format);
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    int edit_hex = (byte_format == STRUCT_META_INTERNAL_BYTE_FORMAT_HEX) && (path_has_terminal_index(path) == 0);
    ret = patch_target(field, (unsigned char *)value, path, edit_array, edit_hex);
    return ret;
}
