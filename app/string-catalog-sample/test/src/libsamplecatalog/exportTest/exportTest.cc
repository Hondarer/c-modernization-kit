#include <testfw.h>

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <string>

#include <cplat/base/platform.h>
#include <samplecatalog/samplecatalog.h>
#include <samplecatalog/samplecatalog_context.h>
#include <samplecatalog/samplecatalog_messages.h>
#include <samplecatalog/samplecatalog_trace.h>

// libsamplecatalog が公開エクスポートすべき関数の一覧。
// 関数の追加・削除は、このテーブルのみを編集する。
// 名前一致チェックとシグネチャの static_assert は、いずれも本テーブルから生成する。
//
// カタログの生成物は、カタログ定義の export が定める公開範囲に従って装飾される。
// 公開範囲 api では、戻り値が cplat の構造体を指す関数 (_catalog、_entries、_entry、
// _key_index、_key_index_count) と、提供元の責務である _verify を公開しない。
//
// samplecatalog_next_sequence_number は app が定義するコンテキスト引数の取得式であり、
// 生成ヘッダーの static inline の中で展開される。利用側の翻訳単位から
// 呼び出されるため、公開が必須となる。
#define SAMPLECATALOG_EXPORT_FUNCTION_TABLE(EXPORT_ENTRY) \
    EXPORT_ENTRY(samplecatalog_initialize, int(SAMPLECATALOG_API *)(cplat_tracer *)) \
    EXPORT_ENTRY(samplecatalog_find_item, int(SAMPLECATALOG_API *)(const char *, char *, size_t)) \
    EXPORT_ENTRY(samplecatalog_next_sequence_number, int32_t(SAMPLECATALOG_API *)(void)) \
    EXPORT_ENTRY(samplecatalog_messages_entry_count, int(SAMPLECATALOG_API *)(void)) \
    EXPORT_ENTRY(samplecatalog_messages_format, int(SAMPLECATALOG_API *)(char *, size_t, int, ...)) \
    EXPORT_ENTRY(samplecatalog_messages_vformat, int(SAMPLECATALOG_API *)(char *, size_t, int, va_list)) \
    EXPORT_ENTRY(samplecatalog_messages_category, int(SAMPLECATALOG_API *)(int)) \
    EXPORT_ENTRY(samplecatalog_messages_id, const char *(SAMPLECATALOG_API *)(int)) \
    EXPORT_ENTRY(samplecatalog_messages_note, const char *(SAMPLECATALOG_API *)(int)) \
    EXPORT_ENTRY(samplecatalog_trace_entry_count, int(SAMPLECATALOG_API *)(void)) \
    EXPORT_ENTRY(samplecatalog_trace_format, int(SAMPLECATALOG_API *)(char *, size_t, int, ...)) \
    EXPORT_ENTRY(samplecatalog_trace_vformat, int(SAMPLECATALOG_API *)(char *, size_t, int, va_list)) \
    EXPORT_ENTRY(samplecatalog_trace_category, int(SAMPLECATALOG_API *)(int)) \
    EXPORT_ENTRY(samplecatalog_trace_id, const char *(SAMPLECATALOG_API *)(int)) \
    EXPORT_ENTRY(samplecatalog_trace_note, const char *(SAMPLECATALOG_API *)(int)) \
    EXPORT_ENTRY(samplecatalog_trace_set_tracer, void(SAMPLECATALOG_API *)(cplat_tracer *)) \
    EXPORT_ENTRY(samplecatalog_trace_get_tracer, cplat_tracer *(SAMPLECATALOG_API *)(void)) \
    EXPORT_ENTRY(samplecatalog_trace_write, int(SAMPLECATALOG_API *)(int, ...))

// libsamplecatalog が公開エクスポートすべき変数の一覧。
// 現時点ではエントリなし (公開ヘッダーに dllexport 付きの変数エクスポートが存在しないため)。
#define SAMPLECATALOG_EXPORT_VARIABLE_TABLE(EXPORT_ENTRY)

#define SAMPLECATALOG_EXPORT_TABLE(EXPORT_ENTRY) \
    SAMPLECATALOG_EXPORT_FUNCTION_TABLE(EXPORT_ENTRY) \
    SAMPLECATALOG_EXPORT_VARIABLE_TABLE(EXPORT_ENTRY)

