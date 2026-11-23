#include <cplat/base/result.h>
#include <cplat/hashtable/hashtable.h>
#include <mock_cplat.h>
#include <struct_meta/meta/index.h>
#include <testfw.h>

#include "index.inject.h"

#include <cstring>

namespace
{
struct Nested
{
    int inner;
};

struct Sample
{
    int id;
    unsigned int age;
    Nested nested;
};

const struct_meta_field kNestedFields[] = {
    {"inner", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Nested, inner), sizeof(int), 1, 0, nullptr, nullptr,
     nullptr, 0},
};
const struct_meta_descriptor kNestedDescriptor = {"Nested", sizeof(Nested), kNestedFields, 1, nullptr, nullptr, 0};

const struct_meta_field kFields[] = {
    {"id", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Sample, id), sizeof(int), 1, 0, nullptr, nullptr, nullptr, 0},
    {"age", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(Sample, age), sizeof(unsigned int), 1, 0, nullptr, nullptr,
     nullptr, 0},
    {"nested", STRUCT_META_FIELD_STRUCT, 0, offsetof(Sample, nested), sizeof(Nested), 1, 0, &kNestedDescriptor, nullptr,
     nullptr, 0},
};
const struct_meta_descriptor kDescriptor = {"Sample", sizeof(Sample), kFields, 3, nullptr, nullptr, 0};

size_t registry_count()
{
    cplat_hashtable *registry = test_struct_meta_index_registry();
    if (registry == nullptr)
    {
        return 0U;
    }

    size_t count = 0U;
    if (cplat_hashtable_count(registry, &count) != CPLAT_OK)
    {
        return static_cast<size_t>(-1);
    }
    return count;
}

int fail_hashtable_create(cplat_hashtable **ht_out, int result)
{
    if (ht_out != nullptr)
    {
        *ht_out = nullptr;
    }
    return result;
}
} // namespace

// フィールド名の表の作成がメモリ不足のとき、登録が失敗し、記録が残らないことの確認
TEST(structMetaIndexFailureTest, field_table_create_out_of_memory_leaves_registry_empty)
{
    // Arrange
    NiceMock<Mock_cplat> mock;

    // Pre-Assert
    EXPECT_CALL(mock, cplat_hashtable_create(_, _, _, _, _, _)).WillOnce(Return(CPLAT_ERR_OUT_OF_MEMORY));
    // [Pre-Assert確認_異常系] - cplat_hashtable_create() が CPLAT_ERR_OUT_OF_MEMORY を 1 回返すこと。

    // Act
    int ret = struct_meta_index_register(
        &kNestedDescriptor); // [手順] - フィールドを 1 個持つ記述子を指定して struct_meta_index_register() を呼び出す。
    cplat_hashtable *registry = test_struct_meta_index_registry();
    size_t count = 0U;
    int count_ret = CPLAT_ERR_INVALID_ARGUMENT;
    if (registry != nullptr)
    {
        count_ret = cplat_hashtable_count(registry, &count); // [手順] - cplat_hashtable_count() で登録簿の件数を取得する。
    }

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_MEMORY,
        ret); // [確認_異常系] - struct_meta_index_register() の戻り値が CPLAT_ERR_OUT_OF_MEMORY であること。
    ASSERT_NE(nullptr, registry); // [確認_異常系] - 登録簿は構築されたままであること。
    EXPECT_EQ(CPLAT_OK, count_ret); // [確認_異常系] - cplat_hashtable_count() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(0U, count);          // [確認_異常系] - 登録簿の件数が 0 であること。
}

