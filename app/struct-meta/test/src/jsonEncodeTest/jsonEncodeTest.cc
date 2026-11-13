#include <cstring>
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
    int ret = struct_meta_json_encode(
        &kWidthsDescriptor, &sample,
        &json); // [手順] - 各幅の整数値を含む構造体を指定して struct_meta_json_encode() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret); // [確認_正常系] - struct_meta_json_encode() の戻り値が CPLAT_OK であること。
    ASSERT_NE(nullptr, json); // [確認_正常系] - json が nullptr 以外であること。
    EXPECT_DOUBLE_EQ(-999999999999999.0, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "wide")->valuedouble);
    // [確認_正常系] - "wide" を取得した cjson_app_cJSON_GetObjectItemCaseSensitive() の戻り値の valuedouble が -999999999999999.0 であること。
    EXPECT_DOUBLE_EQ(4294967295.0, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "flags")->valuedouble);
    // [確認_正常系] - "flags" を取得した cjson_app_cJSON_GetObjectItemCaseSensitive() の戻り値の valuedouble が 4294967295.0 であること。
    EXPECT_DOUBLE_EQ(-32768.0, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "offset")->valuedouble);
    // [確認_正常系] - "offset" を取得した cjson_app_cJSON_GetObjectItemCaseSensitive() の戻り値の valuedouble が -32768.0 であること。
    EXPECT_DOUBLE_EQ(255.0, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "rank")->valuedouble);
    // [確認_正常系] - "rank" を取得した cjson_app_cJSON_GetObjectItemCaseSensitive() の戻り値の valuedouble が 255.0 であること。

    // Cleanup
    cjson_app_cJSON_Delete(json);
}

// 64 ビット整数の全境界値が丸めなく正確に JSON へエンコードされることの確認
TEST(JsonEncodeTest, encodes_64_bit_integer_limits_exactly)
{
    // Arrange
    Integer64Limits sample = {INT64_MIN, INT64_MAX, UINT64_MAX};
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int actual =
        struct_meta_json_encode(&kInteger64LimitsDescriptor, &sample,
                                &json); // [手順] - 境界値を含む構造体を指定して struct_meta_json_encode() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual); // [確認_正常系] - struct_meta_json_encode() の戻り値が CPLAT_OK であること。
    ASSERT_NE(nullptr, json);    // [確認_正常系] - json が nullptr 以外であること。
    int64_t minimum = 0;
    int64_t maximum = 0;
    uint64_t unsigned_maximum = 0U;
    EXPECT_TRUE(cjson_app_cJSON_GetInt64Value(
        cjson_app_cJSON_GetObjectItemCaseSensitive(json, "minimum"),
        &minimum)); // [確認_正常系] - "minimum" を取得した cjson_app_cJSON_GetInt64Value() の戻り値が true であること。
    EXPECT_TRUE(cjson_app_cJSON_GetInt64Value(
        cjson_app_cJSON_GetObjectItemCaseSensitive(json, "maximum"),
        &maximum)); // [確認_正常系] - "maximum" を取得した cjson_app_cJSON_GetInt64Value() の戻り値が true であること。
    EXPECT_TRUE(cjson_app_cJSON_GetUInt64Value(
        cjson_app_cJSON_GetObjectItemCaseSensitive(json, "unsigned_maximum"),
        &unsigned_maximum)); // [確認_正常系] - "unsigned_maximum" を取得した cjson_app_cJSON_GetUInt64Value() の戻り値が true であること。
    EXPECT_EQ(INT64_MIN, minimum);           // [確認_正常系] - minimum が INT64_MIN であること。
    EXPECT_EQ(INT64_MAX, maximum);           // [確認_正常系] - maximum が INT64_MAX であること。
    EXPECT_EQ(UINT64_MAX, unsigned_maximum); // [確認_正常系] - unsigned_maximum が UINT64_MAX であること。
    char *text = cjson_app_cJSON_PrintUnformatted(json);
    ASSERT_NE(nullptr, text); // [確認_正常系] - cjson_app_cJSON_PrintUnformatted() の戻り値が nullptr 以外であること。
    EXPECT_STREQ(
        "{\"minimum\":-9223372036854775808,\"maximum\":9223372036854775807,"
        "\"unsigned_maximum\":18446744073709551615}",
        text); // [確認_正常系] - cjson_app_cJSON_PrintUnformatted() の戻り値が "{\"minimum\":-9223372036854775808,\"maximum\":9223372036854775807,\"unsigned_maximum\":18446744073709551615}" であること。

    // Cleanup
    cjson_app_cJSON_free(text);
    cjson_app_cJSON_Delete(json);
}

