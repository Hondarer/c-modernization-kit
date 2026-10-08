#include <testfw.h>
#include <cplat/crt/path.h>
#include <cplat/crt/stdio.h>

extern "C"
{
#include "struct_meta_gen_emit_array.h"
}

#include <inttypes.h>
#include <stdint.h>

#include <cstdio>
#include <string>

// 64ビット整数配列を改行折り返しと末尾ゼロ埋めを適用したC言語静的配列定義としてストリームへ出力できることの確認
TEST(structMetaGenEmitArrayTest, emits_words_with_line_wrapping_and_zero_padded_tail)
{
    // Arrange
    const uint64_t data[] = {
        UINT64_C(0x0123456789abcdef), UINT64_C(0xfedcba9876543210), UINT64_C(0x1111222233334444),
        UINT64_C(0xaaaabbbbccccdddd), UINT64_C(0xaaaaaaaaaaaaaaaa),
    };
    const char expected[] = "static const uint64_t s_sample[5] = {\n"
                            "    UINT64_C(0x0123456789abcdef), UINT64_C(0xfedcba9876543210), "
                            "UINT64_C(0x1111222233334444), UINT64_C(0xaaaabbbbccccdddd),\n"
                            "    UINT64_C(0x00000000000000aa)\n"
                            "};\n\n";
    char path[PLATFORM_PATH_MAX] = {};
    FILE *stream = cplat_fopen_temp("sma", "w+b", path, sizeof(path),
                                    nullptr); // [状態] - 生成結果を読み戻す一時ストリームを作成する。
    ASSERT_NE(nullptr, stream);               // [状態確認] - 生成結果を読み戻す一時ストリームを作成できること。

    // Pre-Assert

    // Act
    struct_meta_gen_emit_uint64_array(stream, "s_sample", data,
                                      33U); // [手順] - 4 ワードと 1 バイトを静的配列として出力する。
    ASSERT_EQ(0, std::fflush(stream));
    // [確認_正常系] - `std::fflush(stream)` の戻り値が `0` であること。
    std::rewind(stream);
    std::string actual;
    char buffer[256];
    for (size_t size = std::fread(buffer, 1U, sizeof(buffer), stream); size > 0U;
         size = std::fread(buffer, 1U, sizeof(buffer), stream))
    {
        actual.append(buffer, size);
    }

    // Assert
    EXPECT_EQ(expected, actual); // [確認_正常系] - ワード、改行、末尾の 0 埋めを含む配列全体が一致すること。

    // Cleanup
    (void)std::fclose(stream);
    (void)cplat_remove(path, nullptr);
}
