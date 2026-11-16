#include <cplat/base/platform.h>
#include <cplat/base/result.h>
#include <cplat/crt/path.h>
#include <cplat/crt/stdio.h>
#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <struct_meta/print/print.h>
#include <testfw.h>
#include <mock_stdio.h>
#include <mock_cplat.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

namespace
{
struct ByteArrays
{
    int8_t integer_values[3];
    uint8_t hex_values[3];
};
const struct_meta_attribute kHexAttributes[] = {{"meta.format", "hex"}};
const struct_meta_field kFields[] = {
    {"integer_values", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(ByteArrays, integer_values), sizeof(int8_t), 3, 0,
     nullptr, nullptr, nullptr, 0},
    {"hex_values", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(ByteArrays, hex_values), sizeof(uint8_t), 3, 0,
     nullptr, nullptr, kHexAttributes, 1},
};
const struct_meta_descriptor kDescriptor = {"ByteArrays", sizeof(ByteArrays), kFields, 2, nullptr, nullptr, 0};
} // namespace

// バイト配列のフィールドが指定されたフォーマット属性に従って整形出力されることの確認
TEST(structMetaPrintTest, prints_byte_arrays_in_selected_format)
{
    // Arrange
    ByteArrays sample = {{-1, 0, 1}, {0, 165, 255}};
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smp", "w+b", path, sizeof(path), nullptr); // [状態] - 一時出力先を作成する。
    ASSERT_NE(nullptr, stream); // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(
        &kDescriptor, &sample,
        stream); // [手順] - バイト配列を含む構造体を指定して struct_meta_print_write() を呼び出す。
    rewind(stream);
    char output[256] = {};
    size_t read_size = fread(output, 1U, sizeof(output) - 1U, stream);

    // Assert
    ASSERT_EQ(CPLAT_OK, actual); // [確認_正常系] - struct_meta_print_write() の戻り値が CPLAT_OK であること。
    ASSERT_GT(read_size, 0U);    // [確認_正常系] - fread() の戻り値が 0U より大きいこと。(出力が存在することの確認)
    EXPECT_NE(
        nullptr,
        strstr(
            output,
            "integer_values[0] = -1")); // [確認_正常系] - "integer_values[0] = -1" を探した strstr() の戻り値が nullptr 以外であること。
    EXPECT_NE(
        nullptr,
        strstr(
            output,
            "hex_values = 00 a5 ff")); // [確認_正常系] - "hex_values = 00 a5 ff" を探した strstr() の戻り値が nullptr 以外であること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 内部作業領域より長い文字列も引用符付きで末尾まで表示することの確認
TEST(structMetaPrintTest, prints_complete_long_string)
{
    // Arrange
    struct LongString
    {
        char text[256];
    } sample = {};
    memset(sample.text, 'x', sizeof(sample.text) - 1U);
    const struct_meta_field fields[] = {{"text", STRUCT_META_FIELD_CHAR_ARRAY, 0, offsetof(LongString, text),
                                         sizeof(char), 1, 256, nullptr, nullptr, nullptr, 0}};
    const struct_meta_descriptor descriptor = {"LongString", sizeof(LongString), fields, 1, nullptr, nullptr, 0};
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smp", "w+b", path, sizeof(path),
                                    nullptr); // [状態] - 255 文字の文字列と一時出力先を用意する。
    ASSERT_NE(nullptr, stream);               // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - 長い文字列を表示する。
    rewind(stream);
    char output[512] = {};
    size_t count = fread(output, 1, sizeof(output) - 1, stream);

    // Assert
    EXPECT_EQ(CPLAT_OK, actual); // [確認_正常系] - 出力に成功すること。
    EXPECT_GT(count, 255U);
    EXPECT_NE(nullptr, strstr(output, sample.text)); // [確認_正常系] - 文字列の全体を含むこと。
    EXPECT_NE(nullptr, strstr(output, "x\"\n"));     // [確認_正常系] - 閉じ引用符が残ること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 終端のない文字列を表示しようとした場合に拒否することの確認
