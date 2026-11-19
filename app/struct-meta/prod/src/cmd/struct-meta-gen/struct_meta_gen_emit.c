/**
 *******************************************************************************
 *  @file           struct_meta_gen_emit.c
 *  @brief          組み立て済みの記述子から、メタデータ記述子の C ソースを生成します。
 *  @author         Tetsuo Honda
 *  @date           2026/08/16
 *  @version        1.0.0
 *
 *  記述子の初期化子には `offsetof` と `sizeof` を出力し、レイアウトの決定を実際の
 *  コンパイラへ委ねます。あわせて、`libstruct_meta` のレイアウト エンジンが求めた
 *  値と一致することを検査する `_Static_assert` を出力します。これにより、実行時に
 *  ヘッダーを解析する経路 (事後解析型) の正しさが、この経路 (事前組み込み型) の
 *  ビルドのたびに検証されます (`docs/architecture.md` の設計方針を参照)。
 *
 *  型のスペリングは使いません。要素 1 個の大きさもフィールドの式から求めるため、
 *  記述子だけを入力として出力できます。
 *
 *  ネスト構造体メンバーがある場合、参照先の構造体記述子を先に出力してから
 *  (依存順)、それを参照する構造体の記述子を出力します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "struct_meta_gen_emit.h"
#include "struct_meta_gen_emit_array.h"

#include <struct_meta/catalog/catalog.h>

#include <cplat/base/result.h>
#include <cplat/crt/stdio.h>
#include <cplat/hashtable/hashtable.h>
#include <cplat/runtime/abort.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** 生成パス・識別子を組み立てる作業バッファーのバイト数です。 */
#define STRUCT_META_GEN_EMIT_PATH_BYTES 512

/**
 *  @brief          記述子をすでに出力した構造体名を記録する連結リストです。
 */
typedef struct emitted_name
{
    const char *name;
    struct emitted_name *next;
} emitted_name;

static int is_emitted(const emitted_name *list, const char *name)
{
    for (; list != NULL; list = list->next)
    {
        if (strcmp(list->name, name) == 0)
        {
            return 1;
        }
    }

    return 0;
}

/**
 *  @brief          パスの末尾ファイル名を返します (`/` と `\\` を区切りとします)。
 */
static const char *path_basename(const char *path)
{
    const char *base = path;

    for (const char *p = path; *p != '\0'; p++)
    {
        if ((*p == '/') || (*p == '\\'))
        {
            base = p + 1;
        }
    }

    return base;
}

/**
 *  @brief          ヘッダー パスから識別子用 stem を取り出します (`sample_types.h` → `sample_types`)。
 *
 *  英数字と `_` 以外は `_` に置換します。先頭が数字のときは先頭に `x` を付けます。
 */
static int header_stem(const char *header_path, char *dest, size_t dest_size)
{
    const char *base = path_basename(header_path);
    char raw[STRUCT_META_GEN_EMIT_PATH_BYTES];
    size_t raw_len = 0;

    /*
     * ctype.h の分類関数の引数は、unsigned char に収まる値か EOF でなければならない。
     * char が符号付きのとき、上位ビットが立ったバイトをそのまま渡すと未定義動作になる。
     * see: https://man7.org/linux/man-pages/man3/isalnum.3.html
     */
    for (const char *p = base; (*p != '\0') && (*p != '.'); p++)
    {
        if (raw_len + 1U >= sizeof(raw))
        {
            return 1;
        }

        if (isalnum((unsigned char)*p) != 0)
        {
            raw[raw_len] = *p;
        }
        else
        {
            raw[raw_len] = '_';
        }

        raw_len++;
    }

    raw[raw_len] = '\0';

    if (raw_len == 0U)
    {
        return 1;
    }

    if (isdigit((unsigned char)raw[0]) != 0)
    {
        if (dest_size < (raw_len + 2U))
        {
            return 1;
        }

        dest[0] = 'x';
        memcpy(dest + 1, raw, raw_len + 1U);
        return 0;
    }

    if (dest_size < (raw_len + 1U))
    {
        return 1;
    }

    memcpy(dest, raw, raw_len + 1U);

    return 0;
}

/**
 *  @brief          stem を全大文字にした接頭辞を作ります (`sample_types` → `SAMPLE_TYPES`)。
 */
static int stem_prefix(const char *stem, char *dest, size_t dest_size)
{
    size_t i;

    /*
     * toupper() の引数は、unsigned char に収まる値か EOF でなければならない。
     * char が符号付きのとき、上位ビットが立ったバイトをそのまま渡すと未定義動作になる。
     * see: https://man7.org/linux/man-pages/man3/toupper.3.html
     */
    for (i = 0; stem[i] != '\0'; i++)
    {
        if ((i + 1U) >= dest_size)
        {
            return 1;
        }

        dest[i] = (char)toupper((unsigned char)stem[i]);
    }

    dest[i] = '\0';

    return 0;
}

/**
 *  @brief          `--out` の `.c` から同名の `.h` パスを導出します。
 */
static int header_out_path(const char *c_path, char *header_path_out, size_t header_path_size)
{
    size_t len = strlen(c_path);
    if ((len < 2U) || (c_path[len - 2U] != '.') || (c_path[len - 1U] != 'c'))
    {
        return 1;
    }

    if ((len + 1U) > header_path_size)
    {
        return 1;
    }

    memcpy(header_path_out, c_path, len - 1U);
    header_path_out[len - 1U] = 'h';
    header_path_out[len] = '\0';

    return 0;
}