// ネスト先の表の作成が失敗したとき、親の記録を取り消し、再登録が両方へ届くことの確認
TEST(structMetaIndexFailureTest, nested_field_table_failure_is_rolled_back_and_retry_registers_both)
{
    // Arrange
    NiceMock<Mock_cplat> mock;
    int create_count = 0;
    bool fail_second_create = true;

    // Pre-Assert
    EXPECT_CALL(mock, cplat_hashtable_create(_, _, _, _, _, _))
        .WillRepeatedly(Invoke([&](const cplat_hashtable_config *config, void *buf_mgmt, size_t buf_mgmt_size,
                                   void *buf_data, size_t buf_data_size, cplat_hashtable **ht_out) {
            create_count++;
            if (fail_second_create && (create_count == 2))
            {
                return fail_hashtable_create(ht_out, CPLAT_ERR_INVALID_ARGUMENT);
            }
            return delegate_real_cplat_hashtable_create(config, buf_mgmt, buf_mgmt_size, buf_data, buf_data_size,
                                                        ht_out);
        }));
    // [Pre-Assert確認_異常系] - cplat_hashtable_create() の 2 回目だけが CPLAT_ERR_INVALID_ARGUMENT を返すこと。
    // [Pre-Assert手順] - 1 回目は親のフィールド名の表、2 回目はネスト先の表である。

    // Act
    int failed = struct_meta_index_register(
        &kDescriptor); // [手順] - 親の記述子を指定して struct_meta_index_register() を呼び出す。
    size_t count_after_failure = registry_count();
    fail_second_create = false;
    int retried = struct_meta_index_register(
        &kDescriptor); // [手順] - mock を既定へ戻して、同じ記述子を struct_meta_index_register() で再び登録する。
    size_t count_after_retry = registry_count();

    // Assert
    EXPECT_EQ(CPLAT_ERR_UNKNOWN,
              failed); // [確認_異常系] - 作成の失敗がメモリ不足以外のとき、戻り値が CPLAT_ERR_UNKNOWN であること。
    EXPECT_EQ(0U, count_after_failure); // [確認_異常系] - 失敗の後の登録簿の件数が 0 であること。
    EXPECT_EQ(CPLAT_OK, retried);       // [確認_正常系] - 再登録の struct_meta_index_register() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(2U, count_after_retry); // [確認_正常系] - 再登録の後の登録簿の件数が 2 であること。

    // Cleanup
    struct_meta_index_unregister(&kDescriptor); // [破棄] - 登録した記述子を解除する。
}

// 呼び出しの前から登録されていたネスト先が、後の失敗で消えないことの確認
TEST(structMetaIndexFailureTest, previously_registered_nested_descriptor_survives_parent_failure)
{
    // Arrange
    NiceMock<Mock_cplat> mock;
    int prepared = struct_meta_index_register(&kNestedDescriptor);

    ASSERT_EQ(CPLAT_OK, prepared); // [状態確認] - ネスト先の struct_meta_index_register() の戻り値が CPLAT_OK であること。
    ASSERT_EQ(1U, registry_count()); // [状態確認] - 登録簿の件数が 1 であること。

    // Pre-Assert
    EXPECT_CALL(mock, cplat_hashtable_create(_, _, _, _, _, _))
        .WillOnce(Invoke([](const cplat_hashtable_config *, void *, size_t, void *, size_t, cplat_hashtable **ht_out) {
            return fail_hashtable_create(ht_out, CPLAT_ERR_OUT_OF_MEMORY);
        }));
    // [Pre-Assert確認_異常系] - 次の cplat_hashtable_create() が CPLAT_ERR_OUT_OF_MEMORY を返すこと。

    // Act
    int failed = struct_meta_index_register(
        &kDescriptor); // [手順] - 親の記述子を指定して struct_meta_index_register() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_MEMORY,
        failed); // [確認_異常系] - struct_meta_index_register() の戻り値が CPLAT_ERR_OUT_OF_MEMORY であること。
    EXPECT_EQ(1U, registry_count()); // [確認_異常系] - 登録簿の件数が 1 のままであること。
    EXPECT_EQ(CPLAT_OK,
              struct_meta_index_unregister(
                  &kNestedDescriptor)); // [確認_異常系] - ネスト先の struct_meta_index_unregister() の戻り値が CPLAT_OK であること。
}

