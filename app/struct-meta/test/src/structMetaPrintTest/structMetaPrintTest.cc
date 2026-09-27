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
    ASSERT_NE(nullptr, stream); // [状態確認] - 一時出力先を作成できること。

    // Pre-Assert

    // Act
    int actual = struct_meta_print_write(&kDescriptor, &sample, stream); // [手順] - バイト配列を表示する。
    rewind(stream);
    char output[256] = {};
    size_t read_size = fread(output, 1U, sizeof(output) - 1U, stream);

    // Assert
    ASSERT_EQ(CPLAT_OK, actual);                                  // [確認_正常系] - 表示に成功すること。
    ASSERT_GT(read_size, 0U);                                     // [確認_正常系] - 出力が存在すること。
    EXPECT_NE(nullptr, strstr(output, "integer_values[0] = -1")); // [確認_正常系] - 既定形式を整数で表示すること。
    EXPECT_NE(nullptr, strstr(output, "hex_values = 00 a5 ff"));  // [確認_正常系] - hex形式を一行で表示すること。

    // Cleanup
    (void)fclose(stream);
    (void)cplat_remove(path, nullptr);
}
