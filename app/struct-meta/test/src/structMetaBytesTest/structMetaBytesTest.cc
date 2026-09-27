#include <cplat/base/result.h>
#include <struct_meta/meta/bytes.h>
#include <testfw.h>

#include <cstdint>
#include <limits>

namespace
{
const struct_meta_attribute kHexAttributes[] = {{"meta.format", "hex"}};
const struct_meta_attribute kBadFormatAttributes[] = {{"meta.format", "base64"}};
const struct_meta_attribute kBytesAttributes[] = {{"meta.kind", "bytes"}};
const struct_meta_field kScalar = {
    "scalar", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 0, 1, 1, 0, nullptr, nullptr, nullptr, 0};
const struct_meta_field kBytes = {"bytes", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 0, 1, 3, 0, nullptr, nullptr, nullptr,
                                  0};
const struct_meta_field kHexBytes = {
    "hex", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 0, 1, 3, 0, nullptr, nullptr, kHexAttributes, 1};
const struct_meta_field kBadFormat = {
    "bad", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 0, 1, 3, 0, nullptr, nullptr, kBadFormatAttributes, 1};
const struct_meta_field kScalarBytes = {
    "bad", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, 0, 1, 1, 0, nullptr, nullptr, kBytesAttributes, 1};
} // namespace

// バイト配列フィールドの判定およびフォーマット属性の取得が正しく行われることの確認
TEST(structMetaBytesTest, classifies_byte_arrays_and_formats)
{
    // Arrange
    struct_meta_internal_byte_format integer_format = STRUCT_META_INTERNAL_BYTE_FORMAT_HEX;
    struct_meta_internal_byte_format hex_format = STRUCT_META_INTERNAL_BYTE_FORMAT_INTEGER;

    // Pre-Assert

    // Act
    int integer_ret = struct_meta_internal_field_byte_format(&kBytes, &integer_format); // [手順] - 既定形式を取得する。
    int hex_ret = struct_meta_internal_field_byte_format(&kHexBytes, &hex_format);      // [手順] - 16進形式を取得する。

    // Assert
    EXPECT_EQ(0, struct_meta_internal_field_is_byte_array(nullptr)); // [確認_異常系] - NULL はバイト配列でないこと。
    EXPECT_EQ(0,
              struct_meta_internal_field_is_byte_array(&kScalar));   // [確認_正常系] - スカラーはバイト配列でないこと。
    EXPECT_EQ(1, struct_meta_internal_field_is_byte_array(&kBytes)); // [確認_正常系] - 1バイト整数配列を認識すること。
    EXPECT_EQ(CPLAT_OK, integer_ret);                                // [確認_正常系] - 既定形式を取得できること。
    EXPECT_EQ(STRUCT_META_INTERNAL_BYTE_FORMAT_INTEGER, integer_format); // [確認_正常系] - 既定が整数形式であること。
    EXPECT_EQ(CPLAT_OK, hex_ret);                                // [確認_正常系] - 属性付き形式を取得できること。
    EXPECT_EQ(STRUCT_META_INTERNAL_BYTE_FORMAT_HEX, hex_format); // [確認_正常系] - 16進形式であること。
}

// 未知のフォーマット属性や不正なバイト指定が正しく拒否されることの確認
TEST(structMetaBytesTest, rejects_invalid_meta_attributes)
{
    // Arrange
    struct_meta_internal_byte_format format = STRUCT_META_INTERNAL_BYTE_FORMAT_INTEGER;

    // Pre-Assert

    // Act
    int bad_format = struct_meta_internal_field_byte_format(&kBadFormat, &format); // [手順] - 未知の形式を解釈する。
    int scalar_bytes =
        struct_meta_internal_field_byte_format(&kScalarBytes, &format); // [手順] - スカラーへbytesを指定する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, bad_format);   // [確認_異常系] - 未知の形式を拒否すること。
    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, scalar_bytes); // [確認_異常系] - スカラーのbytes指定を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, struct_meta_internal_field_byte_format(
                                              nullptr, &format)); // [確認_異常系] - NULLフィールドを拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT,
              struct_meta_internal_field_byte_format(&kBytes, nullptr)); // [確認_異常系] - NULL出力を拒否すること。
}

