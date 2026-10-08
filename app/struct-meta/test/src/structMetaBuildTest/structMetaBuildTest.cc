#include <cplat/base/result.h>
#include <struct_meta/catalog/arena.h>
#include <struct_meta/catalog/build.h>
#include <struct_meta/parse/ast.h>
#include <testfw.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace
{

/**
 *  @brief          解析結果と同じ形の AST を、手作業で組み立てるための補助です。
 *
 *  構文解析器を通さずに記述子の組み立てだけを確かめます。AST の各要素は
 *  `struct_meta_internal_parse_*_create` と同じく確保した文字列を所有するため、
 *  ここでも複製して渡します。
 */
char *dup(const char *text)
{
    if (text == nullptr)
    {
        return nullptr;
    }
    const size_t length = strlen(text) + 1U;
    char *copy = static_cast<char *>(malloc(length));
    memcpy(copy, text, length);
    return copy;
}

struct_meta_internal_parse_attribute *make_attribute(const char *key, const char *value,
                                                     struct_meta_internal_parse_attribute *next)
{
    auto *attribute =
        static_cast<struct_meta_internal_parse_attribute *>(calloc(1, sizeof(struct_meta_internal_parse_attribute)));
    attribute->key = dup(key);
    attribute->value = dup(value);
    attribute->next = next;
    return attribute;
}

struct_meta_internal_parse_field *make_field(const char *name, const char *type_name, int is_struct_type,
                                             long array_count, struct_meta_internal_parse_attribute *attributes)
{
    return struct_meta_internal_parse_field_create(dup(name), dup(type_name), is_struct_type, array_count, 1, nullptr,
                                                   attributes);
}

/**
 *  @brief          フィールドの配列から構造体 1 個分の AST を作り、リストへ足します。
 */
void append_struct(struct_meta_internal_parse_struct_list **list, const char *name,
                   struct_meta_internal_parse_field *const *fields, size_t field_count,
                   struct_meta_internal_parse_attribute *attributes)
{
    struct_meta_internal_parse_field_list *field_list = struct_meta_internal_parse_field_list_create(fields[0]);
    for (size_t i = 1; i < field_count; i++)
    {
        (void)struct_meta_internal_parse_field_list_append(field_list, fields[i]);
    }
    struct_meta_internal_parse_struct_list_append(
        list, struct_meta_internal_parse_struct_create(dup(name), field_list, nullptr, 1, attributes));
}

/** 実環境の宣言です。レイアウトの照合に使用します。 */
struct inner_probe
{
    char label[8];
    int value;
};

struct outer_probe
{
    unsigned char flag;
    char pad1[3]; /**< 明示的アラインメントです。0 を指定します。 */
    inner_probe inner[2];
    char pad2[4]; /**< 明示的アラインメントです。0 を指定します。 */
    double ratio;
};

/**
 *  @brief          テスト全体で使う 2 段構造の AST を作ります。
 */
struct_meta_internal_parse_struct_list *make_nested_ast()
{
    struct_meta_internal_parse_struct_list *list = nullptr;

    struct_meta_internal_parse_field *inner_fields[] = {
        make_field("label", "char", 0, 8, make_attribute("json.name", "text", nullptr)),
        make_field("value", "int", 0, 0, nullptr),
    };
    append_struct(&list, "inner_probe", inner_fields, 2, make_attribute("sample.category", "inner", nullptr));

    struct_meta_internal_parse_field *outer_fields[] = {
        make_field("flag", "unsigned char", 0, 0, nullptr),
        make_field("inner", "inner_probe", 1, 2, nullptr),
        make_field("ratio", "double", 0, 0, nullptr),
    };
    append_struct(&list, "outer_probe", outer_fields, 3, nullptr);

    return list;
}

/**
 *  @brief          AST から記述子を組み立てます。呼び出し側がアリーナを破棄します。
 */
int build(struct_meta_internal_parse_struct_list *structs, struct_meta_internal_arena **arena_out,
          const struct_meta_descriptor *const **descriptors_out, size_t *count_out, struct_meta_diagnostic *diagnostic)
{
    *arena_out = struct_meta_internal_arena_create();
    const int ret = struct_meta_internal_build_descriptors(structs, *arena_out, descriptors_out, count_out, diagnostic);
    struct_meta_internal_parse_struct_list_destroy(structs);
    return ret;
}

} // namespace

