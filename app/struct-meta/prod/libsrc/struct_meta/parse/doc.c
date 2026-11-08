/**
 *******************************************************************************
 *  @file           doc.c
 *  @brief          Doxygen コメントから短い説明と汎用属性を取り出します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "doc.h"

#include <struct_meta/parse/diagnostic.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
 * Doxygen の開始記号を読み飛ばす。ブロック形式は '/' に続く "**" または "*!"、行形式は "///" または "//!" であり、
 * 後置用はそれぞれの後ろに '<' が付く。3 文字の開始記号と前方一致するため、後置用の 4 文字を先に判定して
 * '<' が本文に残らないようにする。
 */
static const char *skip_doc_open(const char *s)
{
    if ((s == NULL) || (s[0] != '/'))
    {
        return s;
    }

    if (s[1] == '*')
    {
        if ((s[2] == '*') && (s[3] == '<'))
        {
            return s + 4;
        }

        if ((s[2] == '!') && (s[3] == '<'))
        {
            return s + 4;
        }

        if (s[2] == '*')
        {
            return s + 3;
        }

        if (s[2] == '!')
        {
            return s + 3;
        }
    }

    if (s[1] == '/')
    {
        if ((s[2] == '/') && (s[3] == '<'))
        {
            return s + 4;
        }

        if ((s[2] == '!') && (s[3] == '<'))
        {
            return s + 4;
        }

        if (s[2] == '/')
        {
            return s + 3;
        }

        if (s[2] == '!')
        {
            return s + 3;
        }
    }

    return s;
}

static char *strip_doc_text(const char *raw)
{
    const char *body;
    const char *end;
    size_t raw_len;
    char *out;
    size_t out_len;
    int at_line_start;

    if (raw == NULL)
    {
        return NULL;
    }

    body = skip_doc_open(raw);
    raw_len = strlen(body);
    end = body + raw_len;

    /* 行コメントには終了記号がないため、末尾が終了記号の場合だけ除く。 */
    if ((raw_len >= 2U) && (end[-2] == '*') && (end[-1] == '/'))
    {
        end -= 2;
    }

    out = (char *)malloc((size_t)(end - body) + 1U);
    if (out == NULL)
    {
        return NULL;
    }

    /*
     * 各行の先頭から字下げとブロック コメントの装飾の '*' を 1 つ除く。
     * '*' の直後の空白は 1 文字だけ除き、コード例などの本文の字下げは残す。
     */
    out_len = 0;
    at_line_start = 1;

    while (body < end)
    {
        if (at_line_start != 0)
        {
            while ((body < end) && ((*body == ' ') || (*body == '\t')))
            {
                body++;
            }

            if ((body < end) && (*body == '*'))
            {
                body++;

                if ((body < end) && (*body == ' '))
                {
                    body++;
                }
            }

            at_line_start = 0;

            if (body >= end)
            {
                break;
            }
        }

        out[out_len] = *body;

        if (*body == '\n')
        {
            at_line_start = 1;
        }

        out_len++;
        body++;
    }

    out[out_len] = '\0';

    return out;
}

static const char *find_doc_tag(const char *text, const char *tag)
{
    size_t tag_len = strlen(tag);
    const char *p = text;
    int at_line_start = 1;

    /*
     * 字下げを除いた行頭にある @tag だけを検索する。
     * 文中の @tag や、@tagname のように名前が前方一致するだけの別タグは除外する。
     */
    while (*p != '\0')
    {
        if (at_line_start != 0)
        {
            while ((*p == ' ') || (*p == '\t'))
            {
                p++;
            }

            if ((*p == '@') && (strncmp(p + 1, tag, tag_len) == 0))
            {
                const char after = p[1U + tag_len];
                if ((after == '\0') || (isspace((unsigned char)after) != 0))
                {
                    return p;
                }
            }
        }

        if (*p == '\n')
        {
            at_line_start = 1;
        }
        else
        {
            at_line_start = 0;
        }

        /* 字下げの読み飛ばしで終端に達した場合は、p++ で終端を越えないよう抜ける。 */
        if (*p == '\0')
        {
            break;
        }

        p++;
    }

    return NULL;
}

