#!/usr/bin/env python3
"""check_functional_spec.py の単体テスト。"""

from __future__ import annotations

import importlib.util
from pathlib import Path
import sys
import tempfile
import unittest


SCRIPT_PATH = Path(__file__).resolve().parent / "check_functional_spec.py"
SPEC = importlib.util.spec_from_file_location("check_functional_spec", SCRIPT_PATH)
assert SPEC is not None
assert SPEC.loader is not None
CHECKER = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = CHECKER
SPEC.loader.exec_module(CHECKER)


UUID_1 = "123e4567-e89b-42d3-a456-426614174000"
UUID_2 = "223e4567-e89b-42d3-b456-426614174001"
UUID_3 = "323e4567-e89b-42d3-8456-426614174002"

SAMPLE_SUBJECTS = {
    "CLOCK": "sample の時計・時刻機能",
    "BASE": "sample の基盤機能",
    "STRING_CATALOG": "sample の文字列カタログ機能",
}


class CheckFunctionalSpecTest(unittest.TestCase):
    def _write(self, root: Path, relative_path: str, content: str) -> None:
        path = root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")

    def _write_guideline(
        self,
        root: Path,
        app: str = "sample",
        prefix: str = "SAMPLE",
        tag: str = "sample-req",
        heading: str | None = None,
        subjects: dict[str, str] | None = None,
        with_id_config: bool = True,
        with_subjects: bool = True,
    ) -> None:
        heading = f"{app} の要件" if heading is None else heading
        subjects = SAMPLE_SUBJECTS if subjects is None else subjects
        lines = [f"# {app} 機能仕様の記載規範", ""]
        if with_id_config:
            lines += [
                "## 要件 ID の構成",
                "",
                "| 項目 | 値 |",
                "|---|---|",
                f"| 要件 ID 接頭辞 | `{prefix}` |",
                f"| 参照コメント タグ | `{tag}` |",
                f"| 機能要件表の見出し | `{heading}` |",
                "",
            ]
        if with_subjects:
            lines += ["## カテゴリごとの主語", "", "| カテゴリ | 主語 |", "|---|---|"]
            lines += [
                f"| `{category}` | {subject} |"
                for category, subject in subjects.items()
            ]
            lines.append("")
        self._write(
            root, f"app/{app}/docs/functional-spec-guideline.md", "\n".join(lines) + "\n"
        )

    def _write_spec(
        self,
        root: Path,
        rows: list[str],
        app: str = "sample",
        name: str = "clock",
        heading: str | None = None,
    ) -> None:
        heading = f"{app} の要件" if heading is None else heading
        body = "\n".join(rows)
        self._write(
            root,
            f"app/{app}/docs/functional-spec/{name}.md",
            f"# {name} 機能仕様\n\n"
            "## 機能要件\n\n"
            f"| 要件 ID | {heading} |\n"
            "|---|---|\n"
            f"{body}\n",
        )

    def _write_app(self, root: Path, rows: list[str] | None = None) -> None:
        self._write_guideline(root)
        self._write_spec(
            root, [self._row("SAMPLE-CLOCK-FUNC-001", UUID_1)] if rows is None else rows
        )

    @staticmethod
    def _row(requirement_id: str, requirement_uuid: str) -> str:
        return CheckFunctionalSpecTest._row_with_body(
            requirement_id,
            requirement_uuid,
            "sample の時計・時刻機能は、時計を提供します。",
        )

    @staticmethod
    def _row_with_body(
        requirement_id: str,
        requirement_uuid: str,
        body: str,
        tag: str = "sample-req",
    ) -> str:
        return (
            f"| `{requirement_id}` "
            f"<!-- {tag}: uuid={requirement_uuid} --> "
            f"| {body} |"
        )

    def test_accepts_all_reference_formats(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            requirement_id = "SAMPLE-CLOCK-FUNC-001"
            self._write_app(root)
            self._write(
                root,
                "app/sample/docs/design.md",
                f"要件: `{requirement_id}` "
                f"<!-- sample-req: uuid={UUID_1} -->\n",
            )
            self._write(
                root,
                "app/sample/prod/sample.c",
                f"/* sample-req: id={requirement_id}; uuid={UUID_1} */\n"
                "/** 時計を取得する。 */\n"
                "void get_clock(void);\n",
            )
            self._write(
                root,
                "app/sample/prod/sample.cpp",
                f"// sample-req: id={requirement_id}; uuid={UUID_1}\n"
                "/** 時計を取得する。 */\n"
                "void get_clock_cxx(void);\n",
            )
            self._write(
                root,
                "app/sample/prod/sample.cc",
                f"/* sample-req: id={requirement_id}; uuid={UUID_1} */\n"
                "void get_clock_cc(void);\n",
            )
            test_blocks = ""
            for macro in ("TEST", "TEST_F", "TEST_P", "TYPED_TEST"):
                test_blocks += (
                    "// 単調増加クロックを確認する。\n"
                    f"// sample-req: id={requirement_id}; uuid={UUID_1}\n"
                    f"{macro}(clockTest, monotonic_clock)\n"
                    "{\n"
                    "    // Arrange\n"
                    "}\n"
                )
            self._write(root, "app/sample/test/sample.cpp", test_blocks)

            result = CHECKER.check_workspace(root)

            self.assertEqual([], result.errors)
            self.assertEqual(1, result.requirement_count)
            self.assertEqual(8, result.reference_count)
            self.assertEqual({"sample": 1}, result.counts_by_app)

    def test_rejects_duplicate_id_and_uuid(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            row = self._row("SAMPLE-CLOCK-FUNC-001", UUID_1)
            self._write_app(root, [row, row])

            result = CHECKER.check_workspace(root)

            self.assertTrue(any("要件 ID が重複" in error for error in result.errors))
            self.assertTrue(any("UUID が重複" in error for error in result.errors))

    def test_rejects_malformed_uuid(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_app(
                root,
                [
                    "| `SAMPLE-CLOCK-FUNC-001` "
                    "<!-- sample-req: uuid=not-a-uuid --> "
                    "| sample の時計・時刻機能は、時計を提供します。 |"
                ],
            )

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any("機能要件行の書式が不正" in error for error in result.errors)
            )

    def test_rejects_unknown_uuid_and_stale_id(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            current_id = "SAMPLE-CLOCK-FUNC-001"
            self._write_app(root)
            self._write(
                root,
                "app/sample/docs/design.md",
                f"`{current_id}` <!-- sample-req: uuid={UUID_2} -->\n"
                f"`SAMPLE-CLOCK-FUNC-002` "
                f"<!-- sample-req: uuid={UUID_1} -->\n",
            )

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any("正本に存在しない UUID" in error for error in result.errors)
            )
            self.assertTrue(any("現在の要件 ID" in error for error in result.errors))

    def test_rejects_unpaired_and_legacy_ids(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            current_id = "SAMPLE-CLOCK-FUNC-001"
            self._write_app(root)
            self._write(
                root,
                "app/sample/docs/design.md",
                f"要件: `{current_id}`\n旧要件: `SAMPLE-CLOCK-001`\n",
            )

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any("UUID が併記されていません" in error for error in result.errors)
            )
            self.assertTrue(any("旧形式の要件 ID" in error for error in result.errors))

    def test_rejects_test_marker_without_description_or_adjacent_macro(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            requirement_id = "SAMPLE-CLOCK-FUNC-001"
            self._write_app(root)
            self._write(
                root,
                "app/sample/test/sample.cpp",
                f"// sample-req: id={requirement_id}; uuid={UUID_1}\n"
                "\n"
                "TEST(clockTest, monotonic_clock)\n"
                "{\n"
                "    // Arrange\n"
                "}\n",
            )

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any("テスト項目の説明が記述されていません" in error for error in result.errors)
            )
            self.assertTrue(
                any("直後にテスト マクロが記述されていません" in error for error in result.errors)
            )

    def test_rejects_comment_style_for_the_wrong_medium(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            requirement_id = "SAMPLE-CLOCK-FUNC-001"
            self._write_app(root)
            self._write(
                root,
                "app/sample/prod/sample.c",
                f"// sample-req: id={requirement_id}; uuid={UUID_1}\n",
            )
            self._write(
                root,
                "app/sample/prod/sample.h",
                f"// sample-req: id={requirement_id}; uuid={UUID_1}\n",
            )
            self._write(
                root,
                "app/sample/test/sample.cpp",
                "// 単調増加クロックを確認する。\n"
                f"/* sample-req: id={requirement_id}; uuid={UUID_1} */\n"
                "TEST(clockTest, monotonic_clock)\n"
                "{\n"
                "    // Arrange\n"
                "}\n",
            )

            result = CHECKER.check_workspace(root)

            product_errors = [
                error for error in result.errors if "製品コードの要件コメントが不正" in error
            ]
            self.assertEqual(2, len(product_errors))
            self.assertTrue(
                any("テストの要件コメントが不正" in error for error in result.errors)
            )

    def test_rejects_wrong_or_omitted_requirement_subject(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_guideline(root)
            self._write_spec(
                root,
                [
                    self._row_with_body(
                        "SAMPLE-CLOCK-FUNC-001",
                        UUID_1,
                        "sample の基盤機能は、時計を提供します。",
                    ),
                    self._row_with_body(
                        "SAMPLE-CLOCK-FUNC-002",
                        UUID_2,
                        "sample は、時計を提供します。",
                    ),
                    self._row_with_body(
                        "SAMPLE-CLOCK-FUNC-003",
                        UUID_3,
                        "時計を提供します。",
                    ),
                ],
            )

            result = CHECKER.check_workspace(root)

            subject_errors = [
                error for error in result.errors if "要件文の主語" in error
            ]
            self.assertEqual(3, len(subject_errors))

    def test_accepts_underscore_category_id(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_guideline(root)
            self._write_spec(
                root,
                [
                    self._row_with_body(
                        "SAMPLE-STRING_CATALOG-FUNC-001",
                        UUID_1,
                        "sample の文字列カタログ機能は、文字列を提供します。",
                    )
                ],
                name="string_catalog",
            )

            result = CHECKER.check_workspace(root)

            self.assertEqual([], result.errors)
            self.assertEqual(1, result.requirement_count)

    def test_rejects_underscore_category_mismatch(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            # ファイル名 (string_catalog.md) から導出したカテゴリは
            # STRING_CATALOG であるが、要件 ID のカテゴリは CLOCK であるため不一致となる。
            self._write_guideline(root)
            self._write_spec(
                root,
                [self._row("SAMPLE-CLOCK-FUNC-001", UUID_1)],
                name="string_catalog",
            )

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any(
                    "要件 ID のカテゴリが文書名と一致しません" in error
                    for error in result.errors
                )
            )

    def test_rejects_category_missing_from_subject_table(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_guideline(root, subjects={"BASE": "sample の基盤機能"})
            self._write_spec(root, [self._row("SAMPLE-CLOCK-FUNC-001", UUID_1)])

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any("主語表に定義されていません" in error for error in result.errors)
            )

    def test_checks_each_app_with_its_own_prefix_and_tag(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_app(root)
            self._write_guideline(
                root,
                app="other",
                prefix="OTHER",
                tag="other-req",
                subjects={"BASE": "other の基盤機能"},
            )
            self._write_spec(
                root,
                [
                    self._row_with_body(
                        "OTHER-BASE-FUNC-001",
                        UUID_2,
                        "other の基盤機能は、結果を返します。",
                        tag="other-req",
                    )
                ],
                app="other",
                name="base",
            )
            # 別の app の要件を、その app のタグで参照できる。
            self._write(
                root,
                "app/other/docs/design.md",
                f"要件: `SAMPLE-CLOCK-FUNC-001` <!-- sample-req: uuid={UUID_1} -->\n",
            )

            result = CHECKER.check_workspace(root)

            self.assertEqual([], result.errors)
            self.assertEqual(2, result.requirement_count)
            self.assertEqual({"other": 1, "sample": 1}, result.counts_by_app)
            self.assertEqual(1, result.reference_count)

    def test_rejects_tag_that_does_not_match_the_id_prefix(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_app(root)
            self._write_guideline(
                root,
                app="other",
                prefix="OTHER",
                tag="other-req",
                subjects={"BASE": "other の基盤機能"},
            )
            self._write_spec(
                root,
                [
                    self._row_with_body(
                        "OTHER-BASE-FUNC-001",
                        UUID_2,
                        "other の基盤機能は、結果を返します。",
                        tag="other-req",
                    )
                ],
                app="other",
                name="base",
            )
            self._write(
                root,
                "app/other/docs/design.md",
                f"要件: `SAMPLE-CLOCK-FUNC-001` <!-- other-req: uuid={UUID_1} -->\n",
            )

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any("に対するタグは" in error for error in result.errors)
            )

    def test_rejects_duplicate_prefix_across_apps(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_app(root)
            self._write_guideline(root, app="other", tag="other-req")
            self._write_spec(
                root,
                [self._row("SAMPLE-CLOCK-FUNC-002", UUID_2)],
                app="other",
                heading="sample の要件",
            )

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any("要件 ID 接頭辞が" in error for error in result.errors)
            )

    def test_rejects_guideline_without_required_tables(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_guideline(root, with_id_config=False)
            self._write_spec(root, [self._row("SAMPLE-CLOCK-FUNC-001", UUID_1)])

            result = CHECKER.check_workspace(root)

            self.assertTrue(any("項目が定義されていません" in error for error in result.errors))

        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_guideline(root, with_subjects=False)
            self._write_spec(root, [self._row("SAMPLE-CLOCK-FUNC-001", UUID_1)])

            result = CHECKER.check_workspace(root)

            self.assertTrue(
                any("カテゴリの主語が定義されていません" in error for error in result.errors)
            )

    def test_ignores_table_examples_inside_fenced_blocks(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write(
                root,
                "app/sample/docs/functional-spec-guideline.md",
                "# sample 機能仕様の記載規範\n\n"
                "## 要件 ID の構成\n\n"
                "| 項目 | 値 |\n"
                "|---|---|\n"
                "| 要件 ID 接頭辞 | `SAMPLE` |\n"
                "| 参照コメント タグ | `sample-req` |\n"
                "| 機能要件表の見出し | `sample の要件` |\n\n"
                "## カテゴリごとの主語\n\n"
                "次の書式で記載します。\n\n"
                "```markdown\n"
                "| カテゴリ | 主語 |\n"
                "|---|---|\n"
                "| `EXAMPLE` | sample の例示機能 |\n"
                "```\n\n"
                "| カテゴリ | 主語 |\n"
                "|---|---|\n"
                "| `CLOCK` | sample の時計・時刻機能 |\n",
            )
            self._write_spec(root, [self._row("SAMPLE-CLOCK-FUNC-001", UUID_1)])

            result = CHECKER.check_workspace(root)

            self.assertEqual([], result.errors)
            self.assertEqual(1, result.requirement_count)

    def test_reports_missing_requirement_rows(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory).resolve()
            self._write_guideline(root)
            self._write(
                root,
                "app/sample/docs/functional-spec/clock.md",
                "# clock 機能仕様\n\n## 機能要件\n\n"
                "| 要件 ID | sample の要件 |\n|---|---|\n",
            )

            result = CHECKER.check_workspace(root)

            self.assertTrue(any("機能要件が定義されていません" in error for error in result.errors))


if __name__ == "__main__":
    unittest.main()
