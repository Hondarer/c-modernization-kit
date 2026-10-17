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
                               &sample); // [手順] - 各幅の限界値を含む JSON を指定して decode_widths() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual);              // [確認_正常系] - decode_widths() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(-999999999999999, sample.wide); // [確認_正常系] - sample.wide が -999999999999999 であること。
    EXPECT_EQ(4294967295U, sample.flags);     // [確認_正常系] - sample.flags が 4294967295U であること。
    EXPECT_EQ(-32768, sample.offset);         // [確認_正常系] - sample.offset が -32768 であること。
    EXPECT_EQ(255, sample.rank);              // [確認_正常系] - sample.rank が 255 であること。
}

// フィールドのビット幅を超える整数値がデコード時に拒否されることの確認
TEST(JsonDecodeTest, RejectsValueOutsideFieldWidth)
{
    // Arrange
    Widths sample = {}; // [状態] - 対象の幅に収まらない値を用意する。

    // Pre-Assert

    // Act
    int over =
        decode_widths("{\"rank\":256}",
                      &sample); // [手順] - uint8_t の範囲を超える値を含む JSON を指定して decode_widths() を呼び出す。
    int negative = decode_widths(
        "{\"rank\":-1}",
        &sample); // [手順] - 符号なしフィールドへの負値を含む JSON を指定して decode_widths() を呼び出す。
    int wide =
        decode_widths("{\"offset\":40000}",
                      &sample); // [手順] - int16_t の範囲を超える値を含む JSON を指定して decode_widths() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_RANGE,
        over); // [確認_異常系] - uint8_t の上限を超える値に対する decode_widths() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_RANGE,
        negative); // [確認_異常系] - 符号なしへの負値に対する decode_widths() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_RANGE,
        wide); // [確認_異常系] - int16_t の範囲外の値に対する decode_widths() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
}

// 小数や表現不可能な巨大数値が整数フィールドへのデコード時に拒否されることの確認
TEST(JsonDecodeTest, RejectsNonIntegerAndUnrepresentableNumber)
{
    // Arrange
    Widths sample = {}; // [状態] - 整数でない値と表現できない値を用意する。

    // Pre-Assert

    // Act
    int fraction =
        decode_widths("{\"wide\":1.5}", &sample); // [手順] - 小数値を含む JSON を指定して decode_widths() を呼び出す。
    int huge =
        decode_widths("{\"wide\":1e300}",
                      &sample); // [手順] - double の範囲の巨大値を含む JSON を指定して decode_widths() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE,
              fraction); // [確認_異常系] - 小数に対する decode_widths() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_RANGE,
        huge); // [確認_異常系] - 表現できない巨大値に対する decode_widths() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
}

// 64 ビット整数の全境界値が JSON から正確にデコードされることの確認
TEST(JsonDecodeTest, decodes_64_bit_integer_limits_exactly)
{
    // Arrange
    Integer64Limits sample = {};

    // Pre-Assert

    // Act
    int actual = decode_integer64_limits(
        "{\"minimum\":-9223372036854775808,\"maximum\":9223372036854775807,"
        "\"unsigned_maximum\":18446744073709551615}",
        &sample); // [手順] - 64 ビット整数の全境界値を含む JSON を指定して decode_integer64_limits() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual);          // [確認_正常系] - decode_integer64_limits() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(INT64_MIN, sample.minimum); // [確認_正常系] - sample.minimum が INT64_MIN であること。
    EXPECT_EQ(INT64_MAX, sample.maximum); // [確認_正常系] - sample.maximum が INT64_MAX であること。
    EXPECT_EQ(UINT64_MAX,
              sample.unsigned_maximum); // [確認_正常系] - sample.unsigned_maximum が UINT64_MAX であること。
}

// 64 ビット整数の範囲外の値が拒否され他のフィールドが保護されることの確認
TEST(JsonDecodeTest, rejects_values_outside_64_bit_integer_ranges)
{
    // Arrange
    Integer64Limits sample = {1, 2, 3U};

    // Pre-Assert

    // Act
    int signed_over = decode_integer64_limits(
        "{\"maximum\":9223372036854775808}",
        &sample); // [手順] - 符号付き上限を超える値を含む JSON を指定して decode_integer64_limits() を呼び出す。
    int unsigned_negative = decode_integer64_limits(
        "{\"unsigned_maximum\":-1}",
        &sample); // [手順] - 符号なしフィールドへの負値を含む JSON を指定して decode_integer64_limits() を呼び出す。
    int unsigned_over = decode_integer64_limits(
        "{\"unsigned_maximum\":18446744073709551616}",
        &sample); // [手順] - 符号なし上限を超える値を含む JSON を指定して decode_integer64_limits() を呼び出す。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_RANGE,
        signed_over); // [確認_異常系] - 符号付き上限を超える値に対する decode_integer64_limits() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_RANGE,
        unsigned_negative); // [確認_異常系] - 符号なしフィールドの負値に対する decode_integer64_limits() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_RANGE,
        unsigned_over); // [確認_異常系] - 符号なし上限を超える値に対する decode_integer64_limits() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
    EXPECT_EQ(
        1,
        sample.minimum); // [確認_異常系] - sample.minimum が 1 であること。(無関係なフィールドを変更しないことの確認)
    EXPECT_EQ(
        2,
        sample
            .maximum); // [確認_異常系] - sample.maximum が 2 であること。(失敗した符号付きフィールドを変更しないことの確認)
    EXPECT_EQ(
        3U,
        sample
            .unsigned_maximum); // [確認_異常系] - sample.unsigned_maximum が 3U であること。(失敗した符号なしフィールドを変更しないことの確認)
}

