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
    FILE *stream = tmpfile();   // [状態] - 255 文字の文字列と一時出力先を用意する。
    ASSERT_NE(nullptr, stream); // [状態確認] - 一時ファイルを開けること。

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
    fclose(stream);
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
    FILE *stream = tmpfile();   // [状態] - 終端のない文字列を用意する。
    ASSERT_NE(nullptr, stream); // [状態確認] - 一時出力先を開けること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&descriptor, &sample, stream); // [手順] - 文字列を表示する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING, actual); // [確認_異常系] - 配列の外まで読み取らず拒否すること。

    // Cleanup
    fclose(stream);
}

// 出力できないストリームを成功として扱わないことの確認
TEST(structMetaPrintTest, reports_stream_write_failure)
{
    // Arrange
    ByteArrays sample = {};
    char path[PLATFORM_PATH_MAX] = {};
    FILE *temporary = cplat_fopen_temp("smp", "wb", path, sizeof(path), nullptr);
    ASSERT_NE(nullptr, temporary); // [状態確認] - 一時ファイルを作成できること。
    fclose(temporary);
    FILE *stream = fopen(path, "rb"); // [状態] - 書き込みできない読み取り専用ストリームを用意する。
    ASSERT_NE(nullptr, stream);       // [状態確認] - 読み取り用に開けること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&kDescriptor, &sample, stream); // [手順] - 書き込み失敗を検出する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_UNKNOWN, actual); // [確認_異常系] - 出力失敗を返すこと。

    // Cleanup
    fclose(stream);
    remove(path);
}
