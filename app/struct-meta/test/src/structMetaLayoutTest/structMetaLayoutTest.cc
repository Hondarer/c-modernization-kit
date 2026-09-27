#include <cplat/base/result.h>
#include <struct_meta/layout/layout.h>
#include <testfw.h>

#include <cstddef>
#include <cstdint>

namespace
{

/**
 *  幅の異なるスカラーを混在させ、メンバー間パディングと末尾パディングの双方を生じさせます。
 *
 *  レイアウト エンジンは pad メンバーを知らず、first から fifth までだけを並べます。
 *  pad は暗黙のパディングと同じ位置と幅であり、計算結果は一致しなければなりません。
 */
struct mixed_widths
{
    char first;
    char pad1[3];  /**< 明示的アラインメントです。0 を指定します。 */
    int second;
    char third;
    char pad2[7];  /**< 明示的アラインメントです。0 を指定します。 */
    double fourth;
    short fifth;
    char pad3[6];  /**< 明示的アラインメントです。0 を指定します。 */
};

/**
 *  固定長配列だけで構成し、配列のアラインメントが要素のアラインメントと等しいことを確かめます。
 */
struct with_arrays
{
    unsigned char header[3];
    char pad1;    /**< 明示的アラインメントです。0 を指定します。 */
    int values[4];
    char text[5];
    char pad2[3]; /**< 明示的アラインメントです。0 を指定します。 */
};

/**
 *  ネスト構造体を含み、ネストのアラインメントが親へ伝播することを確かめます。
 */
struct nested_holder
{
    char flag;
    char pad1[7]; /**< 明示的アラインメントです。0 を指定します。 */
    mixed_widths body;
    short tail;
    char pad2[6]; /**< 明示的アラインメントです。0 を指定します。 */
};

/**
 *  @brief          型スペリングで 1 メンバーを追加します。
 *  @return         @ref struct_meta_internal_layout_add の結果コードを返します。
 */
int add_scalar(struct_meta_internal_layout_builder *builder, const char *spelling, size_t element_count,
               size_t *offset_out)
{
    const struct_meta_internal_layout_type *type = struct_meta_internal_layout_find_type(spelling);
    if (type == nullptr)
    {
        return CPLAT_ERR_NOT_FOUND;
    }
    return struct_meta_internal_layout_add(builder, type->size, element_count, type->alignment, offset_out);
}

} // namespace

// 対応する基本型の情報を取得でき、プラットフォーム依存の未対応型名やNULLを拒否することの確認
TEST(structMetaLayoutTest, finds_supported_types_and_rejects_platform_dependent_ones)
{
    // Arrange

    // Pre-Assert

    // Act
    const struct_meta_internal_layout_type *type_short =
        struct_meta_internal_layout_find_type("short"); // [手順] - short を引く。
    const struct_meta_internal_layout_type *type_unsigned =
        struct_meta_internal_layout_find_type("unsigned"); // [手順] - unsigned を引く。
    const struct_meta_internal_layout_type *type_double =
        struct_meta_internal_layout_find_type("double"); // [手順] - double を引く。

    // Assert
    ASSERT_NE(nullptr, type_short); // [確認_正常系] - short が表にあること。
    EXPECT_EQ(sizeof(short), type_short->size); // [確認_正常系] - short の大きさが実環境と一致すること。
    EXPECT_EQ(alignof(short), type_short->alignment); // [確認_正常系] - short の境界が実環境と一致すること。
    EXPECT_EQ(STRUCT_META_FIELD_SIGNED_INTEGER, type_short->kind); // [確認_正常系] - short が符号付き整数であること。
    ASSERT_NE(nullptr, type_unsigned);                             // [確認_正常系] - unsigned が表にあること。
    EXPECT_STREQ("unsigned int",
                 type_unsigned->canonical_spelling); // [確認_正常系] - unsigned が正規化した綴りを持つこと。
    ASSERT_NE(nullptr, type_double);                 // [確認_正常系] - double が表にあること。
    EXPECT_EQ(sizeof(double), type_double->size);    // [確認_正常系] - double の大きさが実環境と一致すること。
    EXPECT_EQ(alignof(double), type_double->alignment); // [確認_正常系] - double の境界が実環境と一致すること。
    EXPECT_EQ(nullptr,
              struct_meta_internal_layout_find_type("long")); // [確認_異常系] - long は表に無いこと。
    EXPECT_EQ(nullptr, struct_meta_internal_layout_find_type(
                           "unsigned long")); // [確認_異常系] - unsigned long は表に無いこと。
    EXPECT_EQ(nullptr, struct_meta_internal_layout_find_type(nullptr)); // [確認_異常系] - NULL を拒否すること。
    EXPECT_EQ(nullptr,
              struct_meta_internal_layout_find_type("size_t")); // [確認_異常系] - 未対応の型名を拒否すること。
}

