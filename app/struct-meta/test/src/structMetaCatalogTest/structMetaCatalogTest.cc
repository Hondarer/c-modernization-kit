#include <cplat/base/result.h>
#include <struct_meta/catalog/catalog.h>
#include <testfw.h>

#include <cstring>

namespace
{

/** 解析対象ヘッダーの内容です。ネスト構造体、固定長配列、汎用属性を含みます。 */
const char kHeader[] =
    "/**\n"
    " * @brief 住所です。\n"
    " * @struct_meta{sample.category=location}\n"
    " */\n"
    "typedef struct address\n"
    "{\n"
    "    char city[32]; /**< 都市名です。 @struct_meta{json.name=locality} */\n"
    "    int zip;       /**< 郵便番号です。 */\n"
    "} address;\n"
    "\n"
    "/**\n"
    " * @brief 利用者です。\n"
    " */\n"
    "typedef struct person\n"
    "{\n"
    "    short age;             /**< 年齢です。 */\n"
    "    int id;                /**< 識別子です。 */\n"
    "    double weight;         /**< 体重です。 */\n"
    "    address addresses[2];  /**< 住所です。 */\n"
    "    uint8_t token[4];      /**< トークンです。 @struct_meta{meta.format=hex} */\n"
    "} person;\n";

/** 実環境の person と同じ宣言です。レイアウトの照合に使用します。 */
struct address_probe
{
    char city[32];
    int zip;
};

struct person_probe
{
    short age;
    char pad1[2]; /**< 明示的アラインメントです。0 を指定します。 */
    int id;
    double weight;
    address_probe addresses[2];
    unsigned char token[4];
    char pad2[4]; /**< 明示的アラインメントです。0 を指定します。 */
};

/**
 *  @brief          テキストからカタログを作ります。
 */
int create(const char *text, struct_meta_catalog **catalog_out, struct_meta_diagnostic *diagnostic_out)
{
    return struct_meta_catalog_create_from_header_text(text, strlen(text), catalog_out, diagnostic_out);
}

} // namespace

TEST(structMetaCatalogTest, builds_descriptors_from_header_text)
{
    // Arrange
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic diagnostic = {};
    size_t count = 0;
    const struct_meta_descriptor *address_descriptor = nullptr;
    const struct_meta_descriptor *person_descriptor = nullptr;

    // Pre-Assert

    // Act
    int ret = create(kHeader, &catalog, &diagnostic); // [手順] - ヘッダー内容を実行時に解析する。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret);                    // [確認_正常系] - 解析が成功すること。
    EXPECT_STREQ("", diagnostic.message);        // [確認_正常系] - 診断が残らないこと。
    ASSERT_EQ(CPLAT_OK, struct_meta_catalog_get_count(catalog, &count));
    EXPECT_EQ(2U, count);                        // [確認_正常系] - 宣言した 2 個の構造体を持つこと。
    ASSERT_EQ(CPLAT_OK, struct_meta_catalog_get(catalog, 0, &address_descriptor));
    ASSERT_EQ(CPLAT_OK, struct_meta_catalog_get(catalog, 1, &person_descriptor));
    EXPECT_STREQ("address", address_descriptor->name); // [確認_正常系] - 並びが宣言順であること。
    EXPECT_STREQ("person", person_descriptor->name);   // [確認_正常系] - 並びが宣言順であること。

    struct_meta_catalog_destroy(catalog);
}

TEST(structMetaCatalogTest, computes_the_same_layout_as_the_compiler)
{
    // Arrange
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic diagnostic = {};
    const struct_meta_descriptor *person_descriptor = nullptr;
    ASSERT_EQ(CPLAT_OK, create(kHeader, &catalog, &diagnostic));

    // Pre-Assert

    // Act
    int ret = struct_meta_catalog_find(catalog, "person",
                                       &person_descriptor); // [手順] - 構造体名で記述子を検索する。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret);                                   // [確認_正常系] - 検索できること。
    EXPECT_EQ(sizeof(person_probe), person_descriptor->size);   // [確認_正常系] - 全体の大きさが一致すること。
    EXPECT_EQ(5U, person_descriptor->field_count);              // [確認_正常系] - フィールド数が一致すること。
    EXPECT_EQ(offsetof(person_probe, age), person_descriptor->fields[0].offset);
    EXPECT_EQ(offsetof(person_probe, id), person_descriptor->fields[1].offset);
    EXPECT_EQ(offsetof(person_probe, weight), person_descriptor->fields[2].offset);
    EXPECT_EQ(offsetof(person_probe, addresses),
              person_descriptor->fields[3].offset); // [確認_正常系] - ネスト配列がネストの境界へ揃うこと。
    EXPECT_EQ(offsetof(person_probe, token), person_descriptor->fields[4].offset);

    struct_meta_catalog_destroy(catalog);
}

