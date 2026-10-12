#include <cplat/base/result.h>
#include <struct_meta/access/access.h>
#include <struct_meta/meta/index.h>
#include <testfw.h>

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
} // namespace

// 索引に未登録の記述子でも線形探索によりフィールドを検索できることの確認
TEST(structMetaIndexTest, unregistered_descriptor_is_searched_linearly)
{
    // Arrange
    const struct_meta_field *field = nullptr;

    // Pre-Assert

    // Act
    int actual = struct_meta_descriptor_find_field(&kDescriptor, "age",
                                                   &field); // [手順] - 未登録の記述子でフィールドを検索する。

    // Assert
    ASSERT_EQ(CPLAT_OK,
              actual); // [確認_正常系] - 未登録の struct_meta_descriptor_find_field() の戻り値が CPLAT_OK であること。
    ASSERT_NE(nullptr, field);        // [確認_正常系] - field が nullptr 以外であること。
    EXPECT_STREQ("age", field->name); // [確認_正常系] - field->name が "age" であること。
}

// 記述子を索引へ登録した前後で同じフィールド検索結果が得られることの確認
TEST(structMetaIndexTest, registered_descriptor_returns_same_fields)
{
    // Arrange
    const struct_meta_field *before = nullptr;
    const struct_meta_field *after = nullptr;
    int before_ret = struct_meta_descriptor_find_field(&kDescriptor, "nested", &before);

    ASSERT_EQ(CPLAT_OK,
              before_ret); // [状態確認] - 登録前の struct_meta_descriptor_find_field() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    // Act
    int register_ret = struct_meta_index_register(&kDescriptor); // [手順] - 記述子を索引へ登録する。
    int after_ret = struct_meta_descriptor_find_field(&kDescriptor, "nested",
                                                      &after); // [手順] - 登録後に同じフィールドを検索する。

    // Assert
    EXPECT_EQ(CPLAT_OK, register_ret); // [確認_正常系] - struct_meta_index_register() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        CPLAT_OK,
        after_ret); // [確認_正常系] - 登録後の struct_meta_descriptor_find_field() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(before, after); // [確認_正常系] - after が before と一致すること。

    // Cleanup
    struct_meta_index_unregister(&kDescriptor); // [破棄] - 登録した記述子を解除する。
}

// 索引に登録された記述子において存在しないフィールドの検索が CPLAT_ERR_NOT_FOUND となることの確認
TEST(structMetaIndexTest, registered_descriptor_reports_unknown_field)
{
    // Arrange
    const struct_meta_field *field = reinterpret_cast<const struct_meta_field *>(1);
    int register_ret = struct_meta_index_register(&kDescriptor);

    ASSERT_EQ(CPLAT_OK, register_ret); // [状態確認] - struct_meta_index_register() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_descriptor_find_field(&kDescriptor, "unknown",
                                                   &field); // [手順] - 存在しないフィールドを検索する。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        actual); // [確認_正常系] - 登録後の struct_meta_descriptor_find_field() の戻り値が CPLAT_ERR_NOT_FOUND であること。
    EXPECT_EQ(nullptr, field); // [確認_正常系] - field が nullptr であること。

    // Cleanup
    struct_meta_index_unregister(&kDescriptor); // [破棄] - 登録した記述子を解除する。
}

// 親構造体の記述子を索引へ登録した際にネスト先の記述子も再帰的に登録されることの確認
TEST(structMetaIndexTest, nested_descriptor_is_registered_together)
{
    // Arrange
    const struct_meta_field *field = nullptr;
    int register_ret = struct_meta_index_register(&kDescriptor);

    ASSERT_EQ(
        CPLAT_OK,
        register_ret); // [状態確認] - 親の記述子に対する struct_meta_index_register() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_descriptor_find_field(&kNestedDescriptor, "inner",
                                                   &field); // [手順] - ネスト先のフィールドを検索する。
    int unregister_nested =
        struct_meta_index_unregister(&kNestedDescriptor); // [手順] - ネスト先が登録済みかを登録解除で確かめる。

    // Assert
    ASSERT_EQ(
        CPLAT_OK,
        actual); // [確認_正常系] - ネスト先に対する struct_meta_descriptor_find_field() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("inner", field->name); // [確認_正常系] - field->name が "inner" であること。
    EXPECT_EQ(
        CPLAT_OK,
        unregister_nested); // [確認_正常系] - ネスト先に対する struct_meta_index_unregister() の戻り値が CPLAT_OK であること。(ネスト先も再帰的に登録されたことの確認)

    // Cleanup
    struct_meta_index_unregister(&kDescriptor); // [破棄] - 登録した記述子を解除する。
}

