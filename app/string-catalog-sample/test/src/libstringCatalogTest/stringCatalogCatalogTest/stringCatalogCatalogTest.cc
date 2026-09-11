#include <testfw.h>

#include <string_catalog.h>
#include <string_catalog/catalog.h>
#include <stddef.h>

/** 参照するカタログです。内容は参照の確認だけに使用します。第 2 要素は分類値です。 */
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

/** 添字表を持たないカタログです。検索は線形探索の経路を通ります。 */
static const string_catalog s_catalog_without_index = {s_entries, NULL, 2, 0};

/** 添字表を持つカタログです。 */
static const string_catalog s_catalog_with_index = {s_entries, s_id_index, 2, 4};

/** 内容が壊れた添字表を持つカタログです。 */
static const string_catalog s_catalog_broken_index = {s_entries, s_broken_id_index, 2, 2};

/** 同じ文字列 ID に別の内容を持つ、2 つ目のカタログです。 */
static const string_catalog_entry s_other_entries[] = {
    {1, 7, 0, 0, {}, "OTHER_CATALOG_ID_0001", {"other first", NULL, NULL}, {"", NULL, NULL}}};

/** @ref s_other_entries を参照するカタログです。 */
static const string_catalog s_other_catalog = {s_other_entries, NULL, 1, 0};

class stringCatalogCatalogTest : public Test
{
};

// 参照できないカタログが空のカタログとして振る舞うことの確認
TEST_F(stringCatalogCatalogTest, unusable_catalog_is_empty)
{
    // Arrange
    static const string_catalog null_entries = {NULL, NULL, 0, 0};
    static const string_catalog negative_count = {s_entries, NULL, -1, 0};
    static const string_catalog negative_index_count = {s_entries, s_id_index, 2, -1};

    // Pre-Assert

    // Act / Assert
    EXPECT_FALSE(string_catalog_internal_is_usable(NULL)); // [確認_異常系] - NULL は参照できないこと。
    EXPECT_FALSE(
        string_catalog_internal_is_usable(&null_entries)); // [確認_異常系] - 配列が NULL なら参照できないこと。
    EXPECT_FALSE(string_catalog_internal_is_usable(&negative_count)); // [確認_異常系] - 件数が負なら参照できないこと。
    EXPECT_FALSE(string_catalog_internal_is_usable(
        &negative_index_count)); // [確認_異常系] - 添字表の要素数が負なら参照できないこと。
    EXPECT_TRUE(string_catalog_internal_is_usable(
        &s_catalog_with_index)); // [確認_正常系] - 形の整ったカタログは参照できること。

    EXPECT_EQ(0, string_catalog_internal_entry_count(NULL)); // [確認_異常系] - NULL では件数が 0 であること。
    EXPECT_EQ(nullptr,
              string_catalog_internal_find_entry(NULL, 1)); // [確認_異常系] - NULL では検索が NULL を返すこと。
    EXPECT_EQ(nullptr,
              string_catalog_internal_entry_at(NULL, 0)); // [確認_異常系] - NULL では添字取得が NULL を返すこと。
    EXPECT_EQ(
        0,
        string_catalog_internal_entry_count(&null_entries)); // [確認_異常系] - 配列が NULL では件数が 0 であること。
}

// 添字表を持たないカタログを線形探索で引けることの確認
TEST_F(stringCatalogCatalogTest, find_without_id_index)
{
    // Arrange
    int actual_count;
    const string_catalog_entry *actual_entry_found;
    const string_catalog_entry *actual_entry_unknown;

    // Pre-Assert

    // Act
    actual_count = string_catalog_internal_entry_count(&s_catalog_without_index); // [手順] - 件数を取得する。
    actual_entry_found =
        string_catalog_internal_find_entry(&s_catalog_without_index, 3); // [手順] - 登録済みの文字列 ID で検索する。
    actual_entry_unknown =
        string_catalog_internal_find_entry(&s_catalog_without_index, 2); // [手順] - 未登録の文字列 ID で検索する。

    // Assert
    EXPECT_EQ(2, actual_count);               // [確認_正常系] - カタログが持つ件数を返すこと。
    ASSERT_NE(nullptr, actual_entry_found);   // [確認_正常系] - カタログを取得できること。
    EXPECT_EQ(3, actual_entry_found->id);     // [確認_正常系] - 検索した文字列 ID の定義であること。
    EXPECT_EQ(nullptr, actual_entry_unknown); // [確認_異常系] - 未登録の文字列 ID では NULL を返すこと。
}

