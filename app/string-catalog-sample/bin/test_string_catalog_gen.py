#!/usr/bin/env python3
"""string_catalog_gen.py の単体テスト。"""

from __future__ import annotations

import unittest

import string_catalog_gen as gen


class StripJsoncTest(unittest.TestCase):
    """JSONC の前処理を確認する。"""

    def test_line_comment(self):
        self.assertEqual(gen.strip_jsonc('{"a": 1} // 末尾'), '{"a": 1} ')

    def test_block_comment(self):
        self.assertEqual(gen.strip_jsonc('{/* 中 */"a": 1}'), '{"a": 1}')

    def test_keeps_comment_like_text_in_string(self):
        source = '{"url": "https://example.com/a//b", "path": "/*not a comment*/"}'
        self.assertEqual(gen.strip_jsonc(source), source)

    def test_keeps_escaped_quote(self):
        source = '{"text": "彼は \\"はい\\" と言った // ではない"}'
        self.assertEqual(gen.strip_jsonc(source), source)

    def test_removes_trailing_comma(self):
        self.assertEqual(gen.strip_jsonc('{"a": 1,}'), '{"a": 1}')
        self.assertEqual(gen.strip_jsonc('[1, 2,\n]'), '[1, 2\n]')

    def test_keeps_comma_in_string(self):
        source = '{"text": "a,}"}'
        self.assertEqual(gen.strip_jsonc(source), source)


class JoinTextTest(unittest.TestCase):
    """文字列と文字列配列の受け取りを確認する。"""

    def test_string(self):
        self.assertEqual(gen.join_text("あ"), "あ")

    def test_list(self):
        self.assertEqual(gen.join_text(["あ", "い"]), "あ い")

    def test_rejects_other(self):
        with self.assertRaises(gen.DefinitionError):
            gen.join_text(1)


class PlaceholderTest(unittest.TestCase):
    """位置指定の解析が、実装と同じ構文を受け付けることを確認する。"""

    def test_single_digit(self):
        self.assertEqual(gen.placeholder_indices("{0}/{9}"), [0, 9])

    def test_two_digits(self):
        self.assertEqual(gen.placeholder_indices("{10}/{31}"), [10, 31])

    def test_escape(self):
        self.assertEqual(gen.placeholder_indices("{{ default }}"), [])

    def test_rejects_three_digits(self):
        with self.assertRaises(gen.DefinitionError):
            gen.placeholder_indices("{100}")

    def test_rejects_leading_zero(self):
        with self.assertRaises(gen.DefinitionError):
            gen.placeholder_indices("{01}")

    def test_rejects_unpaired_brace(self):
        with self.assertRaises(gen.DefinitionError):
            gen.placeholder_indices("a}b")

    def test_rejects_non_digit(self):
        with self.assertRaises(gen.DefinitionError):
            gen.placeholder_indices("{abc}")


def minimal_document(**overrides):
    """検査の対象となる最小の定義を組み立てる。"""
    document = {
        "library_prefix": "string_catalog",
        "module_prefix": "string_catalog_definition",
        "id_enum": "string_catalog_id",
        "languages": ["neutral", "japanese"],
        "strings": [
            {
                "id": "STRING_CATALOG_ID_A",
                "id_text": "ID_0001",
                "category": 1,
                "brief": "あ。",
                "summary": "あを組み立てます。",
                "arguments": [{"kind": "STRING", "name": "path", "description": "パス。"}],
                "texts": {"neutral": "{0}"},
                "notes": {"neutral": ""},
            }
        ],
    }
    document.update(overrides)
    return document


