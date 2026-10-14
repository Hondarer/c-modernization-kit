/**
 *******************************************************************************
 *  @file           index.c
 *  @brief          記述子を登録し、検査結果とフィールド名の索引を保持します。
 *
 *  記述子ポインターを鍵にした登録簿を 1 個持ち、記述子ごとに検査結果の控えと
 *  フィールド名の表を保持します。\n
 *  暗黙のメモ化にしないのは、自動変数として組み立てた記述子でアドレスが再利用され、
 *  別の記述子へ誤って一致するためです。登録した記述子だけを対象にします。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/meta/index.h>

#include <struct_meta/meta/index_internal.h>

#include <cplat/base/result.h>
#include <cplat/hashtable/hashtable.h>

#include <string.h>

/** 登録簿の初期レコード数です。登録数は事前に分からないため、自動拡張へ委ねます。 */
#define STRUCT_META_INDEX_REGISTRY_INITIAL_CAPACITY 16U

/** フィールド名の鍵に許す最大バイト数です (NUL を含む)。 */
#define STRUCT_META_INDEX_FIELD_NAME_BYTES 64U

/**
 *  @brief          記述子 1 個分の索引です。
 *
 *  この表は永続化しないため、実行時のみ有効なハンドルを値に持てます。
 */
typedef struct index_record
{
    cplat_hashtable *field_names; /**< フィールド名の表です。索引を作らない場合は NULL です。 */
    int validation_result;        /**< 登録時に求めた検査結果です。 */
    int pad;                      /**< field_names の整列に合わせた明示的パディングです。常に 0 です。 */
} index_record;

/** 記述子ポインターを鍵にした登録簿です。未使用なら NULL です。 */
static cplat_hashtable *s_registry;

/**
 *  @brief          登録簿を必要に応じて構築します。
 *  @return         @c CPLAT_OK 、または @c CPLAT_ERR_OUT_OF_MEMORY を返します。
 */
static int ensure_registry(void)
{
    if (s_registry != NULL)
    {
        return CPLAT_OK;
    }

    cplat_hashtable_config config = {0};
    cplat_hashtable_growth_config growth = {0}; /* 全フィールド 0 で上限なし。 */

    config.capacity = STRUCT_META_INDEX_REGISTRY_INITIAL_CAPACITY;
    config.key_type = CPLAT_HASHTABLE_FIELD_BINARY;
    config.key_size = sizeof(const struct_meta_descriptor *);
    config.value_type = CPLAT_HASHTABLE_FIELD_BINARY;
    config.value_size = sizeof(index_record);
    config.value_align = sizeof(void *);
    config.lifetime = CPLAT_HASHTABLE_LIFETIME_INFINITE;

    if (cplat_hashtable_create_growable(&config, &growth, &s_registry) != CPLAT_OK)
    {
        s_registry = NULL;
        return CPLAT_ERR_OUT_OF_MEMORY;
    }
    return CPLAT_OK;
}

/**
 *  @brief          登録簿から記述子の索引レコードを引き当てます。
 *  @return         見つかれば索引レコード、無ければ NULL を返します。
 */
static const index_record *find_record(const struct_meta_descriptor *descriptor)
{
    if (s_registry == NULL)
    {
        return NULL;
    }

    const void *value = NULL;
    if (cplat_hashtable_find_value_ref(s_registry, &descriptor, &value) != CPLAT_OK)
    {
        return NULL;
    }
    /* value_align に sizeof(void *) を指定しているため、型付きで参照できる。 */
    return (const index_record *)value;
}

/**
 *  @brief          フィールド名の表を構築します。
 *  @return         構築した表を返します。索引を作らない場合と失敗した場合は NULL を返します。
 *
 *  名前が長すぎるフィールドを含む記述子では、表を作らず線形走査へ任せます。
 *  一部だけを表に入れると、表引きの不一致を「無い」と誤って判断するためです。
 */
static cplat_hashtable *build_field_names(const struct_meta_descriptor *descriptor)
{
    if (descriptor->field_count == 0U)
    {
        return NULL;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (strlen(descriptor->fields[i].name) >= STRUCT_META_INDEX_FIELD_NAME_BYTES)
        {
            return NULL;
        }
    }

    cplat_hashtable_config config = {0};
    cplat_hashtable *table = NULL;

    config.capacity = descriptor->field_count * 2U;
    config.key_type = CPLAT_HASHTABLE_FIELD_FIXED_STRING;
    config.key_size = STRUCT_META_INDEX_FIELD_NAME_BYTES;
    config.value_type = CPLAT_HASHTABLE_FIELD_BINARY;
    config.value_size = sizeof(size_t);
    config.value_align = sizeof(size_t);
    config.lifetime = CPLAT_HASHTABLE_LIFETIME_INFINITE;

    if (cplat_hashtable_create(&config, NULL, 0, NULL, 0, &table) != CPLAT_OK)
    {
        return NULL;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (cplat_hashtable_add(table, descriptor->fields[i].name, &i, CPLAT_HASHTABLE_ADD_DELETED_OVERWRITE) !=
            CPLAT_OK)
        {
            /* 同名フィールドなど、全件を入れられない記述子は線形走査へ任せる。 */
            cplat_hashtable_dispose(table);
            return NULL;
        }
    }
    return table;
}

