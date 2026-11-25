#include <cplat/base/platform.h>
#include <cplat/base/result.h>
#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>
#include <mock_stdio.h>
#include <mock_cplat.h>
#include <struct_meta/catalog/catalog.h>

extern "C"
{
#include "struct_meta_gen_emit.h"
}

#include <cstdio>
#include <cstring>
#include <string>

namespace
{

/** 生成に使う、短い名前だけを持つヘッダーの内容です。 */
const char kShortHeader[] = "typedef struct emit_sample\n"
                            "{\n"
                            "    int value; /**< 値です。 */\n"
                            "} emit_sample;\n";

/** 生成ヘッダーと生成 C ソースの出力先を組み立てます。 */
struct output_paths
{
    std::string c_path;
    std::string h_path;
};

output_paths make_output_paths(const char *name)
{
    output_paths paths;
    paths.c_path = testing::TempDir() + name + ".c";
    paths.h_path = testing::TempDir() + name + ".h";
    return paths;
}

void remove_output_paths(const output_paths &paths)
{
    (void)std::remove(paths.c_path.c_str());
    (void)std::remove(paths.h_path.c_str());
}

/** 内容からカタログを作ります。 */
struct_meta_catalog *create_catalog(const std::string &text)
{
    struct_meta_catalog *catalog = nullptr;
    if (struct_meta_catalog_create_from_header_text(text.c_str(), text.size(), &catalog, nullptr) != CPLAT_OK)
    {
        return nullptr;
    }
    return catalog;
}

/** 実関数で閉じてから EOF を返す fclose() の代わりです。 */
int close_then_fail(const char *file, const int line, const char *func, FILE *stream)
{
    (void)delegate_real_fclose(file, line, func, stream);
    return EOF;
}

} // namespace

// 短い名前の記述子から C ソースとヘッダーを生成できることの確認
TEST(structMetaGenEmitTest, emits_source_and_header)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    output_paths paths = make_output_paths("smgen_success");
    struct_meta_catalog *catalog = create_catalog(kShortHeader);
    ASSERT_NE(nullptr, catalog); // [状態確認] - 短い名前のヘッダーからカタログを作れること。

    // Pre-Assert

    // Act
    testing::internal::CaptureStderr();
    int ret = struct_meta_gen_emit(catalog, "emit_sample.h", paths.c_path.c_str()); // [手順] - 生成を実行する。
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(0, ret);           // [確認_正常系] - struct_meta_gen_emit() の戻り値が 0 であること。
    EXPECT_TRUE(errors.empty()); // [確認_正常系] - 標準エラー出力へ診断を出さないこと。

    // Cleanup
    struct_meta_catalog_destroy(catalog);
    remove_output_paths(paths);
}

// 生成コードの名前が作業バッファーに収まらない場合に、診断を出して 1 を返すことの確認
TEST(structMetaGenEmitTest, returns_one_when_name_is_too_long)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    output_paths paths = make_output_paths("smgen_long_name");
    std::string long_field(600, 'f');
    std::string header = "typedef struct long_sample\n{\n    int " + long_field + ";\n} long_sample;\n";
    struct_meta_catalog *catalog = create_catalog(header);
    ASSERT_NE(nullptr, catalog); // [状態確認] - 600 文字のフィールド名を持つヘッダーからカタログを作れること。

    // Pre-Assert

    // Act
    testing::internal::CaptureStderr();
    int ret = struct_meta_gen_emit(catalog, "long_sample.h", paths.c_path.c_str()); // [手順] - 生成を実行する。
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(1, ret); // [確認_異常系] - struct_meta_gen_emit() の戻り値が 1 であること。
    EXPECT_NE(std::string::npos, errors.find("生成コードの名前が長すぎます: long_sample." + long_field));
    // [確認_異常系] - 標準エラー出力へ構造体名とフィールド名を添えた診断を出すこと。

    // Cleanup
    struct_meta_catalog_destroy(catalog);
    remove_output_paths(paths);
}

