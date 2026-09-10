#include <testfw.h>

#include "format_engine.h"

#include <message_catalog/message_catalog_const.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

/**
 *  可変長引数を組み立てて、テスト対象へ中継します。
 *
 *  `va_start` の直前の名前付き引数を持たせるため、メンバー関数ではなく通常の関数とします。
 */
static int collect_arguments(const message_catalog_entry *entry, format_engine_argument_value *values, ...)
{
    va_list args;
    int ret;

    va_start(args, values);
    ret = format_engine_collect_arguments(entry, args, values);
    va_end(args);

    return ret;
}

class messageCatalogArgumentTest : public Test
{
  protected:
    /** 取り出した値の格納先です。 */
    format_engine_argument_value values[MESSAGE_CATALOG_ARGUMENT_MAX];

    /** カタログの 1 件です。テストごとに組み立てます。 */
    message_catalog_entry entry;

    void SetUp() override
    {
        memset(values, 0, sizeof(values));
        memset(&entry, 0, sizeof(entry));
        entry.id = 1;
    }

    /**
     *  引数スキーマへ引数種別を 1 個追加します。
     */
    void add_kind(const message_catalog_argument_kind kind)
    {
        entry.arguments[entry.argument_count] = kind;
        entry.argument_count++;
    }
};

// 引数を取らない定義では値を取り出さないことの確認
TEST_F(messageCatalogArgumentTest, no_argument)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = collect_arguments(&entry, values); // [手順] - 引数個数が 0 の定義で値を取り出す。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_EQ(MESSAGE_CATALOG_ARGUMENT_KIND_STRING, values[0].kind); // [確認_正常系] - 値の配列が書き換わらないこと。
}

// 引数種別ごとに、対応する型の値を取り出すことの確認
TEST_F(messageCatalogArgumentTest, all_argument_kinds)
{
    // Arrange
    static const char sample_object[] = "x";
    int actual_ret;

    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_STRING);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_INT32);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_UINT32);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_HEX32);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_INT64);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_UINT64);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_HEX64);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_SIZE);

    // Pre-Assert

    // Act
    actual_ret = collect_arguments(&entry, values, "text", INT32_C(-1), UINT32_C(2), UINT32_C(3), INT64_C(-4),
                                   UINT64_C(5), UINT64_C(6),
                                   (size_t)7U); // [手順] - 8 種類の引数種別を持つ定義で値を取り出す。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret);            // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_STREQ("text", values[0].value.string_value);   // [確認_正常系] - 文字列を取り出すこと。
    EXPECT_EQ(INT32_C(-1), values[1].value.int32_value);  // [確認_正常系] - 符号付き 32 bit 整数を取り出すこと。
    EXPECT_EQ(UINT32_C(2), values[2].value.uint32_value); // [確認_正常系] - 符号なし 32 bit 整数を取り出すこと。
    EXPECT_EQ(UINT32_C(3), values[3].value.uint32_value); // [確認_正常系] - 16 進表現の 32 bit 整数を取り出すこと。
    EXPECT_EQ(INT64_C(-4), values[4].value.int64_value);  // [確認_正常系] - 符号付き 64 bit 整数を取り出すこと。
    EXPECT_EQ(UINT64_C(5), values[5].value.uint64_value); // [確認_正常系] - 符号なし 64 bit 整数を取り出すこと。
    EXPECT_EQ(UINT64_C(6), values[6].value.uint64_value); // [確認_正常系] - 16 進表現の 64 bit 整数を取り出すこと。
    EXPECT_EQ((size_t)7U, values[7].value.size_value);    // [確認_正常系] - バイト数を取り出すこと。
    EXPECT_EQ(MESSAGE_CATALOG_ARGUMENT_KIND_SIZE,
              values[7].kind); // [確認_正常系] - 取り出した値に引数種別を記録すること。

    // Arrange
    memset(values, 0, sizeof(values));
    memset(&entry, 0, sizeof(entry));

    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_POINTER);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_DOUBLE);
    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_ERROR_CODE);

    // Act
    actual_ret = collect_arguments(&entry, values, (const void *)sample_object, 12.5,
                                   2); // [手順] - 残る 3 種類の引数種別を持つ定義で値を取り出す。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_OK, actual_ret); // [確認_正常系] - 戻り値が MESSAGE_CATALOG_OK であること。
    EXPECT_EQ((const void *)sample_object, values[0].value.pointer_value); // [確認_正常系] - ポインターを取り出すこと。
    EXPECT_DOUBLE_EQ(12.5, values[1].value.double_value); // [確認_正常系] - 倍精度浮動小数点数を取り出すこと。
    EXPECT_EQ(2, values[2].value.error_code_value);       // [確認_正常系] - エラー コードを取り出すこと。
}

// 列挙に無い引数種別が定義エラーになることの確認
TEST_F(messageCatalogArgumentTest, unknown_argument_kind)
{
    // Arrange
    int actual_ret;

    add_kind(MESSAGE_CATALOG_ARGUMENT_KIND_INT32);
    add_kind((message_catalog_argument_kind)15);

    // Pre-Assert

    // Act
    actual_ret = collect_arguments(&entry, values, INT32_C(1),
                                   INT32_C(2)); // [手順] - 列挙に無い引数種別を含む定義で値を取り出す。

    // Assert
    EXPECT_EQ(MESSAGE_CATALOG_ERR_INVALID_DEFINITION,
              actual_ret); // [確認_異常系] - 戻り値が MESSAGE_CATALOG_ERR_INVALID_DEFINITION であること。
    EXPECT_EQ(INT32_C(1), values[0].value.int32_value); // [確認_異常系] - 未知の種別より前の値は取り出されていること。
}