/**
 *  @brief          記述子 1 個と、そのネスト先を再帰的に登録します。
 *  @return         @c CPLAT_OK 、または @c CPLAT_ERR_OUT_OF_MEMORY を返します。
 */
static int register_descriptor(const struct_meta_descriptor *descriptor)
{
    if (find_record(descriptor) != NULL)
    {
        /* 登録済み。ネスト先の共有と、記述子の循環による無限再帰をここで止める。 */
        return CPLAT_OK;
    }

    index_record record = {0};

    /* この時点では登録簿に控えが無いため、検査は実際に実行される。 */
    record.validation_result = struct_meta_descriptor_validate(descriptor);
    if (record.validation_result == CPLAT_OK)
    {
        record.field_names = build_field_names(descriptor);
    }

    if (cplat_hashtable_add(s_registry, &descriptor, &record, CPLAT_HASHTABLE_ADD_DELETED_OVERWRITE) != CPLAT_OK)
    {
        cplat_hashtable_dispose(record.field_names);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    if (record.validation_result != CPLAT_OK)
    {
        /* 壊れた記述子の fields は信用できないため、ネスト先は辿らない。 */
        return CPLAT_OK;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (descriptor->fields[i].nested == NULL)
        {
            continue;
        }
        int ret = register_descriptor(descriptor->fields[i].nested);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
    }
    return CPLAT_OK;
}

/**
 *  @brief          記述子 1 個と、そのネスト先を再帰的に登録解除します。
 *  @return         @c CPLAT_OK 、または @c CPLAT_ERR_NOT_FOUND を返します。
 */
static int unregister_descriptor(const struct_meta_descriptor *descriptor)
{
    const index_record *record = find_record(descriptor);
    if (record == NULL)
    {
        return CPLAT_ERR_NOT_FOUND;
    }

    cplat_hashtable *field_names = record->field_names;
    int validation_result = record->validation_result;

    /* 先に登録簿から外し、記述子の循環による無限再帰を止める。 */
    (void)cplat_hashtable_delete(s_registry, &descriptor);
    cplat_hashtable_dispose(field_names);

    if (validation_result == CPLAT_OK)
    {
        for (size_t i = 0; i < descriptor->field_count; i++)
        {
            if (descriptor->fields[i].nested != NULL)
            {
                (void)unregister_descriptor(descriptor->fields[i].nested);
            }
        }
    }
    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_index_register(const struct_meta_descriptor *descriptor)
{
    if (descriptor == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    int ret = ensure_registry();
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    return register_descriptor(descriptor);
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_index_unregister(const struct_meta_descriptor *descriptor)
{
    if (descriptor == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    return unregister_descriptor(descriptor);
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_internal_index_find_validation(const struct_meta_descriptor *descriptor, int *result_out)
{
    if ((descriptor == NULL) || (result_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    const index_record *record = find_record(descriptor);
    if (record == NULL)
    {
        return CPLAT_SKIPPED;
    }
    *result_out = record->validation_result;
    return CPLAT_OK;
}

/* Doxygen コメントは、ヘッダーに記載 */

int struct_meta_internal_index_find_field(const struct_meta_descriptor *descriptor, const char *name,
                                          size_t name_length, size_t *index_out)
{
    if ((descriptor == NULL) || (name == NULL) || (index_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    const index_record *record = find_record(descriptor);
    if ((record == NULL) || (record->field_names == NULL))
    {
        return CPLAT_SKIPPED;
    }
    if (name_length >= STRUCT_META_INDEX_FIELD_NAME_BYTES)
    {
        /* 表へ入れたどの名前より長いため、線形走査でも一致しない。 */
        return CPLAT_ERR_NOT_FOUND;
    }

    /* 鍵は NUL 終端が要るため、区間名を一時バッファーへ複製する。 */
    char key[STRUCT_META_INDEX_FIELD_NAME_BYTES];
    memcpy(key, name, name_length);
    key[name_length] = '\0';

    const void *value = NULL;
    if (cplat_hashtable_find_value_ref(record->field_names, key, &value) != CPLAT_OK)
    {
        return CPLAT_ERR_NOT_FOUND;
    }
    /* value_align に sizeof(size_t) を指定しているため、型付きで参照できる。 */
    *index_out = *(const size_t *)value;
    return CPLAT_OK;
}
