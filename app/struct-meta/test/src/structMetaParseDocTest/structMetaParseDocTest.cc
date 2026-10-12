#include <testfw.h>

extern "C"
{
#include "doc.h"
}

// 後置コメント内の汎用属性を解析し、brief から除外してキーと値を取得できることの確認
TEST(structMetaParseDocTest, parses_generic_attributes_and_removes_them_from_brief)
{
    // Arrange
    struct_meta_diagnostic diagnostic = {};
    const char comment[] = "/**< 識別子です。 @struct_meta{json.name = person_id} @struct_meta{sample.flag} */";

    // Pre-Assert

    // Act
    struct_meta_internal_parse_doc_attrs actual = struct_meta_internal_parse_doc_attrs_from_raw(
        comment, 1, 10, &diagnostic); // [手順] - 汎用属性を含む後置コメントを解析する。

    // Assert
    EXPECT_EQ(0, actual.invalid); // [確認_正常系] - actual.invalid が 0 であること。
    EXPECT_STREQ(
        "識別子です。",
        actual
            .brief); // [確認_正常系] - actual.brief が "識別子です。" であること。(汎用属性の記述が brief から除外されたことの確認)
    ASSERT_NE(nullptr, actual.attributes); // [確認_正常系] - actual.attributes が nullptr 以外であること。
    EXPECT_STREQ("json.name",
                 actual.attributes->key); // [確認_正常系] - actual.attributes->key が "json.name" であること。
    EXPECT_STREQ("person_id",
                 actual.attributes->value);      // [確認_正常系] - actual.attributes->value が "person_id" であること。
    ASSERT_NE(nullptr, actual.attributes->next); // [確認_正常系] - actual.attributes->next が nullptr 以外であること。
    EXPECT_STREQ(
        "sample.flag",
        actual.attributes->next->key); // [確認_正常系] - actual.attributes->next->key が "sample.flag" であること。
    EXPECT_EQ(nullptr,
              actual.attributes->next->value); // [確認_正常系] - actual.attributes->next->value が nullptr であること。
}

// 1つのコメント内で同じ属性名が重複している場合に解析エラーとなることの確認
TEST(structMetaParseDocTest, rejects_duplicate_attributes)
{
    // Arrange
    struct_meta_diagnostic diagnostic = {};
    const char comment[] = "/**< @struct_meta{sample.key=1} @struct_meta{sample.key=2} */";

    // Pre-Assert

    // Act
    struct_meta_internal_parse_doc_attrs actual = struct_meta_internal_parse_doc_attrs_from_raw(
        comment, 1, 20, &diagnostic); // [手順] - 重複する属性を含むコメントを解析する。

    // Assert
    EXPECT_NE(
        0, actual.invalid); // [確認_異常系] - actual.invalid が 0 以外であること。(重複する属性が拒否されたことの確認)
    EXPECT_EQ(
        20, diagnostic.line); // [確認_異常系] - diagnostic.line が 20 であること。(診断へ行番号が記録されたことの確認)
    EXPECT_STRNE(
        "",
        diagnostic
            .message); // [確認_異常系] - diagnostic.message が "" 以外であること。(診断へ原因が記録されたことの確認)
}

// 前置コメントと後置コメントの間で同じ属性名が重複している場合に統合時にエラーとなることの確認
TEST(structMetaParseDocTest, rejects_duplicate_attributes_across_comments)
{
    // Arrange
    struct_meta_diagnostic diagnostic = {};
    struct_meta_internal_parse_doc_attrs prefix =
        struct_meta_internal_parse_doc_attrs_from_raw("/** @struct_meta{sample.key=1} */", 0, 21, &diagnostic);
    struct_meta_internal_parse_doc_attrs postfix =
        struct_meta_internal_parse_doc_attrs_from_raw("/**< @struct_meta{sample.key=2} */", 1, 21, &diagnostic);

    // Pre-Assert

    // Act
    struct_meta_internal_parse_doc_attrs actual = struct_meta_internal_parse_doc_attrs_choose(
        prefix, postfix, 21, &diagnostic); // [手順] - 同じ属性名を持つ前置と後置コメントを結合する。

    // Assert
    EXPECT_NE(
        0,
        actual
            .invalid); // [確認_異常系] - actual.invalid が 0 以外であること。(コメントをまたいで重複する属性が拒否されたことの確認)
}

