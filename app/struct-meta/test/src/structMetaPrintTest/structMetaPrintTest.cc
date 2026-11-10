#include <cstring>
#include <cplat/base/result.h>
#include <cplat/crt/path.h>
#include <cplat/crt/stdio.h>
#include <struct_meta/print/print.h>
#include <testfw.h>

#include <cstddef>
#include <cstdio>
#include <cstdint>

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
    FILE *stream = cplat_fopen_temp("smp", "w+b", path, sizeof(path), nullptr); // [状態] - 255 文字の文字列と一時出力先を用意する。
    ASSERT_NE(nullptr, stream); // [状態確認] - cplat_fopen_temp() の戻り値が nullptr 以外であること。

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
    ASSERT_NE(nullptr, stream); // [状態確認] - cplat_fopen() の戻り値が nullptr 以外であること。

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
        double wide;
    } sample = {1.5F, 2.5};
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
