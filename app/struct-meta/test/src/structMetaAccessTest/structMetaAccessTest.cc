#include <cplat/base/result.h>
#include <struct_meta/access/access.h>
#include <testfw.h>

namespace
{
struct Sample
{
    int value;
};

const struct_meta_attribute kAttributes[] = {{"schema.version", "1"}, {"sample.flag", nullptr}};
const struct_meta_descriptor kDescriptor = {"Sample", sizeof(Sample), nullptr, 0, "サンプルです。", kAttributes, 2};

struct Fields
{
    int first;
    unsigned int second;
};

const struct_meta_field kFields[] = {
    {"first", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Fields, first), sizeof(int), 1, 0, nullptr, nullptr,
     nullptr, 0},
    {"second", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(Fields, second), sizeof(unsigned int), 1, 0, nullptr,
     nullptr, nullptr, 0},
};
const struct_meta_descriptor kFieldsDescriptor = {"Fields", sizeof(Fields), kFields, 2, nullptr, nullptr, 0};
} // namespace

// 構造体記述子から指定した名前のフィールドを検索できることの確認
TEST(structMetaAccessTest, finds_field_by_name)
{
    // Arrange
    const struct_meta_field *first = nullptr;
    const struct_meta_field *second = nullptr;

    // Pre-Assert

    // Act
    int actual_ret_first = struct_meta_descriptor_find_field(&kFieldsDescriptor, "first",
                                                             &first); // [手順] - 先頭のフィールドを名前で検索する。
    int actual_ret_second = struct_meta_descriptor_find_field(&kFieldsDescriptor, "second",
                                                              &second); // [手順] - 末尾のフィールドを名前で検索する。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual_ret_first);  // [確認_正常系] - 先頭フィールドの検索が成功すること。
    EXPECT_EQ(&kFields[0], first);          // [確認_正常系] - 先頭フィールドの記述子が返ること。
    ASSERT_EQ(CPLAT_OK, actual_ret_second); // [確認_正常系] - 末尾フィールドの検索が成功すること。
    EXPECT_EQ(&kFields[1], second);         // [確認_正常系] - 末尾フィールドの記述子が返ること。
}

// 存在しないフィールド名を検索した場合に CPLAT_ERR_NOT_FOUND が返り、出力先が NULL になることの確認
TEST(structMetaAccessTest, returns_not_found_for_unknown_field)
{
    // Arrange
    const struct_meta_field *field = reinterpret_cast<const struct_meta_field *>(1);

    // Pre-Assert

    // Act
    int actual_ret = struct_meta_descriptor_find_field(&kFieldsDescriptor, "unknown",
                                                       &field); // [手順] - 存在しないフィールドを検索する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, actual_ret); // [確認_正常系] - フィールド検索が CPLAT_ERR_NOT_FOUND を返すこと。
    EXPECT_EQ(nullptr, field);                  // [確認_正常系] - 検索結果が NULL に初期化されること。
}

// フィールド検索関数に NULL の記述子、名前、出力先を渡した場合に不正引数エラーとなることの確認
TEST(structMetaAccessTest, rejects_invalid_arguments_for_find_field)
{
    // Arrange
    const struct_meta_field *field = nullptr;

    // Pre-Assert

    // Act
    int descriptor_ret = struct_meta_descriptor_find_field(nullptr, "first",
                                                           &field); // [手順] - NULL の記述子を指定する。
    int name_ret = struct_meta_descriptor_find_field(&kFieldsDescriptor, nullptr,
                                                     &field); // [手順] - NULL のフィールド名を指定する。
    int output_ret = struct_meta_descriptor_find_field(&kFieldsDescriptor, "first",
                                                       nullptr); // [手順] - NULL の出力先を指定する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, descriptor_ret); // [確認_異常系] - NULL の記述子が拒否されること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, name_ret);       // [確認_異常系] - NULL のフィールド名が拒否されること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, output_ret);     // [確認_異常系] - NULL の出力先が拒否されること。
}