/**
 *  @brief          生成ヘッダーのファイル名だけを返します (`gen/foo.h` → `foo.h`)。
 */
static const char *generated_header_include(const char *header_out)
{
    return path_basename(header_out);
}

/*
 * 文字列を C の文字列リテラルとして書き出す。
 * NULL は識別子 NULL と書く。
 * 引用符とバックスラッシュはバックスラッシュで逃がし、改行はバックスラッシュと n の 2 文字で書く。
 */
static void fprint_c_string(FILE *out, const char *text)
{
    /*
     * 生成 C ソースへの書き込みの失敗はストリームに残り、struct_meta_gen_emit() がファイルを閉じる直前に
     * まとめて検査するため、この関数の out への fputs と fputc の戻り値は明示的に破棄する。
     */
    if (text == NULL)
    {
        (void)fputs("NULL", out);
        return;
    }

    (void)fputc('"', out);

    for (const char *p = text; *p != '\0'; p++)
    {
        if ((*p == '"') || (*p == '\\'))
        {
            (void)fputc('\\', out);
            (void)fputc(*p, out);
        }
        else if (*p == '\n')
        {
            (void)fputs("\\n", out);
        }
        else
        {
            (void)fputc(*p, out);
        }
    }

    (void)fputc('"', out);
}

/**
 *  @brief          フィールド種別から、生成コードへ出力する列挙定数名を求めます。
 */
static const char *field_kind_name(struct_meta_field_kind kind)
{
    switch (kind)
    {
    case STRUCT_META_FIELD_SIGNED_INTEGER:
        return "STRUCT_META_FIELD_SIGNED_INTEGER";
    case STRUCT_META_FIELD_UNSIGNED_INTEGER:
        return "STRUCT_META_FIELD_UNSIGNED_INTEGER";
    case STRUCT_META_FIELD_FLOAT:
        return "STRUCT_META_FIELD_FLOAT";
    case STRUCT_META_FIELD_DOUBLE:
        return "STRUCT_META_FIELD_DOUBLE";
    case STRUCT_META_FIELD_CHAR_ARRAY:
        return "STRUCT_META_FIELD_CHAR_ARRAY";
    case STRUCT_META_FIELD_STRUCT:
        return "STRUCT_META_FIELD_STRUCT";
    default:
        /*
         * 記述子は libstruct_meta が組み立てるため、未知の種別は記述子の破壊を意味する。
         * 別の種別の定数で代用すると、誤った型の生成コードを書き出す。
         */
        CPLAT_ABORT("記述子に未知のフィールド種別があります");
    }
}

/**
 *  @brief          フィールドが配列として宣言されていたかどうかを返します。
 *
 *  `char` 配列は要素数 1 の NUL 終端文字列として記述子に入るため、種別で判定します。
 *  要素数 1 の配列はインデックスの有無で `sizeof` の値が変わらないため、区別しません。
 */
static int field_is_declared_array(const struct_meta_field *field)
{
    if ((field->kind == STRUCT_META_FIELD_CHAR_ARRAY) || (field->element_count > 1U))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 *  @brief          属性の配列を生成コードへ書き出します。
 */
static void emit_attributes(FILE *out, const char *symbol, const struct_meta_attribute *attributes, size_t count)
{
    if (count == 0U)
    {
        return;
    }

    /*
     * 生成 C ソースへの書き込みの失敗はストリームに残り、struct_meta_gen_emit() がファイルを閉じる直前に
     * まとめて検査するため、この関数の out への fprintf の戻り値は明示的に破棄する。
     */
    (void)fprintf(out, "static const struct_meta_attribute g_%s_attributes[] = {\n", symbol);

    for (size_t i = 0; i < count; i++)
    {
        (void)fprintf(out, "    { ");
        fprint_c_string(out, attributes[i].key);
        (void)fprintf(out, ", ");
        fprint_c_string(out, attributes[i].value);
        (void)fprintf(out, " },\n");
    }

    (void)fprintf(out, "};\n\n");
}

/**
 *  @brief          レイアウト エンジンの計算値と、コンパイラの実レイアウトを照合します。
 *
 *  記述子の初期化子は `offsetof` と `sizeof` を使うため、この検査が通る限り、
 *  事前組み込み型と事後解析型は同じレイアウトを表します。破れた場合は、生成コードの
 *  コンパイル時に落ちます。
 *  see: app/struct-meta/docs/architecture.md
 */
static void emit_layout_assertions(FILE *out, const struct_meta_descriptor *descriptor)
{
    /*
     * 生成 C ソースへの書き込みの失敗はストリームに残り、struct_meta_gen_emit() がファイルを閉じる直前に
     * まとめて検査するため、この関数の out への fprintf の戻り値は明示的に破棄する。
     */
    (void)fprintf(out, "/* レイアウト エンジンの計算値と、コンパイラが決めた配置との照合 */\n");
    (void)fprintf(out, "_Static_assert(sizeof(%s) == %zu, \"%s: レイアウト エンジンの計算値と一致しません\");\n",
                  descriptor->name, descriptor->size, descriptor->name);

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        const struct_meta_field *field = &descriptor->fields[i];
        (void)fprintf(
            out, "_Static_assert(offsetof(%s, %s) == %zu, \"%s.%s: レイアウト エンジンの計算値と一致しません\");\n",
            descriptor->name, field->name, field->offset, descriptor->name, field->name);
    }

    (void)fprintf(out, "\n");
}

