#include <cplat/base/result.h>
#include <cplat/hashtable/hashtable.h>
#include <mock_cplat.h>
#include <struct_meta/access/access.h>
#include <struct_meta/catalog/catalog.h>
#include <struct_meta/meta/index.h>
#include <struct_meta/meta/index_internal.h>
#include <testfw.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace
{
struct CatalogProbe
{
    int value;
};

const struct_meta_field kFields[] = {
    {"value", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(CatalogProbe, value), sizeof(int), 1, 0, nullptr, nullptr,
     nullptr, 0},
};
const struct_meta_descriptor kDescriptor = {"CatalogProbe", sizeof(CatalogProbe), kFields, 1, nullptr, nullptr, 0};
const struct_meta_descriptor *const kDescriptors[] = {&kDescriptor};

/**
 *  @brief          外部バッファーを 16 バイト境界へ揃えて確保します。
 *
 *  cplat_hashtable_create() は、管理領域と、value_align が 0 でないデータ領域に境界を要求します。
 *  16 バイトは、その要求を満たす上限です。
 */
void *allocate_aligned(size_t size, void **raw_out)
{
    constexpr size_t alignment = 16U;
    void *raw = std::malloc(size + alignment);
    if (raw == nullptr)
    {
        *raw_out = nullptr;
        return nullptr;
    }

    *raw_out = raw;
    const auto address = reinterpret_cast<uintptr_t>(raw);
    const auto aligned = (address + (alignment - 1U)) & ~(static_cast<uintptr_t>(alignment) - 1U);
    return reinterpret_cast<void *>(aligned);
}
} // namespace

// カタログ作成時のフィールド名表の失敗が、作成を失敗させず、線形走査を残すことの確認
TEST(structMetaCatalogIndexFailureTest, attach_static_keeps_catalog_when_field_index_create_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock;
    cplat_hashtable_config config = {};
    config.capacity = 1U * STRUCT_META_INDEX_CAPACITY_FACTOR;
    config.key_type = CPLAT_HASHTABLE_FIELD_FIXED_STRING;
    config.key_size = strlen(kDescriptor.name) + 1U;
    config.value_type = CPLAT_HASHTABLE_FIELD_BINARY;
    config.value_size = sizeof(size_t);
    config.value_align = sizeof(size_t);
    config.lifetime = CPLAT_HASHTABLE_LIFETIME_INFINITE;

    size_t mgmt_size = 0U;
    size_t data_size = 0U;
    int size_ret = cplat_hashtable_required_size(&config, &mgmt_size, &data_size);
    void *raw_mgmt = nullptr;
    void *raw_data = nullptr;
    void *mgmt = allocate_aligned(mgmt_size, &raw_mgmt);
    void *data = allocate_aligned(data_size, &raw_data);
    cplat_hashtable *table = nullptr;
    size_t index = 0U;
    int create_ret = CPLAT_ERR_INVALID_ARGUMENT;
    int add_ret = CPLAT_ERR_INVALID_ARGUMENT;
    if ((mgmt != nullptr) && (data != nullptr))
    {
        create_ret = cplat_hashtable_create(&config, mgmt, mgmt_size, data, data_size, &table);
        if (create_ret == CPLAT_OK)
        {
            add_ret = cplat_hashtable_add(table, kDescriptor.name, &index, CPLAT_HASHTABLE_ADD_DELETED_OVERWRITE);
            cplat_hashtable_dispose(table);
        }
    }

    ASSERT_EQ(CPLAT_OK, size_ret);   // [状態確認] - cplat_hashtable_required_size() の戻り値が CPLAT_OK であること。
    ASSERT_NE(nullptr, mgmt);        // [状態確認] - 管理領域を確保できること。
    ASSERT_NE(nullptr, data);        // [状態確認] - データ領域を確保できること。
    ASSERT_EQ(CPLAT_OK, create_ret); // [状態確認] - 静的索引イメージの cplat_hashtable_create() の戻り値が CPLAT_OK であること。
    ASSERT_EQ(CPLAT_OK, add_ret);    // [状態確認] - 型名の cplat_hashtable_add() の戻り値が CPLAT_OK であること。

    // Pre-Assert
    EXPECT_CALL(mock, cplat_hashtable_create(_, _, _, _, _, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Invoke([](const cplat_hashtable_config *, void *, size_t, void *, size_t,
                                  cplat_hashtable **ht_out) {
            if (ht_out != nullptr)
            {
                *ht_out = nullptr;
            }
            return CPLAT_ERR_OUT_OF_MEMORY;
        }));
    // [Pre-Assert確認_異常系] - cplat_hashtable_create() が 1 回以上呼び出されること。
    // [Pre-Assert手順] - cplat_hashtable_create() から CPLAT_ERR_OUT_OF_MEMORY を返し、出力の表を nullptr にする。

    // Act
    struct_meta_catalog *catalog = nullptr;
    int attach_ret = struct_meta_catalog_attach_static(kDescriptors, 1U, mgmt, mgmt_size, data, data_size, &catalog);
    // [手順] - 手書きの記述子と静的索引イメージを指定して struct_meta_catalog_attach_static() を呼び出す。
    int validate_ret = struct_meta_descriptor_validate(&kDescriptor);
    // [手順] - 同じ記述子を指定して struct_meta_descriptor_validate() を呼び出す。
    const struct_meta_field *field = nullptr;
    int find_ret = struct_meta_descriptor_find_field(&kDescriptor, "value", &field);
    // [手順] - 同じ記述子と "value" を指定して struct_meta_descriptor_find_field() を呼び出す。
    // 登録簿は index.c の static であり、ADD_SRCS には inject が入らない。公開の解除結果で未登録を確かめる。
    int unregister_ret = struct_meta_index_unregister(&kDescriptor);
    // [手順] - 同じ記述子を指定して struct_meta_index_unregister() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_OK, attach_ret); // [確認_正常系] - struct_meta_catalog_attach_static() の戻り値が CPLAT_OK であること。
    EXPECT_NE(nullptr, catalog);     // [確認_正常系] - catalog が nullptr 以外であること。
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, unregister_ret);
    // [確認_異常系] - struct_meta_index_unregister() の戻り値が CPLAT_ERR_NOT_FOUND であること。
    EXPECT_EQ(CPLAT_OK, validate_ret); // [確認_正常系] - struct_meta_descriptor_validate() の戻り値が CPLAT_OK であること。
    ASSERT_EQ(CPLAT_OK, find_ret); // [確認_正常系] - struct_meta_descriptor_find_field() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("value", field->name); // [確認_正常系] - field->name が "value" であること。

    // Cleanup
    struct_meta_catalog_destroy(catalog); // [破棄] - カタログを破棄する。
    std::free(raw_mgmt);                  // [破棄] - 索引イメージの管理領域を解放する。
    std::free(raw_data);                  // [破棄] - 索引イメージのデータ領域を解放する。
}
