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
#include <cplat/crt/stdlib.h>
#include <cplat/hashtable/hashtable.h>

#include <assert.h>
#include <string.h>

/** 登録簿の初期レコード数です。登録数は事前に分からないため、自動拡張へ委ねます。 */
#define STRUCT_META_INDEX_REGISTRY_INITIAL_CAPACITY 16U

/**
 *  登録簿の削除済みレコードの寿命です。最小値の 2 で、削除したレコードを直ちに空へ戻します。
 *  値の範囲 (2 から 254 が有限、255 が無限) は cplat/hashtable/hashtable.h に従います。
 */
#define STRUCT_META_INDEX_REGISTRY_LIFETIME 2U

static_assert(STRUCT_META_INDEX_REGISTRY_LIFETIME >= 2U &&
                  STRUCT_META_INDEX_REGISTRY_LIFETIME < CPLAT_HASHTABLE_LIFETIME_INFINITE,
              "registry lifetime must be finite and at least 2");

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
 *  @brief          この呼び出しで登録簿へ追加した記述子の記録です。
 *
 *  失敗したときに、追加した記録だけを末尾から取り消すために使います。
 */
typedef struct added_descriptor_log
{
    const struct_meta_descriptor **items; /**< 追加した記述子です。未使用なら NULL です。 */
    size_t count;                         /**< @c items の使用数です。 */
    size_t capacity;                      /**< @c items の要素数です。 */
} added_descriptor_log;

static int ensure_registry(void);
static const index_record *find_record(const struct_meta_descriptor *descriptor);
static int build_field_names(const struct_meta_descriptor *descriptor, cplat_hashtable **table_out);
static int reserve_added_descriptor(added_descriptor_log *log);
static void rollback_added_descriptors(const added_descriptor_log *log);
static int register_descriptor(const struct_meta_descriptor *descriptor, added_descriptor_log *log);
static int unregister_descriptor(const struct_meta_descriptor *descriptor);

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

    added_descriptor_log log = {0};
    ret = register_descriptor(descriptor, &log);
    if (ret != CPLAT_OK)
    {
        rollback_added_descriptors(&log);
    }

    cplat_free(log.items);

    return ret;
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
    cplat_hashtable_growth_config growth = {0}; /* 全フィールド 0 で上限なし */

    config.capacity = STRUCT_META_INDEX_REGISTRY_INITIAL_CAPACITY;
    config.key_type = CPLAT_HASHTABLE_FIELD_BINARY;
    config.key_size = sizeof(const struct_meta_descriptor *);
    config.value_type = CPLAT_HASHTABLE_FIELD_BINARY;
    config.value_size = sizeof(index_record);
    config.value_align = sizeof(void *);
    /*
     * 登録簿は永続化せず、削除済みのレコードを参照する処理がないため、解除したレコードを残さない。
     * 残すと、異なるアドレスの記述子の登録と解除を繰り返すたびに、登録簿が大きくなり続ける。
     */
    config.lifetime = STRUCT_META_INDEX_REGISTRY_LIFETIME;

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
 *  @param[in]      descriptor  対象の記述子。検査に成功した記述子であること。
 *  @param[out]     table_out   構築した表の格納先。表を作らない場合は NULL を格納します。
 *  @return         @c CPLAT_OK (表を作った場合と、意図して作らない場合)、
 *                  @c CPLAT_ERR_OUT_OF_MEMORY 、または @c CPLAT_ERR_UNKNOWN を返します。
 *
 *  フィールドが 0 個の場合と、名前が長すぎるフィールドを含む場合は、表を作らず、成功を返します。
 *  表の作成が @c CPLAT_ERR_OUT_OF_MEMORY のときは、そのまま返します。
 *  それ以外の作成の失敗と、追加の失敗は @c CPLAT_ERR_UNKNOWN です。
 *  失敗した場合は、作りかけの表を解放します。
 */
static int build_field_names(const struct_meta_descriptor *descriptor, cplat_hashtable **table_out)
{
    *table_out = NULL;

    if (descriptor->field_count == 0U)
    {
        return CPLAT_OK;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (strlen(descriptor->fields[i].name) >= STRUCT_META_INDEX_FIELD_NAME_BYTES)
        {
            /* 一部だけを表に入れると、表引きの不一致を「無い」と誤って判断する。表は作らない。 */
            return CPLAT_OK;
        }
    }

    cplat_hashtable_config config = {0};
    cplat_hashtable *table = NULL;

    /* 件数の STRUCT_META_INDEX_CAPACITY_FACTOR 倍とする。正本は app/struct-meta/docs/architecture.md の「索引の容量」節である。 */
    config.capacity = descriptor->field_count * STRUCT_META_INDEX_CAPACITY_FACTOR;
    config.key_type = CPLAT_HASHTABLE_FIELD_FIXED_STRING;
    config.key_size = STRUCT_META_INDEX_FIELD_NAME_BYTES;
    config.value_type = CPLAT_HASHTABLE_FIELD_BINARY;
    config.value_size = sizeof(size_t);
    config.value_align = sizeof(size_t);
    /*
     * 索引は構築後に削除しないため、lifetime は動作に影響しない。
     * 構成検査が 2 以上を求めるため、値を指定する。
     */
    config.lifetime = CPLAT_HASHTABLE_LIFETIME_INFINITE;

    const int create_result = cplat_hashtable_create(&config, NULL, 0, NULL, 0, &table);
    if (create_result != CPLAT_OK)
    {
        /* 失敗した作成が表を書き残しても、呼び出し元へ渡さない。NULL の破棄は何もしない。 */
        cplat_hashtable_dispose(table);

        if (create_result == CPLAT_ERR_OUT_OF_MEMORY)
        {
            return CPLAT_ERR_OUT_OF_MEMORY;
        }

        return CPLAT_ERR_UNKNOWN;
    }

    for (size_t i = 0; i < descriptor->field_count; i++)
    {
        if (cplat_hashtable_add(table, descriptor->fields[i].name, &i, CPLAT_HASHTABLE_ADD_DELETED_OVERWRITE) !=
            CPLAT_OK)
        {
            cplat_hashtable_dispose(table);
            return CPLAT_ERR_UNKNOWN;
        }
    }

    *table_out = table;

    return CPLAT_OK;
}