// ASTから構造体記述子を組み立てた際にオフセットやサイズがコンパイラ計算と一致することの確認
TEST(structMetaBuildTest, computes_the_same_layout_as_the_compiler)
{
    // Arrange
    struct_meta_internal_arena *arena = nullptr;
    const struct_meta_descriptor *const *descriptors = nullptr;
    size_t count = 0;
    struct_meta_diagnostic diagnostic = {};

    // Pre-Assert

    // Act
    int ret = build(make_nested_ast(), &arena, &descriptors, &count, &diagnostic); // [手順] - AST から記述子を作る。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret);                             // [確認_正常系] - 組み立てが成功すること。
    ASSERT_EQ(2U, count);                                 // [確認_正常系] - 2 個の記述子ができること。
    EXPECT_STREQ("inner_probe", descriptors[0]->name);    // [確認_正常系] - 並びが宣言順であること。
    EXPECT_STREQ("outer_probe", descriptors[1]->name);    // [確認_正常系] - 並びが宣言順であること。
    EXPECT_EQ(sizeof(inner_probe), descriptors[0]->size); // [確認_正常系] - ネストの大きさが一致すること。
    EXPECT_EQ(sizeof(outer_probe), descriptors[1]->size); // [確認_正常系] - 全体の大きさが一致すること。
    EXPECT_EQ(offsetof(inner_probe, label), descriptors[0]->fields[0].offset);
    // [確認_正常系] - `descriptors[0]->fields[0].offset` が、対応する構造体メンバーのオフセット `offsetof(inner_probe, label)` であること。
    EXPECT_EQ(offsetof(inner_probe, value), descriptors[0]->fields[1].offset);
    // [確認_正常系] - `descriptors[0]->fields[1].offset` が、対応する構造体メンバーのオフセット `offsetof(inner_probe, value)` であること。
    EXPECT_EQ(offsetof(outer_probe, flag), descriptors[1]->fields[0].offset);
    // [確認_正常系] - `descriptors[1]->fields[0].offset` が、対応する構造体メンバーのオフセット `offsetof(outer_probe, flag)` であること。
    EXPECT_EQ(offsetof(outer_probe, inner),
              descriptors[1]->fields[1].offset); // [確認_正常系] - ネスト配列が境界へ揃うこと。
    EXPECT_EQ(offsetof(outer_probe, ratio), descriptors[1]->fields[2].offset);
    // [確認_正常系] - `descriptors[1]->fields[2].offset` が、対応する構造体メンバーのオフセット `offsetof(outer_probe, ratio)` であること。

    // Cleanup
    struct_meta_internal_arena_destroy(arena); // [破棄] - アリーナを破棄する。
}

