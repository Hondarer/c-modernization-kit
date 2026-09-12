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

static void fprint_c_string(FILE *out, const char *text)
{
    if (text == NULL)
    {
        fputs("NULL", out);
        return;
    }

    fputc('"', out);
    for (const char *p = text; *p != '\0'; p++)
    {
        if ((*p == '"') || (*p == '\\'))
        {
            fputc('\\', out);
            fputc(*p, out);
        }
        else if (*p == '\n')
        {
            fputs("\\n", out);
        }
        else
        {
            fputc(*p, out);
        }
    }
    fputc('"', out);
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
            /* 記述子は libstruct_meta が組み立てるため、未知の種別は起こらない。 */
            return "STRUCT_META_FIELD_SIGNED_INTEGER";
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
    return ((field->kind == STRUCT_META_FIELD_CHAR_ARRAY) || (field->element_count > 1U)) ? 1 : 0;
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
    fprintf(out, "static const struct_meta_attribute g_%s_attributes[] = {\n", symbol);
    for (size_t i = 0; i < count; i++)
    {
        fprintf(out, "    { ");
        fprint_c_string(out, attributes[i].key);
        fprintf(out, ", ");
        fprint_c_string(out, attributes[i].value);
        fprintf(out, " },\n");
    }
    fprintf(out, "};\n\n");
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
    fprintf(out, "/* レイアウト エンジンの計算値と、コンパイラが決めた配置との照合。 */\n");
    fprintf(out, "_Static_assert(sizeof(%s) == %zu, \"%s: レイアウト エンジンの計算値と一致しません\");\n",
            descriptor->name, descriptor->size, descriptor->name);
    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        const struct_meta_field *field = &descriptor->fields[i];
        fprintf(out,
                "_Static_assert(offsetof(%s, %s) == %zu, \"%s.%s: レイアウト エンジンの計算値と一致しません\");\n",
                descriptor->name, field->name, field->offset, descriptor->name, field->name);
    }
    fprintf(out, "\n");
}


/**
 *  @brief          記述子 1 個分の C ソースを書き出します。
 *
 *  ネスト先を先に出力してから自分を出力します。出力済みの構造体は飛ばします。
 */