// スカラーメンバーを持つ構造体のオフセット、サイズ、アラインメントがコンパイラの計算結果と一致することの確認
TEST(structMetaLayoutTest, computes_scalar_layout_like_the_compiler)
{
    // Arrange
    struct_meta_internal_layout_builder builder;
    size_t offset_first = 0;
    size_t offset_second = 0;
    size_t offset_third = 0;
    size_t offset_fourth = 0;
    size_t offset_fifth = 0;
    size_t size = 0;
    size_t alignment = 0;

    // Pre-Assert

    // Act
    struct_meta_internal_layout_begin(&builder);                      // [手順] - 計算を開始する。
    add_scalar(&builder, "char", 1, &offset_first);                   // [手順] - char を追加する。
    add_scalar(&builder, "int", 1, &offset_second);                   // [手順] - int を追加する。
    add_scalar(&builder, "char", 1, &offset_third);                   // [手順] - char を追加する。
    add_scalar(&builder, "double", 1, &offset_fourth);                // [手順] - double を追加する。
    add_scalar(&builder, "short", 1, &offset_fifth);                  // [手順] - short を追加する。
    int ret = struct_meta_internal_layout_end(&builder, &size, &alignment); // [手順] - 計算を終える。

    // Assert
    EXPECT_EQ(CPLAT_OK, ret); // [確認_正常系] - 計算が成功すること。
    EXPECT_EQ(offsetof(mixed_widths, first), offset_first);   // [確認_正常系] - 先頭メンバーの位置が一致すること。
    EXPECT_EQ(offsetof(mixed_widths, second), offset_second); // [確認_正常系] - 4バイト境界へ揃うこと。
    EXPECT_EQ(offsetof(mixed_widths, third), offset_third);   // [確認_正常系] - 詰めずに続くこと。
    EXPECT_EQ(offsetof(mixed_widths, fourth), offset_fourth); // [確認_正常系] - 8バイト境界へ揃うこと。
    EXPECT_EQ(offsetof(mixed_widths, fifth), offset_fifth);   // [確認_正常系] - 2バイト境界へ揃うこと。
    EXPECT_EQ(sizeof(mixed_widths), size);                    // [確認_正常系] - 末尾パディングを含む大きさが一致すること。
    EXPECT_EQ(alignof(mixed_widths), alignment);              // [確認_正常系] - 構造体の境界が一致すること。
}

// 固定長配列メンバーを持つ構造体のオフセット、サイズ、アラインメントがコンパイラの計算結果と一致することの確認
TEST(structMetaLayoutTest, computes_array_layout_like_the_compiler)
{
    // Arrange
    struct_meta_internal_layout_builder builder;
    size_t offset_header = 0;
    size_t offset_values = 0;
    size_t offset_text = 0;
    size_t size = 0;
    size_t alignment = 0;

    // Pre-Assert

    // Act
    struct_meta_internal_layout_begin(&builder);                 // [手順] - 計算を開始する。
    add_scalar(&builder, "unsigned char", 3, &offset_header);    // [手順] - 1バイト配列を追加する。
    add_scalar(&builder, "int", 4, &offset_values);              // [手順] - 4バイト配列を追加する。
    add_scalar(&builder, "char", 5, &offset_text);               // [手順] - char 配列を追加する。
    int ret = struct_meta_internal_layout_end(&builder, &size, &alignment); // [手順] - 計算を終える。

    // Assert
    EXPECT_EQ(CPLAT_OK, ret);                                // [確認_正常系] - 計算が成功すること。
    EXPECT_EQ(offsetof(with_arrays, header), offset_header); // [確認_正常系] - 先頭配列の位置が一致すること。
    EXPECT_EQ(offsetof(with_arrays, values), offset_values); // [確認_正常系] - 配列が要素の境界へ揃うこと。
    EXPECT_EQ(offsetof(with_arrays, text), offset_text);     // [確認_正常系] - char 配列の位置が一致すること。
    EXPECT_EQ(sizeof(with_arrays), size);                    // [確認_正常系] - 末尾パディングを含む大きさが一致すること。
    EXPECT_EQ(alignof(with_arrays), alignment);              // [確認_正常系] - 構造体の境界が一致すること。
}

