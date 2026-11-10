#include <cstring>
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
    int actual_ret_first = struct_meta_descriptor_find_field(
        &kFieldsDescriptor, "first",
        &first); // [手順] - 先頭のフィールド名を指定して struct_meta_descriptor_find_field() を呼び出す。
    int actual_ret_second = struct_meta_descriptor_find_field(
        &kFieldsDescriptor, "second",
        &second); // [手順] - 末尾のフィールド名を指定して struct_meta_descriptor_find_field() を呼び出す。

    // Assert
    ASSERT_EQ(
        CPLAT_OK,
        actual_ret_first); // [確認_正常系] - "first" を検索した struct_meta_descriptor_find_field() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(&kFields[0], first); // [確認_正常系] - first が &kFields[0] と一致すること。
    ASSERT_EQ(
        CPLAT_OK,
        actual_ret_second); // [確認_正常系] - "second" を検索した struct_meta_descriptor_find_field() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(&kFields[1], second); // [確認_正常系] - second が &kFields[1] と一致すること。
}

// 存在しないフィールド名を検索した場合に CPLAT_ERR_NOT_FOUND が返り、出力先が NULL になることの確認
TEST(structMetaAccessTest, returns_not_found_for_unknown_field)
{
    // Arrange
    const struct_meta_field *field = reinterpret_cast<const struct_meta_field *>(1);

    // Pre-Assert

    // Act
    int actual_ret = struct_meta_descriptor_find_field(
        &kFieldsDescriptor, "unknown",
        &field); // [手順] - 存在しないフィールド名を指定して struct_meta_descriptor_find_field() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        actual_ret); // [確認_正常系] - struct_meta_descriptor_find_field() の戻り値が CPLAT_ERR_NOT_FOUND であること。
    EXPECT_EQ(nullptr, field); // [確認_正常系] - field が nullptr であること。
}

// フィールド検索関数に NULL の記述子、名前、出力先を渡した場合に不正引数エラーとなることの確認
TEST(structMetaAccessTest, rejects_invalid_arguments_for_find_field)
{
    // Arrange
    const struct_meta_field *field = nullptr;

    // Pre-Assert

    // Act
    int descriptor_ret = struct_meta_descriptor_find_field(
        nullptr, "first",
        &field); // [手順] - nullptr の記述子を指定して struct_meta_descriptor_find_field() を呼び出す。
    int name_ret = struct_meta_descriptor_find_field(
        &kFieldsDescriptor, nullptr,
        &field); // [手順] - nullptr のフィールド名を指定して struct_meta_descriptor_find_field() を呼び出す。
    int output_ret = struct_meta_descriptor_find_field(
        &kFieldsDescriptor, "first",
        nullptr); // [手順] - nullptr の出力先を指定して struct_meta_descriptor_find_field() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        descriptor_ret); // [確認_異常系] - nullptr の記述子に対する struct_meta_descriptor_find_field() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        name_ret); // [確認_異常系] - nullptr のフィールド名に対する struct_meta_descriptor_find_field() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        output_ret); // [確認_異常系] - nullptr の出力先に対する struct_meta_descriptor_find_field() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
}

// 構造体記述子から指定したキーの属性 (値あり・値なし) を検索できることの確認
TEST(structMetaAccessTest, finds_descriptor_attributes)
{
    // Arrange
    const struct_meta_attribute *version = nullptr;
    const struct_meta_attribute *flag = nullptr;

    // Pre-Assert

    // Act
    int version_ret = struct_meta_descriptor_find_attribute(
        &kDescriptor, "schema.version",
        &version); // [手順] - 値ありの構造体属性を指定して struct_meta_descriptor_find_attribute() を呼び出す。
    int flag_ret = struct_meta_descriptor_find_attribute(
        &kDescriptor, "sample.flag",
        &flag); // [手順] - 値なしの構造体属性を指定して struct_meta_descriptor_find_attribute() を呼び出す。

    // Assert
    ASSERT_EQ(
        CPLAT_OK,
        version_ret); // [確認_正常系] - "schema.version" を検索した struct_meta_descriptor_find_attribute() の戻り値が CPLAT_OK であること。
    ASSERT_NE(nullptr, version);       // [確認_正常系] - version が nullptr 以外であること。
    EXPECT_STREQ("1", version->value); // [確認_正常系] - version->value が "1" であること。
    ASSERT_EQ(
        CPLAT_OK,
        flag_ret); // [確認_正常系] - "sample.flag" を検索した struct_meta_descriptor_find_attribute() の戻り値が CPLAT_OK であること。
    ASSERT_NE(nullptr, flag);        // [確認_正常系] - flag が nullptr 以外であること。
    EXPECT_EQ(nullptr, flag->value); // [確認_正常系] - flag->value が nullptr であること。
}