/* 前後の空白を除き、改行を含む連続空白を 1 つの ' ' に畳み込む。空白だけの文字列は NULL を返す。 */
static char *collapse_ws(const char *begin, const char *end)
{
    char *out;
    size_t out_len;
    int in_space;

    while ((begin < end) && (isspace((unsigned char)*begin) != 0))
    {
        begin++;
    }

    while ((end > begin) && (isspace((unsigned char)end[-1]) != 0))
    {
        end--;
    }

    if (begin >= end)
    {
        return NULL;
    }

    out = (char *)malloc((size_t)(end - begin) + 1U);
    if (out == NULL)
    {
        return NULL;
    }

    out_len = 0;
    in_space = 0;

    while (begin < end)
    {
        if (isspace((unsigned char)*begin) != 0)
        {
            if (in_space == 0)
            {
                out[out_len] = ' ';
                out_len++;
                in_space = 1;
            }
        }
        else
        {
            out[out_len] = *begin;
            out_len++;
            in_space = 0;
        }

        begin++;
    }

    out[out_len] = '\0';

    if (out_len == 0U)
    {
        free(out);
        return NULL;
    }

    return out;
}

static char *extract_brief_tag(const char *stripped)
{
    const char *cursor = stripped;
    char *brief = NULL;

    for (;;)
    {
        const char *tag = find_doc_tag(cursor, "brief");
        const char *begin;
        const char *end;
        const char *nonspace;
        char *part;

        if (tag == NULL)
        {
            return brief;
        }

        begin = tag + (sizeof("@brief") - 1U); /* sizeof は終端の NUL を含むため 1 を引く */
        end = begin;

        /* 各 @brief の本文は、次の行頭が '@' で始まる位置か終端までを範囲とする。 */
        while (*end != '\0')
        {
            if (*end == '\n')
            {
                const char *next = end + 1;
                while ((*next == ' ') || (*next == '\t'))
                {
                    next++;
                }

                if ((*next == '@') || (*next == '\0'))
                {
                    break;
                }
            }

            end++;
        }

        part = collapse_ws(begin, end);
        if (part == NULL)
        {
            nonspace = begin;
            while ((nonspace < end) && (isspace((unsigned char)*nonspace) != 0))
            {
                nonspace++;
            }

            if (nonspace < end)
            {
                free(brief);
                return NULL;
            }
        }
        else
        {
            brief = struct_meta_internal_parse_doc_concat(brief, part);
            if (brief == NULL)
            {
                return NULL;
            }
        }

        cursor = end;
    }
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_internal_parse_doc_has_file_tag(const char *raw)
{
    char *stripped;
    int found;

    if (raw == NULL)
    {
        return 0;
    }

    stripped = strip_doc_text(raw);
    if (stripped == NULL)
    {
        return 0;
    }

    if (find_doc_tag(stripped, "file") != NULL)
    {
        found = 1;
    }
    else
    {
        found = 0;
    }

    free(stripped);

    return found;
}

/*
 * @struct_meta_other のような別タグを除外しつつ、'{' を書き忘れた @struct_meta も
 * 呼び出し側で構文エラーとして診断できるよう、直後が '{'、空白、終端のいずれかなら一致とする。
 */
static int is_meta_cmd(const char *p)
{
    static const char command[] = "@struct_meta";
    const size_t command_len = sizeof(command) - 1U;

    return (strncmp(p, command, command_len) == 0) &&
           ((p[command_len] == '{') || (isspace((unsigned char)p[command_len]) != 0) || (p[command_len] == '\0'));
}

static char *copy_range(const char *begin, const char *end)
{
    const size_t len = (size_t)(end - begin);
    char *out = (char *)malloc(len + 1U);

    if (out == NULL)
    {
        return NULL;
    }

    memcpy(out, begin, len);
    out[len] = '\0';

    return out;
}

static int valid_attribute_key(const char *key)
{
    for (const char *p = key; *p != '\0'; p++)
    {
        if ((isalnum((unsigned char)*p) == 0) && (*p != '.') && (*p != '_') && (*p != '-'))
        {
            return 0;
        }
    }

    return key[0] != '\0'; /* 空の属性名は不正とする */
}

static struct_meta_internal_parse_attribute *find_attribute(struct_meta_internal_parse_attribute *attributes,
                                                            const char *key)
{
    for (struct_meta_internal_parse_attribute *attribute = attributes; attribute != NULL; attribute = attribute->next)
    {
        if (strcmp(attribute->key, key) == 0)
        {
            return attribute;
        }
    }

    return NULL;
}

static void append_attribute(struct_meta_internal_parse_attribute **attributes,
                             struct_meta_internal_parse_attribute *attribute)
{
    if (*attributes == NULL)
    {
        *attributes = attribute;
        return;
    }

    struct_meta_internal_parse_attribute *tail = *attributes;
    while (tail->next != NULL)
    {
        tail = tail->next;
    }

    tail->next = attribute;
}

static int parse_attributes(const char *text, const int line, struct_meta_internal_parse_attribute **attributes_out,
                            struct_meta_diagnostic *diagnostic)
{
    static const size_t command_len = sizeof("@struct_meta") - 1U;
    const char *p = text;

    /*
     * @struct_meta{key} と @struct_meta{key=value} を先頭から順に解析する。
     * 最初の違反を診断した時点で 1 を返す。
     * 書式の正本は app/struct-meta/docs/architecture.md の「Doxygen 属性の書式」節である。
     */
    while (*p != '\0')
    {
        if ((*p != '@') || (is_meta_cmd(p) == 0))
        {
            p++;
            continue;
        }

        const char *open = p + command_len;
        while ((*open == ' ') || (*open == '\t'))
        {
            open++;
        }

        if (*open != '{')
        {
            struct_meta_internal_diagnose(diagnostic, line, "@struct_meta に { がありません");
            return 1;
        }

        const char *close = strchr(open + 1, '}');
        if (close == NULL)
        {
            struct_meta_internal_diagnose(diagnostic, line, "@struct_meta に } がありません");
            return 1;
        }

        for (const char *q = open + 1; q < close; q++)
        {
            if ((*q == '{') || (*q == '\n') || (*q == '\r'))
            {
                struct_meta_internal_diagnose(diagnostic, line, "@struct_meta の属性は 1 行で記載してください");
                return 1;
            }
        }

        /* 波括弧内の前後の空白を除き、'=' の有無で属性名と属性値に分ける。 */
        const char *begin = open + 1;
        const char *end = close;
        while ((begin < end) && (isspace((unsigned char)*begin) != 0))
        {
            begin++;
        }

        while ((end > begin) && (isspace((unsigned char)end[-1]) != 0))
        {
            end--;
        }

        const char *separator = memchr(begin, '=', (size_t)(end - begin));
        const char *key_end;

        if (separator == NULL)
        {
            key_end = end;
        }
        else
        {
            key_end = separator;
        }

        while ((key_end > begin) && (isspace((unsigned char)key_end[-1]) != 0))
        {
            key_end--;
        }

        char *key = copy_range(begin, key_end);
        if ((key == NULL) || (valid_attribute_key(key) == 0))
        {
            struct_meta_internal_diagnose(diagnostic, line, "@struct_meta の属性名が不正です");
            free(key);
            return 1;
        }

        char *value = NULL;

        if (separator != NULL)
        {
            const char *value_begin = separator + 1;
            while ((value_begin < end) && (isspace((unsigned char)*value_begin) != 0))
            {
                value_begin++;
            }

            if (value_begin == end)
            {
                struct_meta_internal_diagnose(diagnostic, line, "@struct_meta{%s=} の属性値が空です", key);
                free(key);
                return 1;
            }

            value = copy_range(value_begin, end);
            if (value == NULL)
            {
                free(key);
                return 1;
            }
        }

        /* 同一コメント内の重複を拒否し、属性リストの末尾へ追加する。 */
        if (find_attribute(*attributes_out, key) != NULL)
        {
            struct_meta_internal_diagnose(diagnostic, line, "属性が重複しています: %s", key);
            free(value);
            free(key);
            return 1;
        }

        struct_meta_internal_parse_attribute *attribute =
            (struct_meta_internal_parse_attribute *)calloc(1, sizeof(*attribute));
        if (attribute == NULL)
        {
            free(value);
            free(key);
            return 1;
        }

        attribute->key = key;
        attribute->value = value;
        append_attribute(attributes_out, attribute);
        p = close + 1;
    }

    return 0;
}

static char *copy_without_meta_cmds(const char *text)
{
    size_t len = strlen(text);
    char *out = (char *)malloc(len + 1U);
    size_t o = 0;
    const char *p = text;

    if (out == NULL)
    {
        return NULL;
    }

    while (*p != '\0')
    {
        if ((*p == '@') && (is_meta_cmd(p) != 0))
        {
            const char *open = p + (sizeof("@struct_meta") - 1U);
            while ((*open == ' ') || (*open == '\t'))
            {
                open++;
            }

            if (*open == '{')
            {
                const char *close = strchr(open + 1, '}');

                /*
                 * 閉じ波括弧がない記述は parse_attributes() が先に拒否するため、通常の解析経路では到達しない。
                 * struct_meta_internal_parse_brief_from_doc() が単独で呼び出された場合も無限ループにならないよう、
                 * '{' の直後から走査を続ける。
                 */
                if (close == NULL)
                {
                    p = (open + 1);
                }
                else
                {
                    p = (close + 1);
                }

                continue;
            }
        }

        out[o] = *p;
        o++;
        p++;
    }

    out[o] = '\0';

    return out;
}

/* Doxygen コメントは、ヘッダーに記載 */

char *struct_meta_internal_parse_brief_from_doc(const char *raw, int is_postfix)
{
    char *stripped;
    char *without_meta;
    char *brief;

    if (raw == NULL)
    {
        return NULL;
    }

    stripped = strip_doc_text(raw);
    if (stripped == NULL)
    {
        return NULL;
    }

    without_meta = copy_without_meta_cmds(stripped);
    free(stripped);

    if (without_meta == NULL)
    {
        return NULL;
    }

    brief = extract_brief_tag(without_meta);
    if ((brief == NULL) && (is_postfix != 0))
    {
        brief = collapse_ws(without_meta, without_meta + strlen(without_meta));
    }

    free(without_meta);

    return brief;
}

/* Doxygen コメントは、ヘッダーに記載 */

struct_meta_internal_parse_doc_attrs struct_meta_internal_parse_doc_attrs_from_raw(const char *raw,
                                                                                   const int is_postfix, const int line,
                                                                                   struct_meta_diagnostic *diagnostic)
{
    struct_meta_internal_parse_doc_attrs attrs = {0};
    char *stripped;

    if (raw == NULL)
    {
        return attrs;
    }

    stripped = strip_doc_text(raw);
    if (stripped == NULL)
    {
        return attrs;
    }

    attrs.invalid = parse_attributes(stripped, line, &attrs.attributes, diagnostic);
    if (attrs.invalid == 0)
    {
        attrs.brief = struct_meta_internal_parse_brief_from_doc(raw, is_postfix);
    }

    free(stripped);

    return attrs;
}

/* Doxygen コメントは、ヘッダーに記載 */

struct_meta_internal_parse_doc_attrs
struct_meta_internal_parse_doc_attrs_choose(struct_meta_internal_parse_doc_attrs prefix,
                                            struct_meta_internal_parse_doc_attrs postfix, const int line,
                                            struct_meta_diagnostic *diagnostic)
{
    struct_meta_internal_parse_doc_attrs out = prefix;

    if (postfix.brief != NULL)
    {
        free(prefix.brief);
        out.brief = postfix.brief;
    }

    out.invalid = (prefix.invalid != 0) || (postfix.invalid != 0);

    /*
     * 重複を診断した場合も属性リストを連結し、所有権を out へ集約する。
     * 呼び出し側は invalid の値によらず out だけを解放すればよい。
     */
    for (struct_meta_internal_parse_attribute *attribute = postfix.attributes; attribute != NULL;
         attribute = attribute->next)
    {
        if (find_attribute(prefix.attributes, attribute->key) != NULL)
        {
            struct_meta_internal_diagnose(diagnostic, line, "属性が重複しています: %s", attribute->key);
            out.invalid = 1;
        }
    }

    if (prefix.attributes == NULL)
    {
        out.attributes = postfix.attributes;
    }
    else
    {
        struct_meta_internal_parse_attribute *tail = prefix.attributes;
        while (tail->next != NULL)
        {
            tail = tail->next;
        }

        tail->next = postfix.attributes;
    }

    return out;
}

/* Doxygen コメントは、ヘッダーに記載 */

char *struct_meta_internal_parse_brief_choose(char *prefix_brief, char *postfix_brief)
{
    if (postfix_brief != NULL)
    {
        free(prefix_brief);
        return postfix_brief;
    }

    return prefix_brief;
}

/* Doxygen コメントは、ヘッダーに記載 */

char *struct_meta_internal_parse_doc_concat(char *first, char *second)
{
    size_t first_len;
    size_t second_len;
    char *out;

    if (first == NULL)
    {
        return second;
    }

    if (second == NULL)
    {
        return first;
    }

    first_len = strlen(first);
    second_len = strlen(second);
    out = (char *)malloc(first_len + 1U + second_len + 1U);
    if (out == NULL)
    {
        free(first);
        free(second);
        return NULL;
    }

    memcpy(out, first, first_len);
    out[first_len] = '\n';
    memcpy(out + first_len + 1U, second, second_len + 1U); /* 終端の NUL も含めてコピーする */
    free(first);
    free(second);

    return out;
}
