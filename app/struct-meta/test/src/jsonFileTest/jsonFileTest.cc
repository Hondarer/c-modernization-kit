#include <cplat/base/platform.h>
#include <cplat/base/result.h>
#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_cplat.h>
#include <struct_meta/json/file.h>
#include <struct_meta/meta/meta.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

namespace
{

struct SampleValue
{
    int32_t count;
};

const struct_meta_field kFields[] = {
    {"count", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(SampleValue, count), sizeof(int32_t), 1, 0, nullptr,
     nullptr, nullptr, 0},
};

const struct_meta_descriptor kDescriptor = {"SampleValue", sizeof(SampleValue), kFields, 1, nullptr, nullptr, 0};

} // namespace

// JSON を書き込めた場合に CPLAT_OK を返すことの確認
TEST(jsonFileTest, save_returns_ok_when_write_and_close_succeed)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    SampleValue sample = {42};
    std::string path = testing::TempDir() + "jsonFileTest_success.json"; // [状態] - 書き込み先の一時ファイルを決める。

    // Pre-Assert

    // Act
    int ret = struct_meta_json_file_save(&kDescriptor, &sample, path.c_str()); // [手順] - JSON を保存する。

    // Assert
    EXPECT_EQ(CPLAT_OK, ret); // [確認_正常系] - struct_meta_json_file_save() の戻り値が CPLAT_OK であること。

    // Cleanup
    (void)std::remove(path.c_str());
}

// 閉じる処理 (残りの書き出しを含む) が失敗した場合に CPLAT_ERR_UNKNOWN を返すことの確認
TEST(jsonFileTest, save_returns_error_when_close_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    SampleValue sample = {42};
    std::string path = testing::TempDir() + "jsonFileTest_close.json"; // [状態] - 書き込み先の一時ファイルを決める。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_fclose(_, _))
        .WillOnce(Invoke(
            [](FILE *stream, cplat_error *detail_out) -> int
            {
                (void)delegate_real_cplat_fclose(stream, detail_out);
                return EOF;
            }));
    // [Pre-Assert確認_異常系] - cplat_fclose() が 1 回呼び出されること。
    // [Pre-Assert手順] - 実関数で閉じてから EOF を返す。

    // Act
    int ret = struct_meta_json_file_save(&kDescriptor, &sample, path.c_str()); // [手順] - JSON を保存する。

    // Assert
    EXPECT_EQ(CPLAT_ERR_UNKNOWN, ret); // [確認_異常系] - struct_meta_json_file_save() の戻り値が CPLAT_ERR_UNKNOWN であること。

    // Cleanup
    (void)std::remove(path.c_str());
}