// 存在しない属性キーを検索した場合に CPLAT_ERR_NOT_FOUND が返り、出力先が NULL になることの確認
TEST(structMetaAccessTest, returns_not_found_for_unknown_key)
{
    // Arrange
    const struct_meta_attribute *attribute = reinterpret_cast<const struct_meta_attribute *>(1);

    // Pre-Assert

    // Act
    int actual_ret = struct_meta_descriptor_find_attribute(
        &kDescriptor, "unknown",
        &attribute); // [手順] - 存在しない構造体属性を指定して struct_meta_descriptor_find_attribute() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        actual_ret); // [確認_正常系] - struct_meta_descriptor_find_attribute() の戻り値が CPLAT_ERR_NOT_FOUND であること。
    EXPECT_EQ(nullptr, attribute); // [確認_正常系] - attribute が nullptr であること。
}

// 属性検索関数に NULL の記述子、キー、出力先を渡した場合に不正引数エラーとなることの確認
TEST(structMetaAccessTest, rejects_invalid_arguments)
{
    // Arrange
    const struct_meta_attribute *attribute = nullptr;

    // Pre-Assert

    // Act
    int descriptor_ret = struct_meta_descriptor_find_attribute(
        nullptr, "schema.version",
        &attribute); // [手順] - nullptr の記述子を指定して struct_meta_descriptor_find_attribute() を呼び出す。
    int key_ret = struct_meta_descriptor_find_attribute(
        &kDescriptor, nullptr,
        &attribute); // [手順] - nullptr の属性キーを指定して struct_meta_descriptor_find_attribute() を呼び出す。
    int output_ret = struct_meta_descriptor_find_attribute(
        &kDescriptor, "schema.version",
        nullptr); // [手順] - nullptr の出力先を指定して struct_meta_descriptor_find_attribute() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        descriptor_ret); // [確認_異常系] - nullptr の記述子に対する struct_meta_descriptor_find_attribute() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        key_ret); // [確認_異常系] - nullptr の属性キーに対する struct_meta_descriptor_find_attribute() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        output_ret); // [確認_異常系] - nullptr の出力先に対する struct_meta_descriptor_find_attribute() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
}

// 重複する属性キーを持つ構造体記述子の検証時に記述子破損エラーとなることの確認
TEST(structMetaAccessTest, rejects_duplicate_descriptor_attributes)
{
    // Arrange
    const struct_meta_attribute duplicate_attributes[] = {{"duplicate", "1"}, {"duplicate", "2"}};
    const struct_meta_descriptor descriptor = {"Sample", sizeof(Sample), nullptr, 0, nullptr, duplicate_attributes, 2};

    // Pre-Assert

    // Act
    int actual_ret = struct_meta_descriptor_validate(
        &descriptor); // [手順] - 重複する構造体属性を持つ記述子を指定して struct_meta_descriptor_validate() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_CORRUPT_DESCRIPTOR,
        actual_ret); // [確認_異常系] - struct_meta_descriptor_validate() の戻り値が CPLAT_ERR_CORRUPT_DESCRIPTOR であること。
}

// フィールドの出力を省略してもパスから値へ到達できることの確認
TEST(structMetaAccessTest, path_resolves_without_field_output)
{
    // Arrange
    Fields instance = {7, 8};
    void *mutable_value = nullptr;
    const void *const_value = nullptr;

    // Pre-Assert

    // Act
    int mutable_ret = struct_meta_path_resolve(&kFieldsDescriptor, &instance, "first", nullptr,
                                               &mutable_value); // [手順] - 変更可能版でフィールド出力を省略する。
    int const_ret = struct_meta_path_resolve_const(&kFieldsDescriptor, &instance, "second", nullptr,
                                                   &const_value); // [手順] - const 版でフィールド出力を省略する。

    // Assert
    EXPECT_EQ(CPLAT_OK, mutable_ret);          // [確認_正常系] - 変更可能版が成功すること。
    EXPECT_EQ(CPLAT_OK, const_ret);            // [確認_正常系] - const 版が成功すること。
    EXPECT_EQ(&instance.first, mutable_value); // [確認_正常系] - 変更可能な値のアドレスが一致すること。
    EXPECT_EQ(&instance.second, const_value);  // [確認_正常系] - const 値のアドレスが一致すること。
}

