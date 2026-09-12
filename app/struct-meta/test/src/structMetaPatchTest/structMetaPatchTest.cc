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

void copy_line(char *dest, size_t dest_size, const std::string &line)
{
    ASSERT_LT(line.size(), dest_size);
    memcpy(dest, line.c_str(), line.size() + 1U);
}
} // namespace

class StructMetaPatchTest : public Test
{
  protected:
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    cplat_prompt *prompt = reinterpret_cast<cplat_prompt *>(static_cast<uintptr_t>(1U));
    std::vector<std::string> prompts;

    void expect_inputs(const std::vector<std::string> &lines)
    {
        auto index = std::make_shared<size_t>(0U);
        auto inputs = std::make_shared<std::vector<std::string>>(lines);

        EXPECT_CALL(mock_cplat, cplat_prompt_create(nullptr))
            .WillOnce(Return(prompt)); // [Pre-Assert確認_正常系] - 編集開始時にプロンプトを作成すること。
        EXPECT_CALL(mock_cplat, cplat_prompt_dispose(prompt))
            .WillOnce(Return()); // [Pre-Assert確認_正常系] - 編集終了時にプロンプトを破棄すること。
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
                    copy_line(dest, dest_size, (*inputs)[*index]);
                    (*index)++;
                    return CPLAT_OK;
                }); // [Pre-Assert手順] - 指定した入力列を順番に返す。
    }
};

TEST_F(StructMetaPatchTest, PathSelectsNestedString)
{
    Sample sample = {}; // [準備_正常系] - ネスト配列を持つ構造体を用意する。
    expect_inputs({"Tokyo"});

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "addresses[0].city");
    // [手順_正常系] - ネストした文字列をパスで指定して編集する。

    EXPECT_EQ(CPLAT_OK, actual_ret);                 // [確認_正常系] - 編集が成功すること。
    EXPECT_STREQ("Tokyo", sample.addresses[0].city); // [確認_正常系] - 指定した文字列だけが更新されること。
    EXPECT_THAT(prompts, Contains(HasSubstr("addresses[0].city (現在値")));
    // [確認_正常系] - パス指定方式の値入力にも完全パスを表示すること。
}

TEST_F(StructMetaPatchTest, PathSelectsArrayElement)
{
    Sample sample = {}; // [準備_正常系] - 整数配列を持つ構造体を用意する。
    expect_inputs({"42"});

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "scores[1]");
    // [手順_正常系] - 整数配列の要素をパスで指定して編集する。

    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 編集が成功すること。
    EXPECT_EQ(42, sample.scores[1]); // [確認_正常系] - 指定した要素だけが更新されること。
}

TEST_F(StructMetaPatchTest, path_edits_hex_array_as_a_whole)
{
    // Arrange
    Sample sample = {};
    expect_inputs({"aa bb gg dd", "AA bb cc DD"});

    // Pre-Assert

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "hex_bytes");
    // [手順] - 不正な入力に続けて正しい16進文字列を指定する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);      // [確認_正常系] - 再入力後に編集が成功すること。
    EXPECT_EQ(0xaa, sample.hex_bytes[0]); // [確認_正常系] - 先頭バイトを更新すること。
    EXPECT_EQ(0xbb, sample.hex_bytes[1]); // [確認_正常系] - 2番目のバイトを更新すること。
    EXPECT_EQ(0xcc, sample.hex_bytes[2]); // [確認_正常系] - 3番目のバイトを更新すること。
    EXPECT_EQ(0xdd, sample.hex_bytes[3]); // [確認_正常系] - 末尾バイトを更新すること。
}

TEST_F(StructMetaPatchTest, path_edits_hex_array_element_as_decimal)
{
    // Arrange
    Sample sample = {};
    expect_inputs({"127"});

    // Pre-Assert

    // Act
    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "hex_bytes[1]");
    // [手順] - hex配列の1要素を10進整数で指定する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret);     // [確認_正常系] - 要素編集が成功すること。
    EXPECT_EQ(127, sample.hex_bytes[1]); // [確認_正常系] - 指定要素を10進値で更新すること。
}

TEST_F(StructMetaPatchTest, PathEndingAtStructOpensFieldMenu)
{
    Sample sample = {}; // [準備_正常系] - 構造体配列を持つ構造体を用意する。
    expect_inputs({"2", "123", ""});
    EXPECT_CALL(mock_stdio, printf(_, _, _, _)).Times(AnyNumber());
    // [Pre-Assert手順] - 検証対象以外のメニュー出力を許可する。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("-- Address (現在位置: addresses[0]) --")))
        .Times(AtLeast(1)); // [Pre-Assert確認_正常系] - パス指定で開始した構造体の現在位置を表示すること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("2) addresses[0].zip = 0")))
        .Times(AtLeast(1)); // [Pre-Assert確認_正常系] - 構造体の候補を完全パスで表示すること。

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "addresses[0]");
    // [手順_正常系] - 構造体要素をパスで指定し、zip を編集して戻る。

    EXPECT_EQ(CPLAT_OK, actual_ret);         // [確認_正常系] - 編集が成功すること。
    EXPECT_EQ(123, sample.addresses[0].zip); // [確認_正常系] - 選択した構造体の zip が更新されること。
}

