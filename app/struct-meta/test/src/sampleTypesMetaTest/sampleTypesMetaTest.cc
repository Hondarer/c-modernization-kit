#include <cplat/base/result.h>
#include <sample_types_meta.h>
#include <struct_meta/access/access.h>
#include <testfw.h>

// 埋め込み索引を通じてカタログ内の型名から記述子を検索でき、未知・空の型名にはNULLを返すことの確認
TEST(sampleTypesMetaTest, finds_descriptor_through_embedded_index)
{
    // Arrange
    /* 生成カタログには線形走査の経路が無い。埋め込みイメージへ接続できなければ
       attach_index() が abort() するため、検索が成立すること自体が
       「索引へ接続できた」ことの表明になる。 */

    // Pre-Assert
    ASSERT_EQ(3U, sample_types_meta_count()); // [状態確認] - 生成カタログが 3 型を持つこと。

    // Act
    const struct_meta_descriptor *person = sample_types_meta_find("person");   // [手順] - 型名で記述子を検索する。
    const struct_meta_descriptor *address = sample_types_meta_find("address"); // [手順] - もう 1 つの型名を検索する。
    const struct_meta_descriptor *unknown = sample_types_meta_find("unknown"); // [手順] - 未知の型名を検索する。
    const struct_meta_descriptor *empty = sample_types_meta_find("");          // [手順] - 空の型名を検索する。

    // Assert
    ASSERT_NE(nullptr, person);             // [確認_正常系] - person の記述子が引けること。
    EXPECT_STREQ("person", person->name);   // [確認_正常系] - 引いた記述子が person であること。
    ASSERT_NE(nullptr, address);            // [確認_正常系] - address の記述子が引けること。
    EXPECT_STREQ("address", address->name); // [確認_正常系] - 引いた記述子が address であること。
    EXPECT_EQ(nullptr, unknown);            // [確認_正常系] - 未知の型名で NULL が返ること。
    EXPECT_EQ(nullptr, empty);              // [確認_正常系] - 空の型名で NULL が返ること。
}

// カタログ内のすべての記述子を宣言順のIDで列挙取得できることの確認
TEST(sampleTypesMetaTest, enumerates_all_descriptors_in_catalog_order)
{
    // Arrange
    const char *expected_names[] = {"address", "byte_fields", "person"};

    // Pre-Assert
    ASSERT_EQ(3U, sample_types_meta_count()); // [状態確認] - 生成カタログが 3 型を持つこと。

    // Act
    const struct_meta_descriptor *actual[3] = {};
    for (size_t i = 0; i < sample_types_meta_count(); i++)
    {
        actual[i] = sample_types_meta_get(static_cast<sample_types_meta_id>(i)); // [手順] - ID 順に記述子を取得する。
    }

    // Assert
    for (size_t i = 0; i < 3U; i++)
    {
        ASSERT_NE(nullptr, actual[i]); // [確認_正常系 回数=3] - カタログの各 ID から記述子を取得できること。
        EXPECT_STREQ(expected_names[i], actual[i]->name); // [確認_正常系 回数=3] - カタログの宣言順に名称が並ぶこと。
    }
}