// 生成 C ソースの fflush、ferror、fclose のどれかが失敗を示した場合に、診断を出して 1 を返し、ヘッダーへ進まないことの確認
TEST(structMetaGenEmitTest, returns_one_when_source_write_fails)
{
    // Arrange
    struct_meta_catalog *catalog = create_catalog(kShortHeader);
    ASSERT_NE(nullptr, catalog); // [状態確認] - 短い名前のヘッダーからカタログを作れること。
    output_paths paths = make_output_paths("smgen_source_fail");

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 生成 C ソースの fflush() が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, fflush(_, _, _, _)).WillOnce(Return(EOF));
        // [Pre-Assert確認_異常系] - fflush() が生成 C ソースに対して 1 回だけ呼び出されること。(ヘッダーへ進まないことの確認)
        // [Pre-Assert手順] - fflush() から EOF を返す。

        // Act
        testing::internal::CaptureStderr();
        int ret = struct_meta_gen_emit(catalog, "emit_sample.h", paths.c_path.c_str()); // [手順] - 生成を実行する。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(1, ret); // [確認_異常系] - struct_meta_gen_emit() の戻り値が 1 であること。
        EXPECT_NE(std::string::npos, errors.find("出力ファイルへ書き込めません: " + paths.c_path));
        // [確認_異常系] - 標準エラー出力へ書き込めなかった C ソースのパスを出すこと。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 生成 C ソースの ferror() がエラーを示す条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, ferror(_, _, _, _)).WillOnce(Return(1));
        // [Pre-Assert確認_異常系] - ferror() が生成 C ソースに対して 1 回だけ呼び出されること。(ヘッダーへ進まないことの確認)
        // [Pre-Assert手順] - ferror() から 1 を返す。

        // Act
        testing::internal::CaptureStderr();
        int ret = struct_meta_gen_emit(catalog, "emit_sample.h", paths.c_path.c_str()); // [手順] - 生成を実行する。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(1, ret); // [確認_異常系] - struct_meta_gen_emit() の戻り値が 1 であること。
        EXPECT_NE(std::string::npos, errors.find("出力ファイルへ書き込めません: " + paths.c_path));
        // [確認_異常系] - 標準エラー出力へ書き込めなかった C ソースのパスを出すこと。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 生成 C ソースの fclose() が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, fclose(_, _, _, _)).WillOnce(Invoke(close_then_fail));
        // [Pre-Assert確認_異常系] - fclose() が生成 C ソースに対して 1 回だけ呼び出されること。(ヘッダーへ進まないことの確認)
        // [Pre-Assert手順] - 実関数で閉じてから EOF を返す。

        // Act
        testing::internal::CaptureStderr();
        int ret = struct_meta_gen_emit(catalog, "emit_sample.h", paths.c_path.c_str()); // [手順] - 生成を実行する。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(1, ret); // [確認_異常系] - struct_meta_gen_emit() の戻り値が 1 であること。
        EXPECT_NE(std::string::npos, errors.find("出力ファイルへ書き込めません: " + paths.c_path));
        // [確認_異常系] - 標準エラー出力へ書き込めなかった C ソースのパスを出すこと。
    }

    // Cleanup
    struct_meta_catalog_destroy(catalog);
    remove_output_paths(paths);
}

