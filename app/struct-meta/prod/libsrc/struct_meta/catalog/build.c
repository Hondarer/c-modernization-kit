/**
 *******************************************************************************
 *  @file           build.c
 *  @brief          構文解析結果から記述子を組み立てます。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/catalog/build.h>

#include <struct_meta/layout/layout.h>
#include <struct_meta/parse/diagnostic.h>

#include <cplat/base/result.h>

#include <string.h>

/**
 *  @brief          構造体 1 個分の組み立て状態です。
 */
typedef struct build_entry
{
    const struct_meta_internal_parse_struct *source; /**< 構文解析結果です。 */
    struct_meta_descriptor *descriptor;              /**< 組み立て先です。 */
    size_t alignment;                                /**< 構造体のアラインメントです。 */
    int state;                                       /**< 0:未着手 1:組み立て中 2:完了 */
    int pad;                                         /**< 明示的アラインメントです。0 を指定します。 */
} build_entry;

/**
 *  @brief          組み立て全体で共有する状態です。
 */
typedef struct build_context
{
    build_entry *entries;               /**< 構造体ごとの状態です。 */
    size_t entry_count;                 /**< @c entries の要素数です。 */
    struct_meta_internal_arena *arena;  /**< 記述子の記憶域です。 */
    struct_meta_diagnostic *diagnostic; /**< 診断の書き込み先です。 */
} build_context;

#define BUILD_STATE_PENDING 0
#define BUILD_STATE_ACTIVE  1
#define BUILD_STATE_DONE    2

static int build_one(build_context *context, build_entry *entry);

/**
 *  @brief          属性の連結リストの要素数を返します。
 */
static size_t count_attributes(const struct_meta_internal_parse_attribute *attributes)
{
    size_t count = 0U;
    for (const struct_meta_internal_parse_attribute *item = attributes; item != NULL; item = item->next)
    {
        count++;
    }
    return count;
}

/**
 *  @brief          フィールドの連結リストの要素数を返します。
 */
static size_t count_fields(const struct_meta_internal_parse_field *fields)
{
    size_t count = 0U;
    for (const struct_meta_internal_parse_field *item = fields; item != NULL; item = item->next)
    {
        count++;
    }
    return count;
}

/**
 *  @brief          属性名で属性を探します。
 *  @return         見つかった属性です。無ければ NULL を返します。
 */
static const struct_meta_internal_parse_attribute *
find_attribute(const struct_meta_internal_parse_attribute *attributes, const char *key)
{
    for (const struct_meta_internal_parse_attribute *item = attributes; item != NULL; item = item->next)
    {
        if (strcmp(item->key, key) == 0)
        {
            return item;
        }
    }
    return NULL;
}

/**
 *  @brief          宣言型そのものがバイト配列を表すかどうかを返します。
 *
 *  `signed char`、`unsigned char`、`int8_t`、`uint8_t` は、配列にしたときバイト配列です。
 *  see: app/struct-meta/docs/architecture.md の「文字列とバイト配列」
 */
