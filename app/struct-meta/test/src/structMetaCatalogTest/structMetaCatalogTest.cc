#include <cplat/base/result.h>
#include <struct_meta/catalog/catalog.h>
#include <testfw.h>

#include <cstring>

namespace
{

/** 解析対象ヘッダーの内容です。ネスト構造体、固定長配列、汎用属性を含みます。 */
const char kHeader[] = "/**\n"
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

// C 言語ヘッダー テキストの解析により構造体記述子カタログが正しく構築されることの確認
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
    int ret = create(kHeader, &catalog, &diagnostic); // [手順] - ヘッダー内容 kHeader を指定して create() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret);             // [確認_正常系] - create() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("", diagnostic.message); // [確認_正常系] - diagnostic.message が "" であること。
    ASSERT_EQ(CPLAT_OK,
              struct_meta_catalog_get_count(
                  catalog, &count)); // [確認_正常系] - struct_meta_catalog_get_count() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(2U, count);            // [確認_正常系] - count が 2U であること。
    ASSERT_EQ(
        CPLAT_OK,
        struct_meta_catalog_get(
            catalog, 0,
            &address_descriptor)); // [確認_正常系] - 0 番目に対する struct_meta_catalog_get() の戻り値が CPLAT_OK であること。
    ASSERT_EQ(
        CPLAT_OK,
        struct_meta_catalog_get(
            catalog, 1,
            &person_descriptor)); // [確認_正常系] - 1 番目に対する struct_meta_catalog_get() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("address",
                 address_descriptor->name); // [確認_正常系] - address_descriptor->name が "address" であること。
    EXPECT_STREQ("person", person_descriptor->name); // [確認_正常系] - person_descriptor->name が "person" であること。

    struct_meta_catalog_destroy(catalog);
}

// カタログ生成による構造体・メンバーのレイアウト計算結果がコンパイラの実装と一致することの確認
TEST(structMetaCatalogTest, computes_the_same_layout_as_the_compiler)
{
    // Arrange
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic diagnostic = {};
    const struct_meta_descriptor *person_descriptor = nullptr;
    ASSERT_EQ(CPLAT_OK,
              create(kHeader, &catalog, &diagnostic)); // [状態確認] - create() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    // Act
    int ret = struct_meta_catalog_find(
        catalog, "person",
        &person_descriptor); // [手順] - 構造体名 "person" を指定して struct_meta_catalog_find() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret); // [確認_正常系] - struct_meta_catalog_find() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(
        sizeof(person_probe),
        person_descriptor->size); // [確認_正常系] - person_descriptor->size が sizeof(person_probe) と一致すること。
    EXPECT_EQ(5U, person_descriptor->field_count); // [確認_正常系] - person_descriptor->field_count が 5U であること。
    EXPECT_EQ(offsetof(person_probe, age), person_descriptor->fields[0].offset);
    // [確認_正常系] - person_descriptor->fields[0].offset が offsetof(person_probe, age) と一致すること。
    EXPECT_EQ(offsetof(person_probe, id), person_descriptor->fields[1].offset);
    // [確認_正常系] - person_descriptor->fields[1].offset が offsetof(person_probe, id) と一致すること。
    EXPECT_EQ(offsetof(person_probe, weight), person_descriptor->fields[2].offset);
    // [確認_正常系] - person_descriptor->fields[2].offset が offsetof(person_probe, weight) と一致すること。
    EXPECT_EQ(
        offsetof(person_probe, addresses),
        person_descriptor->fields[3]
            .offset); // [確認_正常系] - person_descriptor->fields[3].offset が offsetof(person_probe, addresses) と一致すること。(ネスト配列がネストの境界へ揃うことの確認)
    EXPECT_EQ(offsetof(person_probe, token), person_descriptor->fields[4].offset);
    // [確認_正常系] - person_descriptor->fields[4].offset が offsetof(person_probe, token) と一致すること。

    struct_meta_catalog_destroy(catalog);
}

