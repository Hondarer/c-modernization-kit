#include <testfw.h>
#include <mock_cplat.h>
#include <mock_stdio.h>

#include <struct_meta/patch/patch.h>

#include <cplat/base/result.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

using testing::_;
using testing::AnyNumber;
using testing::AtLeast;
using testing::Contains;
using testing::HasSubstr;
using testing::NiceMock;
using testing::Return;

namespace
{
struct Address
{
    char city[16];
    int zip;
};

struct Sample
{
    Address addresses[2];
    int scores[3];
    int id;
    uint8_t hex_bytes[4];
};

const struct_meta_field kAddressFields[] = {
    {"city", STRUCT_META_FIELD_CHAR_ARRAY, 0, offsetof(Address, city), sizeof(char), 1, sizeof(Address::city), nullptr,
     nullptr, nullptr, 0},
    {"zip", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Address, zip), sizeof(int), 1, 0, nullptr, nullptr, nullptr,
     0},
};
const struct_meta_descriptor kAddressDescriptor = {"Address", sizeof(Address), kAddressFields, 2, nullptr, nullptr, 0};
const struct_meta_attribute kHexAttributes[] = {{"meta.format", "hex"}};
const struct_meta_field kSampleFields[] = {
    {"addresses", STRUCT_META_FIELD_STRUCT, 0, offsetof(Sample, addresses), sizeof(Address), 2, 0, &kAddressDescriptor,
     nullptr, nullptr, 0},
    {"scores", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Sample, scores), sizeof(int), 3, 0, nullptr, nullptr,
     nullptr, 0},
    {"id", STRUCT_META_FIELD_SIGNED_INTEGER, 0, offsetof(Sample, id), sizeof(int), 1, 0, nullptr, nullptr, nullptr, 0},
    {"hex_bytes", STRUCT_META_FIELD_UNSIGNED_INTEGER, 0, offsetof(Sample, hex_bytes), sizeof(uint8_t), 4, 0, nullptr,
     nullptr, kHexAttributes, 1},
};
const struct_meta_descriptor kSampleDescriptor = {"Sample", sizeof(Sample), kSampleFields, 4, nullptr, nullptr, 0};

// [サブ手順 名前=structMetaPatchTest.copy_line]
void copy_line(char *dest, size_t dest_size, const std::string &line)
{
    ASSERT_LT(line.size(), dest_size);
    // [状態確認] - line.size() の戻り値が dest_size より小さいこと。
    memcpy(dest, line.c_str(), line.size() + 1U);
}
// [サブ手順終了]
} // namespace

class StructMetaPatchTest : public Test
{
  protected:
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    cplat_prompt *prompt = reinterpret_cast<cplat_prompt *>(static_cast<uintptr_t>(1U));
    std::vector<std::string> prompts;

    // [サブ手順 名前=StructMetaPatchTest.expect_inputs]
    void expect_inputs(const std::vector<std::string> &lines)
    {
        auto index = std::make_shared<size_t>(0U);
        auto inputs = std::make_shared<std::vector<std::string>>(lines);

        EXPECT_CALL(mock_cplat, cplat_prompt_create(nullptr))
            .WillOnce(Return(prompt)); // 編集開始時にプロンプトを作成する。
        // [Pre-Assert確認_正常系] - 編集開始時に cplat_prompt_create() が nullptr を指定して 1 回呼び出されること。
        // [Pre-Assert手順] - 入力用のプロンプト ハンドルを返す。
        EXPECT_CALL(mock_cplat, cplat_prompt_dispose(prompt))
            .WillOnce(Return()); // 編集終了時にプロンプトを破棄する。
        // [Pre-Assert確認_正常系] - 編集終了時に cplat_prompt_dispose() が prompt を指定して 1 回呼び出されること。
        // [Pre-Assert手順] - プロンプトの破棄を受け付ける。
        EXPECT_CALL(mock_cplat, cplat_prompt_readline_fmt_at(prompt, _, _, _, _, _, _))
            .WillRepeatedly(
                [this, index, inputs](cplat_prompt *, char *dest, size_t dest_size, const char *, int, const char *fmt,
                                      va_list args) -> int
                {
                    char formatted[512];
                    va_list args_copy;
                    va_copy(args_copy, args);
                    int formatted_length = vsnprintf(formatted, sizeof(formatted), fmt, args_copy);
                    va_end(args_copy);
                    if ((formatted_length >= 0) && (static_cast<size_t>(formatted_length) < sizeof(formatted)))
                    {
                        prompts.emplace_back(formatted);
                    }
                    if (*index >= inputs->size())
                    {
                        return CPLAT_ERR_EOF;
                    }
                    // [サブ手順参照 名前=structMetaPatchTest.copy_line]
                    copy_line(dest, dest_size, (*inputs)[*index]);
                    (*index)++;
                    return CPLAT_OK;
                }); // [Pre-Assert確認_正常系] - cplat_prompt_readline_fmt_at() が prompt を指定して任意の回数呼び出されること。
                    // [Pre-Assert手順] - 指定した入力列を順番に返す。
    }
    // [サブ手順終了]
};