// json.name 属性で指定された別名キーからフィールド値が正しく読み込まれることの確認
TEST(JsonDecodeTest, UsesGenericJsonAttributes)
{
    // Arrange
    cJSON *json = cJSON_Parse("{\"person_id\":42}");
    Sample sample = {0, 9}; // [状態] - 任意フィールドに既存値を持たせる。

    ASSERT_NE(nullptr, json); // [状態確認] - cJSON_Parse() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_json_decode(
        &kDescriptor, json, &sample); // [手順] - 別名キーを含む JSON を指定して struct_meta_json_decode() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual); // [確認_正常系] - struct_meta_json_decode() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(42, sample.id);    // [確認_正常系] - sample.id が 42 であること。
    EXPECT_EQ(
        9,
        sample
            .optional); // [確認_正常系] - sample.optional が 9 であること。(指定のないフィールドが保持されることの確認)

    // Cleanup
    cJSON_Delete(json);
}

// json.required 属性が付与された必須キーが欠落している場合にエラーが報告されることの確認
TEST(JsonDecodeTest, ReportsMissingRequiredAttribute)
{
    // Arrange
    cJSON *json = cJSON_CreateObject();
    Sample sample = {}; // [状態] - 必須キーのない JSON を用意する。

    ASSERT_NE(nullptr, json); // [状態確認] - cJSON_CreateObject() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_json_decode(
        &kDescriptor, json,
        &sample); // [手順] - 必須キーが欠落した JSON を指定して struct_meta_json_decode() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_ERR_MISSING_REQUIRED,
              actual); // [確認_異常系] - struct_meta_json_decode() の戻り値が CPLAT_ERR_MISSING_REQUIRED であること。

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
                     &sample); // [手順] - 整数配列と 16 進文字列を含む JSON を指定して decode_bytes() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual);               // [確認_正常系] - decode_bytes() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(-128, sample.signed_values[0]);  // [確認_正常系] - sample.signed_values[0] が -128 であること。
    EXPECT_EQ(255, sample.unsigned_values[2]); // [確認_正常系] - sample.unsigned_values[2] が 255 であること。
    EXPECT_EQ(0, sample.hex_values[0]);        // [確認_正常系] - sample.hex_values[0] が 0 であること。
    EXPECT_EQ(0xa5, sample.hex_values[1]);     // [確認_正常系] - sample.hex_values[1] が 0xa5 であること。
    EXPECT_EQ(0xff, sample.hex_values[2]);     // [確認_正常系] - sample.hex_values[2] が 0xff であること。
}

// 不正な 16 進数文字列が拒否されバイト配列が部分更新されないことの確認
TEST(JsonDecodeTest, rejects_invalid_hex_without_partial_update)
{
    // Arrange
    ByteArrays sample = {{0, 0, 0}, {0, 0, 0}, {1, 2, 3}};

    // Pre-Assert

    // Act
    int actual = decode_bytes("{\"hex_values\":\"aa bb gg\"}",
                              &sample); // [手順] - 不正な 16 進文字列を含む JSON を指定して decode_bytes() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING,
              actual); // [確認_異常系] - decode_bytes() の戻り値が CPLAT_ERR_INVALID_ENCODING であること。
    EXPECT_EQ(
        1,
        sample.hex_values[0]); // [確認_異常系] - sample.hex_values[0] が 1 であること。(先頭要素を更新しないことの確認)
    EXPECT_EQ(
        2,
        sample.hex_values[1]); // [確認_異常系] - sample.hex_values[1] が 2 であること。(中央要素を更新しないことの確認)
    EXPECT_EQ(
        3,
        sample.hex_values[2]); // [確認_異常系] - sample.hex_values[2] が 3 であること。(末尾要素を更新しないことの確認)
}
