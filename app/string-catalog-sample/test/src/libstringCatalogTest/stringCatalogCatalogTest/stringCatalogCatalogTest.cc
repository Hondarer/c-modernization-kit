#include <testfw.h>

#include <string_catalog.h>
#include <string_catalog/catalog.h>
#include <stddef.h>

/** 注入するカタログです。内容は参照の確認だけに使用します。第 2 要素は分類値です。 */
static const string_catalog_entry s_entries[] = {
    {1, 3, 0, 0, {}, "STRING_CATALOG_ID_0001", {"first", NULL, NULL}, {"", NULL, NULL}},
    {3, 1, 0, 0, {}, "STRING_CATALOG_ID_0003", {"second", NULL, NULL}, {"", NULL, NULL}}};

/** @ref s_entries の要素数です。 */
static const int s_entry_count = (int)(sizeof(s_entries) / sizeof(s_entries[0]));

/** 文字列 ID を添字として @ref s_entries の添字を引く表です。 */
static const int s_id_index[] = {-1, 0, -1, 1};

/** @ref s_id_index の要素数です。 */
static const int s_id_index_count = (int)(sizeof(s_id_index) / sizeof(s_id_index[0]));

/** 範囲外の添字を格納した、内容が壊れた添字表です。 */
static const int s_broken_id_index[] = {-1, 99};

class stringCatalogCatalogTest : public Test
{
};

// カタログを注入していないプロセスが空のカタログとして振る舞うことの確認
// カタログはプロセス グローバルな状態のため、注入前を確認するテストを最初に置く
TEST_F(stringCatalogCatalogTest, unregistered_catalog_is_empty)
{
    // Arrange
    int actual_count;
    const string_catalog_entry *actual_entry;

    // Pre-Assert

    // Act
    actual_count = string_catalog_internal_entry_count(); // [手順] - 注入せずに件数を取得する。
    actual_entry = string_catalog_internal_find_entry(1); // [手順] - 注入せずに文字列 ID で検索する。

    // Assert
    EXPECT_EQ(0, actual_count);       // [確認_正常系] - 件数が 0 であること。
    EXPECT_EQ(nullptr, actual_entry); // [確認_正常系] - 検索結果が NULL であること。
}

