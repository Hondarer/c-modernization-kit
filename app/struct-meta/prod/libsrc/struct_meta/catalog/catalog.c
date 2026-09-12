/**
 *******************************************************************************
 *  @file           catalog.c
 *  @brief          記述子の集合を、取得方法によらない共通の形で扱います。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/catalog/catalog.h>

#include <struct_meta/catalog/arena.h>
#include <struct_meta/catalog/build.h>
#include <struct_meta/meta/index.h>
#include <struct_meta/parse/diagnostic.h>
#include <struct_meta/parse/parse_internal.h>

#include <cplat/base/result.h>
#include <cplat/hashtable/hashtable.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct struct_meta_catalog
{
    const struct_meta_descriptor *const *descriptors; /**< 記述子の配列です。宣言順に並びます。 */
    size_t descriptor_count;                    /**< @c descriptors の要素数です。 */
    cplat_hashtable *index;                     /**< 構造体名からインデックスを引き当てる索引です。 */
    struct_meta_internal_arena *arena;          /**< 記述子の記憶域です。静的カタログでは NULL です。 */
};

/**
 *  @brief          構造体名のうち、終端を含む最長のバイト数を求めます。
 */
static size_t max_name_bytes(const struct_meta_descriptor *const *descriptors, size_t count)
{
    size_t longest = 0U;
    for (size_t i = 0; i < count; i++)
    {
        const size_t length = strlen(descriptors[i]->name);
        if (length > longest)
        {
            longest = length;
        }
    }
    return longest + 1U;
}

/**
 *  @brief          構造体名からインデックスを引き当てる索引を構築します。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_OUT_OF_MEMORY 、または
 *                  @c CPLAT_ERR_DUPLICATE_KEY を返します。
 *
 *  件数もキーの最大長も、構文解析を終えた時点で確定します。自動拡張版ではなく、
 *  必要量を求めてから 1 回だけ構築します。
 *  see: app/struct-meta/docs/architecture.md
 */