// 登録簿への追加だけが失敗したとき、その呼び出しで追加した記録が取り消されることの確認
TEST(structMetaIndexFailureTest, registry_add_failure_rolls_back_records_added_by_the_call)
{
    // Arrange
    NiceMock<Mock_cplat> mock;
    bool fail_nested_registry_add = true;

    // Pre-Assert
    EXPECT_CALL(mock, cplat_hashtable_add(_, _, _, _))
        .WillRepeatedly(Invoke([&](cplat_hashtable *ht, const void *key, const void *value,
                                   cplat_hashtable_add_deleted_policy policy) {
            cplat_hashtable *registry = test_struct_meta_index_registry();
            if (fail_nested_registry_add && (ht == registry) && (registry != nullptr) && (key != nullptr))
            {
                const struct_meta_descriptor *descriptor = *static_cast<const struct_meta_descriptor *const *>(key);
                if (descriptor == &kNestedDescriptor)
                {
                    return CPLAT_ERR_OUT_OF_MEMORY;
                }
            }
            return delegate_real_cplat_hashtable_add(ht, key, value, policy);
        }));
    // [Pre-Assert確認_異常系] - 登録簿への追加のうち、ネスト先の追加だけが CPLAT_ERR_OUT_OF_MEMORY を返すこと。
    // [Pre-Assert手順] - フィールド名の表への追加は実関数へ委譲する。親の登録簿への追加は成功させ、取り消す対象を残す。

    // Act
    int failed = struct_meta_index_register(
        &kDescriptor); // [手順] - 親の記述子を指定して struct_meta_index_register() を呼び出す。
    size_t count_after_failure = registry_count();
    fail_nested_registry_add = false;
    int retried = struct_meta_index_register(
        &kDescriptor); // [手順] - 追加の失敗をやめ、同じ記述子を struct_meta_index_register() で再び登録する。
    size_t count_after_retry = registry_count();

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_MEMORY,
        failed); // [確認_異常系] - struct_meta_index_register() の戻り値が CPLAT_ERR_OUT_OF_MEMORY であること。
    EXPECT_EQ(0U, count_after_failure); // [確認_異常系] - 失敗の後の登録簿の件数が 0 であること。
    EXPECT_EQ(CPLAT_OK, retried); // [確認_正常系] - 再登録の struct_meta_index_register() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(2U, count_after_retry); // [確認_正常系] - 再登録の後の登録簿の件数が 2 であること。

    // Cleanup
    struct_meta_index_unregister(&kDescriptor); // [破棄] - 登録した記述子を解除する。
}

// 名前が 64 バイト以上のフィールドでは、表を作らず成功することの確認
TEST(structMetaIndexFailureTest, field_name_of_64_bytes_skips_field_table_and_succeeds)
{
    // Arrange
    NiceMock<Mock_cplat> mock;
    struct LongNameHost
    {
        int value;
    };
    char name[65];
    memset(name, 'n', 64);
    name[64] = '\0';
    struct_meta_field fields[] = {
        {name, STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(LongNameHost, value), sizeof(int), 1, 0, nullptr, nullptr,
         nullptr, 0},
    };
    struct_meta_descriptor descriptor = {"LongNameHost", sizeof(LongNameHost), fields, 1, nullptr, nullptr, 0};
    int valid = struct_meta_descriptor_validate(&descriptor);

    ASSERT_EQ(64U, strlen(name)); // [状態確認] - フィールド名の長さが 64 であること。
    ASSERT_EQ(CPLAT_OK, valid);   // [状態確認] - struct_meta_descriptor_validate() の戻り値が CPLAT_OK であること。

    // Pre-Assert
    EXPECT_CALL(mock, cplat_hashtable_create(_, _, _, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - cplat_hashtable_create() が呼ばれないこと。

    // Act
    int ret = struct_meta_index_register(
        &descriptor); // [手順] - 長いフィールド名を持つ記述子を指定して struct_meta_index_register() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_OK, ret); // [確認_正常系] - struct_meta_index_register() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(1U, registry_count()); // [確認_正常系] - 登録簿の件数が 1 であること。

    // Cleanup
    struct_meta_index_unregister(&descriptor); // [破棄] - 登録した記述子を解除する。
}