// フィールドの型分類、ネスト先参照、要素数、および属性の複写が正しく行われることの確認
TEST(structMetaBuildTest, classifies_fields_and_copies_attributes)
{
    // Arrange
    struct_meta_internal_arena *arena = nullptr;
    const struct_meta_descriptor *const *descriptors = nullptr;
    size_t count = 0;
    struct_meta_diagnostic diagnostic = {};
    ASSERT_EQ(CPLAT_OK, build(make_nested_ast(), &arena, &descriptors, &count, &diagnostic));
    // [状態確認] - `build(make_nested_ast(), &arena, &descriptors, &count, &diagnostic)` の戻り値が `CPLAT_OK` であること。

    // Pre-Assert

    // Act
    const struct_meta_field *label = &descriptors[0]->fields[0]; // [手順] - char 配列を取り出す。
    const struct_meta_field *flag = &descriptors[1]->fields[0];  // [手順] - スカラーを取り出す。
    const struct_meta_field *inner = &descriptors[1]->fields[1]; // [手順] - ネスト配列を取り出す。
    const struct_meta_field *ratio = &descriptors[1]->fields[2]; // [手順] - double を取り出す。

    // Assert
    EXPECT_EQ(STRUCT_META_FIELD_CHAR_ARRAY, label->kind);      // [確認_正常系] - char 配列を文字列として扱うこと。
    EXPECT_EQ(1U, label->element_count);                       // [確認_正常系] - 文字列は要素数 1 であること。
    EXPECT_EQ(8U, label->char_buffer_size);                    // [確認_正常系] - 全体のバイト数を保持すること。
    EXPECT_EQ(1U, label->element_size);                        // [確認_正常系] - 要素 1 個は 1 バイトであること。
    EXPECT_EQ(STRUCT_META_FIELD_UNSIGNED_INTEGER, flag->kind); // [確認_正常系] - unsigned char が符号なしであること。
    EXPECT_EQ(STRUCT_META_FIELD_STRUCT, inner->kind);          // [確認_正常系] - ネスト構造体であること。
    EXPECT_EQ(descriptors[0], inner->nested);                  // [確認_正常系] - ネスト先が張られること。
    EXPECT_EQ(sizeof(inner_probe), inner->element_size);       // [確認_正常系] - 要素サイズがネストの大きさであること。
    EXPECT_EQ(2U, inner->element_count);                       // [確認_正常系] - 要素数が宣言どおりであること。
    EXPECT_EQ(STRUCT_META_FIELD_DOUBLE, ratio->kind);          // [確認_正常系] - double を認識すること。
    ASSERT_EQ(1U, label->attribute_count);                     // [確認_正常系] - フィールドの属性を複写すること。
    EXPECT_STREQ("json.name", label->attributes[0].key);
    // [確認_正常系] - `label->attributes[0].key` の値が `"json.name"` であること。
    EXPECT_STREQ("text", label->attributes[0].value);
    // [確認_正常系] - `label->attributes[0].value` の値が `"text"` であること。
    ASSERT_EQ(1U, descriptors[0]->attribute_count); // [確認_正常系] - 構造体の属性を複写すること。
    EXPECT_STREQ("sample.category", descriptors[0]->attributes[0].key);
    // [確認_正常系] - `descriptors[0]->attributes[0].key` の値が `"sample.category"` であること。
    EXPECT_EQ(0U, descriptors[1]->attribute_count); // [確認_正常系] - 属性が無ければ 0 であること。

    // Cleanup
    struct_meta_internal_arena_destroy(arena); // [破棄] - アリーナを破棄する。
}

// meta.kind=bytes 属性が指定された char 配列が文字列ではなくバイト配列として扱われることの確認
TEST(structMetaBuildTest, treats_char_array_as_bytes_when_requested)
{
    // Arrange
    struct_meta_internal_arena *arena = nullptr;
    const struct_meta_descriptor *const *descriptors = nullptr;
    size_t count = 0;
    struct_meta_diagnostic diagnostic = {};
    struct_meta_internal_parse_struct_list *list = nullptr;
    struct_meta_internal_parse_field *fields[] = {
        make_field("raw", "char", 0, 3, make_attribute("meta.kind", "bytes", nullptr)),
        make_field("text", "char", 0, 3, nullptr),
    };
    append_struct(&list, "bytes_probe", fields, 2, nullptr);

    // Pre-Assert

    // Act
    int ret =
        build(list, &arena, &descriptors, &count, &diagnostic); // [手順] - meta.kind=bytes を含む AST を組み立てる。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret); // [確認_正常系] - 組み立てが成功すること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER,
              descriptors[0]->fields[0].kind);                 // [確認_正常系] - バイト配列を符号付き整数とすること。
    EXPECT_EQ(3U, descriptors[0]->fields[0].element_count);    // [確認_正常系] - 要素数が宣言どおりであること。
    EXPECT_EQ(0U, descriptors[0]->fields[0].char_buffer_size); // [確認_正常系] - 文字列ではないこと。
    EXPECT_EQ(STRUCT_META_FIELD_CHAR_ARRAY,
              descriptors[0]->fields[1].kind); // [確認_正常系] - 既定の char 配列は文字列であること。

    // Cleanup
    struct_meta_internal_arena_destroy(arena); // [破棄] - アリーナを破棄する。
}