// 添字表なしで注入したカタログを線形探索で引けることの確認
TEST_F(stringCatalogCatalogTest, find_without_id_index)
{
    // Arrange
    int actual_ret;
    int actual_count;
    const string_catalog_entry *actual_entry_found;
    const string_catalog_entry *actual_entry_unknown;

    // Pre-Assert

    // Act
    actual_ret =
        string_catalog_set_catalog(s_entries, s_entry_count, NULL, 0); // [手順] - 添字表を渡さずにカタログを注入する。
    actual_count = string_catalog_internal_entry_count();              // [手順] - 件数を取得する。
    actual_entry_found = string_catalog_internal_find_entry(3);        // [手順] - 登録済みの文字列 ID で検索する。
    actual_entry_unknown = string_catalog_internal_find_entry(2);      // [手順] - 未登録の文字列 ID で検索する。

    // Assert
    EXPECT_EQ(STRING_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が STRING_CATALOG_OK であること。
    EXPECT_EQ(2, actual_count);                // [確認_正常系] - 注入した件数を返すこと。
    ASSERT_NE(nullptr, actual_entry_found);    // [確認_正常系] - カタログを取得できること。
    EXPECT_EQ(3, actual_entry_found->id);      // [確認_正常系] - 検索した文字列 ID の定義であること。
    EXPECT_EQ(nullptr, actual_entry_unknown);  // [確認_異常系] - 未登録の文字列 ID では NULL を返すこと。
}

// 添字表でカタログを引けることの確認
TEST_F(stringCatalogCatalogTest, find_with_id_index)
{
    // Arrange
    const string_catalog_entry *actual_entry_found;
    const string_catalog_entry *actual_entry_absent;
    const string_catalog_entry *actual_entry_over_index;
    const string_catalog_entry *actual_entry_negative_id;

    string_catalog_set_catalog(s_entries, s_entry_count, s_id_index, s_id_index_count);

    // Pre-Assert

    // Act
    actual_entry_found = string_catalog_internal_find_entry(3); // [手順] - 添字表に登録した文字列 ID で検索する。
    actual_entry_absent =
        string_catalog_internal_find_entry(2); // [手順] - 添字表が負の値を持つ文字列 ID で検索する。
    actual_entry_over_index =
        string_catalog_internal_find_entry(9); // [手順] - 添字表の範囲を超える文字列 ID で検索する。
    actual_entry_negative_id = string_catalog_internal_find_entry(-1); // [手順] - 負の文字列 ID で検索する。

    // Assert
    ASSERT_NE(nullptr, actual_entry_found);       // [確認_正常系] - 添字表からカタログを取得できること。
    EXPECT_EQ(3, actual_entry_found->id);         // [確認_正常系] - 検索した文字列 ID の定義であること。
    EXPECT_EQ(nullptr, actual_entry_absent);      // [確認_異常系] - 添字表が負の値を持つ場合は NULL を返すこと。
    EXPECT_EQ(nullptr, actual_entry_over_index);  // [確認_異常系] - 添字表の範囲外では線形探索へ落ち、NULL を返すこと。
    EXPECT_EQ(nullptr, actual_entry_negative_id); // [確認_異常系] - 負の文字列 ID では NULL を返すこと。
}

// 添字表が範囲外の添字を持つ場合に参照しないことの確認
TEST_F(stringCatalogCatalogTest, broken_id_index)
{
    // Arrange
    const string_catalog_entry *actual_entry;

    string_catalog_set_catalog(s_entries, s_entry_count, s_broken_id_index, 2);

    // Pre-Assert

    // Act
    actual_entry = string_catalog_internal_find_entry(1); // [手順] - 範囲外の添字を持つ添字表で検索する。

    // Assert
    EXPECT_EQ(nullptr, actual_entry); // [確認_異常系] - カタログを参照せずに NULL を返すこと。
}

// 添字でカタログを取得できることの確認
TEST_F(stringCatalogCatalogTest, entry_at)
{
    // Arrange
    const string_catalog_entry *actual_entry;
    const string_catalog_entry *actual_entry_negative;
    const string_catalog_entry *actual_entry_over;

    string_catalog_set_catalog(s_entries, s_entry_count, s_id_index, s_id_index_count);

    // Pre-Assert

    // Act
    actual_entry = string_catalog_internal_entry_at(1);                  // [手順] - 添字で 2 件目を取得する。
    actual_entry_negative = string_catalog_internal_entry_at(-1);        // [手順] - 負の添字で取得する。
    actual_entry_over = string_catalog_internal_entry_at(s_entry_count); // [手順] - 登録件数と同じ添字で取得する。

    // Assert
    ASSERT_NE(nullptr, actual_entry);          // [確認_正常系] - カタログを取得できること。
    EXPECT_EQ(3, actual_entry->id);            // [確認_正常系] - 添字に対応する文字列であること。
    EXPECT_EQ(nullptr, actual_entry_negative); // [確認_異常系] - 負の添字では NULL を返すこと。
    EXPECT_EQ(nullptr, actual_entry_over);     // [確認_異常系] - 登録件数以上の添字では NULL を返すこと。
}

// 不正な指定で注入できず、設定が変わらないことの確認
TEST_F(stringCatalogCatalogTest, invalid_argument)
{
    // Arrange
    int actual_ret_null;
    int actual_ret_negative_count;
    int actual_ret_negative_index_count;
    int actual_count;

    string_catalog_set_catalog(s_entries, s_entry_count, s_id_index, s_id_index_count);

    // Pre-Assert

    // Act
    actual_ret_null =
        string_catalog_set_catalog(NULL, s_entry_count, s_id_index, s_id_index_count); // [手順] - 配列へ NULL を渡す。
    actual_ret_negative_count =
        string_catalog_set_catalog(s_entries, -1, s_id_index, s_id_index_count); // [手順] - 件数へ負の値を渡す。
    actual_ret_negative_index_count = string_catalog_set_catalog(s_entries, s_entry_count, s_id_index,
                                                                  -1); // [手順] - 添字表の要素数へ負の値を渡す。
    actual_count = string_catalog_internal_entry_count();             // [手順] - 件数を取得する。

    // Assert
    EXPECT_EQ(STRING_CATALOG_ERR_INVALID_ARGUMENT, actual_ret_null); // [確認_異常系] - NULL では引数不正を返すこと。
    EXPECT_EQ(STRING_CATALOG_ERR_INVALID_ARGUMENT,
              actual_ret_negative_count); // [確認_異常系] - 負の件数では引数不正を返すこと。
    EXPECT_EQ(STRING_CATALOG_ERR_INVALID_ARGUMENT,
              actual_ret_negative_index_count); // [確認_異常系] - 負の要素数では引数不正を返すこと。
    EXPECT_EQ(2, actual_count);                 // [確認_異常系] - 設定が変わらないこと。
}