/**
 *  @brief          snprintf() の結果が、失敗も切り詰めもなくバッファーへ収まったかを判定します。
 *  @return         収まった場合は 1、失敗または切り詰めの場合は 0 を返します。
 */
static int formatted_fits(int written, size_t buffer_size)
{
    if ((written < 0) || ((size_t)written >= buffer_size))
    {
        return 0;
    }

    return 1;
}

/**
 *  @brief          生成コードの名前が長すぎることを標準エラー出力へ出します。
 */
static void report_name_too_long(const struct_meta_descriptor *descriptor, const struct_meta_field *field)
{
    /* 診断の出力先がほかにないため、戻り値は明示的に破棄する。 */
    (void)fprintf(stderr, "struct-meta-gen: 生成コードの名前が長すぎます: %s.%s\n", descriptor->name, field->name);
}

/**
 *  @brief          記述子 1 個分の C ソースを書き出します。
 *  @return         成功時は 0、生成コードの名前が作業バッファーに収まらない場合は 1 を返します。
 *
 *  ネスト先を先に出力してから自分を出力します。出力済みの構造体は飛ばします。\n
 *  名前が長すぎる場合は、標準エラー出力へ構造体名とフィールド名を出して 1 を返します。
 *  その時点までの出力は out に残ります。
 */
static int emit_struct(FILE *out, const struct_meta_descriptor *descriptor, emitted_name **emitted)
{
    if (is_emitted(*emitted, descriptor->name))
    {
        return 0;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (descriptor->fields[i].nested != NULL)
        {
            if (emit_struct(out, descriptor->fields[i].nested, emitted) != 0)
            {
                return 1;
            }
        }
    }

    /*
     * フィールド配列と記述子の初期化子が属性配列の名前を参照する。
     * そのため属性配列を先に出力する。
     */
    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        const struct_meta_field *field = &descriptor->fields[i];
        char symbol[STRUCT_META_GEN_EMIT_PATH_BYTES];
        if (formatted_fits(snprintf(symbol, sizeof(symbol), "%s_%s", descriptor->name, field->name), sizeof(symbol)) ==
            0)
        {
            report_name_too_long(descriptor, field);
            return 1;
        }

        emit_attributes(out, symbol, field->attributes, field->attribute_count);
    }

    emit_attributes(out, descriptor->name, descriptor->attributes, descriptor->attribute_count);

    /*
     * 生成 C ソースへの書き込みの失敗はストリームに残り、struct_meta_gen_emit() がファイルを閉じる直前に
     * まとめて検査するため、この関数の out への fprintf と fputs の戻り値は明示的に破棄する。
     */
    (void)fprintf(out, "static const struct_meta_field g_%s_fields[] = {\n", descriptor->name);

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        const struct_meta_field *field = &descriptor->fields[i];
        const char *element;

        if (field_is_declared_array(field) != 0)
        {
            element = "[0]";
        }
        else
        {
            element = "";
        }

        char element_size_expr[STRUCT_META_GEN_EMIT_PATH_BYTES];
        char char_buffer_expr[STRUCT_META_GEN_EMIT_PATH_BYTES];
        char nested_expr[STRUCT_META_GEN_EMIT_PATH_BYTES];

        if (formatted_fits(snprintf(element_size_expr, sizeof(element_size_expr), "sizeof(((%s *)0)->%s%s)",
                                    descriptor->name, field->name, element),
                           sizeof(element_size_expr)) == 0)
        {
            report_name_too_long(descriptor, field);
            return 1;
        }

        if (field->char_buffer_size != 0U)
        {
            if (formatted_fits(snprintf(char_buffer_expr, sizeof(char_buffer_expr), "sizeof(((%s *)0)->%s)",
                                        descriptor->name, field->name),
                               sizeof(char_buffer_expr)) == 0)
            {
                report_name_too_long(descriptor, field);
                return 1;
            }
        }
        else
        {
            /*
             * 固定の短い文字列を STRUCT_META_GEN_EMIT_PATH_BYTES バイトのバッファーへ書き、切り詰めは起きないため、
             * 戻り値は明示的に破棄する。
             */
            (void)snprintf(char_buffer_expr, sizeof(char_buffer_expr), "0");
        }

        if (field->nested != NULL)
        {
            if (formatted_fits(snprintf(nested_expr, sizeof(nested_expr), "&g_%s_desc", field->nested->name),
                               sizeof(nested_expr)) == 0)
            {
                report_name_too_long(descriptor, field);
                return 1;
            }
        }
        else
        {
            /*
             * 固定の短い文字列を STRUCT_META_GEN_EMIT_PATH_BYTES バイトのバッファーへ書き、切り詰めは起きないため、
             * 戻り値は明示的に破棄する。
             */
            (void)snprintf(nested_expr, sizeof(nested_expr), "NULL");
        }

        (void)fprintf(out, "    { \"%s\", %s, 0, offsetof(%s, %s), %s, %zu, %s, %s, ", field->name,
                      field_kind_name(field->kind), descriptor->name, field->name, element_size_expr,
                      field->element_count, char_buffer_expr, nested_expr);
        fprint_c_string(out, field->brief);

        if (field->attribute_count == 0U)
        {
            (void)fputs(", NULL, 0 },\n", out);
        }
        else
        {
            (void)fprintf(out, ", g_%s_%s_attributes, %zu },\n", descriptor->name, field->name, field->attribute_count);
        }
    }

    (void)fprintf(out, "};\n\n");

    (void)fprintf(out, "static const struct_meta_descriptor g_%s_desc = { \"%s\", sizeof(%s), g_%s_fields, %zu, ",
                  descriptor->name, descriptor->name, descriptor->name, descriptor->name, descriptor->field_count);
    fprint_c_string(out, descriptor->brief);

    if (descriptor->attribute_count == 0U)
    {
        (void)fprintf(out, ", NULL, 0 };\n\n");
    }
    else
    {
        (void)fprintf(out, ", g_%s_attributes, %zu };\n\n", descriptor->name, descriptor->attribute_count);
    }

    emit_layout_assertions(out, descriptor);

    emitted_name *node = (emitted_name *)calloc(1, sizeof(*node));
    if (node == NULL)
    {
        /* 診断の出力先がほかにないため、戻り値は明示的に破棄する。 */
        (void)fprintf(stderr, "struct-meta-gen: 出力状態を確保できません\n");
        exit(1);
    }

    node->name = descriptor->name;
    node->next = *emitted;
    *emitted = node;

    return 0;
}

