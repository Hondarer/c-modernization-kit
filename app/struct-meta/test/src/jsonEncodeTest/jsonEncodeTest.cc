#include <testfw.h>
#include <struct_meta/json/json.h>
#include <cJSON_Integer.h>
#include <cplat/base/result.h>
#include <cstddef>
#include <cstdint>

namespace
{
struct Sample
{
    int id;
    int hidden;
};
const struct_meta_attribute kIdAttributes[] = {{"json.name", "person_id"}};
const struct_meta_attribute kHiddenAttributes[] = {{"json.ignore", nullptr}};
const struct_meta_field kFields[] = {
    {"id", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Sample, id), sizeof(int), 1, 0, nullptr, nullptr,
     kIdAttributes, 1},
    {"hidden", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Sample, hidden), sizeof(int), 1, 0, nullptr, nullptr,
     kHiddenAttributes, 1},
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
} // namespace

// 各ビット幅の整数値が JSON へ正しくエンコードされることの確認
TEST(JsonEncodeTest, EncodesEachIntegerWidth)
{
    // Arrange
    Widths sample = {-999999999999999, 4294967295U, -32768, 255, 0}; // [状態] - 各幅の限界値を用意する。
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int ret = struct_meta_json_encode(&kWidthsDescriptor, &sample, &json); // [手順] - 各幅の整数値を含む構造体を JSON へエンコードする。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret); // [確認_正常系] - エンコードが成功すること。
    ASSERT_NE(nullptr, json); // [確認_正常系] - 幅ごとの値が JSON へ変換されること。
    EXPECT_DOUBLE_EQ(-999999999999999.0, cJSON_GetObjectItemCaseSensitive(json, "wide")->valuedouble);
    EXPECT_DOUBLE_EQ(4294967295.0, cJSON_GetObjectItemCaseSensitive(json, "flags")->valuedouble);
    EXPECT_DOUBLE_EQ(-32768.0, cJSON_GetObjectItemCaseSensitive(json, "offset")->valuedouble);
    EXPECT_DOUBLE_EQ(255.0, cJSON_GetObjectItemCaseSensitive(json, "rank")->valuedouble);

    // Cleanup
    cJSON_Delete(json);
}

// 64 ビット整数の全境界値が丸めなく正確に JSON へエンコードされることの確認
TEST(JsonEncodeTest, encodes_64_bit_integer_limits_exactly)
{
    // Arrange
    Integer64Limits sample = {INT64_MIN, INT64_MAX, UINT64_MAX};
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int actual = struct_meta_json_encode(&kInteger64LimitsDescriptor, &sample, &json); // [手順] - 境界値を変換する。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual); // [確認_正常系] - 64 ビット整数の全境界値を変換できること。
    ASSERT_NE(nullptr, json);    // [確認_正常系] - JSON オブジェクトが生成されること。
    int64_t minimum = 0;
    int64_t maximum = 0;
    uint64_t unsigned_maximum = 0U;
    EXPECT_TRUE(cJSON_GetInt64Value(cJSON_GetObjectItemCaseSensitive(json, "minimum"),
                                    &minimum)); // [確認_正常系] - 符号付き最小値を整数として取得できること。
    EXPECT_TRUE(cJSON_GetInt64Value(cJSON_GetObjectItemCaseSensitive(json, "maximum"),
                                    &maximum)); // [確認_正常系] - 符号付き最大値を整数として取得できること。
    EXPECT_TRUE(cJSON_GetUInt64Value(cJSON_GetObjectItemCaseSensitive(json, "unsigned_maximum"),
                                     &unsigned_maximum)); // [確認_正常系] - 符号なし最大値を整数として取得できること。
    EXPECT_EQ(INT64_MIN, minimum);                        // [確認_正常系] - 符号付き最小値を正確に保持すること。
    EXPECT_EQ(INT64_MAX, maximum);                        // [確認_正常系] - 符号付き最大値を正確に保持すること。
    EXPECT_EQ(UINT64_MAX, unsigned_maximum);              // [確認_正常系] - 符号なし最大値を正確に保持すること。
    char *text = cJSON_PrintUnformatted(json);
    ASSERT_NE(nullptr, text); // [確認_正常系] - JSON テキストを生成できること。
    EXPECT_STREQ("{\"minimum\":-9223372036854775808,\"maximum\":9223372036854775807,"
                 "\"unsigned_maximum\":18446744073709551615}",
                 text); // [確認_正常系] - 丸めや指数表記のない十進整数として出力すること。

    // Cleanup
    cJSON_free(text);
    cJSON_Delete(json);
}