// 添字表でカタログを引けることの確認
TEST_F(stringCatalogCatalogTest, find_with_id_index)
{
    // Arrange
    const string_catalog_entry *actual_entry_found;
    const string_catalog_entry *actual_entry_absent;
    const string_catalog_entry *actual_entry_over_index;
    const string_catalog_entry *actual_entry_negative_id;

    // Pre-Assert

    // Act
    actual_entry_found =
        string_catalog_internal_find_entry(&s_catalog_with_index, 3); // [手順] - 添字表に登録した文字列 ID で検索する。
    actual_entry_absent = string_catalog_internal_find_entry(&s_catalog_with_index,
                                                             2); // [手順] - 添字表が負の値を持つ文字列 ID で検索する。
    actual_entry_over_index = string_catalog_internal_find_entry(
        &s_catalog_with_index, 9); // [手順] - 添字表の範囲を超える文字列 ID で検索する。
    actual_entry_negative_id =
        string_catalog_internal_find_entry(&s_catalog_with_index, -1); // [手順] - 負の文字列 ID で検索する。

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

    // Pre-Assert

    // Act
    actual_entry =
        string_catalog_internal_find_entry(&s_catalog_broken_index, 1); // [手順] - 範囲外の添字を持つ添字表で検索する。

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

    // Pre-Assert

    // Act
    actual_entry = string_catalog_internal_entry_at(&s_catalog_with_index, 1); // [手順] - 添字で 2 件目を取得する。
    actual_entry_negative =
        string_catalog_internal_entry_at(&s_catalog_with_index, -1); // [手順] - 負の添字で取得する。
    actual_entry_over = string_catalog_internal_entry_at(&s_catalog_with_index,
                                                         s_entry_count); // [手順] - 登録件数と同じ添字で取得する。

    // Assert
    ASSERT_NE(nullptr, actual_entry);          // [確認_正常系] - カタログを取得できること。
    EXPECT_EQ(3, actual_entry->id);            // [確認_正常系] - 添字に対応する文字列であること。
    EXPECT_EQ(nullptr, actual_entry_negative); // [確認_異常系] - 負の添字では NULL を返すこと。
    EXPECT_EQ(nullptr, actual_entry_over);     // [確認_異常系] - 登録件数以上の添字では NULL を返すこと。
}

// 複数のカタログが互いに影響しないことの確認
TEST_F(stringCatalogCatalogTest, multiple_catalogs_are_independent)
{
    // Arrange
    const string_catalog_entry *actual_entry_first;
    const string_catalog_entry *actual_entry_other;
    const string_catalog_entry *actual_entry_only_in_first;

    // Pre-Assert

    // Act
    actual_entry_first =
        string_catalog_internal_find_entry(&s_catalog_with_index, 1); // [手順] - 1 つ目のカタログで文字列 ID 1 を引く。
    actual_entry_other =
        string_catalog_internal_find_entry(&s_other_catalog, 1); // [手順] - 2 つ目のカタログで文字列 ID 1 を引く。
    actual_entry_only_in_first =
        string_catalog_internal_find_entry(&s_other_catalog, 3); // [手順] - 1 つ目にだけある文字列 ID を 2 つ目で引く。

    // Assert
    ASSERT_NE(nullptr, actual_entry_first); // [確認_正常系] - 1 つ目のカタログから取得できること。
    ASSERT_NE(nullptr, actual_entry_other); // [確認_正常系] - 2 つ目のカタログから取得できること。
    EXPECT_STREQ("STRING_CATALOG_ID_0001",
                 actual_entry_first->id_text); // [確認_正常系] - 1 つ目の内容を返すこと。
    EXPECT_STREQ("OTHER_CATALOG_ID_0001",
                 actual_entry_other->id_text);  // [確認_正常系] - 2 つ目の内容を返すこと。
    EXPECT_EQ(3, actual_entry_first->category); // [確認_正常系] - 1 つ目の分類値を返すこと。
    EXPECT_EQ(7, actual_entry_other->category); // [確認_正常系] - 2 つ目の分類値を返すこと。
    EXPECT_EQ(nullptr,
              actual_entry_only_in_first); // [確認_異常系] - 一方にだけある文字列 ID は他方から引けないこと。
}
