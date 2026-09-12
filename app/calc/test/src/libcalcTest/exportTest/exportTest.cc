#include <testfw.h>

#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include <cplat/base/platform.h>
#include <calc/calc_spec.h>

// libcalc が公開エクスポートすべき関数の一覧。
// 関数の追加・削除は、このテーブルのみを編集する。
// 名前一致チェックとシグネチャの static_assert は、いずれも本テーブルから生成する。
//
// calcbase (静的リンク専用ライブラリ、DLL/SO を生成しない) は対象外とする。
// app/calc/prod/include/calcbase/calcbase_spec.h の関数は export マクロを一切伴わない
// extern 宣言であり、これは意図した設計 (see: framework/testfw/docs/about-shared-lib-static-linking.md)。
#define CALC_EXPORT_TABLE(EXPORT_ENTRY) EXPORT_ENTRY(calc_handler, int(CALC_API *)(int kind, int a, int b, int *result))

// テーブルからシグネチャの static_assert と期待シンボル名一覧を生成する。
// 定型マクロ (TESTFW_EXPORT_STATIC_ASSERT_ENTRY/TESTFW_EXPORT_NAME_ENTRY) は
// framework/testfw/include/export_check.h 側の共通定義を使用する。
CALC_EXPORT_TABLE(TESTFW_EXPORT_STATIC_ASSERT_ENTRY)

static const char *const kExpectedExportNames[] = {CALC_EXPORT_TABLE(TESTFW_EXPORT_NAME_ENTRY)};

static const std::map<std::string, std::string> kExpectedExportSignatures = {
    CALC_EXPORT_TABLE(TESTFW_EXPORT_SIGNATURE_ENTRY)};

class exportTest : public Test
{
  protected:
    std::string workspace_root;
    std::string dll_path;
    std::string include_dir;

    void SetUp() override
    {
        workspace_root = findWorkspaceRoot();
        ASSERT_FALSE(workspace_root.empty()) << "ワークスペースルートが見つかりません";
        dll_path = workspace_root + "/app/calc/prod/lib/libcalc" TESTFW_SHARED_LIBRARY_EXTENSION;
    }
};

// libcalc のエクスポート シンボル名一致テスト
TEST_F(exportTest, symbol_names_match)
{
    // Arrange
    std::set<std::string> expected(
        std::begin(kExpectedExportNames),
        std::end(kExpectedExportNames)); // [状態] - CALC_EXPORT_TABLE から期待シンボル名一覧を構築する。
#if defined(PLATFORM_WINDOWS)
    // _ident_manifest_libcalc_dll は gen_ident_manifest.py が自動生成するビルド識別データであり、
    // 関数ではないためシグネチャ検証の対象外としつつ、名前一致の期待値には含める。
    expected.insert(testing::identManifestSymbolName(
        "libcalc" TESTFW_SHARED_LIBRARY_EXTENSION)); // [状態] - IDENT manifest シンボル名を期待値へ追加する (Windows のみ実際にエクスポートされる)。
#endif                                               /* PLATFORM_WINDOWS */

    // Pre-Assert

    // Act
    std::set<std::string> actual =
        testing::getActualExportNames(dll_path); // [手順] - dumpbin/nm で libcalc の実際のエクスポート一覧を取得する。

    // Assert
    testing::expectExportNamesMatch(
        expected, actual,
        kExpectedExportSignatures); // [確認_正常系] - 期待シンボルとの不足/想定外がないこと (Windows / Linux とも完全一致)。
}

// 公開ヘッダー (calc/ サブディレクトリのみ) の変数宣言が dllexport マクロ (CALC_EXPORT) を
// 伴わずに追加されていないことの確認
TEST_F(exportTest, public_header_variables_declare_export_macro)
{
    // Arrange
    // calcbase/ は静的リンク専用のため走査対象から明示的に除外する (calc/ サブディレクトリのみを対象とする)。
    include_dir =
        workspace_root +
        "/app/calc/prod/include/calc"; // [状態] - 公開ヘッダーのディレクトリを "/app/calc/prod/include/calc" に設定する。

    // Pre-Assert

    // Act
    std::vector<std::string> undecorated = testing::findUndecoratedExternVariables(
        include_dir,
        "CALC_EXPORT"); // [手順] - prod/include/calc 配下を走査し、CALC_EXPORT を伴わない extern 変数宣言を集める。

    // Assert
    EXPECT_TRUE(undecorated.empty()) << "CALC_EXPORT を伴わない変数宣言: "
                                     << testing::joinNames(
                                            undecorated); // [確認_正常系] - 該当する宣言が 1 件もないこと。
}