// 文字型、バイト型、整数型および配列フィールドの種別と要素数が正しく分類されることの確認
TEST(sampleTypesMetaTest, classifies_character_and_byte_fields)
{
    // Arrange
    const struct_meta_descriptor *descriptor = sample_types_meta_find("byte_fields");

    // Pre-Assert
    ASSERT_NE(nullptr, descriptor); // [状態確認] - 生成した確認用記述子が存在すること。

    // Act
    const struct_meta_field *character = nullptr;
    const struct_meta_field *signed_character = nullptr;
    const struct_meta_field *unsigned_character = nullptr;
    const struct_meta_field *fixed_signed = nullptr;
    const struct_meta_field *fixed_unsigned = nullptr;
    const struct_meta_field *text = nullptr;
    const struct_meta_field *raw_chars = nullptr;
    const struct_meta_field *signed_bytes = nullptr;
    const struct_meta_field *unsigned_bytes = nullptr;
    const struct_meta_field *fixed_signed_bytes = nullptr;
    const struct_meta_field *fixed_unsigned_bytes = nullptr;
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "character", &character));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "character", &character)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "signed_character", &signed_character));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "signed_character", &signed_character)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "unsigned_character", &unsigned_character));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "unsigned_character", &unsigned_character)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "fixed_signed", &fixed_signed));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "fixed_signed", &fixed_signed)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "fixed_unsigned", &fixed_unsigned));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "fixed_unsigned", &fixed_unsigned)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "text", &text));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "text", &text)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "raw_chars", &raw_chars));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "raw_chars", &raw_chars)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "signed_bytes", &signed_bytes));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "signed_bytes", &signed_bytes)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "unsigned_bytes", &unsigned_bytes));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "unsigned_bytes", &unsigned_bytes)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "fixed_signed_bytes", &fixed_signed_bytes));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "fixed_signed_bytes", &fixed_signed_bytes)` の戻り値が `CPLAT_OK` であること。
    ASSERT_EQ(CPLAT_OK, struct_meta_descriptor_find_field(descriptor, "fixed_unsigned_bytes", &fixed_unsigned_bytes));
    // [確認_正常系] - `struct_meta_descriptor_find_field(descriptor, "fixed_unsigned_bytes", &fixed_unsigned_bytes)` の戻り値が `CPLAT_OK` であること。

    // Assert
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER,
              character->kind); // [確認_正常系] - plain char が符号付き整数であること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER,
              signed_character->kind); // [確認_正常系] - signed char が符号付き整数であること。
    EXPECT_EQ(STRUCT_META_FIELD_UNSIGNED_INTEGER,
              unsigned_character->kind); // [確認_正常系] - unsigned char が符号なし整数であること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER,
              fixed_signed->kind); // [確認_正常系] - int8_t が符号付き整数であること。
    EXPECT_EQ(STRUCT_META_FIELD_UNSIGNED_INTEGER,
              fixed_unsigned->kind);                     // [確認_正常系] - uint8_t が符号なし整数であること。
    EXPECT_EQ(STRUCT_META_FIELD_CHAR_ARRAY, text->kind); // [確認_正常系] - char 配列の既定が文字列であること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER,
              raw_chars->kind);              // [確認_正常系] - 属性付き char 配列が符号付きバイト配列であること。
    EXPECT_EQ(3U, raw_chars->element_count); // [確認_正常系] - char 配列の全要素が保持されること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER,
              signed_bytes->kind); // [確認_正常系] - signed char 配列が符号付きであること。
    EXPECT_EQ(STRUCT_META_FIELD_UNSIGNED_INTEGER,
              unsigned_bytes->kind); // [確認_正常系] - unsigned char 配列が符号なしであること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER,
              fixed_signed_bytes->kind); // [確認_正常系] - int8_t 配列が符号付きであること。
    EXPECT_EQ(STRUCT_META_FIELD_UNSIGNED_INTEGER,
              fixed_unsigned_bytes->kind); // [確認_正常系] - uint8_t 配列が符号なしであること。
}

// 型名に NULL を渡して検索した場合に NULL が返ることの確認
TEST(sampleTypesMetaTest, rejects_null_name)
{
    // Arrange

    // Pre-Assert

    // Act
    const struct_meta_descriptor *actual = sample_types_meta_find(nullptr); // [手順] - NULL の型名を検索する。

    // Assert
    EXPECT_EQ(nullptr, actual); // [確認_異常系] - NULL の型名で NULL が返ること。
}

// 索引から取得した記述子からフィールド情報を正しく検索・取得できることの確認
TEST(sampleTypesMetaTest, registers_descriptors_into_index)
{
    // Arrange
    const struct_meta_field *field = nullptr;
    const struct_meta_descriptor *person = sample_types_meta_find("person");

    // Pre-Assert
    ASSERT_NE(nullptr, person); // [状態確認] - person の記述子が引けること。

    // Act
    /* 検索は索引の有無に関わらず成立するため、ここでは結果の同一性だけを確かめる。 */
    int actual = struct_meta_descriptor_find_field(person, "balance",
                                                   &field); // [手順] - 登録済み記述子のフィールドを検索する。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual);                              // [確認_正常系] - フィールド検索が成功すること。
    EXPECT_STREQ("balance", field->name);                     // [確認_正常系] - 要求したフィールドが返ること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER, field->kind); // [確認_正常系] - 符号付き整数と判定されること。
    EXPECT_EQ(sizeof(int64_t), field->element_size);          // [確認_正常系] - 幅が 8 バイトであること。
}