TEST(structMetaCatalogTest, classifies_fields_and_keeps_attributes)
{
    // Arrange
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic diagnostic = {};
    const struct_meta_descriptor *address_descriptor = nullptr;
    const struct_meta_descriptor *person_descriptor = nullptr;
    ASSERT_EQ(CPLAT_OK, create(kHeader, &catalog, &diagnostic));
    ASSERT_EQ(CPLAT_OK, struct_meta_catalog_find(catalog, "address", &address_descriptor));
    ASSERT_EQ(CPLAT_OK, struct_meta_catalog_find(catalog, "person", &person_descriptor));

    // Pre-Assert

    // Act
    const struct_meta_field *city = &address_descriptor->fields[0];        // [手順] - char 配列を取り出す。
    const struct_meta_field *age = &person_descriptor->fields[0];          // [手順] - short を取り出す。
    const struct_meta_field *addresses = &person_descriptor->fields[3];    // [手順] - ネスト配列を取り出す。
    const struct_meta_field *token = &person_descriptor->fields[4];        // [手順] - バイト配列を取り出す。

    // Assert
    EXPECT_EQ(STRUCT_META_FIELD_CHAR_ARRAY, city->kind); // [確認_正常系] - char 配列を文字列として扱うこと。
    EXPECT_EQ(1U, city->element_count);                  // [確認_正常系] - 文字列は要素数 1 であること。
    EXPECT_EQ(32U, city->char_buffer_size);              // [確認_正常系] - 全体のバイト数を保持すること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER, age->kind); // [確認_正常系] - short が符号付き整数であること。
    EXPECT_EQ(2U, age->element_size);                       // [確認_正常系] - short が 2 バイトであること。
    EXPECT_EQ(STRUCT_META_FIELD_STRUCT, addresses->kind);   // [確認_正常系] - ネスト構造体であること。
    EXPECT_EQ(2U, addresses->element_count);                // [確認_正常系] - 要素数が宣言どおりであること。
    EXPECT_EQ(address_descriptor, addresses->nested);       // [確認_正常系] - ネスト先が張られること。
    EXPECT_EQ(STRUCT_META_FIELD_UNSIGNED_INTEGER, token->kind); // [確認_正常系] - バイト配列を符号なし整数とすること。
    EXPECT_EQ(4U, token->element_count);                        // [確認_正常系] - 要素数が宣言どおりであること。
    ASSERT_EQ(1U, city->attribute_count);                       // [確認_正常系] - 属性を保持すること。
    EXPECT_STREQ("json.name", city->attributes[0].key);         // [確認_正常系] - 属性名を保持すること。
    EXPECT_STREQ("locality", city->attributes[0].value);        // [確認_正常系] - 属性値を保持すること。
    ASSERT_EQ(1U, address_descriptor->attribute_count);         // [確認_正常系] - 構造体の属性を保持すること。
    EXPECT_STREQ("sample.category", address_descriptor->attributes[0].key);
    EXPECT_STREQ("住所です。", address_descriptor->brief); // [確認_正常系] - brief を保持すること。

    struct_meta_catalog_destroy(catalog);
}