// 生成ヘッダーの fflush、ferror、fclose のどれかが失敗を示した場合に、診断を出して 1 を返すことの確認
TEST(structMetaGenEmitTest, returns_one_when_header_write_fails)
{
    // Arrange
    struct_meta_catalog *catalog = create_catalog(kShortHeader);
    ASSERT_NE(nullptr, catalog); // [状態確認] - 短い名前のヘッダーからカタログを作れること。
    output_paths paths = make_output_paths("smgen_header_fail");

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 生成ヘッダーの fflush() が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, fflush(_, _, _, _)).WillOnce(Invoke(delegate_real_fflush)).WillOnce(Return(EOF));
        // [Pre-Assert確認_異常系] - fflush() が生成 C ソースと生成ヘッダーの順に 1 回ずつ呼び出されること。
        // [Pre-Assert手順] - 生成 C ソースでは実関数へ委譲し、生成ヘッダーでは EOF を返す。

        // Act
        testing::internal::CaptureStderr();
        int ret = struct_meta_gen_emit(catalog, "emit_sample.h", paths.c_path.c_str()); // [手順] - 生成を実行する。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(1, ret); // [確認_異常系] - struct_meta_gen_emit() の戻り値が 1 であること。
        EXPECT_NE(std::string::npos, errors.find("出力ファイルへ書き込めません: " + paths.h_path));
        // [確認_異常系] - 標準エラー出力へ書き込めなかったヘッダーのパスを出すこと。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 生成ヘッダーの ferror() がエラーを示す条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, ferror(_, _, _, _)).WillOnce(Invoke(delegate_real_ferror)).WillOnce(Return(1));
        // [Pre-Assert確認_異常系] - ferror() が生成 C ソースと生成ヘッダーの順に 1 回ずつ呼び出されること。
        // [Pre-Assert手順] - 生成 C ソースでは実関数へ委譲し、生成ヘッダーでは 1 を返す。

        // Act
        testing::internal::CaptureStderr();
        int ret = struct_meta_gen_emit(catalog, "emit_sample.h", paths.c_path.c_str()); // [手順] - 生成を実行する。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(1, ret); // [確認_異常系] - struct_meta_gen_emit() の戻り値が 1 であること。
        EXPECT_NE(std::string::npos, errors.find("出力ファイルへ書き込めません: " + paths.h_path));
        // [確認_異常系] - 標準エラー出力へ書き込めなかったヘッダーのパスを出すこと。
    }

    {
        // Arrange
        NiceMock<Mock_cplat> mock_cplat;
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 生成ヘッダーの fclose() が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_stdio, fclose(_, _, _, _))
            .WillOnce(Invoke(delegate_real_fclose))
            .WillOnce(Invoke(close_then_fail));
        // [Pre-Assert確認_異常系] - fclose() が生成 C ソースと生成ヘッダーの順に 1 回ずつ呼び出されること。
        // [Pre-Assert手順] - 生成 C ソースでは実関数へ委譲し、生成ヘッダーでは実関数で閉じてから EOF を返す。

        // Act
        testing::internal::CaptureStderr();
        int ret = struct_meta_gen_emit(catalog, "emit_sample.h", paths.c_path.c_str()); // [手順] - 生成を実行する。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(1, ret); // [確認_異常系] - struct_meta_gen_emit() の戻り値が 1 であること。
        EXPECT_NE(std::string::npos, errors.find("出力ファイルへ書き込めません: " + paths.h_path));
        // [確認_異常系] - 標準エラー出力へ書き込めなかったヘッダーのパスを出すこと。
    }

    // Cleanup
    struct_meta_catalog_destroy(catalog);
    remove_output_paths(paths);
}

// 同名メンバーのヘッダーが、生成の前のカタログ作成で拒否されることの確認
TEST(structMetaGenEmitTest, duplicate_member_header_fails_before_emit)
{
    // Arrange
    const char header[] = "typedef struct duplicate_probe\n"
                          "{\n"
                          "    int value;\n"
                          "    int value;\n"
                          "} duplicate_probe;\n";
    struct_meta_catalog *catalog = nullptr;
    struct_meta_diagnostic diagnostic = {};

    // Pre-Assert

    // Act
    int ret = struct_meta_catalog_create_from_header_text(
        header, strlen(header), &catalog,
        &diagnostic); // [手順] - 同名メンバーを持つヘッダー文字列からカタログを作る。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_CORRUPT_DESCRIPTOR,
        ret); // [確認_異常系] - struct_meta_catalog_create_from_header_text() の戻り値が CPLAT_ERR_CORRUPT_DESCRIPTOR であること。
    EXPECT_EQ(nullptr, catalog);   // [確認_異常系] - カタログが作られず、生成へ進まないこと。
    /*
     * 2 つ目の "int value;" の字面は 4 行目である。フィールド規則は、フィールド名の字句の位置
     * (bison の位置情報) を記録するため、診断の行も 4 行目になる。
     */
    EXPECT_EQ(4, diagnostic.line); // [確認_異常系] - diagnostic.line が 2 つ目のメンバーの source の行 (4) であること。
    EXPECT_NE(nullptr,
              strstr(diagnostic.message,
                     "メンバー名が重複しています")); // [確認_異常系] - 診断の文面に重複の説明があること。
    EXPECT_NE(nullptr, strstr(diagnostic.message, "value")); // [確認_異常系] - 診断の文面にメンバー名があること。
    EXPECT_NE(nullptr,
              strstr(diagnostic.message, "duplicate_probe")); // [確認_異常系] - 診断の文面に構造体名があること。
}
