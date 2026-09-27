#include <testfw.h>
#include <struct_meta/json/json.h>
#include <cplat/base/result.h>
#include <cstddef>
#include <cstdint>

namespace
{
struct Sample
{
    int id;
    int optional;
};
const struct_meta_attribute kIdAttributes[] = {{"json.name", "person_id"}, {"json.required", nullptr}};
const struct_meta_field kFields[] = {
    {"id", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Sample, id), sizeof(int), 1, 0, nullptr, nullptr,
     kIdAttributes, 2},
    {"optional", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Sample, optional), sizeof(int), 1, 0, nullptr, nullptr,
     nullptr, 0},
};
const struct_meta_descriptor kDescriptor = {"Sample", sizeof(Sample), kFields, 2, nullptr, nullptr, 0};

struct Widths
{
    int64_t wide;
    uint32_t flags;
    int16_t offset;
    uint8_t rank;
    uint8_t reserved; /* 明示的アラインメント。 */
};
const struct_meta_field kWidthFields[] = {
    {"wide", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Widths, wide), sizeof(int64_t), 1, 0, nullptr, nullptr,
     nullptr, 0},
    {"flags", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(Widths, flags), sizeof(uint32_t), 1, 0, nullptr, nullptr,
     nullptr, 0},
    {"offset", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Widths, offset), sizeof(int16_t), 1, 0, nullptr, nullptr,
     nullptr, 0},
    {"rank", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(Widths, rank), sizeof(uint8_t), 1, 0, nullptr, nullptr,
     nullptr, 0},
};
const struct_meta_descriptor kWidthsDescriptor = {"Widths", sizeof(Widths), kWidthFields, 4, nullptr, nullptr, 0};

struct Integer64Limits
{
    int64_t minimum;
    int64_t maximum;
    uint64_t unsigned_maximum;
};
const struct_meta_field kInteger64LimitFields[] = {
    {"minimum", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Integer64Limits, minimum), sizeof(int64_t), 1, 0, nullptr,
     nullptr, nullptr, 0},
    {"maximum", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Integer64Limits, maximum), sizeof(int64_t), 1, 0, nullptr,
     nullptr, nullptr, 0},
    {"unsigned_maximum", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(Integer64Limits, unsigned_maximum),
     sizeof(uint64_t), 1, 0, nullptr, nullptr, nullptr, 0},
};
const struct_meta_descriptor kInteger64LimitsDescriptor = {
    "Integer64Limits", sizeof(Integer64Limits), kInteger64LimitFields, 3, nullptr, nullptr, 0};

struct ByteArrays
{
    int8_t signed_values[3];
    uint8_t unsigned_values[3];
    uint8_t hex_values[3];
};
const struct_meta_attribute kHexAttributes[] = {{"meta.format", "hex"}};
const struct_meta_field kByteFields[] = {
    {"signed_values", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(ByteArrays, signed_values), sizeof(int8_t), 3, 0,
     nullptr, nullptr, nullptr, 0},
    {"unsigned_values", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(ByteArrays, unsigned_values), sizeof(uint8_t),
     3, 0, nullptr, nullptr, nullptr, 0},
    {"hex_values", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(ByteArrays, hex_values), sizeof(uint8_t), 3, 0,
     nullptr, nullptr, kHexAttributes, 1},
};
const struct_meta_descriptor kByteDescriptor = {"ByteArrays", sizeof(ByteArrays), kByteFields, 3, nullptr, nullptr, 0};

/** 幅ごとの限界値を JSON から書き戻し、結果コードを返します。 */
int decode_widths(const char *text, Widths *sample)
{
    cJSON *json = cJSON_Parse(text);
    if (json == nullptr)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    int ret = struct_meta_json_decode(&kWidthsDescriptor, json, sample);
    cJSON_Delete(json);
    return ret;
}

/** バイト配列を JSON から書き戻し、結果コードを返します。 */
int decode_bytes(const char *text, ByteArrays *sample)
{
    cJSON *json = cJSON_Parse(text);
    if (json == nullptr)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    int ret = struct_meta_json_decode(&kByteDescriptor, json, sample);
    cJSON_Delete(json);
    return ret;
}

/** 64 ビット整数を JSON から書き戻し、結果コードを返します。 */
int decode_integer64_limits(const char *text, Integer64Limits *sample)
{
    cJSON *json = cJSON_Parse(text);
    if (json == nullptr)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    int ret = struct_meta_json_decode(&kInteger64LimitsDescriptor, json, sample);
    cJSON_Delete(json);
    return ret;
}
} // namespace