/**
 *  @brief          生成ファイルへの書き込みの失敗を検査してから閉じます。
 *  @param[in]      out   閉じる出力ストリームです。
 *  @param[in]      path  診断に出す出力ファイルのパスです。
 *  @return         書き込みと閉じる処理が成功した場合は 0、失敗した場合は 1 を返します。
 *
 *  途中の書き込みの失敗はストリームに残るため、閉じる直前に fflush() と ferror() でまとめて検査します。\n
 *  失敗した場合は、標準エラー出力へ書き込めなかった出力ファイルを出します。
 */
static int close_generated_file(FILE *out, const char *path)
{
    if ((fflush(out) != 0) || (ferror(out) != 0))
    {
        /* 診断の出力先がほかにないため、戻り値は明示的に破棄する。 */
        (void)fprintf(stderr, "struct-meta-gen: 出力ファイルへ書き込めません: %s\n", path);

        /* 書き込みの失敗で 1 を返す経路のため、戻り値は明示的に破棄する。 */
        (void)fclose(out);
        return 1;
    }

    if (fclose(out) != 0)
    {
        /* 診断の出力先がほかにないため、戻り値は明示的に破棄する。 */
        (void)fprintf(stderr, "struct-meta-gen: 出力ファイルへ書き込めません: %s\n", path);
        return 1;
    }

    return 0;
}

/**
 *  @brief          型一覧 enum と取得関数の宣言をヘッダーへ書き出します。
 */