// 廃止された古い JSON 固有コマンドが汎用属性として誤変換されず構文エラーにもならないことの確認
TEST(structMetaParseDocTest, does_not_convert_legacy_json_commands)
{
    // Arrange
    struct_meta_diagnostic diagnostic = {};
    const char comment[] = "/**< @json_name{person_id} @json_required @json_ignore */";

    // Pre-Assert

    // Act
    struct_meta_internal_parse_doc_attrs actual = struct_meta_internal_parse_doc_attrs_from_raw(
        comment, 1, 22, &diagnostic); // [手順] - 廃止した JSON 固有コマンドを含むコメントを解析する。

    // Assert
    EXPECT_EQ(
        0,
        actual
            .invalid); // [確認_正常系] - actual.invalid が 0 であること。(未知の Doxygen コマンドが汎用属性の構文エラーにならないことの確認)
    EXPECT_EQ(
        nullptr,
        actual
            .attributes); // [確認_正常系] - actual.attributes が nullptr であること。(廃止した JSON 固有コマンドが属性へ変換されないことの確認)
}

// 空の属性名や値、空白入りキー、閉じ括弧不足、改行混入などの不正な属性構文が拒否されることの確認
TEST(structMetaParseDocTest, rejects_invalid_attribute_forms)
{
    // Arrange
    struct_meta_diagnostic diagnostic = {};
    const char empty_key[] = "/**< @struct_meta{=value} */";
    const char empty_value[] = "/**< @struct_meta{sample.key=} */";
    const char invalid_key[] = "/**< @struct_meta{sample key=value} */";
    const char missing_close[] = "/**< @struct_meta{sample.key=value */";
    const char multiline[] = "/**< @struct_meta{sample.key=line1\nline2} */";

    // Pre-Assert

    // Act
    struct_meta_internal_parse_doc_attrs empty_key_result =
        struct_meta_internal_parse_doc_attrs_from_raw(empty_key, 1, 30, &diagnostic); // [手順] - 空の属性名を解析する。
    struct_meta_internal_parse_doc_attrs empty_value_result = struct_meta_internal_parse_doc_attrs_from_raw(
        empty_value, 1, 31, &diagnostic); // [手順] - 空の属性値を解析する。
    struct_meta_internal_parse_doc_attrs invalid_key_result = struct_meta_internal_parse_doc_attrs_from_raw(
        invalid_key, 1, 32, &diagnostic); // [手順] - 空白を含む属性名を解析する。
    struct_meta_internal_parse_doc_attrs missing_close_result = struct_meta_internal_parse_doc_attrs_from_raw(
        missing_close, 1, 33, &diagnostic); // [手順] - 閉じ波括弧が無い属性を解析する。
    struct_meta_internal_parse_doc_attrs multiline_result = struct_meta_internal_parse_doc_attrs_from_raw(
        multiline, 1, 34, &diagnostic); // [手順] - 改行を含む属性値を解析する。

    // Assert
    EXPECT_NE(
        0,
        empty_key_result
            .invalid); // [確認_異常系] - empty_key_result.invalid が 0 以外であること。(空の属性名が拒否されたことの確認)
    EXPECT_NE(
        0,
        empty_value_result
            .invalid); // [確認_異常系] - empty_value_result.invalid が 0 以外であること。(空の属性値が拒否されたことの確認)
    EXPECT_NE(
        0,
        invalid_key_result
            .invalid); // [確認_異常系] - invalid_key_result.invalid が 0 以外であること。(空白を含む属性名が拒否されたことの確認)
    EXPECT_NE(
        0,
        missing_close_result
            .invalid); // [確認_異常系] - missing_close_result.invalid が 0 以外であること。(閉じ波括弧が無い属性が拒否されたことの確認)
    EXPECT_NE(
        0,
        multiline_result
            .invalid); // [確認_異常系] - multiline_result.invalid が 0 以外であること。(改行を含む属性値が拒否されたことの確認)
}