// 各ビット幅の整数値が JSON から正しくデコードされることの確認
TEST(JsonDecodeTest, DecodesEachIntegerWidth)
{
    // Arrange
    Widths sample = {}; // [状態] - 各幅の限界値を含む JSON を用意する。

    // Pre-Assert

    // Act
    int actual = decode_widths("{\"wide\":-999999999999999,\"flags\":4294967295,\"offset\":-32768,\"rank\":255}",
                               &sample); // [手順] - 各幅の限界値を含む JSON をデコードする。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual);              // [確認_正常系] - 幅ごとの限界値が受理されること。
    EXPECT_EQ(-999999999999999, sample.wide); // [確認_正常系] - 64 ビット符号付きが復元されること。
    EXPECT_EQ(4294967295U, sample.flags);     // [確認_正常系] - 32 ビット符号なしが復元されること。
    EXPECT_EQ(-32768, sample.offset);         // [確認_正常系] - 16 ビット符号付きが復元されること。
    EXPECT_EQ(255, sample.rank);              // [確認_正常系] - 8 ビット符号なしが復元されること。
}

// フィールドのビット幅を超える整数値がデコード時に拒否されることの確認
TEST(JsonDecodeTest, RejectsValueOutsideFieldWidth)
{
    // Arrange
    Widths sample = {}; // [状態] - 対象の幅に収まらない値を用意する。

    // Pre-Assert

    // Act
    int over = decode_widths("{\"rank\":256}", &sample);     // [手順] - uint8_t の範囲を超える値をデコードする。
    int negative = decode_widths("{\"rank\":-1}", &sample);  // [手順] - 符号なしへ負値を与えてデコードする。
    int wide = decode_widths("{\"offset\":40000}", &sample); // [手順] - int16_t の範囲を超える値をデコードする。

    // Assert
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, over);     // [確認_異常系] - 幅を超える値が拒否されること。
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, negative); // [確認_異常系] - 符号なしへの負値が拒否されること。
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, wide);     // [確認_異常系] - 符号付きの範囲外が拒否されること。
}

// 小数や表現不可能な巨大数値が整数フィールドへのデコード時に拒否されることの確認
TEST(JsonDecodeTest, RejectsNonIntegerAndUnrepresentableNumber)
{
    // Arrange
    Widths sample = {}; // [状態] - 整数でない値と表現できない値を用意する。

    // Pre-Assert

    // Act
    int fraction = decode_widths("{\"wide\":1.5}", &sample); // [手順] - 小数値を与えてデコードする。
    int huge = decode_widths("{\"wide\":1e300}", &sample);   // [手順] - double の範囲の巨大値を与えてデコードする。

    // Assert
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, fraction); // [確認_異常系] - 小数が拒否されること。
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, huge);     // [確認_異常系] - 表現できない値が拒否されること。
}

// 64 ビット整数の全境界値が JSON から正確にデコードされることの確認
TEST(JsonDecodeTest, decodes_64_bit_integer_limits_exactly)
{
    // Arrange
    Integer64Limits sample = {};

    // Pre-Assert

    // Act
    int actual = decode_integer64_limits("{\"minimum\":-9223372036854775808,\"maximum\":9223372036854775807,"
                                         "\"unsigned_maximum\":18446744073709551615}",
                                         &sample); // [手順] - 64 ビット整数の全境界値を読み込む。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual);                    // [確認_正常系] - 全境界値を読み込めること。
    EXPECT_EQ(INT64_MIN, sample.minimum);           // [確認_正常系] - 符号付き最小値を正確に復元すること。
    EXPECT_EQ(INT64_MAX, sample.maximum);           // [確認_正常系] - 符号付き最大値を正確に復元すること。
    EXPECT_EQ(UINT64_MAX, sample.unsigned_maximum); // [確認_正常系] - 符号なし最大値を正確に復元すること。
}

// 64 ビット整数の範囲外の値が拒否され他のフィールドが保護されることの確認
TEST(JsonDecodeTest, rejects_values_outside_64_bit_integer_ranges)
{
    // Arrange
    Integer64Limits sample = {1, 2, 3U};

    // Pre-Assert

    // Act
    int signed_over = decode_integer64_limits("{\"maximum\":9223372036854775808}", &sample); // [手順] - 符号付き上限を超える値をデコードする。
    int unsigned_negative = decode_integer64_limits("{\"unsigned_maximum\":-1}", &sample); // [手順] - 符号なしフィールドへ負値をデコードする。
    int unsigned_over = decode_integer64_limits("{\"unsigned_maximum\":18446744073709551616}", &sample); // [手順] - 符号なし上限を超える値をデコードする。

    // Assert
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, signed_over);       // [確認_異常系] - 符号付き上限を超える値を拒否すること。
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, unsigned_negative); // [確認_異常系] - 符号なしフィールドの負値を拒否すること。
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, unsigned_over);     // [確認_異常系] - 符号なし上限を超える値を拒否すること。
    EXPECT_EQ(1, sample.minimum);                         // [確認_異常系] - 無関係なフィールドを変更しないこと。
    EXPECT_EQ(2, sample.maximum);           // [確認_異常系] - 失敗した符号付きフィールドを変更しないこと。
    EXPECT_EQ(3U, sample.unsigned_maximum); // [確認_異常系] - 失敗した符号なしフィールドを変更しないこと。
}