// 固定長バイト配列と 16 進数文字列の相互変換が正しく行われることの確認
TEST(structMetaBytesTest, converts_fixed_length_hex_text)
{
    // Arrange
    const unsigned char source[] = {0x00U, 0xa5U, 0xffU};
    unsigned char decoded[] = {1U, 2U, 3U};
    char text[9] = {};

    // Pre-Assert

    // Act
    int encode_ret =
        struct_meta_internal_bytes_to_hex(source, 3U, text, sizeof(text)); // [手順] - 16進文字列へ変換する。
    int decode_ret =
        struct_meta_internal_bytes_from_hex(decoded, 3U, "00  A5 ff"); // [手順] - 複数空白と大文字を含む入力を戻す。

    // Assert
    EXPECT_EQ(CPLAT_OK, encode_ret); // [確認_正常系] - エンコードできること。
    EXPECT_STREQ("00 a5 ff", text);  // [確認_正常系] - 正規形が小文字・単一空白であること。
    EXPECT_EQ(CPLAT_OK, decode_ret); // [確認_正常系] - デコードできること。
    EXPECT_EQ(0x00U, decoded[0]);    // [確認_正常系] - 先頭バイトを復元すること。
    EXPECT_EQ(0xa5U, decoded[1]);    // [確認_正常系] - 中央バイトを復元すること。
    EXPECT_EQ(0xffU, decoded[2]);    // [確認_正常系] - 末尾バイトを復元すること。
}

// 不正な 16 進数表記が拒否されバッファーが書き換えられないことの確認
TEST(structMetaBytesTest, rejects_invalid_hex_without_writing)
{
    // Arrange
    unsigned char bytes[] = {1U, 2U, 3U};

    // Pre-Assert

    // Act
    int invalid_digit = struct_meta_internal_bytes_from_hex(bytes, 3U, "aa bb gg");   // [手順] - 不正な桁を与える。
    int leading_space = struct_meta_internal_bytes_from_hex(bytes, 3U, " aa bb cc");  // [手順] - 先頭空白を与える。
    int trailing_space = struct_meta_internal_bytes_from_hex(bytes, 3U, "aa bb cc "); // [手順] - 末尾空白を与える。

    // Assert
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING, invalid_digit);  // [確認_異常系] - 不正な桁を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING, leading_space);  // [確認_異常系] - 先頭空白を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ENCODING, trailing_space); // [確認_異常系] - 末尾空白を拒否すること。
    EXPECT_EQ(1U, bytes[0]);                               // [確認_異常系] - 先頭バイトを更新しないこと。
    EXPECT_EQ(2U, bytes[1]);                               // [確認_異常系] - 中央バイトを更新しないこと。
    EXPECT_EQ(3U, bytes[2]);                               // [確認_異常系] - 末尾バイトを更新しないこと。
}

// 16 進数文字列の必要サイズ計算とバッファー境界値が正しく検証されることの確認
TEST(structMetaBytesTest, validates_sizes_and_buffers)
{
    // Arrange
    size_t text_size = 0U;
    const unsigned char byte = 0U;
    char short_buffer[2] = {};

    // Pre-Assert

    // Act
    int size_ret = struct_meta_internal_bytes_hex_text_size(3U, &text_size); // [手順] - 必要サイズを求める。

    // Assert
    EXPECT_EQ(CPLAT_OK, size_ret); // [確認_正常系] - 必要サイズを計算できること。
    EXPECT_EQ(9U, text_size);      // [確認_正常系] - NULを含む必要サイズであること。
    EXPECT_EQ(CPLAT_ERR_BUFFER_TOO_SMALL,
              struct_meta_internal_bytes_to_hex(&byte, 1U, short_buffer,
                                                sizeof(short_buffer))); // [確認_異常系] - 小さい出力先を拒否すること。
    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, struct_meta_internal_bytes_hex_text_size(
                                          std::numeric_limits<size_t>::max(),
                                          &text_size)); // [確認_異常系] - サイズのオーバーフローを拒否すること。
}