static void emit_struct(FILE *out, const struct_meta_descriptor *descriptor, emitted_name **emitted)
{
    if (is_emitted(*emitted, descriptor->name))
    {
        return;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (descriptor->fields[i].nested != NULL)
        {
            emit_struct(out, descriptor->fields[i].nested, emitted);
        }
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        const struct_meta_field *field = &descriptor->fields[i];
        char symbol[STRUCT_META_GEN_EMIT_PATH_BYTES];
        snprintf(symbol, sizeof(symbol), "%s_%s", descriptor->name, field->name);
        emit_attributes(out, symbol, field->attributes, field->attribute_count);
    }
    emit_attributes(out, descriptor->name, descriptor->attributes, descriptor->attribute_count);

    fprintf(out, "static const struct_meta_field g_%s_fields[] = {\n", descriptor->name);
    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        const struct_meta_field *field = &descriptor->fields[i];
        const char *element = (field_is_declared_array(field) != 0) ? "[0]" : "";
        char element_size_expr[STRUCT_META_GEN_EMIT_PATH_BYTES];
        char char_buffer_expr[STRUCT_META_GEN_EMIT_PATH_BYTES];
        char nested_expr[STRUCT_META_GEN_EMIT_PATH_BYTES];

        snprintf(element_size_expr, sizeof(element_size_expr), "sizeof(((%s *)0)->%s%s)", descriptor->name,
                 field->name, element);
        if (field->char_buffer_size != 0U)
        {
            snprintf(char_buffer_expr, sizeof(char_buffer_expr), "sizeof(((%s *)0)->%s)", descriptor->name,
                     field->name);
        }
        else
        {
            snprintf(char_buffer_expr, sizeof(char_buffer_expr), "0");
        }
        if (field->nested != NULL)
        {
            snprintf(nested_expr, sizeof(nested_expr), "&g_%s_desc", field->nested->name);
        }
        else
        {
            snprintf(nested_expr, sizeof(nested_expr), "NULL");
        }

        fprintf(out, "    { \"%s\", %s, 0, offsetof(%s, %s), %s, %zu, %s, %s, ", field->name,
                field_kind_name(field->kind), descriptor->name, field->name, element_size_expr,
                field->element_count, char_buffer_expr, nested_expr);
        fprint_c_string(out, field->brief);
        if (field->attribute_count == 0U)
        {
            fputs(", NULL, 0 },\n", out);
        }
        else
        {
            fprintf(out, ", g_%s_%s_attributes, %zu },\n", descriptor->name, field->name, field->attribute_count);
        }
    }
    fprintf(out, "};\n\n");

    fprintf(out, "static const struct_meta_descriptor g_%s_desc = { \"%s\", sizeof(%s), g_%s_fields, %zu, ",
            descriptor->name, descriptor->name, descriptor->name, descriptor->name, descriptor->field_count);
    fprint_c_string(out, descriptor->brief);
    if (descriptor->attribute_count == 0U)
    {
        fprintf(out, ", NULL, 0 };\n\n");
    }
    else
    {
        fprintf(out, ", g_%s_attributes, %zu };\n\n", descriptor->name, descriptor->attribute_count);
    }

    emit_layout_assertions(out, descriptor);

    emitted_name *node = (emitted_name *)calloc(1, sizeof(*node));
    if (node == NULL)
    {
        fprintf(stderr, "struct-meta-gen: 出力状態を確保できません\n");
        exit(1);
    }
    node->name = descriptor->name;
    node->next = *emitted;
    *emitted = node;
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
        fprintf(stderr, "struct-meta-gen: 出力ファイルを作成できません: %s\n", header_out);
        return 1;
    }

    size_t count = 0U;
    (void)struct_meta_catalog_get_count(catalog, &count);

    fprintf(out, "/* このファイルは struct-meta-gen が自動生成しました。手編集しないでください。 */\n");
    fprintf(out, "#ifndef %s_META_H\n", prefix);
    fprintf(out, "#define %s_META_H\n\n", prefix);
    fprintf(out, "#include <struct_meta/catalog/catalog.h>\n");
    fprintf(out, "#include <struct_meta/meta/meta.h>\n\n");
    fprintf(out, "#ifdef __cplusplus\n");
    fprintf(out, "extern \"C\"\n");
    fprintf(out, "{\n");
    fprintf(out, "#endif /* __cplusplus */\n\n");
    fprintf(out, "typedef enum %s_meta_id\n", stem);
    fprintf(out, "{\n");
    for (size_t index = 0; index < count; index++)
    {
        const struct_meta_descriptor *descriptor = NULL;
        (void)struct_meta_catalog_get(catalog, index, &descriptor);
        fprintf(out, "    %s_META_", prefix);
        for (const char *p = descriptor->name; *p != '\0'; p++)
        {
            fputc(toupper((unsigned char)*p), out);
        }
        fprintf(out, " = %zu,\n", index);
    }
    fprintf(out, "    %s_META_COUNT = %zu,\n", prefix, count);
    fprintf(out, "} %s_meta_id;\n\n", stem);
    fprintf(out, "/**\n");
    fprintf(out, " *  @brief          カタログが持つ型の数を返します。\n");
    fprintf(out, " *  @return         型の数です。\n");
    fprintf(out, " *\n");
    fprintf(out, " *  @par            スレッド セーフ\n");
    fprintf(out, " *  本関数はスレッド セーフです。内部に共有状態を持ちません。\n");
    fprintf(out, " */\n");
    fprintf(out, "size_t %s_meta_count(void);\n\n", stem);

    fprintf(out, "/**\n");
    fprintf(out, " *  @brief          列挙 ID で記述子を取得します。\n");
    fprintf(out, " *  @param[in]      id 取得する型の ID です。\n");
    fprintf(out, " *  @return         記述子です。範囲外の ID では NULL を返します。\n");
    fprintf(out, " *\n");
    fprintf(out, " *  @par            スレッド セーフ\n");
    fprintf(out, " *  本関数はスレッド セーフです。内部に共有状態を持ちません。\n");
    fprintf(out, " */\n");
    fprintf(out, "const struct_meta_descriptor *%s_meta_get(%s_meta_id id);\n\n", stem, stem);

    fprintf(out, "/**\n");
    fprintf(out, " *  @brief          構造体名で記述子を検索します。\n");
    fprintf(out, " *  @param[in]      name 検索する構造体名です。NULL を渡せます。\n");
    fprintf(out, " *  @return         記述子です。NULL または未知の名前では NULL を返します。\n");
    fprintf(out, " *\n");
    fprintf(out, " *  検索には、生成時に構築した索引の埋め込みイメージを使います。\n");
    fprintf(out, " *  イメージへの接続は初回呼び出し時に 1 回だけ行います。\n");
    fprintf(out, " *\n");
    fprintf(out, " *  @attention      埋め込みイメージへ接続できない場合、本関数はプロセスを終了させます。\n");
    fprintf(out, " *                  同一ビルドで生成したイメージが読めないという不変条件の破れであり、\n");
    fprintf(out, " *                  検索を続けても記述子を返せないためです。\n");
    fprintf(out, " *\n");
    fprintf(out, " *  @par            スレッド セーフ\n");
    fprintf(out, " *  本関数はスレッド セーフです。初回の接続は 1 回だけ実行されます。\n");
    fprintf(out, " */\n");
    fprintf(out, "const struct_meta_descriptor *%s_meta_find(const char *name);\n\n", stem);

    fprintf(out, "/**\n");
    fprintf(out, " *  @brief          カタログ ハンドルを返します。\n");
    fprintf(out, " *  @return         カタログです。接続できない場合はプロセスを終了させます。\n");
    fprintf(out, " *\n");
    fprintf(out, " *  実行時にヘッダーを解析して作ったカタログと同じ API で扱えます。\n");
    fprintf(out, " *  このカタログは静的領域を指すため、破棄してはなりません。\n");
    fprintf(out, " *\n");
    fprintf(out, " *  @par            スレッド セーフ\n");
    fprintf(out, " *  本関数はスレッド セーフです。初回の接続は 1 回だけ実行されます。\n");
    fprintf(out, " */\n");
    fprintf(out, "const struct_meta_catalog *%s_meta_catalog(void);\n\n", stem);
    fprintf(out, "#ifdef __cplusplus\n");
    fprintf(out, "}\n");
    fprintf(out, "#endif /* __cplusplus */\n\n");
    fprintf(out, "#endif /* %s_META_H */\n", prefix);

    fclose(out);
    return 0;
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
    cplat_hashtable_config config = {0};
    cplat_hashtable *table = NULL;

    config.capacity = count * 2U;
    config.key_type = CPLAT_HASHTABLE_FIELD_FIXED_STRING;
    config.key_size = max_struct_name_bytes(catalog, count);
    config.value_type = CPLAT_HASHTABLE_FIELD_FIXED_BINARY;
    config.value_size = sizeof(size_t);
    config.value_align = sizeof(size_t);
    config.lifetime = CPLAT_HASHTABLE_LIFETIME_INFINITE;

    if (cplat_hashtable_create(&config, NULL, 0, NULL, 0, &table) != CPLAT_OK)
    {
        fprintf(stderr, "struct-meta-gen: 索引テーブルを構築できません\n");
        return 1;
    }

    for (size_t index = 0; index < count; index++)
    {
        const struct_meta_descriptor *descriptor = NULL;
        (void)struct_meta_catalog_get(catalog, index, &descriptor);
        if (cplat_hashtable_add(table, descriptor->name, &index, CPLAT_HASHTABLE_ADD_DELETED_OVERWRITE) != CPLAT_OK)
        {
            fprintf(stderr, "struct-meta-gen: 索引へ型名を登録できません: %s\n", descriptor->name);
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
        fprintf(stderr, "struct-meta-gen: 索引の永続化イメージを取得できません\n");
        cplat_hashtable_dispose(table);
        return 1;
    }

    fprintf(out, "/* struct-meta-gen が構築済みの索引を永続化したイメージです。手編集しないでください。 */\n");
    struct_meta_gen_emit_uint64_array(out, "s_index_mgmt", mgmt, mgmt_size);
    struct_meta_gen_emit_uint64_array(out, "s_index_data", data, data_size);

    /* 埋め込みイメージが前提とする ABI を、生成先のコンパイル時に検査する。
       生成器が動作した環境での実測値を出力する。 */
    fprintf(out, "/* 埋め込みイメージが前提とする ABI。破れた場合はコンパイル時に落とす。 */\n");
    fprintf(out, "_Static_assert(sizeof(size_t) == %zu, \"%s: 埋め込み索引は size_t %zu バイトを前提とします\");\n",
            sizeof(size_t), prefix, sizeof(size_t));
    fprintf(out, "_Static_assert(sizeof(time_t) == %zu, \"%s: 埋め込み索引は time_t %zu バイトを前提とします\");\n",
            sizeof(time_t), prefix, sizeof(time_t));
    fprintf(out,
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
static int emit_catalog_source(FILE *out, const char *stem, const char *prefix,
                               const struct_meta_catalog *catalog, size_t count)
{
    fprintf(out, "static const struct_meta_descriptor *const s_descriptors[%s_META_COUNT] = {\n", prefix);
    for (size_t index = 0; index < count; index++)
    {
        const struct_meta_descriptor *descriptor = NULL;
        (void)struct_meta_catalog_get(catalog, index, &descriptor);
        fprintf(out, "    &g_%s_desc,\n", descriptor->name);
    }
    fprintf(out, "};\n\n");

    if (emit_catalog_index_image(out, prefix, catalog, count) != 0)
    {
        return 1;
    }

    fprintf(out, "static cplat_once_flag s_catalog_once;\n");
    fprintf(out, "static struct_meta_catalog *s_catalog;\n\n");

    fprintf(out, "static void destroy_catalog(const cplat_shutdown_event *event, void *context)\n");
    fprintf(out, "{\n");
    fprintf(out, "    (void)event;\n");
    fprintf(out, "    (void)context;\n");
    fprintf(out, "    /* ハンドルだけを解放する。記述子とイメージは静的領域であり解放しない。 */\n");
    fprintf(out, "    struct_meta_catalog_destroy(s_catalog);\n");
    fprintf(out, "    s_catalog = NULL;\n");
    fprintf(out, "}\n\n");

    fprintf(out, "static void attach_catalog(void)\n");
    fprintf(out, "{\n");
    fprintf(out, "    if (struct_meta_catalog_attach_static(s_descriptors, %s_META_COUNT,\n", prefix);
    fprintf(out, "                                          s_index_mgmt, sizeof(s_index_mgmt),\n");
    fprintf(out, "                                          s_index_data, sizeof(s_index_data),\n");
    fprintf(out, "                                          &s_catalog) != CPLAT_OK)\n");
    fprintf(out, "    {\n");
    fprintf(out, "        /* 同一ビルドで作ったイメージが読めないという不変条件の破れ。\n");
    fprintf(out, "           線形走査へ縮退させず、その場で落とす。 */\n");
    fprintf(out, "        (void)fprintf(stderr, \"%s: 埋め込み索引へ接続できません\\n\");\n", stem);
    fprintf(out, "        abort();\n");
    fprintf(out, "    }\n");
    fprintf(out, "    (void)cplat_shutdown_register(destroy_catalog, NULL);\n");
    fprintf(out, "}\n\n");

    fprintf(out, "const struct_meta_catalog *%s_meta_catalog(void)\n", stem);
    fprintf(out, "{\n");
    fprintf(out, "    cplat_call_once(&s_catalog_once, attach_catalog);\n");
    fprintf(out, "    return s_catalog;\n");
    fprintf(out, "}\n\n");

    fprintf(out, "size_t %s_meta_count(void)\n", stem);
    fprintf(out, "{\n");
    fprintf(out, "    return %s_META_COUNT;\n", prefix);
    fprintf(out, "}\n\n");

    fprintf(out, "const struct_meta_descriptor *%s_meta_get(%s_meta_id id)\n", stem, stem);
    fprintf(out, "{\n");
    fprintf(out, "    if (((int)id < 0) || (id >= %s_META_COUNT))\n", prefix);
    fprintf(out, "    {\n");
    fprintf(out, "        return NULL;\n");
    fprintf(out, "    }\n");
    fprintf(out, "    return s_descriptors[id];\n");
    fprintf(out, "}\n\n");

    fprintf(out, "const struct_meta_descriptor *%s_meta_find(const char *name)\n", stem);
    fprintf(out, "{\n");
    fprintf(out, "    const struct_meta_descriptor *descriptor = NULL;\n");
    fprintf(out, "    if (struct_meta_catalog_find(%s_meta_catalog(), name, &descriptor) != CPLAT_OK)\n", stem);
    fprintf(out, "    {\n");
    fprintf(out, "        return NULL;\n");
    fprintf(out, "    }\n");
    fprintf(out, "    return descriptor;\n");
    fprintf(out, "}\n");
    return 0;
}


int struct_meta_gen_emit(const struct_meta_catalog *catalog, const char *header_path, const char *out_path)
{
    char stem[STRUCT_META_GEN_EMIT_PATH_BYTES];
    char prefix[STRUCT_META_GEN_EMIT_PATH_BYTES];
    char header_out[STRUCT_META_GEN_EMIT_PATH_BYTES];
    size_t count = 0U;

    if ((catalog == NULL) || (header_path == NULL) || (out_path == NULL))
    {
        fprintf(stderr, "struct-meta-gen: 生成に必要な引数がありません\n");
        return 1;
    }
    if ((struct_meta_catalog_get_count(catalog, &count) != CPLAT_OK) || (count == 0U))
    {
        fprintf(stderr, "struct-meta-gen: 構造体が見つかりません: %s\n", header_path);
        return 1;
    }
    if (header_stem(header_path, stem, sizeof(stem)) != 0)
    {
        fprintf(stderr, "struct-meta-gen: ヘッダー名から識別子を作れません: %s\n", header_path);
        return 1;
    }
    if (stem_prefix(stem, prefix, sizeof(prefix)) != 0)
    {
        fprintf(stderr, "struct-meta-gen: ヘッダー名から接頭辞を作れません: %s\n", header_path);
        return 1;
    }
    if (header_out_path(out_path, header_out, sizeof(header_out)) != 0)
    {
        fprintf(stderr, "struct-meta-gen: 出力パスは .c で終わる必要があります: %s\n", out_path);
        return 1;
    }

    FILE *out = cplat_fopen(out_path, "w", NULL);
    if (out == NULL)
    {
        fprintf(stderr, "struct-meta-gen: 出力ファイルを作成できません: %s\n", out_path);
        return 1;
    }

    fprintf(out, "/* このファイルは struct-meta-gen が自動生成しました。手編集しないでください。 */\n");
    fprintf(out, "#include \"../%s\"\n", header_path);
    fprintf(out, "#include \"%s\"\n\n", generated_header_include(header_out));
    fprintf(out, "#include <struct_meta/catalog/catalog.h>\n");
    fprintf(out, "#include <struct_meta/meta/index.h>\n\n");
    fprintf(out, "#include <cplat/base/result.h>\n");
    fprintf(out, "#include <cplat/hashtable/hashtable.h>\n");
    fprintf(out, "#include <cplat/runtime/shutdown.h>\n");
    fprintf(out, "#include <cplat/sync/sync.h>\n\n");
    fprintf(out, "#include <stddef.h>\n");
    fprintf(out, "#include <limits.h>\n");
    fprintf(out, "#include <stdint.h>\n");
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <time.h>\n\n");
    fprintf(out, "_Static_assert(CHAR_BIT == 8, \"struct-meta は8ビットの char を前提とします\");\n");
    fprintf(out, "_Static_assert(CHAR_MIN == INT8_MIN, \"struct-meta は符号付きの char を前提とします\");\n");
    fprintf(out,
            "_Static_assert(CHAR_MAX == INT8_MAX, \"struct-meta は char と int8_t の同じ範囲を前提とします\");\n\n");

    emitted_name *emitted = NULL;
    for (size_t index = 0; index < count; index++)
    {
        const struct_meta_descriptor *descriptor = NULL;
        (void)struct_meta_catalog_get(catalog, index, &descriptor);
        emit_struct(out, descriptor, &emitted);
    }
    while (emitted != NULL)
    {
        emitted_name *next = emitted->next;
        free(emitted);
        emitted = next;
    }

    int catalog_ret = emit_catalog_source(out, stem, prefix, catalog, count);
    fclose(out);
    if (catalog_ret != 0)
    {
        return 1;
    }

    return emit_catalog_header(header_out, stem, prefix, catalog);
}