static int is_explicit_byte_type(const char *type_name)
{
    if ((strcmp(type_name, "signed char") == 0) || (strcmp(type_name, "unsigned char") == 0) ||
        (strcmp(type_name, "int8_t") == 0) || (strcmp(type_name, "uint8_t") == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 *  @brief          フィールドがバイト配列かどうかを返します。
 *
 *  `char[N]` は既定で NUL 終端文字列であり、`meta.kind=bytes` を付けたときだけ
 *  符号付きバイト配列として扱います。
 */
static int field_is_byte_array(const struct_meta_internal_parse_field *field)
{
    if (field->array_count <= 0)
    {
        return 0;
    }
    if (is_explicit_byte_type(field->type_name) != 0)
    {
        return 1;
    }
    const struct_meta_internal_parse_attribute *kind = find_attribute(field->attributes, "meta.kind");
    if ((strcmp(field->type_name, "char") == 0) && (kind != NULL) && (kind->value != NULL) &&
        (strcmp(kind->value, "bytes") == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 *  @brief          `meta.kind` と `meta.format` の指定が妥当かを検査します。
 *  @return         @c CPLAT_OK または @c CPLAT_ERR_CORRUPT_DESCRIPTOR を返します。
 *
 *  意味を解釈する属性はこの 2 個だけです。その他の属性名は解釈しません。
 */
static int validate_meta_attributes(const struct_meta_internal_parse_field *field, struct_meta_diagnostic *diagnostic)
{
    const struct_meta_internal_parse_attribute *kind = find_attribute(field->attributes, "meta.kind");
    if ((kind != NULL) &&
        ((kind->value == NULL) || (strcmp(kind->value, "bytes") != 0) || (field_is_byte_array(field) == 0)))
    {
        struct_meta_internal_diagnose(diagnostic, field->line, "meta.kind はバイト配列へ bytes だけを指定できます: %s",
                                      field->name);
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    const struct_meta_internal_parse_attribute *format = find_attribute(field->attributes, "meta.format");
    if ((format != NULL) &&
        ((format->value == NULL) || (strcmp(format->value, "hex") != 0) || (field_is_byte_array(field) == 0)))
    {
        struct_meta_internal_diagnose(diagnostic, field->line, "meta.format=hex はバイト配列だけへ指定できます: %s",
                                      field->name);
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }
    return CPLAT_OK;
}

/**
 *  @brief          属性の連結リストを記述子用の配列へ複写します。
 *  @return         @c CPLAT_OK または @c CPLAT_ERR_OUT_OF_MEMORY を返します。
 */
static int copy_attributes(build_context *context, const struct_meta_internal_parse_attribute *source,
                           const struct_meta_attribute **attributes_out, size_t *count_out)
{
    const size_t count = count_attributes(source);
    *attributes_out = NULL;
    *count_out = 0U;
    if (count == 0U)
    {
        return CPLAT_OK;
    }

    struct_meta_attribute *attributes = (struct_meta_attribute *)struct_meta_internal_arena_allocate(
        context->arena, count * sizeof(*attributes), sizeof(void *));
    if (attributes == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    size_t index = 0U;
    for (const struct_meta_internal_parse_attribute *item = source; item != NULL; item = item->next)
    {
        attributes[index].key = struct_meta_internal_arena_copy_string(context->arena, item->key);
        if (attributes[index].key == NULL)
        {
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
        if (item->value != NULL)
        {
            attributes[index].value = struct_meta_internal_arena_copy_string(context->arena, item->value);
            if (attributes[index].value == NULL)
            {
                return CPLAT_ERR_OUT_OF_MEMORY;
            }
        }
        index++;
    }

    *attributes_out = attributes;
    *count_out = count;
    return CPLAT_OK;
}

/**
 *  @brief          構造体名から組み立て状態を探します。
 *  @return         見つかった状態です。無ければ NULL を返します。
 */
static build_entry *find_entry(build_context *context, const char *name)
{
    for (size_t i = 0; i < context->entry_count; i++)
    {
        if (strcmp(context->entries[i].source->name, name) == 0)
        {
            return &context->entries[i];
        }
    }
    return NULL;
}

/**
 *  @brief          フィールド 1 個分の記述子を埋め、レイアウトへ登録します。
 *  @return         結果コードを返します。
 */
static int build_field(build_context *context, const struct_meta_internal_parse_field *source,
                       struct_meta_field *target, struct_meta_internal_layout_builder *layout)
{
    int ret = validate_meta_attributes(source, context->diagnostic);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    size_t element_size = 0U;
    size_t alignment = 0U;
    struct_meta_field_kind kind = STRUCT_META_FIELD_SIGNED_INTEGER;
    const struct_meta_descriptor *nested = NULL;

    if (source->is_struct_type != 0)
    {
        build_entry *entry = find_entry(context, source->type_name);
        if (entry == NULL)
        {
            struct_meta_internal_diagnose(context->diagnostic, source->line, "未知の型です: %s", source->type_name);
            return CPLAT_ERR_NOT_FOUND;
        }
        if (entry->state == BUILD_STATE_ACTIVE)
        {
            struct_meta_internal_diagnose(context->diagnostic, source->line, "構造体が循環しています: %s",
                                          source->type_name);
            return CPLAT_ERR_CORRUPT_DESCRIPTOR;
        }
        ret = build_one(context, entry);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
        kind = STRUCT_META_FIELD_STRUCT;
        element_size = entry->descriptor->size;
        alignment = entry->alignment;
        nested = entry->descriptor;
    }
    else
    {
        const struct_meta_internal_layout_type *type = struct_meta_internal_layout_find_type(source->type_name);
        if (type == NULL)
        {
            struct_meta_internal_diagnose(context->diagnostic, source->line, "未知の型です: %s", source->type_name);
            return CPLAT_ERR_NOT_FOUND;
        }
        kind = type->kind;
        element_size = type->size;
        alignment = type->alignment;
    }

    /* 記憶域の占有はここで決める。char 配列も宣言どおりの要素数で場所を取る。 */
    size_t declared_count;
    if (source->array_count > 0)
    {
        declared_count = (size_t)source->array_count;
    }
    else
    {
        declared_count = 1U;
    }
    size_t offset = 0U;
    ret = struct_meta_internal_layout_add(layout, element_size, declared_count, alignment, &offset);
    if (ret != CPLAT_OK)
    {
        struct_meta_internal_diagnose(context->diagnostic, source->line, "フィールドを配置できません: %s",
                                      source->name);
        return ret;
    }

    /*
     * 記述子の表現は記憶域の占有と異なる。char 配列は NUL 終端文字列 1 個として扱い、
     * 全体のバイト数を char_buffer_size が表す。
     * see: app/struct-meta/docs/architecture.md の「文字列とバイト配列」
     */
    int is_char_array;
    if ((source->is_struct_type == 0) && (strcmp(source->type_name, "char") == 0) && (source->array_count > 0) &&
        (field_is_byte_array(source) == 0))
    {
        is_char_array = 1;
    }
    else
    {
        is_char_array = 0;
    }
    if (is_char_array != 0)
    {
        kind = STRUCT_META_FIELD_CHAR_ARRAY;
        target->element_count = 1U;
        target->char_buffer_size = element_size * declared_count;
    }
    else
    {
        target->element_count = declared_count;
        target->char_buffer_size = 0U;
    }

    target->name = struct_meta_internal_arena_copy_string(context->arena, source->name);
    if (target->name == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    if (source->brief != NULL)
    {
        target->brief = struct_meta_internal_arena_copy_string(context->arena, source->brief);
        if (target->brief == NULL)
        {
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
    }
    target->kind = kind;
    target->pad = 0U;
    target->offset = offset;
    target->element_size = element_size;
    target->nested = nested;

    return copy_attributes(context, source->attributes, &target->attributes, &target->attribute_count);
}

/**
 *  @brief          構造体 1 個分の記述子を組み立てます。
 *  @return         結果コードを返します。
 *
 *  ネストした構造体を先に組み立てます。組み立て済みなら何もしません。
 */
static int build_one(build_context *context, build_entry *entry)
{
    if (entry->state == BUILD_STATE_DONE)
    {
        return CPLAT_OK;
    }
    if (entry->state == BUILD_STATE_ACTIVE)
    {
        struct_meta_internal_diagnose(context->diagnostic, entry->source->line, "構造体が循環しています: %s",
                                      entry->source->name);
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }
    entry->state = BUILD_STATE_ACTIVE;

    const size_t field_count = count_fields(entry->source->fields);
    if (field_count == 0U)
    {
        struct_meta_internal_diagnose(context->diagnostic, entry->source->line, "フィールドがありません: %s",
                                      entry->source->name);
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }

    struct_meta_field *fields = (struct_meta_field *)struct_meta_internal_arena_allocate(
        context->arena, field_count * sizeof(*fields), sizeof(void *));
    if (fields == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    struct_meta_internal_layout_builder layout;
    struct_meta_internal_layout_begin(&layout);

    size_t index = 0U;
    for (const struct_meta_internal_parse_field *source = entry->source->fields; source != NULL; source = source->next)
    {
        const int ret = build_field(context, source, &fields[index], &layout);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
        index++;
    }

    size_t size = 0U;
    size_t alignment = 0U;
    int ret = struct_meta_internal_layout_end(&layout, &size, &alignment);
    if (ret != CPLAT_OK)
    {
        struct_meta_internal_diagnose(context->diagnostic, entry->source->line, "構造体を配置できません: %s",
                                      entry->source->name);
        return ret;
    }

    entry->descriptor->name = struct_meta_internal_arena_copy_string(context->arena, entry->source->name);
    if (entry->descriptor->name == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    if (entry->source->brief != NULL)
    {
        entry->descriptor->brief = struct_meta_internal_arena_copy_string(context->arena, entry->source->brief);
        if (entry->descriptor->brief == NULL)
        {
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
    }
    entry->descriptor->size = size;
    entry->descriptor->fields = fields;
    entry->descriptor->field_count = field_count;
    entry->alignment = alignment;

    ret = copy_attributes(context, entry->source->attributes, &entry->descriptor->attributes,
                          &entry->descriptor->attribute_count);
    if (ret != CPLAT_OK)
    {
        return ret;
    }

    entry->state = BUILD_STATE_DONE;
    return CPLAT_OK;
}

int struct_meta_internal_build_descriptors(const struct_meta_internal_parse_struct_list *structs,
                                           struct_meta_internal_arena *arena,
                                           const struct_meta_descriptor *const **descriptors_out, size_t *count_out,
                                           struct_meta_diagnostic *diagnostic)
{
    if ((structs == NULL) || (arena == NULL) || (descriptors_out == NULL) || (count_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    size_t count = 0U;
    for (const struct_meta_internal_parse_struct *item = structs->head; item != NULL; item = item->next)
    {
        count++;
    }
    if (count == 0U)
    {
        return CPLAT_ERR_NOT_FOUND;
    }

    build_entry *entries =
        (build_entry *)struct_meta_internal_arena_allocate(arena, count * sizeof(*entries), sizeof(void *));
    struct_meta_descriptor *descriptors = (struct_meta_descriptor *)struct_meta_internal_arena_allocate(
        arena, count * sizeof(*descriptors), sizeof(void *));
    const struct_meta_descriptor **table = (const struct_meta_descriptor **)struct_meta_internal_arena_allocate(
        arena, count * sizeof(*table), sizeof(void *));
    if ((entries == NULL) || (descriptors == NULL) || (table == NULL))
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    size_t index = 0U;
    for (const struct_meta_internal_parse_struct *item = structs->head; item != NULL; item = item->next)
    {
        entries[index].source = item;
        entries[index].descriptor = &descriptors[index];
        entries[index].state = BUILD_STATE_PENDING;
        index++;
    }

    build_context context = {entries, count, arena, diagnostic};
    for (size_t i = 0; i < count; i++)
    {
        const int ret = build_one(&context, &entries[i]);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
        table[i] = entries[i].descriptor;
    }

    /* 配列の中身はこの先変えない。C では多段ポインターへの const 付与が暗黙に
       行われないため、カタログが保持する型へ明示的に変換する。 */
    *descriptors_out = (const struct_meta_descriptor *const *)table;
    *count_out = count;
    return CPLAT_OK;
}