// json.name 属性で指定された別名キーからフィールド値が正しく読み込まれることの確認
TEST(JsonDecodeTest, UsesGenericJsonAttributes)
{
    // Arrange
    cJSON *json = cJSON_Parse("{\"person_id\":42}");
    Sample sample = {0, 9}; // [状態] - 任意フィールドに既存値を持たせる。

    // Pre-Assert
    ASSERT_NE(nullptr, json); // [状態確認] - JSON オブジェクトをパースできること。

    // Act
    int actual = struct_meta_json_decode(&kDescriptor, json, &sample); // [手順] - 別名キーを含む JSON をデコードする。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual); // [確認_正常系] - 必須の別名キーを読み込めること。
    EXPECT_EQ(42, sample.id);    // [確認_正常系] - 別名キーの値が id に反映されること。
    EXPECT_EQ(9, sample.optional); // [確認_正常系] - 指定のないフィールドが保持されること。

    // Cleanup
    cJSON_Delete(json);
}

// json.required 属性が付与された必須キーが欠落している場合にエラーが報告されることの確認
TEST(JsonDecodeTest, ReportsMissingRequiredAttribute)
{
    // Arrange
    cJSON *json = cJSON_CreateObject();
    Sample sample = {}; // [状態] - 必須キーのない JSON を用意する。

    // Pre-Assert
    ASSERT_NE(nullptr, json); // [状態確認] - 空の JSON オブジェクトを作成できること。

    // Act
    int actual = struct_meta_json_decode(&kDescriptor, json, &sample); // [手順] - 必須キーが欠落した JSON をデコードする。

    // Assert
    EXPECT_EQ(CPLAT_ERR_MISSING_REQUIRED, actual); // [確認_異常系] - 必須キー欠落を報告すること。

    // Cleanup
    cJSON_Delete(json);
}

// バイト配列フィールドが整数配列形式および 16 進数文字列形式からデコードされることの確認
TEST(JsonDecodeTest, decodes_byte_arrays_in_selected_format)
{
    // Arrange
    ByteArrays sample = {};

    // Pre-Assert

    // Act
    int actual =
        decode_bytes("{\"signed_values\":[-128,0,127],\"unsigned_values\":[0,128,255],\"hex_values\":\"00  A5 ff\"}",
                     &sample); // [手順] - 整数配列と16進文字列を読み込む。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual);               // [確認_正常系] - 両形式を読み込めること。
    EXPECT_EQ(-128, sample.signed_values[0]);  // [確認_正常系] - 符号付き値を維持すること。
    EXPECT_EQ(255, sample.unsigned_values[2]); // [確認_正常系] - 符号なし値を維持すること。
    EXPECT_EQ(0, sample.hex_values[0]);        // [確認_正常系] - 先頭バイトを復元すること。
    EXPECT_EQ(0xa5, sample.hex_values[1]);     // [確認_正常系] - 大文字の16進入力を受理すること。
    EXPECT_EQ(0xff, sample.hex_values[2]);     // [確認_正常系] - 末尾バイトを復元すること。
}

// 不正な 16 進数文字列が拒否されバイト配列が部分更新されないことの確認
TEST(JsonDecodeTest, rejects_invalid_hex_without_partial_update)
{
    // Arrange
    ByteArrays sample = {{0, 0, 0}, {0, 0, 0}, {1, 2, 3}};

    // Pre-Assert

    // Act
    int actual = decode_bytes("{\"hex_values\":\"aa bb gg\"}", &sample); // [手順] - 不正な16進入力を与える。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING, actual); // [確認_異常系] - 不正な16進入力を拒否すること。
    EXPECT_EQ(1, sample.hex_values[0]);            // [確認_異常系] - 先頭要素を更新しないこと。
    EXPECT_EQ(2, sample.hex_values[1]);            // [確認_異常系] - 中央要素を更新しないこと。
    EXPECT_EQ(3, sample.hex_values[2]);            // [確認_異常系] - 末尾要素を更新しないこと。
}