// float/double の大小両方の幅不一致を直下、配列、ネストで拒否することの確認
TEST(structMetaAccessTest, rejects_mismatched_floating_widths)
{
    // Arrange
    const struct_meta_field_kind kinds[] = {STRUCT_META_FIELD_FLOAT, STRUCT_META_FIELD_DOUBLE};
    const size_t widths[] = {sizeof(float), sizeof(double)};
    unsigned char storage[128];
    memset(storage, 0x5a, sizeof(storage));

    // Pre-Assert

    for (size_t type = 0; type < 2U; type++)
    {
        for (size_t delta = 0; delta < 2U; delta++)
        {
            for (size_t count = 1; count <= 2U; count++)
            {
                for (size_t nested = 0; nested < 2U; nested++)
                {
                    // Arrange
                    const size_t bad_widths[] = {widths[type] - 1U, widths[type] + 1U};
                    const struct_meta_field field = {"value", kinds[type], 0,       0, bad_widths[delta], count, 0,
                                                     nullptr, nullptr,     nullptr, 0};
                    const struct_meta_descriptor child = {"Child", 32, &field, 1, nullptr, nullptr, 0};
                    const struct_meta_field parent_field = {
                        "nested", STRUCT_META_FIELD_STRUCT, 0, 0, 32, 1, 0, &child, nullptr, nullptr, 0};
                    const struct_meta_descriptor descriptors[] = {
                        child, {"Parent", sizeof(storage), &parent_field, 1, nullptr, nullptr, 0}};
                    const struct_meta_descriptor *descriptor = &descriptors[nested];
                    const struct_meta_field *found = nullptr;
                    const void *value = nullptr;
                    void *mutable_value = nullptr;
                    const char *paths[] = {"value[0]", "nested.value[0]"};

                    // Pre-Assert

                    // Act
                    int validation_ret = struct_meta_descriptor_validate(descriptor);
                    int ret = struct_meta_path_resolve_const(descriptor, storage, paths[nested], &found, &value);
                    int mutable_ret =
                        struct_meta_path_resolve(descriptor, storage, paths[nested], &found, &mutable_value);
                    // [手順] - 不正幅の共通検証と const 版、可変版のパス解決を行う (各 16 回)。

                    // Assert
                    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, validation_ret);
                    // [確認_異常系 回数=2*2*2*2] - 共通の検証で幅不一致を拒否すること (16 件)。
                    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, ret);
                    // [確認_異常系 回数=2*2*2*2] - const 版が不正幅を拒否すること (16 件)。
                    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, mutable_ret);
                    // [確認_異常系 回数=2*2*2*2] - 可変版が不正幅を拒否すること (16 件)。
                    EXPECT_EQ(nullptr, value); // [確認_異常系 回数=2*2*2*2] - 値のアドレスを返さないこと (16 件)。
                    EXPECT_EQ(nullptr,
                              mutable_value); // [確認_異常系 回数=2*2*2*2] - 可変値のアドレスを返さないこと (16 件)。
                }
            }
        }
    }
}

// 正しい幅の float と double を処理できることの確認
TEST(structMetaAccessTest, accepts_matching_floating_widths)
{
    // Arrange
    struct FloatingValues
    {
        float single;
        double wide;
    } sample = {1.5F, 2.5};
    const struct_meta_field fields[] = {
        {"single", STRUCT_META_FIELD_FLOAT, 0, offsetof(FloatingValues, single), sizeof(float), 1, 0, nullptr, nullptr,
         nullptr, 0},
        {"wide", STRUCT_META_FIELD_DOUBLE, 0, offsetof(FloatingValues, wide), sizeof(double), 1, 0, nullptr, nullptr,
         nullptr, 0},
    };
    const struct_meta_descriptor descriptor = {"FloatingValues", sizeof(sample), fields, 2, nullptr, nullptr, 0};
    const void *single = nullptr;
    void *wide = nullptr;

    // Pre-Assert

    // Act
    int validation_ret = struct_meta_descriptor_validate(&descriptor); // [手順] - 正しい幅を検証する。
    int single_ret = struct_meta_path_resolve_const(&descriptor, &sample, "single", nullptr, &single);
    // [手順] - const 版で float のアドレスを取得する。
    int wide_ret = struct_meta_path_resolve(&descriptor, &sample, "wide", nullptr, &wide);
    // [手順] - 可変版で double のアドレスを取得する。

    // Assert
    EXPECT_EQ(CPLAT_OK, validation_ret); // [確認_正常系] - 共通検証に成功すること。
    EXPECT_EQ(CPLAT_OK, single_ret);     // [確認_正常系] - float のパス解決に成功すること。
    EXPECT_EQ(CPLAT_OK, wide_ret);       // [確認_正常系] - double のパス解決に成功すること。
    EXPECT_EQ(&sample.single, single);   // [確認_正常系] - float のアドレスが一致すること。
    EXPECT_EQ(&sample.wide, wide);       // [確認_正常系] - double のアドレスが一致すること。
}