// 未知の型名、不正な属性指定、循環参照、NULL引数などの不正な入力がエラーとして拒否されることの確認
TEST(structMetaBuildTest, rejects_invalid_input)
{
    // Arrange
    struct_meta_internal_arena *arena = nullptr;
    const struct_meta_descriptor *const *descriptors = nullptr;
    size_t count = 0;
    struct_meta_diagnostic unknown_type = {};
    struct_meta_diagnostic bad_kind = {};
    struct_meta_diagnostic bad_format = {};
    struct_meta_diagnostic cyclic = {};

    struct_meta_internal_parse_struct_list *unknown_list = nullptr;
    struct_meta_internal_parse_field *unknown_fields[] = {make_field("value", "missing_type", 0, 0, nullptr)};
    append_struct(&unknown_list, "unknown_probe", unknown_fields, 1, nullptr);

    struct_meta_internal_parse_struct_list *kind_list = nullptr;
    struct_meta_internal_parse_field *kind_fields[] = {
        make_field("value", "int", 0, 0, make_attribute("meta.kind", "bytes", nullptr))};
    append_struct(&kind_list, "kind_probe", kind_fields, 1, nullptr);

    struct_meta_internal_parse_struct_list *format_list = nullptr;
    struct_meta_internal_parse_field *format_fields[] = {
        make_field("value", "int", 0, 0, make_attribute("meta.format", "hex", nullptr))};
    append_struct(&format_list, "format_probe", format_fields, 1, nullptr);

    struct_meta_internal_parse_struct_list *cyclic_list = nullptr;
    struct_meta_internal_parse_field *cyclic_fields[] = {make_field("self", "cyclic_probe", 1, 0, nullptr)};
    append_struct(&cyclic_list, "cyclic_probe", cyclic_fields, 1, nullptr);

    // Pre-Assert

    // Act
    int unknown_ret = build(unknown_list, &arena, &descriptors, &count,
                            &unknown_type); // [手順] - 未知の型名を持つ AST を組み立てる。
    struct_meta_internal_arena_destroy(arena);
    int kind_ret = build(kind_list, &arena, &descriptors, &count,
                         &bad_kind); // [手順] - スカラーに meta.kind を指定した AST を組み立てる。
    struct_meta_internal_arena_destroy(arena);
    int format_ret = build(format_list, &arena, &descriptors, &count,
                           &bad_format); // [手順] - スカラーに meta.format を指定した AST を組み立てる。
    struct_meta_internal_arena_destroy(arena);
    int cyclic_ret =
        build(cyclic_list, &arena, &descriptors, &count, &cyclic); // [手順] - 循環参照を持つ AST を組み立てる。
    struct_meta_internal_arena_destroy(arena);
    int null_structs = struct_meta_internal_build_descriptors(nullptr, nullptr, &descriptors, &count,
                                                              nullptr); // [手順] - NULL 引数を渡す。

    // Assert
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, unknown_ret);         // [確認_異常系] - 未知の型を拒否すること。
    EXPECT_STRNE("", unknown_type.message);              // [確認_異常系] - 診断が残ること。
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, kind_ret);   // [確認_異常系] - スカラーへの meta.kind を拒否すること。
    EXPECT_STRNE("", bad_kind.message);                  // [確認_異常系] - 診断が残ること。
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, format_ret); // [確認_異常系] - スカラーへの meta.format を拒否すること。
    EXPECT_STRNE("", bad_format.message);                // [確認_異常系] - 診断が残ること。
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, cyclic_ret); // [確認_異常系] - 循環する構造体を拒否すること。
    EXPECT_STRNE("", cyclic.message);                    // [確認_異常系] - 診断が残ること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, null_structs); // [確認_異常系] - 引数の NULL を拒否すること。
}