// テーブルからシグネチャの static_assert と期待シンボル名一覧を生成する。
SAMPLECATALOG_EXPORT_TABLE(TESTFW_EXPORT_STATIC_ASSERT_ENTRY)

static const char *const kExpectedExportNames[] = {SAMPLECATALOG_EXPORT_TABLE(TESTFW_EXPORT_NAME_ENTRY)};

static const std::map<std::string, std::string> kExpectedExportSignatures = {
    SAMPLECATALOG_EXPORT_TABLE(TESTFW_EXPORT_SIGNATURE_ENTRY)};

class exportTest : public Test
{
  protected:
    std::string workspace_root;
    std::string dll_path;

    void SetUp() override
    {
        workspace_root = findWorkspaceRoot();
        ASSERT_FALSE(workspace_root.empty()) << "ワークスペース ルートが見つかりません";
        dll_path = workspace_root +
                   "/app/string-catalog-sample/prod/lib/libsamplecatalog" TESTFW_SHARED_LIBRARY_EXTENSION;
    }
};

// libsamplecatalog のエクスポート シンボル名に不足や想定外がないことの確認
TEST_F(exportTest, symbol_names_match)
{
    // Arrange
    std::set<std::string> expected(
        std::begin(kExpectedExportNames),
        std::end(kExpectedExportNames)); // [状態] - SAMPLECATALOG_EXPORT_TABLE から期待シンボル名一覧を構築する。
#if defined(PLATFORM_WINDOWS)
    // _ident_manifest_libsamplecatalog_dll は gen_ident_manifest.py が自動生成するビルド識別データであり、
    // 関数ではないためシグネチャ検証の対象外としつつ、名前一致の期待値には含める。
    expected.insert(testing::identManifestSymbolName(
        "libsamplecatalog" TESTFW_SHARED_LIBRARY_EXTENSION)); // [状態] - IDENT manifest シンボル名を期待値へ追加する (Windows のみ実際にエクスポートされる)。
#endif                                                        /* PLATFORM_WINDOWS */

    // Pre-Assert

    // Act
    std::set<std::string> actual = testing::getActualExportNames(
        dll_path); // [手順] - dumpbin/nm で libsamplecatalog の実際のエクスポート一覧を取得する。

    // Assert
    testing::expectExportNamesMatch(
        expected, actual,
        kExpectedExportSignatures); // [確認_正常系] - 期待シンボルとの不足や想定外がないこと (Windows / Linux とも完全一致)。
}

// 公開範囲 api では、cplat の構造体を返す関数を公開しないことの確認
TEST_F(exportTest, structure_returning_functions_are_not_exported)
{
    // Arrange
    const std::set<std::string> hidden = {
        "samplecatalog_messages_catalog", "samplecatalog_messages_entries",   "samplecatalog_messages_entry",
        "samplecatalog_messages_verify",  "samplecatalog_trace_catalog",      "samplecatalog_trace_entries",
        "samplecatalog_trace_entry",
        "samplecatalog_trace_verify"}; // [状態] - 公開しない関数の名前一覧を構築する。

    // Pre-Assert

    // Act
    std::set<std::string> actual =
        testing::getActualExportNames(dll_path); // [手順] - 実際のエクスポート一覧を取得する。

    // Assert
    for (const std::string &name : hidden)
    {
        EXPECT_EQ(0U, actual.count(name))
            << "公開範囲 api で公開しない関数がエクスポートされています: "
            << name; // [確認_正常系] - 利用側が cplat の構造体レイアウトへ依存しないこと。
    }
}

// 公開ヘッダーの変数宣言が dllexport マクロ (SAMPLECATALOG_EXPORT) を
// 伴わずに追加されていないことの確認
TEST_F(exportTest, public_header_variables_declare_export_macro)
{
    // Arrange
    std::string include_dir =
        workspace_root + "/app/string-catalog-sample/prod/include"; // [状態] - 公開ヘッダーのディレクトリを設定する。

    // Pre-Assert

    // Act
    std::vector<std::string> undecorated = testing::findUndecoratedExternVariables(
        include_dir,
        "SAMPLECATALOG_EXPORT"); // [手順] - prod/include 配下を走査し、装飾を伴わない extern 変数宣言を集める。

    // Assert
    EXPECT_TRUE(undecorated.empty())
        << "SAMPLECATALOG_EXPORT を伴わない変数宣言: "
        << testing::joinNames(undecorated); // [確認_正常系] - 該当する宣言が 1 件もないこと。
}