/**
 *  @brief          取り消しの記録へ、1 要素分の空きを用意します。
 *  @return         @c CPLAT_OK 、または @c CPLAT_ERR_OUT_OF_MEMORY を返します。
 *
 *  登録簿へ追加する前に呼びます。確保に失敗した場合は登録簿へ追加しないため、
 *  記録と登録簿の内容がずれません。
 *  失敗した cplat_realloc() は元の領域を解放しないため、成功したときだけ代入します。
 */
static int reserve_added_descriptor(added_descriptor_log *log)
{
    if (log->count < log->capacity)
    {
        return CPLAT_OK;
    }

    const size_t new_capacity = log->capacity + 1U;
    const struct_meta_descriptor **items =
        (const struct_meta_descriptor **)cplat_realloc(log->items, new_capacity, sizeof(*log->items));
    if (items == NULL)
    {
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    log->items = items;
    log->capacity = new_capacity;

    return CPLAT_OK;
}

/**
 *  @brief          この呼び出しで追加した記録を、末尾から取り消します。
 *
 *  取り消しは、やり直しがネスト先へ届かないことを避けるために行います。
 *  親だけが残ると、次の登録は親を登録済みと見てネスト先へ進みません。
 *  記録に載せた記述子だけを対象にし、再帰では解除しません。
 *  呼び出しの前から登録されていた記述子と、共有されたネスト先には触れません。
 *  ensure_registry() が構築した登録簿は、空のまま残します。
 */
static void rollback_added_descriptors(const added_descriptor_log *log)
{
    for (size_t i = log->count; i > 0U; i--)
    {
        const struct_meta_descriptor *descriptor = log->items[i - 1U];
        const index_record *record = find_record(descriptor);
        if (record == NULL)
        {
            continue;
        }

        /* 削除が値のポインターを無効にするため、フィールド名の表は先に控える。 */
        cplat_hashtable *field_names = record->field_names;

        /* 直前に登録した記述子のキーの削除で失敗しないため、戻り値は明示的に破棄する。 */
        (void)cplat_hashtable_delete(s_registry, &descriptor);
        cplat_hashtable_dispose(field_names);
    }
}

/**
 *  @brief          記述子 1 個と、そのネスト先を再帰的に登録します。
 *  @param[in]      descriptor  登録する記述子です。
 *  @param[in,out]  log         この呼び出しで追加した記述子の記録です。
 *  @return         @c CPLAT_OK 、@c CPLAT_ERR_OUT_OF_MEMORY 、または
 *                  @c CPLAT_ERR_UNKNOWN を返します。
 */
static int register_descriptor(const struct_meta_descriptor *descriptor, added_descriptor_log *log)
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
        const int ret = build_field_names(descriptor, &record.field_names);
        if (ret != CPLAT_OK)
        {
            return ret;
        }
    }

    int ret = reserve_added_descriptor(log);
    if (ret != CPLAT_OK)
    {
        cplat_hashtable_dispose(record.field_names);
        return ret;
    }

    if (cplat_hashtable_add(s_registry, &descriptor, &record, CPLAT_HASHTABLE_ADD_DELETED_OVERWRITE) != CPLAT_OK)
    {
        cplat_hashtable_dispose(record.field_names);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    log->items[log->count] = descriptor;
    log->count++;

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

        ret = register_descriptor(descriptor->fields[i].nested, log);
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

    /*
     * 先に登録簿から外し、記述子の循環による無限再帰を止める。
     * 直前の find_record() で登録を確かめたキーの削除であり、失敗しないため、戻り値は明示的に破棄する。
     */
    (void)cplat_hashtable_delete(s_registry, &descriptor);
    cplat_hashtable_dispose(field_names);

    if (validation_result == CPLAT_OK)
    {
        for (size_t i = 0; i < descriptor->field_count; i++)
        {
            if (descriptor->fields[i].nested != NULL)
            {
                /*
                 * ネスト先が未登録または既に解除されていても、全体の解除処理を継続するため、
                 * 戻り値は明示的に破棄する。
                 */
                (void)unregister_descriptor(descriptor->fields[i].nested);
            }
        }
    }

    return CPLAT_OK;
}