TEST(structMetaPrintTest, rejects_unterminated_string)
{
    // Arrange
    struct Text
    {
        char value[3];
    } sample = {{'a', 'b', 'c'}};
    const struct_meta_field fields[] = {{"value", STRUCT_META_FIELD_CHAR_ARRAY, 0, offsetof(Text, value), sizeof(char),
                                         1, sizeof(sample.value), nullptr, nullptr, nullptr, 0}};
    const struct_meta_descriptor descriptor = {"Text", sizeof(Text), fields, 1, nullptr, nullptr, 0};
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smp", "w+b", path, sizeof(path), nullptr); // [状態] - 終端のない文字列を用意する。
    ASSERT_NE(nullptr, stream); // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - 文字列を表示する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING, actual); // [確認_異常系] - 配列の外まで読み取らず拒否すること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 出力できないストリームを成功として扱わないことの確認
TEST(structMetaPrintTest, reports_stream_write_failure)
{
    // Arrange
    ByteArrays sample = {};
    char path[PLATFORM_PATH_MAX] = {};
    FILE *temporary = cplat_fopen_temp("smp", "wb", path, sizeof(path), nullptr);
    ASSERT_NE(nullptr, temporary); // [状態確認] - 一時ファイルを作成できること。
    (void)fclose(temporary);
    FILE *stream = cplat_fopen(path, "rb", nullptr); // [状態] - 書き込みできない読み取り専用ストリームを用意する。
    ASSERT_NE(nullptr, stream);                      // [状態確認] - cplat_fopen() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&kDescriptor, &sample, stream); // [手順] - 書き込み失敗を検出する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_UNKNOWN, actual); // [確認_異常系] - 出力失敗を返すこと。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// float/double の大小両方の幅不一致を直下、配列、ネストで拒否することの確認
TEST(structMetaPrintTest, rejects_mismatched_floating_widths)
{
    // Arrange
    const struct_meta_field_kind kinds[] = {STRUCT_META_FIELD_FLOAT, STRUCT_META_FIELD_DOUBLE};
    const size_t widths[] = {sizeof(float), sizeof(double)};
    unsigned char storage[128];
    memset(storage, 0x5a, sizeof(storage));
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smf", "w+b", path, sizeof(path), nullptr);
    ASSERT_NE(nullptr, stream); // [状態確認] - 空の一時出力先を用意できること。

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

                    // Pre-Assert

                    // Act
                    int ret = struct_meta_print_write(descriptor, reinterpret_cast<const void *>(1), stream);
                    // [手順] - 不正幅の記述子と読み出せないデータ アドレスを渡す (16 回)。

                    // Assert
                    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, ret);
                    // [確認_異常系 回数=2*2*2*2] - データを読み出す前に不正幅を拒否すること (16 件)。
                    EXPECT_EQ(0L, ftell(stream));
                    // [確認_異常系 回数=2*2*2*2] - 見出しを含め何も出力しないこと (16 件)。
                }
            }
        }
    }

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 正しい幅の float と double を処理できることの確認
TEST(structMetaPrintTest, accepts_matching_floating_widths)
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
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smf", "w+b", path, sizeof(path), nullptr);
    ASSERT_NE(nullptr, stream); // [状態確認] - 一時出力先を作成できること。

    // Pre-Assert

    // Act
    int ret = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - float と double を表示する。
    rewind(stream);
    char output[256] = {};
    (void)fread(output, 1, sizeof(output) - 1U, stream);

    // Assert
    EXPECT_EQ(CPLAT_OK, ret);                           // [確認_正常系] - 表示に成功すること。
    EXPECT_NE(nullptr, strstr(output, "single = 1.5")); // [確認_正常系] - float 値を表示すること。
    EXPECT_NE(nullptr, strstr(output, "wide = 2.5"));   // [確認_正常系] - double 値を表示すること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 128 バイトを超える長いフィールド名と複数要素の配列インデックスが切り詰められずに全文出力されることの確認