// パス指定によりネストした構造体内の文字列フィールドを対話編集できることの確認
TEST_F(StructMetaPatchTest, PathSelectsNestedString)
{
    // Arrange
    Sample sample = {}; // [状態] - ネスト配列を持つ構造体を用意する。

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"Tokyo"});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "addresses[0].city");
    // [手順] - ネストした文字列をパスで指定して編集する。

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret); // [確認_正常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("Tokyo", sample.addresses[0].city); // [確認_正常系] - sample.addresses[0].city が "Tokyo" であること。
    EXPECT_THAT(prompts, Contains(HasSubstr("addresses[0].city (現在値")));
    // [確認_正常系] - prompts が "addresses[0].city (現在値" を含む文字列を要素に持つこと。
}

// パス指定により配列の特定要素を対話編集できることの確認
TEST_F(StructMetaPatchTest, PathSelectsArrayElement)
{
    // Arrange
    Sample sample = {}; // [状態] - 整数配列を持つ構造体を用意する。

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"42"});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "scores[1]");
    // [手順] - 整数配列の要素をパスで指定して編集する。

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret); // [確認_正常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(42, sample.scores[1]); // [確認_正常系] - sample.scores[1] が 42 であること。
}

// パス指定により 16 進配列全体を文字列として一括編集できることの確認
TEST_F(StructMetaPatchTest, path_edits_hex_array_as_a_whole)
{
    // Arrange
    Sample sample = {};

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"aa bb gg dd", "AA bb cc DD"});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "hex_bytes");
    // [手順] - 不正な入力に続けて正しい 16 進文字列を指定する。

    // Assert
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret); // [確認_正常系] - 再入力後の struct_meta_patch_path_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(0xaa, sample.hex_bytes[0]); // [確認_正常系] - sample.hex_bytes[0] が 0xaa であること。
    EXPECT_EQ(0xbb, sample.hex_bytes[1]); // [確認_正常系] - sample.hex_bytes[1] が 0xbb であること。
    EXPECT_EQ(0xcc, sample.hex_bytes[2]); // [確認_正常系] - sample.hex_bytes[2] が 0xcc であること。
    EXPECT_EQ(0xdd, sample.hex_bytes[3]); // [確認_正常系] - sample.hex_bytes[3] が 0xdd であること。
}

// パス指定により 16 進配列の個別要素を 10 進整数として対話編集できることの確認
TEST_F(StructMetaPatchTest, path_edits_hex_array_element_as_decimal)
{
    // Arrange
    Sample sample = {};

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"127"});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "hex_bytes[1]");
    // [手順] - hex 配列の 1 要素を 10 進整数で指定する。

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret); // [確認_正常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(127, sample.hex_bytes[1]); // [確認_正常系] - sample.hex_bytes[1] が 127 であること。
}