static int create_index(struct_meta_catalog *catalog, struct_meta_diagnostic *diagnostic)
{
    cplat_hashtable_config config = {0};
    config.capacity = catalog->descriptor_count * 2U;
    config.key_type = CPLAT_HASHTABLE_FIELD_FIXED_STRING;
    config.key_size = max_name_bytes(catalog->descriptors, catalog->descriptor_count);
    config.value_type = CPLAT_HASHTABLE_FIELD_FIXED_BINARY;
    config.value_size = sizeof(size_t);
    config.value_align = sizeof(size_t);
    config.lifetime = CPLAT_HASHTABLE_LIFETIME_INFINITE;

    if (cplat_hashtable_create(&config, NULL, 0, NULL, 0, &catalog->index) != CPLAT_OK)
    {
        struct_meta_internal_diagnose(diagnostic, 0, "索引を構築できません");
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    for (size_t i = 0; i < catalog->descriptor_count; i++)
    {
        if (cplat_hashtable_add(catalog->index, catalog->descriptors[i]->name, &i,
                                CPLAT_HASHTABLE_ADD_DELETED_OVERWRITE) != CPLAT_OK)
        {
            struct_meta_internal_diagnose(diagnostic, 0, "索引へ型名を登録できません: %s",
                                          catalog->descriptors[i]->name);
            return CPLAT_ERR_DUPLICATE_KEY;
        }
    }
    return CPLAT_OK;
}

/**
 *  @brief          カタログの記述子を索引へ登録します。
 *
 *  登録は検索を速くするだけで、失敗しても未登録の記述子として正しく動作します。
 *  そのため戻り値を持たず、失敗を握りつぶします。
 */
static void register_descriptors(const struct_meta_catalog *catalog)
{
    for (size_t i = 0; i < catalog->descriptor_count; i++)
    {
        (void)struct_meta_index_register(catalog->descriptors[i]);
    }
}

/**
 *  @brief          構文解析結果からカタログを組み立てます。
 *  @return         結果コードを返します。
 */
static int create_from_structs(struct_meta_internal_parse_struct_list *structs, struct_meta_catalog **catalog_out,
                               struct_meta_diagnostic *diagnostic)
{
    struct_meta_internal_arena *arena = struct_meta_internal_arena_create();
    if (arena == NULL)
    {
        struct_meta_internal_parse_struct_list_destroy(structs);
        struct_meta_internal_diagnose(diagnostic, 0, "記述子の記憶域を確保できません");
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    struct_meta_catalog *catalog = (struct_meta_catalog *)calloc(1, sizeof(*catalog));
    if (catalog == NULL)
    {
        struct_meta_internal_arena_destroy(arena);
        struct_meta_internal_parse_struct_list_destroy(structs);
        struct_meta_internal_diagnose(diagnostic, 0, "カタログを確保できません");
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    catalog->arena = arena;

    int ret = struct_meta_internal_build_descriptors(structs, arena, &catalog->descriptors,
                                                     &catalog->descriptor_count, diagnostic);
    /* AST はここまでで役目を終える。記述子は arena へ複写済み。 */
    struct_meta_internal_parse_struct_list_destroy(structs);
    if (ret != CPLAT_OK)
    {
        struct_meta_catalog_destroy(catalog);
        return ret;
    }

    ret = create_index(catalog, diagnostic);
    if (ret != CPLAT_OK)
    {
        struct_meta_catalog_destroy(catalog);
        return ret;
    }

    register_descriptors(catalog);
    *catalog_out = catalog;
    return CPLAT_OK;
}

int struct_meta_catalog_create_from_header_file(const char *path, struct_meta_catalog **catalog_out,
                                                struct_meta_diagnostic *diagnostic_out)
{
    if ((path == NULL) || (catalog_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    struct_meta_internal_parse_struct_list *structs = NULL;
    const int ret = struct_meta_internal_parse_header_file(path, &structs, diagnostic_out);
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    return create_from_structs(structs, catalog_out, diagnostic_out);
}

int struct_meta_catalog_create_from_header_text(const char *text, size_t length, struct_meta_catalog **catalog_out,
                                                struct_meta_diagnostic *diagnostic_out)
{
    if ((text == NULL) || (catalog_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    struct_meta_internal_parse_struct_list *structs = NULL;
    const int ret = struct_meta_internal_parse_header_text(text, length, &structs, diagnostic_out);
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    return create_from_structs(structs, catalog_out, diagnostic_out);
}

int struct_meta_catalog_attach_static(const struct_meta_descriptor *const *descriptors, size_t descriptor_count,
                                      const void *index_image_mgmt, size_t index_image_mgmt_size,
                                      const void *index_image_data, size_t index_image_data_size,
                                      struct_meta_catalog **catalog_out)
{
    if ((descriptors == NULL) || (descriptor_count == 0U) || (index_image_mgmt == NULL) ||
        (index_image_data == NULL) || (catalog_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    struct_meta_catalog *catalog = (struct_meta_catalog *)calloc(1, sizeof(*catalog));
    if (catalog == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    catalog->descriptors = descriptors;
    catalog->descriptor_count = descriptor_count;
    catalog->arena = NULL;

    /* イメージは読み取り専用。cplat_hashtable_attach() は領域へ書き込まず、
       この表へ書き込み API を呼び出すこともないため、const を外して渡す。
       uintptr_t を経由するのは cplat と同じ書き方に揃えるため。
       see: app/c-platform/prod/libsrc/cplat/hashtable/hashtable_create.c の
            cplat_hashtable_attach() */
    if (cplat_hashtable_attach((void *)(uintptr_t)index_image_mgmt, index_image_mgmt_size,
                               (void *)(uintptr_t)index_image_data, index_image_data_size,
                               &catalog->index) != CPLAT_OK)
    {
        free(catalog);
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    register_descriptors(catalog);
    *catalog_out = catalog;
    return CPLAT_OK;
}

void struct_meta_catalog_destroy(struct_meta_catalog *catalog)
{
    if (catalog == NULL)
    {
        return;
    }
    for (size_t i = 0; i < catalog->descriptor_count; i++)
    {
        (void)struct_meta_index_unregister(catalog->descriptors[i]);
    }
    if (catalog->index != NULL)
    {
        /* ハンドルだけを解放する。静的カタログのイメージは解放しない。 */
        cplat_hashtable_dispose(catalog->index);
    }
    /* 実行時に組み立てた記述子はアリーナごと返す。静的カタログでは arena が NULL。 */
    struct_meta_internal_arena_destroy(catalog->arena);
    free(catalog);
}

int struct_meta_catalog_get_count(const struct_meta_catalog *catalog, size_t *count_out)
{
    if ((catalog == NULL) || (count_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *count_out = catalog->descriptor_count;
    return CPLAT_OK;
}

int struct_meta_catalog_get(const struct_meta_catalog *catalog, size_t index,
                            const struct_meta_descriptor **descriptor_out)
{
    if ((catalog == NULL) || (descriptor_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if (index >= catalog->descriptor_count)
    {
        return CPLAT_ERR_NOT_FOUND;
    }
    *descriptor_out = catalog->descriptors[index];
    return CPLAT_OK;
}

int struct_meta_catalog_find(const struct_meta_catalog *catalog, const char *name,
                             const struct_meta_descriptor **descriptor_out)
{
    if ((catalog == NULL) || (name == NULL) || (descriptor_out == NULL) || (catalog->index == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    const void *value = NULL;
    if (cplat_hashtable_find_value_ref(catalog->index, name, &value) != CPLAT_OK)
    {
        return CPLAT_ERR_NOT_FOUND;
    }
    /* value_align に sizeof(size_t) を指定しているため、型付きで参照できる。 */
    const size_t index = *(const size_t *)value;
    if (index >= catalog->descriptor_count)
    {
        return CPLAT_ERR_CORRUPT_DESCRIPTOR;
    }
    *descriptor_out = catalog->descriptors[index];
    return CPLAT_OK;
}