// 構造体記述子から指定したキーの属性（値あり・値なし）を検索できることの確認
TEST(structMetaAccessTest, finds_descriptor_attributes)
{
    // Arrange
    const struct_meta_attribute *version = nullptr;
    const struct_meta_attribute *flag = nullptr;

    // Pre-Assert

    // Act
    int version_ret = struct_meta_descriptor_find_attribute(&kDescriptor, "schema.version",
                                                            &version); // [手順] - 値ありの構造体属性を検索する。
    int flag_ret = struct_meta_descriptor_find_attribute(&kDescriptor, "sample.flag",
                                                         &flag); // [手順] - 値なしの構造体属性を検索する。

    // Assert
    ASSERT_EQ(CPLAT_OK, version_ret);  // [確認_正常系] - schema.version 属性の検索が成功すること。
    ASSERT_NE(nullptr, version);       // [確認_正常系] - schema.version 属性が取得できること。
    EXPECT_STREQ("1", version->value); // [確認_正常系] - schema.version 属性の値が 1 であること。
    ASSERT_EQ(CPLAT_OK, flag_ret);     // [確認_正常系] - sample.flag 属性の検索が成功すること。
    ASSERT_NE(nullptr, flag);          // [確認_正常系] - sample.flag 属性が取得できること。
    EXPECT_EQ(nullptr, flag->value);   // [確認_正常系] - sample.flag 属性の値が NULL であること。
}

// 存在しない属性キーを検索した場合に CPLAT_ERR_NOT_FOUND が返り、出力先が NULL になることの確認
TEST(structMetaAccessTest, returns_not_found_for_unknown_key)
{
    // Arrange
    const struct_meta_attribute *attribute = reinterpret_cast<const struct_meta_attribute *>(1);

    // Pre-Assert

    // Act
    int actual_ret = struct_meta_descriptor_find_attribute(&kDescriptor, "unknown",
                                                           &attribute); // [手順] - 存在しない構造体属性を検索する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, actual_ret); // [確認_正常系] - 属性検索が CPLAT_ERR_NOT_FOUND を返すこと。
    EXPECT_EQ(nullptr, attribute);              // [確認_正常系] - 検索結果が NULL に初期化されること。
}

// 属性検索関数に NULL の記述子、キー、出力先を渡した場合に不正引数エラーとなることの確認
TEST(structMetaAccessTest, rejects_invalid_arguments)
{
    // Arrange
    const struct_meta_attribute *attribute = nullptr;

    // Pre-Assert

    // Act
    int descriptor_ret = struct_meta_descriptor_find_attribute(nullptr, "schema.version",
                                                               &attribute); // [手順] - NULL の記述子を指定する。
    int key_ret = struct_meta_descriptor_find_attribute(&kDescriptor, nullptr,
                                                        &attribute); // [手順] - NULL の属性キーを指定する。
    int output_ret = struct_meta_descriptor_find_attribute(&kDescriptor, "schema.version",
                                                           nullptr); // [手順] - NULL の出力先を指定する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, descriptor_ret); // [確認_異常系] - NULL の記述子が拒否されること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, key_ret);        // [確認_異常系] - NULL の属性キーが拒否されること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, output_ret);     // [確認_異常系] - NULL の出力先が拒否されること。
}

// 重複する属性キーを持つ構造体記述子の検証時に記述子破損エラーとなることの確認
TEST(structMetaAccessTest, rejects_duplicate_descriptor_attributes)
{
    // Arrange
    const struct_meta_attribute duplicate_attributes[] = {{"duplicate", "1"}, {"duplicate", "2"}};
    const struct_meta_descriptor descriptor = {"Sample", sizeof(Sample), nullptr, 0, nullptr, duplicate_attributes, 2};

    // Pre-Assert

    // Act
    int actual_ret =
        struct_meta_descriptor_validate(&descriptor); // [手順] - 重複する構造体属性を持つ記述子を検査する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual_ret); // [確認_異常系] - 重複する構造体属性が拒否されること。
}