// 構造体要素を終端とするパスを指定した場合にフィールド選択メニューが開くことの確認
TEST_F(StructMetaPatchTest, PathEndingAtStructOpensFieldMenu)
{
    // Arrange
    Sample sample = {}; // [状態] - 構造体配列を持つ構造体を用意する。

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"2", "123", ""});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。
    EXPECT_CALL(mock_stdio, printf(_, _, _, _)).Times(AnyNumber());
    // [Pre-Assert確認_正常系] - printf() が任意の回数呼び出されること。
    // [Pre-Assert手順] - 検証対象以外のメニュー出力を許可する。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("-- Address (現在位置: addresses[0]) --")))
        .Times(AtLeast(
            1)); // [Pre-Assert確認_正常系] - printf() が "-- Address (現在位置: addresses[0]) --" を含む文字列を指定して 1 回以上呼び出されること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("2) addresses[0].zip = 0")))
        .Times(AtLeast(
            1)); // [Pre-Assert確認_正常系] - printf() が "2) addresses[0].zip = 0" を含む文字列を指定して 1 回以上呼び出されること。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "addresses[0]");
    // [手順] - 構造体要素をパスで指定し、zip を編集して戻る。

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret); // [確認_正常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(123, sample.addresses[0].zip); // [確認_正常系] - sample.addresses[0].zip が 123 であること。
}

// 配列を終端とするパスを指定した場合に要素選択メニューが開くことの確認
TEST_F(StructMetaPatchTest, PathEndingAtArrayOpensElementMenu)
{
    // Arrange
    Sample sample = {}; // [状態] - 整数配列を持つ構造体を用意する。

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"1", "77", ""});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。
    EXPECT_CALL(mock_stdio, printf(_, _, _, _)).Times(AnyNumber());
    // [Pre-Assert確認_正常系] - printf() が任意の回数呼び出されること。
    // [Pre-Assert手順] - 検証対象以外のメニュー出力を許可する。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("-- scores (現在位置: scores、配列、要素数 3) --")))
        .Times(AtLeast(
            1)); // [Pre-Assert確認_正常系] - printf() が "-- scores (現在位置: scores、配列、要素数 3) --" を含む文字列を指定して 1 回以上呼び出されること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("1) scores[1]")))
        .Times(AtLeast(
            1)); // [Pre-Assert確認_正常系] - printf() が "1) scores[1]" を含む文字列を指定して 1 回以上呼び出されること。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "scores");
    // [手順] - 配列全体をパスで指定し、要素を選択して編集する。

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret); // [確認_正常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(77, sample.scores[1]); // [確認_正常系] - sample.scores[1] が 77 であること。
}

// 構造体配列を終端とするパスを指定した場合に要素メニューとフィールド メニューが順に開くことの確認
TEST_F(StructMetaPatchTest, PathEndingAtStructArrayOpensElementAndFieldMenus)
{
    // Arrange
    Sample sample = {}; // [状態] - 構造体配列を持つ構造体を用意する。

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"1", "1", "Osaka", "", ""});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "addresses");
    // [手順] - 構造体配列全体を指定し、要素とフィールドを選択して編集する。

    // Assert
    EXPECT_EQ(CPLAT_OK,
              actual_ret); // [確認_正常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("Osaka", sample.addresses[1].city); // [確認_正常系] - sample.addresses[1].city が "Osaka" であること。
}

// 対話入力で空行を入力した場合に現在の値が維持されることの確認
TEST_F(StructMetaPatchTest, EmptyValueKeepsCurrentValue)
{
    // Arrange
    Sample sample = {};
    sample.id = 12; // [状態] - 変更前の値を設定する。

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({""});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "id");
    // [手順] - 値の入力で空行を指定する。

    // Assert
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret); // [確認_正常系] - 空行入力後の struct_meta_patch_path_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(12,
              sample.id); // [確認_正常系] - sample.id が 12 であること。(空行を入力しても元の値を維持することの確認)
}

// メニューからメンバー名を直接入力してフィールドを選択・編集できることの確認
TEST_F(StructMetaPatchTest, MenuSelectsFieldByMemberName)
{
    // Arrange
    Sample sample = {};

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"id", "42", ""});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。

    // Act
    int actual_ret = struct_meta_patch_interactive(&kSampleDescriptor, &sample);
    // [手順] - メンバー名で id を選択して編集する。

    // Assert
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret); // [確認_正常系] - メンバー名選択後の struct_meta_patch_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_EQ(42, sample.id); // [確認_正常系] - sample.id が 42 であること。
    EXPECT_THAT(prompts, Contains(HasSubstr("フィールド番号またはメンバー名を選択")));
    // [確認_正常系] - prompts が "フィールド番号またはメンバー名を選択" を含む文字列を要素に持つこと。
}