// フィールドの型種別分類およびメタ属性・ドキュメント情報が正しく保持されることの確認
TEST(structMetaCatalogTest, classifies_fields_and_keeps_attributes)
{
    // Arrange
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic diagnostic = {};
    const struct_meta_descriptor *address_descriptor = nullptr;
    const struct_meta_descriptor *person_descriptor = nullptr;
    ASSERT_EQ(CPLAT_OK,
              create(kHeader, &catalog, &diagnostic)); // [状態確認] - create() の戻り値が CPLAT_OK であること。
    ASSERT_EQ(
        CPLAT_OK,
        struct_meta_catalog_find(
            catalog, "address",
            &address_descriptor)); // [状態確認] - "address" を探した struct_meta_catalog_find() の戻り値が CPLAT_OK であること。
    ASSERT_EQ(
        CPLAT_OK,
        struct_meta_catalog_find(
            catalog, "person",
            &person_descriptor)); // [状態確認] - "person" を探した struct_meta_catalog_find() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    // Act
    const struct_meta_field *city = &address_descriptor->fields[0];     // [手順] - char 配列を取り出す。
    const struct_meta_field *age = &person_descriptor->fields[0];       // [手順] - short を取り出す。
    const struct_meta_field *addresses = &person_descriptor->fields[3]; // [手順] - ネスト配列を取り出す。
    const struct_meta_field *token = &person_descriptor->fields[4];     // [手順] - バイト配列を取り出す。

    // Assert
    EXPECT_EQ(STRUCT_META_FIELD_CHAR_ARRAY,
              city->kind);                  // [確認_正常系] - city->kind が STRUCT_META_FIELD_CHAR_ARRAY であること。
    EXPECT_EQ(1U, city->element_count);     // [確認_正常系] - city->element_count が 1U であること。
    EXPECT_EQ(32U, city->char_buffer_size); // [確認_正常系] - city->char_buffer_size が 32U であること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER,
              age->kind);             // [確認_正常系] - age->kind が STRUCT_META_FIELD_SIGNED_INTEGER であること。
    EXPECT_EQ(2U, age->element_size); // [確認_正常系] - age->element_size が 2U であること。
    EXPECT_EQ(STRUCT_META_FIELD_STRUCT,
              addresses->kind);              // [確認_正常系] - addresses->kind が STRUCT_META_FIELD_STRUCT であること。
    EXPECT_EQ(2U, addresses->element_count); // [確認_正常系] - addresses->element_count が 2U であること。
    EXPECT_EQ(address_descriptor,
              addresses->nested); // [確認_正常系] - addresses->nested が address_descriptor と一致すること。
    EXPECT_EQ(STRUCT_META_FIELD_UNSIGNED_INTEGER,
              token->kind); // [確認_正常系] - token->kind が STRUCT_META_FIELD_UNSIGNED_INTEGER であること。
    EXPECT_EQ(4U, token->element_count);  // [確認_正常系] - token->element_count が 4U であること。
    ASSERT_EQ(1U, city->attribute_count); // [確認_正常系] - city->attribute_count が 1U であること。
    EXPECT_STREQ("json.name",
                 city->attributes[0].key); // [確認_正常系] - city->attributes[0].key が "json.name" であること。
    EXPECT_STREQ("locality",
                 city->attributes[0].value); // [確認_正常系] - city->attributes[0].value が "locality" であること。
    ASSERT_EQ(
        1U,
        address_descriptor->attribute_count); // [確認_正常系] - address_descriptor->attribute_count が 1U であること。
    EXPECT_STREQ("sample.category",
                 address_descriptor->attributes[0]
                     .key); // [確認_正常系] - address_descriptor->attributes[0].key が "sample.category" であること。
    EXPECT_STREQ("住所です。",
                 address_descriptor->brief); // [確認_正常系] - address_descriptor->brief が "住所です。" であること。

    struct_meta_catalog_destroy(catalog);
}

// 不正な宣言や未知の型を含むヘッダーが適切な診断メッセージとともに拒否されることの確認
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
    int long_ret = create(kLongMember, &catalog,
                          &platform_dependent); // [手順] - long を含むヘッダーを指定して create() を呼び出す。
    int pointer_ret = create(kPointerMember, &catalog,
                             &pointer_member); // [手順] - ポインターを含むヘッダーを指定して create() を呼び出す。
    int unknown_ret = create(kUnknownType, &catalog,
                             &unknown_type); // [手順] - 未知の型を含むヘッダーを指定して create() を呼び出す。
    int attribute_ret = create(kBadAttribute, &catalog,
                               &bad_attribute); // [手順] - 不正な属性を含むヘッダーを指定して create() を呼び出す。

    // Assert
    EXPECT_NE(CPLAT_OK,
              long_ret); // [確認_異常系] - long を含むヘッダーに対する create() の戻り値が CPLAT_OK 以外であること。
    EXPECT_STRNE("", platform_dependent.message); // [確認_異常系] - platform_dependent.message が "" 以外であること。
    EXPECT_NE(
        CPLAT_OK,
        pointer_ret); // [確認_異常系] - ポインターを含むヘッダーに対する create() の戻り値が CPLAT_OK 以外であること。
    EXPECT_STRNE("", pointer_member.message); // [確認_異常系] - pointer_member.message が "" 以外であること。
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        unknown_ret); // [確認_異常系] - 未知の型を含むヘッダーに対する create() の戻り値が CPLAT_ERR_NOT_FOUND であること。
    EXPECT_STRNE("", unknown_type.message); // [確認_異常系] - unknown_type.message が "" 以外であること。
    EXPECT_EQ(
        CPLAT_ERR_CORRUPT_DESCRIPTOR,
        attribute_ret); // [確認_異常系] - 不正な属性を含むヘッダーに対する create() の戻り値が CPLAT_ERR_CORRUPT_DESCRIPTOR であること。
    EXPECT_STRNE("", bad_attribute.message); // [確認_異常系] - bad_attribute.message が "" 以外であること。
}