TEST_F(StructMetaPatchTest, PathEndingAtArrayOpensElementMenu)
{
    Sample sample = {}; // [準備_正常系] - 整数配列を持つ構造体を用意する。
    expect_inputs({"1", "77", ""});
    EXPECT_CALL(mock_stdio, printf(_, _, _, _)).Times(AnyNumber());
    // [Pre-Assert手順] - 検証対象以外のメニュー出力を許可する。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("-- scores (現在位置: scores、配列、要素数 3) --")))
        .Times(AtLeast(1)); // [Pre-Assert確認_正常系] - 配列の現在位置を表示すること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("1) scores[1]")))
        .Times(AtLeast(1)); // [Pre-Assert確認_正常系] - 配列要素を完全パスで表示すること。

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "scores");
    // [手順_正常系] - 配列全体をパスで指定し、要素を選択して編集する。

    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 編集が成功すること。
    EXPECT_EQ(77, sample.scores[1]); // [確認_正常系] - メニューで選択した要素が更新されること。
}

TEST_F(StructMetaPatchTest, PathEndingAtStructArrayOpensElementAndFieldMenus)
{
    Sample sample = {}; // [準備_正常系] - 構造体配列を持つ構造体を用意する。
    expect_inputs({"1", "1", "Osaka", "", ""});

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "addresses");
    // [手順_正常系] - 構造体配列全体を指定し、要素とフィールドを選択して編集する。

    EXPECT_EQ(CPLAT_OK, actual_ret);                 // [確認_正常系] - 編集が成功すること。
    EXPECT_STREQ("Osaka", sample.addresses[1].city); // [確認_正常系] - 選択した要素の city が更新されること。
}

TEST_F(StructMetaPatchTest, EmptyValueKeepsCurrentValue)
{
    Sample sample = {};
    sample.id = 12; // [準備_正常系] - 変更前の値を設定する。
    expect_inputs({""});

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "id");
    // [手順_正常系] - 値の入力で空行を指定する。

    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - 変更なしを正常終了として扱うこと。
    EXPECT_EQ(12, sample.id);        // [確認_正常系] - 元の値を維持すること。
}

TEST_F(StructMetaPatchTest, MenuSelectsFieldByMemberName)
{
    // Arrange
    Sample sample = {};
    expect_inputs({"id", "42", ""});

    // Pre-Assert

    // Act
    int actual_ret = struct_meta_patch_interactive(&kSampleDescriptor, &sample);
    // [手順] - メンバー名で id を選択して編集する。

    // Assert
    EXPECT_EQ(CPLAT_OK, actual_ret); // [確認_正常系] - メンバー名選択後の編集が成功すること。
    EXPECT_EQ(42, sample.id);        // [確認_正常系] - メンバー名で選択した id が更新されること。
    EXPECT_THAT(prompts, Contains(HasSubstr("フィールド番号またはメンバー名を選択")));
    // [確認_正常系] - メンバー名を入力できるプロンプトを表示すること。
}

TEST_F(StructMetaPatchTest, DrillDownRemainsAvailable)
{
    Sample sample = {}; // [準備_正常系] - 従来のメニュー選択に使う構造体を用意する。
    expect_inputs({"1", "0", "1", "Tokyo", "", "", ""});
    EXPECT_CALL(mock_stdio, printf(_, _, _, _)).Times(AnyNumber());
    // [Pre-Assert手順] - 検証対象以外のメニュー出力を許可する。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("-- Sample (現在位置: <root>) --")))
        .Times(AtLeast(1)); // [Pre-Assert確認_正常系] - ルートの現在位置を表示すること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("1) addresses [配列 2 件]")))
        .Times(AtLeast(1)); // [Pre-Assert確認_正常系] - ルート候補をパスとして表示すること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("0) addresses[0]")))
        .Times(AtLeast(1)); // [Pre-Assert確認_正常系] - 選択した配列の要素パスを表示すること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, HasSubstr("1) addresses[0].city = \"\"")))
        .Times(AtLeast(1)); // [Pre-Assert確認_正常系] - ネスト先の候補を完全パスで表示すること。

    int actual_ret = struct_meta_patch_interactive(&kSampleDescriptor, &sample);
    // [手順_正常系] - 従来のドリルダウン式で addresses[0].city を編集する。

    EXPECT_EQ(CPLAT_OK, actual_ret);                 // [確認_正常系] - 従来の編集が成功すること。
    EXPECT_STREQ("Tokyo", sample.addresses[0].city); // [確認_正常系] - 選択したフィールドが更新されること。
    EXPECT_THAT(prompts, Contains(HasSubstr("addresses[0].city (現在値")));
    // [確認_正常系] - 値入力にも完全パスを表示すること。
}