static int emit_catalog_header(const char *header_out, const char *stem, const char *prefix,
                               const struct_meta_catalog *catalog)
{
    FILE *out = cplat_fopen(header_out, "w", NULL);
    if (out == NULL)
    {
        /* 診断の出力先がほかにないため、戻り値は明示的に破棄する。 */
        (void)fprintf(stderr, "struct-meta-gen: 出力ファイルを作成できません: %s\n", header_out);
        return 1;
    }

    size_t count = 0U;

    /* 引数は非 NULL であり、失敗しないため、戻り値は明示的に破棄する。 */
    (void)struct_meta_catalog_get_count(catalog, &count);

    /*
     * 生成ヘッダーへの書き込みの失敗はストリームに残り、この関数でファイルを閉じる直前にまとめて検査するため、
     * この関数の out への fprintf と fputc の戻り値は明示的に破棄する。
     */
    (void)fprintf(out, "/* このファイルは struct-meta-gen が自動生成した。手編集しないこと。 */\n");
    (void)fprintf(out, "#ifndef %s_META_H\n", prefix);
    (void)fprintf(out, "#define %s_META_H\n\n", prefix);
    (void)fprintf(out, "#include <struct_meta/catalog/catalog.h>\n");
    (void)fprintf(out, "#include <struct_meta/meta/meta.h>\n\n");
    (void)fprintf(out, "#ifdef __cplusplus\n");
    (void)fprintf(out, "extern \"C\"\n");
    (void)fprintf(out, "{\n");
    (void)fprintf(out, "#endif /* __cplusplus */\n\n");
    (void)fprintf(out, "typedef enum %s_meta_id\n", stem);
    (void)fprintf(out, "{\n");

    for (size_t index = 0; index < count; index++)
    {
        const struct_meta_descriptor *descriptor = NULL;

        /* 引数は非 NULL で、添字は get_count で得た件数未満であり、失敗しないため、戻り値は明示的に破棄する。 */
        (void)struct_meta_catalog_get(catalog, index, &descriptor);
        (void)fprintf(out, "    %s_META_", prefix);

        /*
         * toupper() の引数は、unsigned char に収まる値か EOF でなければならない。
         * char が符号付きのとき、上位ビットが立ったバイトをそのまま渡すと未定義動作になる。
         * see: https://man7.org/linux/man-pages/man3/toupper.3.html
         */
        for (const char *p = descriptor->name; *p != '\0'; p++)
        {
            (void)fputc(toupper((unsigned char)*p), out);
        }

        (void)fprintf(out, " = %zu,\n", index);
    }

    (void)fprintf(out, "    %s_META_COUNT = %zu,\n", prefix, count);
    (void)fprintf(out, "} %s_meta_id;\n\n", stem);
    (void)fprintf(out, "/**\n");
    (void)fprintf(out, " *  @brief          カタログが持つ型の数を返します。\n");
    (void)fprintf(out, " *  @return         型の数です。\n");
    (void)fprintf(out, " *\n");
    (void)fprintf(out, " *  @par            スレッド セーフ\n");
    (void)fprintf(out, " *  本関数はスレッド セーフです。\n");
    (void)fprintf(out, " */\n");
    (void)fprintf(out, "size_t %s_meta_count(void);\n\n", stem);

    (void)fprintf(out, "/**\n");
    (void)fprintf(out, " *  @brief          列挙 ID で記述子を取得します。\n");
    (void)fprintf(out, " *  @param[in]      id 取得する型の ID です。\n");
    (void)fprintf(out, " *  @return         記述子です。範囲外の ID では NULL を返します。\n");
    (void)fprintf(out, " *\n");
    (void)fprintf(out, " *  @par            スレッド セーフ\n");
    (void)fprintf(out, " *  本関数はスレッド セーフです。\n");
    (void)fprintf(out, " */\n");
    (void)fprintf(out, "const struct_meta_descriptor *%s_meta_get(%s_meta_id id);\n\n", stem, stem);

    (void)fprintf(out, "/**\n");
    (void)fprintf(out, " *  @brief          構造体名で記述子を検索します。\n");
    (void)fprintf(out, " *  @param[in]      name 検索する構造体名です。NULL を渡せます。\n");
    (void)fprintf(out, " *  @return         記述子です。NULL または未知の名前では NULL を返します。\n");
    (void)fprintf(out, " *\n");
    (void)fprintf(out, " *  検索には、生成時に構築した索引の埋め込みイメージを使います。\n");
    (void)fprintf(out, " *  イメージへの接続は初回呼び出し時に 1 回だけ行います。\n");
    (void)fprintf(out, " *\n");
    (void)fprintf(out, " *  @attention      埋め込みイメージへ接続できない場合、本関数はプロセスを終了させます。\n");
    (void)fprintf(out, " *                  同一ビルドで生成したイメージが読めないという不変条件の破れであり、\n");
    (void)fprintf(out, " *                  検索を続けても記述子を返せないためです。\n");
    (void)fprintf(out, " *\n");
    (void)fprintf(out, " *  @par            スレッド セーフ\n");
    (void)fprintf(out, " *  本関数は条件付きスレッド セーフです。\\n\n");
    (void)fprintf(out,
                  " *  初回の呼び出しは、カタログの記述子を索引 (@c struct_meta_index_register) へ登録します。\\n\n");
    (void)fprintf(out, " *  そのため、初回の呼び出しは、索引を更新する操作 (カタログの作成と破棄など) と、\n");
    (void)fprintf(out, " *  索引を参照する操作 (@c struct_meta_descriptor_validate など) と\n");
    (void)fprintf(out, " *  同時に実行しないでください。\\n\n");
    (void)fprintf(out, " *  初回の呼び出しが完了した後は、同時に実行できます。\n");
    (void)fprintf(out, " */\n");
    (void)fprintf(out, "const struct_meta_descriptor *%s_meta_find(const char *name);\n\n", stem);

    (void)fprintf(out, "/**\n");
    (void)fprintf(out, " *  @brief          カタログ ハンドルを返します。\n");
    (void)fprintf(out, " *  @return         カタログです。ハンドルを確保できない場合は NULL を返します。\n");
    (void)fprintf(out, " *\n");
    (void)fprintf(out, " *  埋め込み索引へ接続できない場合は、プロセスを異常終了させます。\n");
    (void)fprintf(out, " *\n");
    (void)fprintf(out, " *  実行時にヘッダーを解析して作ったカタログと同じ API で扱えます。\n");
    (void)fprintf(out, " *  このカタログは静的領域を指すため、破棄してはなりません。\n");
    (void)fprintf(out, " *\n");
    (void)fprintf(out, " *  @par            スレッド セーフ\n");
    (void)fprintf(out, " *  本関数は条件付きスレッド セーフです。\\n\n");
    (void)fprintf(out,
                  " *  初回の呼び出しは、カタログの記述子を索引 (@c struct_meta_index_register) へ登録します。\\n\n");
    (void)fprintf(out, " *  そのため、初回の呼び出しは、索引を更新する操作 (カタログの作成と破棄など) と、\n");
    (void)fprintf(out, " *  索引を参照する操作 (@c struct_meta_descriptor_validate など) と\n");
    (void)fprintf(out, " *  同時に実行しないでください。\\n\n");
    (void)fprintf(out, " *  初回の呼び出しが完了した後は、同時に実行できます。\n");
    (void)fprintf(out, " */\n");
    (void)fprintf(out, "const struct_meta_catalog *%s_meta_catalog(void);\n\n", stem);
    (void)fprintf(out, "#ifdef __cplusplus\n");
    (void)fprintf(out, "}\n");
    (void)fprintf(out, "#endif /* __cplusplus */\n\n");
    (void)fprintf(out, "#endif /* %s_META_H */\n", prefix);

    return close_generated_file(out, header_out);
}

/**
 *  @brief          構造体名のうち、終端を含む最長のバイト数を求めます。
 */
static size_t max_struct_name_bytes(const struct_meta_catalog *catalog, size_t count)
{
    size_t longest = 0U;

    for (size_t i = 0; i < count; i++)
    {
        const struct_meta_descriptor *descriptor = NULL;

        /* 引数は非 NULL で、添字は記述子数である引数 count 未満であり、失敗しないため、戻り値は明示的に破棄する。 */
        (void)struct_meta_catalog_get(catalog, i, &descriptor);
        const size_t length = strlen(descriptor->name);
        if (length > longest)
        {
            longest = length;
        }
    }

    return longest + 1U;
}