// 同一プロセス内で複数回の解析および失敗後の再解析が正常に継続できることの確認
TEST(structMetaCatalogTest, parses_repeatedly_in_the_same_process)
{
    // Arrange
    struct_meta_catalog *first = nullptr;
    struct_meta_catalog *second = nullptr;
    struct_meta_diagnostic diagnostic = {};
    const struct_meta_descriptor *descriptor = nullptr;

    // Pre-Assert

    // Act
    int first_ret = create(kHeader, &first,
                           &diagnostic); // [手順] - ヘッダー内容 kHeader を指定して、1 回目の create() を呼び出す。
    int failed_ret = create("typedef struct s { long v; } s;\n", &second,
                            &diagnostic); // [手順] - 失敗するヘッダーを指定して create() を呼び出す。
    struct_meta_catalog *third = nullptr;
    int third_ret = create(kHeader, &third,
                           &diagnostic); // [手順] - ヘッダー内容 kHeader を指定して、3 回目の create() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_OK, first_ret); // [確認_正常系] - 1 回目の create() の戻り値が CPLAT_OK であること。
    EXPECT_NE(
        CPLAT_OK,
        failed_ret); // [確認_異常系] - 失敗する解析に対する create() の戻り値が CPLAT_OK 以外であること。(失敗がプロセスを終了させないことの確認)
    EXPECT_EQ(
        CPLAT_OK,
        third_ret); // [確認_正常系] - 3 回目の create() の戻り値が CPLAT_OK であること。(失敗の後も解析を続けられることの確認)
    EXPECT_EQ(
        CPLAT_OK,
        struct_meta_catalog_find(
            third, "person",
            &descriptor)); // [確認_正常系] - "person" を探した struct_meta_catalog_find() の戻り値が CPLAT_OK であること。

    struct_meta_catalog_destroy(first);
    struct_meta_catalog_destroy(third);
}

// カタログ API の各関数で不正な引数や範囲外アクセスが適切に拒否されることの確認
TEST(structMetaCatalogTest, rejects_invalid_arguments)
{
    // Arrange
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic diagnostic = {};
    size_t count = 0;
    const struct_meta_descriptor *descriptor = nullptr;
    ASSERT_EQ(CPLAT_OK,
              create(kHeader, &catalog, &diagnostic)); // [状態確認] - create() の戻り値が CPLAT_OK であること。

    // Pre-Assert

    // Act
    int null_path = struct_meta_catalog_create_from_header_file(
        nullptr, &catalog,
        &diagnostic); // [手順] - パスに nullptr を指定して struct_meta_catalog_create_from_header_file() を呼び出す。
    int missing_file = struct_meta_catalog_create_from_header_file(
        "does_not_exist.h", &catalog,
        &diagnostic); // [手順] - 存在しないファイル パスを指定して struct_meta_catalog_create_from_header_file() を呼び出す。
    int null_count = struct_meta_catalog_get_count(
        nullptr, &count); // [手順] - カタログに nullptr を指定して struct_meta_catalog_get_count() を呼び出す。
    int out_of_range = struct_meta_catalog_get(
        catalog, 99, &descriptor); // [手順] - 範囲外のインデックス 99 を指定して struct_meta_catalog_get() を呼び出す。
    int null_name = struct_meta_catalog_find(
        catalog, nullptr, &descriptor); // [手順] - 名前に nullptr を指定して struct_meta_catalog_find() を呼び出す。
    int unknown_name = struct_meta_catalog_find(
        catalog, "missing",
        &descriptor); // [手順] - 未登録の名前 "missing" を指定して struct_meta_catalog_find() を呼び出す。
    struct_meta_catalog_destroy(nullptr); // [手順] - nullptr を指定して struct_meta_catalog_destroy() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        null_path); // [確認_異常系] - nullptr のパスに対する struct_meta_catalog_create_from_header_file() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        missing_file); // [確認_異常系] - 存在しないファイル パスに対する struct_meta_catalog_create_from_header_file() の戻り値が CPLAT_ERR_NOT_FOUND であること。
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        null_count); // [確認_異常系] - nullptr のカタログに対する struct_meta_catalog_get_count() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        out_of_range); // [確認_異常系] - 範囲外のインデックスに対する struct_meta_catalog_get() の戻り値が CPLAT_ERR_NOT_FOUND であること。
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        null_name); // [確認_異常系] - nullptr の名前に対する struct_meta_catalog_find() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        unknown_name); // [確認_異常系] - 未登録の名前に対する struct_meta_catalog_find() の戻り値が CPLAT_ERR_NOT_FOUND であること。

    struct_meta_catalog_destroy(catalog);
}