// ルートからの階層メニュー選択 (ドリルダウン) による編集が正しく機能することの確認
TEST_F(StructMetaPatchTest, DrillDownRemainsAvailable)
{
    // Arrange
    Sample sample = {}; // [状態] - 従来のメニュー選択に使う構造体を用意する。

    // Pre-Assert
    // [サブ手順参照 名前=StructMetaPatchTest.expect_inputs]
    expect_inputs({"1", "0", "1", "Tokyo", "", "", ""});
    // プロンプトの作成、破棄、入力の 3 つの呼び出し期待を満たすこと。
    EXPECT_CALL(mock_stdio, printf(_, _, _, _)).Times(AnyNumber());
    // [Pre-Assert確認_正常系] - printf() が任意の回数呼び出されること。
    // [Pre-Assert手順] - 検証対象以外のメニュー出力を許可する。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("-- Sample (現在位置: <root>) --")))
        .Times(AtLeast(
            1)); // [Pre-Assert確認_正常系] - printf() が "-- Sample (現在位置: <root>) --" を含む文字列を指定して 1 回以上呼び出されること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("1) addresses [配列 2 件]")))
        .Times(AtLeast(
            1)); // [Pre-Assert確認_正常系] - printf() が "1) addresses [配列 2 件]" を含む文字列を指定して 1 回以上呼び出されること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("0) addresses[0]")))
        .Times(AtLeast(
            1)); // [Pre-Assert確認_正常系] - printf() が "0) addresses[0]" を含む文字列を指定して 1 回以上呼び出されること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("1) addresses[0].city = \"\"")))
        .Times(AtLeast(
            1)); // [Pre-Assert確認_正常系] - printf() が "1) addresses[0].city = \"\"" を含む文字列を指定して 1 回以上呼び出されること。

    // Act
    int actual_ret = struct_meta_patch_interactive(&kSampleDescriptor, &sample);
    // [手順] - 従来のドリルダウン式で addresses[0].city を編集する。

    // Assert
    EXPECT_EQ(
        CPLAT_OK,
        actual_ret); // [確認_正常系] - ドリルダウン後の struct_meta_patch_interactive() の戻り値が CPLAT_OK であること。
    EXPECT_STREQ("Tokyo", sample.addresses[0].city); // [確認_正常系] - sample.addresses[0].city が "Tokyo" であること。
    EXPECT_THAT(prompts, Contains(HasSubstr("addresses[0].city (現在値")));
    // [確認_正常系] - prompts が "addresses[0].city (現在値" を含む文字列を要素に持つこと。
}

// 範囲外の配列インデックスなど無効なパスが指定された場合にプロンプト生成前にエラーを返すことの確認
TEST_F(StructMetaPatchTest, InvalidPathIsRejectedBeforePromptCreation)
{
    // Arrange
    Sample sample = {}; // [状態] - パス解決対象の構造体を用意する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_prompt_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_prompt_create() が呼び出されないこと。(パス解決に失敗した場合はプロンプトを作成しないことの確認)

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "addresses[2].city");
    // [手順] - 範囲外の配列インデックスを指定する。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_RANGE,
        actual_ret); // [確認_異常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_ERR_OUT_OF_RANGE であること。
}

// 存在しないフィールド名を含むパスが指定された場合にプロンプト生成前にエラーを返すことの確認
TEST_F(StructMetaPatchTest, UnknownFieldIsRejectedBeforePromptCreation)
{
    // Arrange
    Sample sample = {}; // [状態] - パス解決対象の構造体を用意する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_prompt_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_prompt_create() が呼び出されないこと。(未知フィールドの場合はプロンプトを作成しないことの確認)

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "unknown");
    // [手順] - 存在しないフィールドを指定する。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_NOT_FOUND,
        actual_ret); // [確認_異常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_ERR_NOT_FOUND であること。
}