TEST(structMetaPrintTest, prints_complete_long_field_names_and_multi_index_arrays)
{
    // Arrange
    struct ChildValues
    {
        uint8_t items[2];
    };
    struct LongNamesAndArrays
    {
        int32_t scalar_value;
        uint16_t array_values[12];
        ChildValues nested_value;
        uint16_t pad;
    };

    std::string long_scalar_name(160U, 'a');
    std::string long_array_name(160U, 'b');
    std::string long_nested_name(160U, 'c');
    std::string long_child_name(160U, 'd');

    const struct_meta_field child_fields[] = {
        {long_child_name.c_str(), STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(ChildValues, items), sizeof(uint8_t),
         2, 0, nullptr, nullptr, nullptr, 0},
    };
    const struct_meta_descriptor child_descriptor = {
        "ChildValues", sizeof(ChildValues), child_fields, 1, nullptr, nullptr, 0};
    const struct_meta_field parent_fields[] = {
        {long_scalar_name.c_str(), STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(LongNamesAndArrays, scalar_value),
         sizeof(int32_t), 1, 0, nullptr, nullptr, nullptr, 0},
        {long_array_name.c_str(), STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(LongNamesAndArrays, array_values),
         sizeof(uint16_t), 12, 0, nullptr, nullptr, nullptr, 0},
        {long_nested_name.c_str(), STRUCT_META_FIELD_STRUCT, 0, offsetof(LongNamesAndArrays, nested_value),
         sizeof(ChildValues), 1, 0, &child_descriptor, nullptr, nullptr, 0},
    };
    const struct_meta_descriptor parent_descriptor = {
        "LongNamesAndArrays", sizeof(LongNamesAndArrays), parent_fields, 3, nullptr, nullptr, 0};

    LongNamesAndArrays sample = {42, {10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U, 21U}, {{1U, 2U}}, 0U};
    std::string expected = "LongNamesAndArrays:\n  " + long_scalar_name + " = 42\n";
    for (size_t i = 0U; i < 12U; i++)
    {
        expected +=
            "  " + long_array_name + "[" + std::to_string(i) + "] = " + std::to_string(sample.array_values[i]) + "\n";
    }
    expected += "  " + long_nested_name + ":\n";
    expected += "    " + long_child_name + "[0] = 1\n";
    expected += "    " + long_child_name + "[1] = 2\n";

    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("sml", "w+b", path, sizeof(path),
                                    nullptr); // [状態] - 160 文字のフィールド名を持つ構造体と一時出力先を用意する。
    ASSERT_NE(nullptr, stream);               // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&parent_descriptor, &sample, stream); // [手順] - 長いラベルを含めて出力する。
    long output_size = ftell(stream);
    rewind(stream);
    std::string output(static_cast<size_t>(output_size > 0L ? output_size : 0L), '\0');
    if (!output.empty())
    {
        (void)fread(&output[0], 1U, output.size(), stream);
    }

    // Assert
    EXPECT_EQ(CPLAT_OK, actual); // [確認_正常系] - 長い名前とインデックスの出力が成功すること。
    EXPECT_EQ(expected, output); // [確認_正常系] - すべてのラベルと値が切り詰めなく一致すること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 符号付き・符号なし整数の各幅の上限・下限値が正しく出力されることの確認
TEST(structMetaPrintTest, prints_signed_and_unsigned_integer_bounds)
{
    // Arrange
    struct IntegerBounds
    {
        int64_t s64_min;
        int64_t s64_max;
        uint64_t u64_min;
        uint64_t u64_max;
        int32_t s32_min;
        int32_t s32_max;
        uint32_t u32_min;
        uint32_t u32_max;
        int16_t s16_min;
        int16_t s16_max;
        uint16_t u16_min;
        uint16_t u16_max;
        int8_t s8_min;
        int8_t s8_max;
        uint8_t u8_min;
        uint8_t u8_max;
        uint32_t pad;
    } sample = {INT64_MIN, INT64_MAX, 0U,         UINT64_MAX, INT32_MIN, INT32_MAX, 0U,        UINT32_MAX, INT16_MIN,
                INT16_MAX, 0U,        UINT16_MAX, INT8_MIN,   INT8_MAX,  0U,        UINT8_MAX, 0U};

    const struct_meta_field fields[] = {
        {"s8_min", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(IntegerBounds, s8_min), sizeof(int8_t), 1, 0, nullptr,
         nullptr, nullptr, 0},
        {"s8_max", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(IntegerBounds, s8_max), sizeof(int8_t), 1, 0, nullptr,
         nullptr, nullptr, 0},
        {"s16_min", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(IntegerBounds, s16_min), sizeof(int16_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"s16_max", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(IntegerBounds, s16_max), sizeof(int16_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"s32_min", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(IntegerBounds, s32_min), sizeof(int32_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"s32_max", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(IntegerBounds, s32_max), sizeof(int32_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"s64_min", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(IntegerBounds, s64_min), sizeof(int64_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"s64_max", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(IntegerBounds, s64_max), sizeof(int64_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"u8_min", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(IntegerBounds, u8_min), sizeof(uint8_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"u8_max", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(IntegerBounds, u8_max), sizeof(uint8_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"u16_min", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(IntegerBounds, u16_min), sizeof(uint16_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"u16_max", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(IntegerBounds, u16_max), sizeof(uint16_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"u32_min", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(IntegerBounds, u32_min), sizeof(uint32_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"u32_max", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(IntegerBounds, u32_max), sizeof(uint32_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"u64_min", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(IntegerBounds, u64_min), sizeof(uint64_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
        {"u64_max", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(IntegerBounds, u64_max), sizeof(uint64_t), 1, 0,
         nullptr, nullptr, nullptr, 0},
    };
    const struct_meta_descriptor descriptor = {"IntegerBounds", sizeof(IntegerBounds), fields, 16, nullptr, nullptr, 0};
    const std::string expected = "IntegerBounds:\n"
                                 "  s8_min = -128\n"
                                 "  s8_max = 127\n"
                                 "  s16_min = -32768\n"
                                 "  s16_max = 32767\n"
                                 "  s32_min = -2147483648\n"
                                 "  s32_max = 2147483647\n"
                                 "  s64_min = -9223372036854775808\n"
                                 "  s64_max = 9223372036854775807\n"
                                 "  u8_min = 0\n"
                                 "  u8_max = 255\n"
                                 "  u16_min = 0\n"
                                 "  u16_max = 65535\n"
                                 "  u32_min = 0\n"
                                 "  u32_max = 4294967295\n"
                                 "  u64_min = 0\n"
                                 "  u64_max = 18446744073709551615\n";

    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smi", "w+b", path, sizeof(path),
                                    nullptr); // [状態] - 1/2/4/8 バイト符号付き・符号なし整数上下限の出力先を用意する。
    ASSERT_NE(nullptr, stream);               // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - 整数上下限をすべて出力する。
    rewind(stream);
    char output[512] = {};
    (void)fread(output, 1U, sizeof(output) - 1U, stream);

    // Assert
    EXPECT_EQ(CPLAT_OK, actual);              // [確認_正常系] - すべての整数幅の出力が成功すること。
    EXPECT_EQ(expected, std::string(output)); // [確認_正常系] - 各整数幅の上下限文字列が一致すること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 配列を含む float と double の負値と 0 が正しく出力されることの確認
TEST(structMetaPrintTest, prints_floating_point_arrays)
{
    // Arrange
    struct FloatArrays
    {
        float f_values[2];
        double d_values[2];
    } sample = {{-1.25F, 0.0F}, {-2.5, 3.75}};
    const struct_meta_field fields[] = {
        {"f_values", STRUCT_META_FIELD_FLOAT, 0, offsetof(FloatArrays, f_values), sizeof(float), 2, 0, nullptr, nullptr,
         nullptr, 0},
        {"d_values", STRUCT_META_FIELD_DOUBLE, 0, offsetof(FloatArrays, d_values), sizeof(double), 2, 0, nullptr,
         nullptr, nullptr, 0},
    };
    const struct_meta_descriptor descriptor = {"FloatArrays", sizeof(FloatArrays), fields, 2, nullptr, nullptr, 0};
    const std::string expected = "FloatArrays:\n"
                                 "  f_values[0] = -1.25\n"
                                 "  f_values[1] = 0\n"
                                 "  d_values[0] = -2.5\n"
                                 "  d_values[1] = 3.75\n";

    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smfa", "w+b", path, sizeof(path),
                                    nullptr); // [状態] - 負値と 0 を含む float/double 配列の出力先を用意する。
    ASSERT_NE(nullptr, stream);               // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - float/double 配列を出力する。
    rewind(stream);
    char output[256] = {};
    (void)fread(output, 1U, sizeof(output) - 1U, stream);

    // Assert
    EXPECT_EQ(CPLAT_OK, actual);              // [確認_正常系] - 浮動小数点配列の出力が成功すること。
    EXPECT_EQ(expected, std::string(output)); // [確認_正常系] - 配列要素ごとの float/double 値が一致すること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 出力途中で書き込み失敗が発生した場合に失敗を返すことの確認
TEST(structMetaPrintTest, reports_mid_write_failure_during_output)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    struct TwoFields
    {
        int32_t first;
        int32_t second;
    } sample = {10, 20};
    const struct_meta_field fields[] = {
        {"first", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(TwoFields, first), sizeof(int32_t), 1, 0, nullptr,
         nullptr, nullptr, 0},
        {"second", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(TwoFields, second), sizeof(int32_t), 1, 0, nullptr,
         nullptr, nullptr, 0},
    };
    const struct_meta_descriptor descriptor = {"TwoFields", sizeof(TwoFields), fields, 2, nullptr, nullptr, 0};
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smw", "w+b", path, sizeof(path), nullptr); // [状態] - 一時出力先を用意する。
    ASSERT_NE(nullptr, stream); // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fprintf(_, _, _, stream, _))
        .WillOnce(Invoke(delegate_real_fprintf))
        .WillOnce(Invoke(delegate_real_fprintf))
        .WillOnce(Return(-1));
    // [Pre-Assert確認_異常系] - 先頭フィールド出力後、2 番目のラベル書き込みで失敗すること。
    // [Pre-Assert手順] - 最初の 2 回は実出力し、3 回目の fprintf() で -1 を返す。

    // Act
    int actual = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - 途中の書き込み失敗を発生させる。

    // Assert
    EXPECT_EQ(CPLAT_ERR_UNKNOWN, actual); // [確認_異常系] - 途中の出力失敗を呼び出し元へ伝えること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// fprintf が負値を返さなくても ferror(stream) が非 0 なら失敗を返すことの確認
TEST(structMetaPrintTest, reports_stream_error_indicator_after_write)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    struct SingleField
    {
        int32_t value;
    } sample = {7};
    const struct_meta_field field = {"value",
                                     STRUCT_META_FIELD_SIGNED_INTEGER,
                                     0,
                                     offsetof(SingleField, value),
                                     sizeof(int32_t),
                                     1,
                                     0,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     0};
    const struct_meta_descriptor descriptor = {"SingleField", sizeof(SingleField), &field, 1, nullptr, nullptr, 0};
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("sme", "w+b", path, sizeof(path), nullptr); // [状態] - 一時出力先を用意する。
    ASSERT_NE(nullptr, stream); // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    // Pre-Assert
    EXPECT_CALL(mock_stdio, ferror(_, _, _, stream)).WillOnce(Return(1));
    // [Pre-Assert確認_異常系] - ラベル書き込み直後の ferror() 検査が実行されること。
    // [Pre-Assert手順] - ferror() がストリーム エラーを表す 1 を返す。

    // Act
    int actual = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - ferror 検出経路を実行する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_UNKNOWN, actual); // [確認_異常系] - ストリーム エラーを失敗として返すこと。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}

// 数値の書式化失敗が成功へ置き換わらず呼び出し元へ伝わることの確認
TEST(structMetaPrintTest, reports_cplat_snprintf_formatting_failure)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    struct ScalarValues
    {
        double double_value;
        int32_t signed_value;
        uint32_t unsigned_value;
        float float_value;
        uint32_t pad;
    } sample = {2.5, -1, 1U, 1.5F, 0U};
    const struct_meta_field fields[] = {
        {"signed_value", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(ScalarValues, signed_value), sizeof(int32_t), 1,
         0, nullptr, nullptr, nullptr, 0},
        {"unsigned_value", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(ScalarValues, unsigned_value),
         sizeof(uint32_t), 1, 0, nullptr, nullptr, nullptr, 0},
        {"float_value", STRUCT_META_FIELD_FLOAT, 0, offsetof(ScalarValues, float_value), sizeof(float), 1, 0, nullptr,
         nullptr, nullptr, 0},
        {"double_value", STRUCT_META_FIELD_DOUBLE, 0, offsetof(ScalarValues, double_value), sizeof(double), 1, 0,
         nullptr, nullptr, nullptr, 0},
    };
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("smn", "w+b", path, sizeof(path),
                                    nullptr); // [状態] - 4 種の数値フィールドと書式化失敗条件を用意する。
    ASSERT_NE(nullptr, stream);               // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

    for (size_t i = 0U; i < 4U; i++)
    {
        // Arrange
        const struct_meta_descriptor descriptor = {
            "SingleScalar", sizeof(ScalarValues), &fields[i], 1, nullptr, nullptr, 0};
        long before_pos = ftell(stream);

        // Pre-Assert
        EXPECT_CALL(mock_cplat, cplat_snprintf(_, _, _)).WillOnce(Return(CPLAT_ERR_OUT_OF_MEMORY));
        // [Pre-Assert確認_異常系 回数=4] - 各数値種別で cplat_snprintf() が 1 回ずつ呼ばれること (4 件)。
        // [Pre-Assert手順] - cplat_snprintf() から CPLAT_ERR_OUT_OF_MEMORY を返す。

        // Act
        int actual = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - 書式化失敗を発生させる (4 回)。
        long after_pos = ftell(stream);

        // Assert
        EXPECT_EQ(CPLAT_ERR_OUT_OF_MEMORY, actual);
        // [確認_異常系 回数=4] - 書式化失敗の結果コードがそのまま呼び出し元へ返ること (4 件)。
        EXPECT_EQ(static_cast<long>(strlen("SingleScalar:\n")), after_pos - before_pos);
        // [確認_異常系 回数=4] - 書式化失敗時にフィールド ラベルや不完全な値を出力しないこと (4 件)。
    }

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}