// ネストした構造体を持つ構造体のオフセット、サイズ、アラインメントがコンパイラの計算結果と一致することの確認
TEST(structMetaLayoutTest, computes_nested_layout_like_the_compiler)
{
    // Arrange
    struct_meta_internal_layout_builder body_builder;
    struct_meta_internal_layout_builder builder;
    size_t unused_offset = 0;
    size_t body_size = 0;
    size_t body_alignment = 0;
    size_t offset_flag = 0;
    size_t offset_body = 0;
    size_t offset_tail = 0;
    size_t size = 0;
    size_t alignment = 0;

    // Pre-Assert

    // Act
    struct_meta_internal_layout_begin(&body_builder);              // [手順] - ネスト側の計算を開始する。
    add_scalar(&body_builder, "char", 1, &unused_offset);          // [手順] - ネストのメンバーを並べる。
    add_scalar(&body_builder, "int", 1, &unused_offset);
    add_scalar(&body_builder, "char", 1, &unused_offset);
    add_scalar(&body_builder, "double", 1, &unused_offset);
    add_scalar(&body_builder, "short", 1, &unused_offset);
    struct_meta_internal_layout_end(&body_builder, &body_size,
                                    &body_alignment); // [手順] - ネストの大きさと境界を求める。

    struct_meta_internal_layout_begin(&builder);       // [手順] - 親側の計算を開始する。
    add_scalar(&builder, "char", 1, &offset_flag);     // [手順] - char を追加する。
    struct_meta_internal_layout_add(&builder, body_size, 1, body_alignment,
                                    &offset_body);     // [手順] - ネスト構造体を追加する。
    add_scalar(&builder, "short", 1, &offset_tail);    // [手順] - short を追加する。
    int ret = struct_meta_internal_layout_end(&builder, &size, &alignment); // [手順] - 計算を終える。

    // Assert
    EXPECT_EQ(CPLAT_OK, ret);                                     // [確認_正常系] - 計算が成功すること。
    EXPECT_EQ(sizeof(mixed_widths), body_size);                   // [確認_正常系] - ネストの大きさが一致すること。
    EXPECT_EQ(alignof(mixed_widths), body_alignment);             // [確認_正常系] - ネストの境界が一致すること。
    EXPECT_EQ(offsetof(nested_holder, flag), offset_flag);        // [確認_正常系] - 先頭メンバーの位置が一致すること。
    EXPECT_EQ(offsetof(nested_holder, body), offset_body);        // [確認_正常系] - ネストが自身の境界へ揃うこと。
    EXPECT_EQ(offsetof(nested_holder, tail), offset_tail);        // [確認_正常系] - ネストの直後に続くこと。
    EXPECT_EQ(sizeof(nested_holder), size);                       // [確認_正常系] - 全体の大きさが一致すること。
    EXPECT_EQ(alignof(nested_holder), alignment);                 // [確認_正常系] - ネストの境界が親へ伝播すること。
}

// レイアウト計算の各関数に不正な引数を渡した場合に適切なエラーコードを返すことの確認
TEST(structMetaLayoutTest, rejects_invalid_arguments)
{
    // Arrange
    struct_meta_internal_layout_builder builder;
    struct_meta_internal_layout_builder empty_builder;
    size_t offset = 0;
    size_t size = 0;
    size_t alignment = 0;

    // Pre-Assert
    struct_meta_internal_layout_begin(&builder);
    struct_meta_internal_layout_begin(&empty_builder);
    struct_meta_internal_layout_begin(nullptr); // [状態] - NULL で異常終了しないこと。

    // Act
    int null_builder = struct_meta_internal_layout_add(nullptr, 4, 1, 4, &offset); // [手順] - builder に NULL を指定して追加する。
    int null_offset = struct_meta_internal_layout_add(&builder, 4, 1, 4, nullptr);  // [手順] - offset_out に NULL を指定して追加する。
    int zero_size = struct_meta_internal_layout_add(&builder, 0, 1, 4, &offset);    // [手順] - 要素サイズに 0 を指定して追加する。
    int zero_count = struct_meta_internal_layout_add(&builder, 4, 0, 4, &offset);   // [手順] - 要素数に 0 を指定して追加する。
    int zero_alignment = struct_meta_internal_layout_add(&builder, 4, 1, 0, &offset); // [手順] - 境界に 0 を指定して追加する。
    int odd_alignment = struct_meta_internal_layout_add(&builder, 4, 1, 3, &offset);  // [手順] - 2の冪でない境界を指定して追加する。
    int huge_count = struct_meta_internal_layout_add(&builder, 8, SIZE_MAX, 8, &offset); // [手順] - 桁あふれする要素数を指定して追加する。
    int empty_struct = struct_meta_internal_layout_end(&empty_builder, &size, &alignment); // [手順] - メンバー無しの状態で終了する。
    int null_size = struct_meta_internal_layout_end(&empty_builder, nullptr, &alignment); // [手順] - size_out に NULL を指定して終了する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, null_builder);   // [確認_異常系] - 途中経過の NULL を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, null_offset);    // [確認_異常系] - 出力先の NULL を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, zero_size);      // [確認_異常系] - 要素サイズ 0 を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, zero_count);     // [確認_異常系] - 要素数 0 を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, zero_alignment); // [確認_異常系] - 境界 0 を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, odd_alignment);  // [確認_異常系] - 2 の冪でない境界を拒否すること。
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, huge_count);         // [確認_異常系] - 桁あふれする要素数を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, empty_struct);   // [確認_異常系] - メンバーの無い構造体を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, null_size);      // [確認_異常系] - 出力先の NULL を拒否すること。
}