/**
 *  @brief          構造体名からインデックスを引き当てるハッシュ表を構築し、永続化イメージを書き出します。
 *  @return         成功なら 0、失敗なら 1 です。
 *
 *  レコード数もキーの最大長も生成時に確定するため、実行時に構築せず、ここで構築した
 *  テーブルの永続化イメージを生成コードへ埋め込みます。実行時は接続するだけになります。
 */
static int emit_catalog_index_image(FILE *out, const char *prefix, const struct_meta_catalog *catalog, size_t count)
{
    /* 診断の出力先がほかにないため、この関数の stderr への fprintf の戻り値は明示的に破棄する。 */

    cplat_hashtable_config config = {0};
    cplat_hashtable *table = NULL;

    config.capacity = count * 2U;
    config.key_type = CPLAT_HASHTABLE_FIELD_FIXED_STRING;
    config.key_size = max_struct_name_bytes(catalog, count);
    config.value_type = CPLAT_HASHTABLE_FIELD_BINARY;
    config.value_size = sizeof(size_t);
    config.value_align = sizeof(size_t); /* 返るポインターを size_t として参照する境界 */
    config.lifetime = CPLAT_HASHTABLE_LIFETIME_INFINITE;

    if (cplat_hashtable_create(&config, NULL, 0, NULL, 0, &table) != CPLAT_OK)
    {
        (void)fprintf(stderr, "struct-meta-gen: 索引テーブルを構築できません\n");
        return 1;
    }

    for (size_t index = 0; index < count; index++)
    {
        const struct_meta_descriptor *descriptor = NULL;

        /* 引数は非 NULL で、添字は記述子数である引数 count 未満であり、失敗しないため、戻り値は明示的に破棄する。 */
        (void)struct_meta_catalog_get(catalog, index, &descriptor);

        if (cplat_hashtable_add(table, descriptor->name, &index, CPLAT_HASHTABLE_ADD_DELETED_OVERWRITE) != CPLAT_OK)
        {
            (void)fprintf(stderr, "struct-meta-gen: 索引へ型名を登録できません: %s\n", descriptor->name);
            cplat_hashtable_dispose(table);
            return 1;
        }
    }

    size_t mgmt_size = 0;
    size_t data_size = 0;
    const void *mgmt = NULL;
    const void *data = NULL;

    if ((cplat_hashtable_buffer_size(table, &mgmt_size, &data_size) != CPLAT_OK) ||
        (cplat_hashtable_buffer_ref(table, &mgmt, &data) != CPLAT_OK))
    {
        (void)fprintf(stderr, "struct-meta-gen: 索引の永続化イメージを取得できません\n");
        cplat_hashtable_dispose(table);
        return 1;
    }

    if ((mgmt == NULL) || (data == NULL) || (mgmt_size == 0U) || (data_size == 0U))
    {
        (void)fprintf(stderr, "struct-meta-gen: 索引の永続化イメージが空です\n");
        cplat_hashtable_dispose(table);
        return 1;
    }

    /*
     * 生成 C ソースへの書き込みの失敗はストリームに残り、struct_meta_gen_emit() がファイルを閉じる直前に
     * まとめて検査するため、この関数の out への fprintf の戻り値は明示的に破棄する。
     */
    (void)fprintf(out, "/* struct-meta-gen が構築済みの索引を永続化した。手編集しないこと。 */\n");

    /*
     * 永続化イメージは 8 [byte] 境界を要求されるため、uint64_t の配列として出力する。
     * 正本は app/struct-meta/docs/architecture.md の「生成カタログの索引」節である。
     */
    struct_meta_gen_emit_uint64_array(out, "s_index_mgmt", mgmt, mgmt_size);
    struct_meta_gen_emit_uint64_array(out, "s_index_data", data, data_size);

    /*
     * 埋め込みイメージが前提とする ABI を、生成先のコンパイル時に検査する。
     * 生成器が動作した環境での実測値を出力する。
     */
    (void)fprintf(out, "/* 埋め込みイメージが前提とする ABI を検査する。破れた場合はコンパイル時に落とす。 */\n");
    (void)fprintf(out,
                  "_Static_assert(sizeof(size_t) == %zu, \"%s: 埋め込み索引は size_t %zu バイトを前提とします\");\n",
                  sizeof(size_t), prefix, sizeof(size_t));
    (void)fprintf(out,
                  "_Static_assert(sizeof(time_t) == %zu, \"%s: 埋め込み索引は time_t %zu バイトを前提とします\");\n",
                  sizeof(time_t), prefix, sizeof(time_t));
    (void)fprintf(out,
                  "_Static_assert(sizeof(cplat_hashtable_config) == %zu,\n"
                  "               \"%s: 埋め込み索引は生成時と同じ cplat_hashtable_config 配置を前提とします\");\n\n",
                  sizeof(cplat_hashtable_config), prefix);

    cplat_hashtable_dispose(table);

    return 0;
}

/**
 *  @brief          型一覧テーブルと取得関数を C ソースへ書き出します。
 *  @return         成功なら 0、失敗なら 1 です。
 *
 *  テーブルの並びはヘッダーの宣言順 (enum と同じ) です。記述子の出力順
 *  (依存順) とは独立です。\n
 *  索引への接続とカタログの組み立ては `libstruct_meta` の
 *  @c struct_meta_catalog_attach_static が行います。実行時に解析して作った
 *  カタログと同じハンドルになるため、利用側は経路を意識しません。
 */