TEST(structMetaCatalogTest, rejects_invalid_headers_with_diagnostics)
{
    // Arrange
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic platform_dependent = {};
    struct_meta_diagnostic pointer_member = {};
    struct_meta_diagnostic unknown_type = {};
    struct_meta_diagnostic bad_attribute = {};
    const char kLongMember[] = "typedef struct s { long value; } s;\n";
    const char kPointerMember[] = "typedef struct s { int *value; } s;\n";
    const char kUnknownType[] = "typedef struct s { missing_type value; } s;\n";
    const char kBadAttribute[] = "typedef struct s { int value; /**< @struct_meta{meta.format=hex} */ } s;\n";

    // Pre-Assert

    // Act
    int long_ret = create(kLongMember, &catalog, &platform_dependent);      // [手順] - long を含むヘッダーを解析する。
    int pointer_ret = create(kPointerMember, &catalog, &pointer_member);    // [手順] - ポインターを含むヘッダーを解析する。
    int unknown_ret = create(kUnknownType, &catalog, &unknown_type);        // [手順] - 未知の型を含むヘッダーを解析する。
    int attribute_ret = create(kBadAttribute, &catalog, &bad_attribute);    // [手順] - 不正な属性を含むヘッダーを解析する。

    // Assert
    EXPECT_NE(CPLAT_OK, long_ret);     // [確認_異常系] - long を拒否すること。
    EXPECT_STRNE("", platform_dependent.message); // [確認_異常系] - 診断が残ること。
    EXPECT_NE(CPLAT_OK, pointer_ret);  // [確認_異常系] - ポインター メンバーを拒否すること。
    EXPECT_STRNE("", pointer_member.message);     // [確認_異常系] - 診断が残ること。
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, unknown_ret);  // [確認_異常系] - 未知の型を拒否すること。
    EXPECT_STRNE("", unknown_type.message);       // [確認_異常系] - 診断が残ること。
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, attribute_ret); // [確認_異常系] - 不正な属性を拒否すること。
    EXPECT_STRNE("", bad_attribute.message);                // [確認_異常系] - 診断が残ること。
}

TEST(structMetaCatalogTest, parses_repeatedly_in_the_same_process)
{
    // Arrange
    struct_meta_catalog *first = nullptr;
    struct_meta_catalog *second = nullptr;
    struct_meta_diagnostic diagnostic = {};
    const struct_meta_descriptor *descriptor = nullptr;

    // Pre-Assert

    // Act
    int first_ret = create(kHeader, &first, &diagnostic);   // [手順] - 1 回目の解析を行う。
    int failed_ret = create("typedef struct s { long v; } s;\n", &second,
                            &diagnostic);                  // [手順] - 失敗する解析を挟む。
    struct_meta_catalog *third = nullptr;
    int third_ret = create(kHeader, &third, &diagnostic);   // [手順] - 3 回目の解析を行う。

    // Assert
    EXPECT_EQ(CPLAT_OK, first_ret);    // [確認_正常系] - 1 回目が成功すること。
    EXPECT_NE(CPLAT_OK, failed_ret);   // [確認_異常系] - 失敗がプロセスを終了させないこと。
    EXPECT_EQ(CPLAT_OK, third_ret);    // [確認_正常系] - 失敗の後も解析を続けられること。
    EXPECT_EQ(CPLAT_OK, struct_meta_catalog_find(third, "person", &descriptor));

    struct_meta_catalog_destroy(first);
    struct_meta_catalog_destroy(third);
}

TEST(structMetaCatalogTest, rejects_invalid_arguments)
{
    // Arrange
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic diagnostic = {};
    size_t count = 0;
    const struct_meta_descriptor *descriptor = nullptr;
    ASSERT_EQ(CPLAT_OK, create(kHeader, &catalog, &diagnostic));

    // Pre-Assert

    // Act
    int null_path = struct_meta_catalog_create_from_header_file(nullptr, &catalog, &diagnostic);
    int missing_file = struct_meta_catalog_create_from_header_file("does_not_exist.h", &catalog, &diagnostic);
    int null_count = struct_meta_catalog_get_count(nullptr, &count);
    int out_of_range = struct_meta_catalog_get(catalog, 99, &descriptor);
    int null_name = struct_meta_catalog_find(catalog, nullptr, &descriptor);
    int unknown_name = struct_meta_catalog_find(catalog, "missing", &descriptor);
    struct_meta_catalog_destroy(nullptr); // [手順] - NULL の破棄で異常終了しないこと。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, null_path);   // [確認_異常系] - パスの NULL を拒否すること。
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, missing_file);       // [確認_異常系] - 開けないヘッダーを拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, null_count);  // [確認_異常系] - カタログの NULL を拒否すること。
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, out_of_range);       // [確認_異常系] - 範囲外のインデックスを拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, null_name);   // [確認_異常系] - 名前の NULL を拒否すること。
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, unknown_name);       // [確認_異常系] - 未知の名前を拒否すること。

    struct_meta_catalog_destroy(catalog);
}