// 記述子の登録解除後に線形探索へ復帰し、未登録の二重解除が拒否されることの確認
TEST(structMetaIndexTest, unregister_restores_linear_search)
{
    // Arrange
    const struct_meta_field *field = nullptr;
    int register_ret = struct_meta_index_register(&kDescriptor);

    ASSERT_EQ(CPLAT_OK, register_ret); // [状態確認] - struct_meta_index_register() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    // Act
    int unregister_ret = struct_meta_index_unregister(&kDescriptor); // [手順] - 記述子を登録解除する。
    int again_ret = struct_meta_index_unregister(&kDescriptor);      // [手順] - 登録解除済みの記述子を再度解除する。
    int find_ret = struct_meta_descriptor_find_field(&kDescriptor, "id",
                                                     &field); // [手順] - 登録解除後にフィールドを検索する。

    // Assert
    EXPECT_EQ(
        CPLAT_OK,
        unregister_ret); // [確認_正常系] - 1 回目の struct_meta_index_unregister() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        again_ret); // [確認_異常系] - 登録解除後の struct_meta_index_unregister() の戻り値が CPLAT_ERR_NOT_FOUND であること。
    ASSERT_EQ(
        CPLAT_OK,
        find_ret); // [確認_正常系] - 登録解除後の struct_meta_descriptor_find_field() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("id", field->name); // [確認_正常系] - field->name が "id" であること。
}

// 破損した記述子を索引へ登録しても記述子の検証結果が一貫してエラーとなることの確認
TEST(structMetaIndexTest, corrupt_descriptor_keeps_same_validation_result)
{
    // Arrange
    const struct_meta_attribute duplicate_attributes[] = {{"duplicate", "1"}, {"duplicate", "2"}};
    const struct_meta_descriptor corrupt = {"Corrupt", sizeof(Sample), nullptr, 0, nullptr, duplicate_attributes, 2};
    int before = struct_meta_descriptor_validate(&corrupt);

    ASSERT_EQ(
        CPLAT_ERR_CORRUPT_DESCRIPTOR,
        before); // [状態確認] - 登録前の struct_meta_descriptor_validate() の戻り値が CPLAT_ERR_CORRUPT_DESCRIPTOR であること。

    // Pre-Assert

    // Act
    int register_ret = struct_meta_index_register(&corrupt); // [手順] - 壊れた記述子を索引へ登録する。
    int after = struct_meta_descriptor_validate(&corrupt);   // [手順] - 登録後に同じ記述子を検査する。

    // Assert
    EXPECT_EQ(
        CPLAT_OK,
        register_ret); // [確認_正常系] - 壊れた記述子に対する struct_meta_index_register() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        CPLAT_ERR_CORRUPT_DESCRIPTOR,
        after); // [確認_異常系] - 登録後の struct_meta_descriptor_validate() の戻り値が CPLAT_ERR_CORRUPT_DESCRIPTOR であること。

    // Cleanup
    struct_meta_index_unregister(&corrupt); // [破棄] - 登録した記述子を解除する。
}

// NULLの記述子の登録および登録解除が不正引数エラーとなることの確認
TEST(structMetaIndexTest, rejects_invalid_arguments)
{
    // Arrange

    // Pre-Assert

    // Act
    int register_ret = struct_meta_index_register(nullptr);     // [手順] - NULL の記述子を登録する。
    int unregister_ret = struct_meta_index_unregister(nullptr); // [手順] - NULL の記述子を登録解除する。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        register_ret); // [確認_異常系] - NULL の記述子に対する struct_meta_index_register() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        unregister_ret); // [確認_異常系] - NULL の記述子に対する struct_meta_index_unregister() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
}