static int emit_catalog_source(FILE *out, const char *stem, const char *prefix, const struct_meta_catalog *catalog,
                               size_t count)
{
    /*
     * 生成 C ソースへの書き込みの失敗はストリームに残り、struct_meta_gen_emit() がファイルを閉じる直前に
     * まとめて検査するため、この関数の out への fprintf の戻り値は明示的に破棄する。
     */
    (void)fprintf(out, "static const struct_meta_descriptor *const s_descriptors[%s_META_COUNT] = {\n", prefix);

    for (size_t index = 0; index < count; index++)
    {
        const struct_meta_descriptor *descriptor = NULL;

        /* 引数は非 NULL で、添字は記述子数である引数 count 未満であり、失敗しないため、戻り値は明示的に破棄する。 */
        (void)struct_meta_catalog_get(catalog, index, &descriptor);
        (void)fprintf(out, "    &g_%s_desc,\n", descriptor->name);
    }

    (void)fprintf(out, "};\n\n");

    if (emit_catalog_index_image(out, prefix, catalog, count) != 0)
    {
        return 1;
    }

    (void)fprintf(out, "static cplat_once_flag s_catalog_once;\n");
    (void)fprintf(out, "static struct_meta_catalog *s_catalog;\n\n");

    (void)fprintf(out, "static void destroy_catalog(const cplat_shutdown_event *event, void *context)\n");
    (void)fprintf(out, "{\n");
    (void)fprintf(out, "    (void)event;\n");
    (void)fprintf(out, "    (void)context;\n");
    (void)fprintf(out, "    /* ハンドルだけを解放する。記述子とイメージは静的領域であり解放しない。 */\n");
    (void)fprintf(out, "    struct_meta_catalog_destroy(s_catalog);\n");
    (void)fprintf(out, "    s_catalog = NULL;\n");
    (void)fprintf(out, "}\n\n");

    (void)fprintf(out, "static void attach_catalog(void)\n");
    (void)fprintf(out, "{\n");
    (void)fprintf(out, "    int ret;\n\n");
    (void)fprintf(out, "    ret = struct_meta_catalog_attach_static(s_descriptors, %s_META_COUNT, s_index_mgmt,\n",
                  prefix);
    (void)fprintf(
        out, "                                            sizeof(s_index_mgmt), s_index_data, sizeof(s_index_data),\n");
    (void)fprintf(out, "                                            &s_catalog);\n");
    (void)fprintf(out, "    if (ret == CPLAT_ERR_OUT_OF_MEMORY)\n");
    (void)fprintf(out, "    {\n");
    (void)fprintf(out,
                  "        /* ハンドルを確保できない。カタログは NULL のままとし、検索は見つからない結果を返す。 */\n");
    (void)fprintf(out, "        return;\n");
    (void)fprintf(out, "    }\n");
    (void)fprintf(out, "    /*\n");
    (void)fprintf(out, "     * それ以外の失敗は、同一ビルドで作ったイメージが読めないため、不変条件が破れている。\n");
    (void)fprintf(out, "     * 線形走査へ縮退させず、その場で落とす。\n");
    (void)fprintf(out, "     */\n");
    (void)fprintf(out, "    CPLAT_ASSERT(ret == CPLAT_OK);\n");
    (void)fprintf(out, "    /*\n");
    (void)fprintf(out, "     * 登録に失敗しても、終了時にハンドルを解放しないだけでカタログは使用でき、\n");
    (void)fprintf(out, "     * ハンドルはプロセスの終了時に OS が回収する。\n");
    (void)fprintf(out, "     * cplat_call_once のコールバックであり、失敗を伝える先もないため、\n");
    (void)fprintf(out, "     * 戻り値は明示的に破棄する。\n");
    (void)fprintf(out, "     */\n");
    (void)fprintf(out, "    (void)cplat_shutdown_register(destroy_catalog, NULL);\n");
    (void)fprintf(out, "}\n\n");

    (void)fprintf(out, "const struct_meta_catalog *%s_meta_catalog(void)\n", stem);
    (void)fprintf(out, "{\n");
    (void)fprintf(out, "    cplat_call_once(&s_catalog_once, attach_catalog);\n");
    (void)fprintf(out, "    return s_catalog;\n");
    (void)fprintf(out, "}\n\n");

    (void)fprintf(out, "size_t %s_meta_count(void)\n", stem);
    (void)fprintf(out, "{\n");
    (void)fprintf(out, "    return %s_META_COUNT;\n", prefix);
    (void)fprintf(out, "}\n\n");

    (void)fprintf(out, "const struct_meta_descriptor *%s_meta_get(%s_meta_id id)\n", stem, stem);
    (void)fprintf(out, "{\n");
    (void)fprintf(out, "    if (((int)id < 0) || (id >= %s_META_COUNT))\n", prefix);
    (void)fprintf(out, "    {\n");
    (void)fprintf(out, "        return NULL;\n");
    (void)fprintf(out, "    }\n");
    (void)fprintf(out, "    return s_descriptors[id];\n");
    (void)fprintf(out, "}\n\n");

    (void)fprintf(out, "const struct_meta_descriptor *%s_meta_find(const char *name)\n", stem);
    (void)fprintf(out, "{\n");
    (void)fprintf(out, "    const struct_meta_descriptor *descriptor = NULL;\n");
    (void)fprintf(out, "    if (struct_meta_catalog_find(%s_meta_catalog(), name, &descriptor) != CPLAT_OK)\n", stem);
    (void)fprintf(out, "    {\n");
    (void)fprintf(out, "        return NULL;\n");
    (void)fprintf(out, "    }\n");
    (void)fprintf(out, "    return descriptor;\n");
    (void)fprintf(out, "}\n");

    return 0;
}