// json.name 属性および json.ignore 属性に従って JSON が生成されることの確認
TEST(JsonEncodeTest, UsesGenericJsonAttributes)
{
    // Arrange
    Sample sample = {42, 7}; // [状態] - 名前変更属性と除外属性を持つ値を用意する。
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int ret = struct_meta_json_encode(
        &kDescriptor, &sample, &json); // [手順] - 属性付き構造体を指定して struct_meta_json_encode() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret); // [確認_正常系] - struct_meta_json_encode() の戻り値が CPLAT_OK であること。
    ASSERT_NE(nullptr, json); // [確認_正常系] - json が nullptr 以外であること。
    EXPECT_EQ(42, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "person_id")->valueint);
    // [確認_正常系] - "person_id" を取得した cjson_app_cJSON_GetObjectItemCaseSensitive() の戻り値の valueint が 42 であること。
    EXPECT_EQ(nullptr, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "id"));
    // [確認_正常系] - "id" を取得した cjson_app_cJSON_GetObjectItemCaseSensitive() の戻り値が nullptr であること。
    EXPECT_EQ(nullptr, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "hidden"));
    // [確認_正常系] - "hidden" を取得した cjson_app_cJSON_GetObjectItemCaseSensitive() の戻り値が nullptr であること。

    // Cleanup
    cjson_app_cJSON_Delete(json);
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
    int actual = struct_meta_json_encode(
        &descriptor, &sample, &json); // [手順] - 破損した記述子を指定して struct_meta_json_encode() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR,
              actual); // [確認_異常系] - struct_meta_json_encode() の戻り値が CPLAT_ERR_CORRUPT_DESCRIPTOR であること。
    EXPECT_EQ(nullptr, json); // [確認_異常系] - json が nullptr であること。
}

// バイト配列フィールドが指定フォーマットに従い整数配列または 16 進数文字列としてエンコードされることの確認
TEST(JsonEncodeTest, encodes_byte_arrays_in_selected_format)
{
    // Arrange
    ByteArrays sample = {{-128, 0, 127}, {0, 128, 255}, {0, 165, 255}};
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int actual = struct_meta_json_encode(
        &kByteDescriptor, &sample,
        &json); // [手順] - バイト配列を含む構造体を指定して struct_meta_json_encode() を呼び出す。

    // Assert
    ASSERT_EQ(CPLAT_OK, actual); // [確認_正常系] - struct_meta_json_encode() の戻り値が CPLAT_OK であること。
    ASSERT_NE(nullptr, json);    // [確認_正常系] - json が nullptr 以外であること。
    const cJSON *signed_values = cjson_app_cJSON_GetObjectItemCaseSensitive(json, "signed_values");
    const cJSON *unsigned_values = cjson_app_cJSON_GetObjectItemCaseSensitive(json, "unsigned_values");
    const cJSON *hex_values = cjson_app_cJSON_GetObjectItemCaseSensitive(json, "hex_values");
    ASSERT_TRUE(cjson_app_cJSON_IsArray(
        signed_values)); // [確認_正常系] - signed_values に対する cjson_app_cJSON_IsArray() の戻り値が true であること。
    EXPECT_DOUBLE_EQ(
        -128.0,
        cjson_app_cJSON_GetArrayItem(signed_values, 0)
            ->valuedouble); // [確認_正常系] - signed_values の要素 0 を取得した cjson_app_cJSON_GetArrayItem() の戻り値の valuedouble が -128.0 であること。
    ASSERT_TRUE(cjson_app_cJSON_IsArray(
        unsigned_values)); // [確認_正常系] - unsigned_values に対する cjson_app_cJSON_IsArray() の戻り値が true であること。
    EXPECT_DOUBLE_EQ(
        255.0,
        cjson_app_cJSON_GetArrayItem(unsigned_values, 2)
            ->valuedouble); // [確認_正常系] - unsigned_values の要素 2 を取得した cjson_app_cJSON_GetArrayItem() の戻り値の valuedouble が 255.0 であること。
    ASSERT_TRUE(cjson_app_cJSON_IsString(hex_values)); // [確認_正常系] - cjson_app_cJSON_IsString() の戻り値が true であること。
    EXPECT_STREQ(
        "00 a5 ff",
        cjson_app_cJSON_GetStringValue(hex_values)); // [確認_正常系] - cjson_app_cJSON_GetStringValue() の戻り値が "00 a5 ff" であること。

    // Cleanup
    cjson_app_cJSON_Delete(json);
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
    int actual = struct_meta_json_encode(
        &descriptor, &sample,
        &json); // [手順] - NUL のない配列を含む構造体を指定して struct_meta_json_encode() を呼び出す。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING,
              actual); // [確認_異常系] - struct_meta_json_encode() の戻り値が CPLAT_ERR_INVALID_ENCODING であること。
    EXPECT_EQ(nullptr, json); // [確認_異常系] - json が nullptr であること。
}