TEST_F(StructMetaPatchTest, InvalidPathIsRejectedBeforePromptCreation)
{
    Sample sample = {}; // [準備_異常系] - パス解決対象の構造体を用意する。
    EXPECT_CALL(mock_cplat, cplat_prompt_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - パス解決に失敗した場合はプロンプトを作成しないこと。

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "addresses[2].city");
    // [手順_異常系] - 範囲外の配列インデックスを指定する。

    EXPECT_EQ(CPLAT_ERR_OUT_OF_RANGE, actual_ret); // [確認_異常系] - 範囲外エラーを返すこと。
}

TEST_F(StructMetaPatchTest, UnknownFieldIsRejectedBeforePromptCreation)
{
    Sample sample = {}; // [準備_異常系] - パス解決対象の構造体を用意する。
    EXPECT_CALL(mock_cplat, cplat_prompt_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - 未知フィールドの場合はプロンプトを作成しないこと。

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "unknown");
    // [手順_異常系] - 存在しないフィールドを指定する。

    EXPECT_EQ(CPLAT_ERR_NOT_FOUND, actual_ret); // [確認_異常系] - 未検出エラーを返すこと。
}

TEST_F(StructMetaPatchTest, InvalidArgumentsAreRejectedBeforePromptCreation)
{
    Sample sample = {}; // [準備_異常系] - 引数検査に使う構造体を用意する。
    EXPECT_CALL(mock_cplat, cplat_prompt_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - 引数が不正な場合はプロンプトを作成しないこと。

    int null_path_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, nullptr);
    int empty_path_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "");
    // [手順_異常系] - NULL と空文字列のパスを指定する。

    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, null_path_ret);  // [確認_異常系] - NULL を拒否すること。
    EXPECT_EQ(CPLAT_ERR_INVALID_ARGUMENT, empty_path_ret); // [確認_異常系] - 空文字列を拒否すること。
}

TEST_F(StructMetaPatchTest, CorruptDescriptorIsRejectedBeforePromptCreation)
{
    Sample sample = {}; // [準備_異常系] - 不正な記述子の対象インスタンスを用意する。
    const struct_meta_descriptor corrupt_descriptor = {nullptr, sizeof(Sample), kSampleFields, 4, nullptr, nullptr, 0};
    EXPECT_CALL(mock_cplat, cplat_prompt_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - 記述子検査に失敗した場合はプロンプトを作成しないこと。

    int actual_ret = struct_meta_patch_path_interactive(&corrupt_descriptor, &sample, "id");
    // [手順_異常系] - 名前のない壊れた記述子を指定する。

    EXPECT_EQ(CPLAT_ERR_CORRUPT_DESCRIPTOR, actual_ret); // [確認_異常系] - 記述子破損エラーを返すこと。
}

TEST_F(StructMetaPatchTest, PromptCreationFailureIsReturned)
{
    Sample sample = {}; // [準備_異常系] - 編集対象の構造体を用意する。
    EXPECT_CALL(mock_cplat, cplat_prompt_create(nullptr))
        .WillOnce(Return(nullptr)); // [Pre-Assert確認_異常系] - プロンプト生成失敗を発生させる。

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "id");
    // [手順_異常系] - プロンプトを生成して編集を開始する。

    EXPECT_EQ(CPLAT_ERR_OUT_OF_MEMORY, actual_ret); // [確認_異常系] - メモリ不足エラーを返すこと。
}

TEST_F(StructMetaPatchTest, PromptInputFailureIsReturnedAfterDisposal)
{
    Sample sample = {}; // [準備_異常系] - 編集対象の構造体を用意する。
    EXPECT_CALL(mock_cplat, cplat_prompt_create(nullptr)).WillOnce(Return(prompt));
    EXPECT_CALL(mock_cplat, cplat_prompt_readline_fmt_at(prompt, _, _, _, _, _, _))
        .WillOnce(Return(CPLAT_ERR_CANCELED)); // [Pre-Assert確認_異常系] - 入力キャンセルを発生させる。
    EXPECT_CALL(mock_cplat, cplat_prompt_dispose(prompt))
        .WillOnce(Return()); // [Pre-Assert確認_異常系] - 失敗時にもプロンプトを破棄すること。

    int actual_ret = struct_meta_patch_path_interactive(&kSampleDescriptor, &sample, "id");
    // [手順_異常系] - 対話入力中にキャンセルする。

    EXPECT_EQ(CPLAT_ERR_CANCELED, actual_ret); // [確認_異常系] - 入力元のエラーを返すこと。
}