class ValidateTest(unittest.TestCase):
    """定義の検査を確認する。"""

    def test_accepts_minimal(self):
        self.assertEqual(len(gen.validate(minimal_document())), 1)

    def test_rejects_duplicate_id(self):
        document = minimal_document()
        document["strings"].append(dict(document["strings"][0]))
        document["strings"][1]["id_text"] = "ID_0002"
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)

    def test_rejects_duplicate_id_text(self):
        document = minimal_document()
        duplicated = dict(document["strings"][0])
        duplicated["id"] = "STRING_CATALOG_ID_B"
        document["strings"].append(duplicated)
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)

    def test_rejects_unknown_kind(self):
        document = minimal_document()
        document["strings"][0]["arguments"][0]["kind"] = "FLOAT"
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)

    def test_rejects_placeholder_over_argument_count(self):
        document = minimal_document()
        document["strings"][0]["texts"]["neutral"] = "{1}"
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)

    def test_rejects_missing_neutral_text(self):
        document = minimal_document()
        document["strings"][0]["texts"] = {"japanese": "{0}"}
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)

    def test_rejects_unlisted_language(self):
        document = minimal_document()
        document["strings"][0]["texts"]["german"] = "{0}"
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)

    def test_rejects_symbolic_category(self):
        # 分類値を生値に限るのは、生成物を特定の app の列挙から独立させるため
        document = minimal_document()
        document["strings"][0]["category"] = "STRING_CATALOG_TRACE_LEVEL_ERROR"
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)

    def test_rejects_boolean_category(self):
        document = minimal_document()
        document["strings"][0]["category"] = True
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)

    def test_rejects_too_many_arguments(self):
        document = minimal_document()
        argument = document["strings"][0]["arguments"][0]
        document["strings"][0]["arguments"] = [dict(argument) for _ in range(gen.ARGUMENT_MAX + 1)]
        with self.assertRaises(gen.DefinitionError):
            gen.validate(document)


class WrapperNameTest(unittest.TestCase):
    """関数名の導出規則を確認する。"""

    def test_lowercases_whole_id(self):
        document = minimal_document()
        self.assertEqual(
            gen.wrapper_name(document, "STRING_CATALOG_ID_FILE_OPEN_FAILED"),
            "string_catalog_definition_string_catalog_id_file_open_failed",
        )

    def test_has_no_double_underscore(self):
        document = minimal_document()
        name = gen.wrapper_name(document, "STRING_CATALOG_ID_A")
        self.assertNotIn("__", name)


class ArgumentTypeTest(unittest.TestCase):
    """引数種別と C の型の対応を確認する。"""

    def test_covers_every_kind_used_by_the_library(self):
        expected = {
            "STRING", "CHAR", "INT8", "UINT8", "INT16", "UINT16", "INT32", "UINT32",
            "INT64", "UINT64", "HEX8", "HEX16", "HEX32", "HEX64", "SIZE", "SSIZE",
            "POINTER", "DOUBLE", "ERROR_CODE",
        }
        self.assertEqual(set(gen.ARGUMENT_TYPES), expected)


if __name__ == "__main__":
    unittest.main()


class GeneratedOutputTest(unittest.TestCase):
    """生成物が特定の app へ依存しないことを確認する。"""

    def setUp(self):
        self.document = minimal_document(module_dir="prod/src/cmd/example")
        self.strings = gen.validate(self.document)

    def test_header_includes_only_library_and_standard_headers(self):
        header = gen.emit_header(self.document, self.strings, "example.jsonc")
        includes = [line for line in header.splitlines() if line.startswith("#include")]
        self.assertEqual(
            includes,
            [
                "#include <string_catalog/string_catalog_spec.h>",
                "#include <stdarg.h>",
                "#include <stddef.h>",
                "#include <stdint.h>",
            ],
        )

    def test_source_emits_raw_category(self):
        source = gen.emit_source(self.document, self.strings, "example.jsonc")
        self.assertIn("    {STRING_CATALOG_ID_A,\n     1,\n", source)
        self.assertNotIn("TRACE_LEVEL", source)

    def test_module_dir_appears_in_documentation(self):
        header = gen.emit_header(self.document, self.strings, "example.jsonc")
        self.assertIn("`prod/src/cmd/example/` のモジュール私有ヘッダー", header)
        source = gen.emit_source(self.document, self.strings, "example.jsonc")
        self.assertIn("@file           src/cmd/example/string_catalog_definition.c", source)