// float/double の大小両方の幅不一致を直下、配列、ネストで拒否することの確認
TEST(jsonEncodeTest, rejects_mismatched_floating_widths)
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
                    cJSON *json = nullptr;

                    // Pre-Assert

                    // Act
                    int ret = struct_meta_json_encode(descriptor, reinterpret_cast<const void *>(1), &json);
                    // [手順] - 不正幅の記述子と読み出せないデータ アドレスを渡す (16 回)。

                    // Assert
                    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, ret);
                    // [確認_異常系 回数=2*2*2*2] - データを読み出す前に不正幅を拒否すること (16 件)。
                    EXPECT_EQ(nullptr, json); // [確認_異常系 回数=2*2*2*2] - JSON を出力しないこと (16 件)。

                    // Cleanup
                    cjson_app_cJSON_Delete(json);
                }
            }
        }
    }
}

// 正しい幅の float と double を処理できることの確認
TEST(jsonEncodeTest, accepts_matching_floating_widths)
{
    // Arrange
    struct FloatingValues
    {
        float single;
        char pad[4]; /* 明示的アラインメント。0 を指定する。 */
        double wide;
    } sample = {1.5F, {}, 2.5};
    const struct_meta_field fields[] = {
        {"single", STRUCT_META_FIELD_FLOAT, 0, offsetof(FloatingValues, single), sizeof(float), 1, 0, nullptr, nullptr,
         nullptr, 0},
        {"wide", STRUCT_META_FIELD_DOUBLE, 0, offsetof(FloatingValues, wide), sizeof(double), 1, 0, nullptr, nullptr,
         nullptr, 0},
    };
    const struct_meta_descriptor descriptor = {"FloatingValues", sizeof(sample), fields, 2, nullptr, nullptr, 0};
    cJSON *json = nullptr;

    // Pre-Assert

    // Act
    int ret = struct_meta_json_encode(&descriptor, &sample, &json); // [手順] - float と double を JSON にする。

    // Assert
    ASSERT_EQ(CPLAT_OK, ret);                                             // [確認_正常系] - エンコードに成功すること。
    ASSERT_NE(nullptr, json);                                             // [確認_正常系] - JSON を出力すること。
    ASSERT_NE(nullptr, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "single")); // [確認_正常系] - float の項目があること。
    ASSERT_NE(nullptr, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "wide"));   // [確認_正常系] - double の項目があること。
    EXPECT_DOUBLE_EQ(1.5, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "single")->valuedouble);
    // [確認_正常系] - float 値が一致すること。
    EXPECT_DOUBLE_EQ(2.5, cjson_app_cJSON_GetObjectItemCaseSensitive(json, "wide")->valuedouble);
    // [確認_正常系] - double 値が一致すること。

    // Cleanup
    cjson_app_cJSON_Delete(json);
}