int struct_meta_gen_emit(const struct_meta_catalog *catalog, const char *header_path, const char *out_path)
{
    /* 診断の出力先がほかにないため、この関数の stderr への fprintf の戻り値は明示的に破棄する。 */

    char stem[STRUCT_META_GEN_EMIT_PATH_BYTES];
    char prefix[STRUCT_META_GEN_EMIT_PATH_BYTES];
    char header_out[STRUCT_META_GEN_EMIT_PATH_BYTES];
    size_t count = 0U;

    if ((catalog == NULL) || (header_path == NULL) || (out_path == NULL))
    {
        (void)fprintf(stderr, "struct-meta-gen: 生成に必要な引数がありません\n");
        return 1;
    }

    if ((struct_meta_catalog_get_count(catalog, &count) != CPLAT_OK) || (count == 0U))
    {
        (void)fprintf(stderr, "struct-meta-gen: 構造体が見つかりません: %s\n", header_path);
        return 1;
    }

    if (header_stem(header_path, stem, sizeof(stem)) != 0)
    {
        (void)fprintf(stderr, "struct-meta-gen: ヘッダー名から識別子を作れません: %s\n", header_path);
        return 1;
    }

    if (stem_prefix(stem, prefix, sizeof(prefix)) != 0)
    {
        (void)fprintf(stderr, "struct-meta-gen: ヘッダー名から接頭辞を作れません: %s\n", header_path);
        return 1;
    }

    if (header_out_path(out_path, header_out, sizeof(header_out)) != 0)
    {
        (void)fprintf(stderr, "struct-meta-gen: 出力パスは .c で終わる必要があります: %s\n", out_path);
        return 1;
    }

    FILE *out = cplat_fopen(out_path, "w", NULL);
    if (out == NULL)
    {
        (void)fprintf(stderr, "struct-meta-gen: 出力ファイルを作成できません: %s\n", out_path);
        return 1;
    }

    /*
     * 生成 C ソースへの書き込みの失敗はストリームに残り、この関数でファイルを閉じる直前にまとめて検査するため、
     * この関数の out への fprintf の戻り値は明示的に破棄する。
     */
    (void)fprintf(out, "/* このファイルは struct-meta-gen が自動生成した。手編集しないこと。 */\n");
    (void)fprintf(out, "#include \"../%s\"\n", header_path);
    (void)fprintf(out, "#include \"%s\"\n\n", generated_header_include(header_out));
    (void)fprintf(out, "#include <struct_meta/catalog/catalog.h>\n");
    (void)fprintf(out, "#include <struct_meta/meta/index.h>\n\n");
    (void)fprintf(out, "#include <cplat/base/result.h>\n");
    (void)fprintf(out, "#include <cplat/hashtable/hashtable.h>\n");
    (void)fprintf(out, "#include <cplat/runtime/abort.h>\n");
    (void)fprintf(out, "#include <cplat/runtime/shutdown.h>\n");
    (void)fprintf(out, "#include <cplat/sync/sync.h>\n\n");
    (void)fprintf(out, "#include <stddef.h>\n");
    (void)fprintf(out, "#include <limits.h>\n");
    (void)fprintf(out, "#include <stdint.h>\n");
    (void)fprintf(out, "#include <stdio.h>\n");
    (void)fprintf(out, "#include <stdlib.h>\n");
    (void)fprintf(out, "#include <time.h>\n\n");
    (void)fprintf(out, "_Static_assert(CHAR_BIT == 8, \"struct-meta は8ビットの char を前提とします\");\n");
    (void)fprintf(out, "_Static_assert(CHAR_MIN == INT8_MIN, \"struct-meta は符号付きの char を前提とします\");\n");
    (void)fprintf(
        out, "_Static_assert(CHAR_MAX == INT8_MAX, \"struct-meta は char と int8_t の同じ範囲を前提とします\");\n\n");

    emitted_name *emitted = NULL;
    int struct_ret = 0;

    for (size_t index = 0; (index < count) && (struct_ret == 0); index++)
    {
        const struct_meta_descriptor *descriptor = NULL;

        /* 引数は非 NULL で、添字は get_count で得た件数未満であり、失敗しないため、戻り値は明示的に破棄する。 */
        (void)struct_meta_catalog_get(catalog, index, &descriptor);
        struct_ret = emit_struct(out, descriptor, &emitted);
    }

    /* emit_struct() が calloc() したノードの所有権はリストにあり、出力が終わったらここで解放する。 */
    while (emitted != NULL)
    {
        emitted_name *next = emitted->next;
        free(emitted);
        emitted = next;
    }

    if (struct_ret != 0)
    {
        /* 名前が長すぎる失敗で 1 を返す経路のため、戻り値は明示的に破棄する。 */
        (void)fclose(out);
        return 1;
    }

    int catalog_ret = emit_catalog_source(out, stem, prefix, catalog, count);

    if (catalog_ret != 0)
    {
        /* 生成の失敗で 1 を返す経路のため、戻り値は明示的に破棄する。 */
        (void)fclose(out);
        return 1;
    }

    if (close_generated_file(out, out_path) != 0)
    {
        return 1;
    }

    return emit_catalog_header(header_out, stem, prefix, catalog);
}