// NULL または空文字列のパスが指定された場合にプロンプト生成前にエラーを返すことの確認
TEST_F(StructMetaPatchTest, InvalidArgumentsAreRejectedBeforePromptCreation)
{
    // Arrange
    Sample sample = {}; // [状態] - 引数検査に使う構造体を用意する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_prompt_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_prompt_create() が呼び出されないこと。(引数が不正な場合はプロンプトを作成しないことの確認)

    // Act
    int null_path_ret =
        struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, nullptr); // [手順] - NULL パスを指定する。
    int empty_path_ret =
        struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, ""); // [手順] - 空文字列パスを指定する。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        null_path_ret); // [確認_異常系] - NULL パスに対する struct_meta_patch_path_interactive() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
    EXPECT_EQ(
        CPLAT_ERR_INVALID_ARGUMENT,
        empty_path_ret); // [確認_異常系] - 空文字列パスに対する struct_meta_patch_path_interactive() の戻り値が CPLAT_ERR_INVALID_ARGUMENT であること。
}

// 名前のない破損した記述子が指定された場合にプロンプト生成前にエラーを返すことの確認
TEST_F(StructMetaPatchTest, CorruptDescriptorIsRejectedBeforePromptCreation)
{
    // Arrange
    Sample sample = {}; // [状態] - 不正な記述子の対象インスタンスを用意する。
    const struct_meta_descriptor corrupt_descriptor = {nullptr, sizeof(Sample), kSampleFields, 4, nullptr, nullptr, 0};

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_prompt_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_prompt_create() が呼び出されないこと。(記述子検査に失敗した場合はプロンプトを作成しないことの確認)

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&corrupt_descriptor, &sample, "id");
    // [手順] - 名前のない壊れた記述子を指定する。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_CORRUPT_DESCRIPTOR,
        actual_ret); // [確認_異常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_ERR_CORRUPT_DESCRIPTOR であること。
}

// プロンプト生成に失敗した場合にメモリ不足エラーを返すことの確認
TEST_F(StructMetaPatchTest, PromptCreationFailureIsReturned)
{
    // Arrange
    Sample sample = {}; // [状態] - 編集対象の構造体を用意する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_prompt_create(nullptr))
        .WillOnce(Return(
            nullptr)); // [Pre-Assert確認_異常系] - cplat_prompt_create() が nullptr を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - プロンプト生成失敗を発生させる。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "id");
    // [手順] - プロンプトを生成して編集を開始する。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_OUT_OF_MEMORY,
        actual_ret); // [確認_異常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_ERR_OUT_OF_MEMORY であること。
}

// 対話入力中にキャンセルが発生した場合にプロンプト破棄後にエラー コードを返すことの確認
TEST_F(StructMetaPatchTest, PromptInputFailureIsReturnedAfterDisposal)
{
    // Arrange
    Sample sample = {}; // [状態] - 編集対象の構造体を用意する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_prompt_create(nullptr)).WillOnce(Return(prompt));
    // [Pre-Assert確認_異常系] - cplat_prompt_create() が nullptr を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - 入力用のプロンプト ハンドルを返す。
    EXPECT_CALL(mock_cplat, cplat_prompt_readline_fmt_at(prompt, _, _, _, _, _, _))
        .WillOnce(Return(
            CPLAT_ERR_CANCELED)); // [Pre-Assert確認_異常系] - cplat_prompt_readline_fmt_at() が prompt を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - 入力キャンセルを発生させる。
    EXPECT_CALL(mock_cplat, cplat_prompt_dispose(prompt))
        .WillOnce(
            Return()); // [Pre-Assert確認_異常系] - cplat_prompt_dispose() が prompt を指定して 1 回呼び出されること。(失敗時にもプロンプトを破棄することの確認)
    // [Pre-Assert手順] - プロンプトの破棄を受け付ける。

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "id");
    // [手順] - 対話入力中にキャンセルする。

    // Assert
    EXPECT_EQ(
        CPLAT_ERR_CANCELED,
        actual_ret); // [確認_異常系] - struct_meta_patch_path_interactive() の戻り値が CPLAT_ERR_CANCELED であること。
}