// json.name 属性および json.ignore 属性に従って JSON が生成されることの確認
TEST(JsonEncodeTest, UsesGenericJsonAttributes)
{
    // Arrange
    Sample sample = {42, 7}; // [状態] - 名前変更属性と除外属性を持つ値を用意する。
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int ret = struct_meta_json_encode(&kDescriptor, &sample, &json); // [手順] - 属性付き構造体を JSON へエンコードする。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret); // [確認_正常系] - エンコードが成功すること。
    ASSERT_NE(nullptr, json); // [確認_正常系] - 属性に従った JSON が生成されること。
    EXPECT_EQ(42, cJSON_GetObjectItemCaseSensitive(json, "person_id")->valueint);
    EXPECT_EQ(nullptr, cJSON_GetObjectItemCaseSensitive(json, "id"));
    EXPECT_EQ(nullptr, cJSON_GetObjectItemCaseSensitive(json, "hidden"));

    // Cleanup
    cJSON_Delete(json);
}

// 破損した構造体記述子での JSON エンコードが適切に拒否されることの確認
TEST(JsonEncodeTest, RejectsCorruptDescriptor)
{
    // Arrange
    const struct_meta_descriptor descriptor = {"Sample", sizeof(Sample), nullptr, 1, nullptr, nullptr, 0};
    Sample sample = {}; // [状態] - フィールド配列が欠けた記述子を用意する。
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int actual = struct_meta_json_encode(&descriptor, &sample, &json); // [手順] - 破損した記述子でエンコードを試みる。

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual);                   // [確認_異常系] - 検査エラーになること。
    EXPECT_EQ(nullptr, json);                                          // [確認_異常系] - JSON が生成されないこと。
}

// バイト配列フィールドが指定フォーマットに従い整数配列または 16 進数文字列としてエンコードされることの確認
TEST(JsonEncodeTest, encodes_byte_arrays_in_selected_format)
{
    // Arrange
    ByteArrays sample = {{-128, 0, 127}, {0, 128, 255}, {0, 165, 255}};
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int actual = struct_meta_json_encode(&kByteDescriptor, &sample, &json); // [手順] - バイト配列を変換する。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual); // [確認_正常系] - バイト配列を変換できること。
    ASSERT_NE(nullptr, json);    // [確認_正常系] - JSON オブジェクトが生成されること。
    const cJSON *signed_values = cJSON_GetObjectItemCaseSensitive(json, "signed_values");
    const cJSON *unsigned_values = cJSON_GetObjectItemCaseSensitive(json, "unsigned_values");
    const cJSON *hex_values = cJSON_GetObjectItemCaseSensitive(json, "hex_values");
    ASSERT_TRUE(cJSON_IsArray(signed_values)); // [確認_正常系] - 既定形式が整数配列であること。
    EXPECT_DOUBLE_EQ(-128.0,
                     cJSON_GetArrayItem(signed_values, 0)->valuedouble); // [確認_正常系] - 符号付き値を維持すること。
    ASSERT_TRUE(cJSON_IsArray(unsigned_values)); // [確認_正常系] - 符号なし配列も整数配列であること。
    EXPECT_DOUBLE_EQ(255.0,
                     cJSON_GetArrayItem(unsigned_values, 2)->valuedouble); // [確認_正常系] - 符号なし値を維持すること。
    ASSERT_TRUE(cJSON_IsString(hex_values));                    // [確認_正常系] - hex 指定が文字列になること。
    EXPECT_STREQ("00 a5 ff", cJSON_GetStringValue(hex_values)); // [確認_正常系] - 小文字2桁の空白区切りであること。

    // Cleanup
    cJSON_Delete(json);
}

// NUL 終端されていない文字配列が不正なエンコーディングとして拒否されることの確認
TEST(JsonEncodeTest, rejects_unterminated_character_array)
{
    // Arrange
    struct Text
    {
        char value[3];
    } sample = {{'a', 'b', 'c'}};
    const struct_meta_field fields[] = {{"value", STRUCT_META_FIELD_CHAR_ARRAY, 0, offsetof(Text, value), sizeof(char),
                                         1, sizeof(sample.value), nullptr, nullptr, nullptr, 0}};
    const struct_meta_descriptor descriptor = {"Text", sizeof(Text), fields, 1, nullptr, nullptr, 0};
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int actual = struct_meta_json_encode(&descriptor, &sample, &json); // [手順] - NUL のない配列を変換する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING, actual); // [確認_異常系] - NUL のない文字列を拒否すること。
    EXPECT_EQ(nullptr, json);                      // [確認_異常系] - JSON が生成されないこと。
}
