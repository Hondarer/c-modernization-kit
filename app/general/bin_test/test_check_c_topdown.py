#!/usr/bin/env python3
"""check_c_topdown.py の単体テスト。"""

from __future__ import annotations

import contextlib
import importlib.util
import io
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import textwrap
import unittest


SCRIPT_PATH = Path(__file__).resolve().parent.parent / "bin/check_c_topdown.py"
SPEC = importlib.util.spec_from_file_location("check_c_topdown", SCRIPT_PATH)
assert SPEC is not None
assert SPEC.loader is not None
CHECKER = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = CHECKER
SPEC.loader.exec_module(CHECKER)


def source(text: str) -> str:
    return textwrap.dedent(text).lstrip("\n")


def findings(text: str) -> list[tuple[int, str, str]]:
    """検出結果を、1 始まりの行番号、レベル、種別の組で返す。"""
    return [(item.line, item.level, item.rule) for item in CHECKER.check_text(source(text), "sample.c")]


class MissingPrototypeTest(unittest.TestCase):
    def test_static_definition_without_prototype(self) -> None:
        text = """
            #include "sample.h"

            static int helper(int value)
            {
                return value + 1;
            }

            int sample_run(int value)
            {
                return helper(value);
            }
            """
        self.assertEqual(findings(text), [
            (3, "error", "missing-prototype"),
            (8, "error", "non-topdown-order"),
        ])

    def test_topdown_file_has_no_finding(self) -> None:
        text = """
            #include "sample.h"

            static int helper(int value);
            static void unused_helper(void);

            static int counter = 0;

            int sample_run(int value)
            {
                return helper(value);
            }

            static int helper(int value)
            {
                return value + counter;
            }

            static void unused_helper(void)
            {
            }
            """
        self.assertEqual(findings(text), [])

    def test_prototype_after_first_definition_is_missing(self) -> None:
        text = """
            int sample_run(void)
            {
                return 0;
            }

            static void helper(void);

            static void helper(void)
            {
            }
            """
        self.assertEqual(findings(text), [(8, "error", "missing-prototype")])

    def test_prototype_without_static_is_not_accepted(self) -> None:
        text = """
            void helper(void);

            int sample_run(void)
            {
                return 0;
            }

            static void helper(void)
            {
            }
            """
        self.assertEqual(findings(text), [(8, "error", "missing-prototype")])

    def test_static_inline_is_treated_as_static(self) -> None:
        text = """
            static inline int twice(int value);

            int sample_run(int value)
            {
                return twice(value);
            }

            static inline int twice(int value)
            {
                return value * 2;
            }

            static inline int missing(void)
            {
                return 0;
            }
            """
        self.assertEqual(findings(text), [(13, "error", "missing-prototype")])

    def test_attributes_and_multiline_declarators(self) -> None:
        text = """
            static void fail(const char *message) __attribute__((noreturn));
            static int
            compute(int left,
                    int right);

            int sample_run(void)
            {
                compute(1, 2);
                fail("x");
            }

            static void fail(const char *message)
            {
            }

            static int
            compute(int left,
                    int right)
            {
                return left + right;
            }
            """
        self.assertEqual(findings(text), [])

    def test_function_pointer_variable_is_not_prototype(self) -> None:
        text = """
            static void (*callback)(void);
            static int (*const table[2])(int) = {0};

            int sample_run(void)
            {
                return 0;
            }

            static void callback_impl(void)
            {
            }
            """
        self.assertEqual(findings(text), [(9, "error", "missing-prototype")])

    def test_comments_and_literals_are_ignored(self) -> None:
        text = """
            /* static void ghost(void) { } */
            // static void ghost2(void) { }
            static const char *text = "static void ghost3(void) { }";
            static const char brace = '{';

            int sample_run(void)
            {
                return text[0] == brace;
            }
            """
        self.assertEqual(findings(text), [])


class NonTopdownOrderTest(unittest.TestCase):
    def test_public_function_after_static_function(self) -> None:
        text = """
            static void helper(void);

            static void helper(void)
            {
            }

            void sample_open(void)
            {
                helper();
            }

            void sample_close(void)
            {
            }
            """
        self.assertEqual(findings(text), [
            (7, "error", "non-topdown-order"),
            (12, "error", "non-topdown-order"),
        ])

    def test_entry_function_must_be_first_definition(self) -> None:
        text = """
            int sample_internal_value(void)
            {
                return 0;
            }

            int main(void)
            {
                return sample_internal_value();
            }
            """
        self.assertEqual(findings(text), [(6, "error", "non-topdown-order")])

    def test_entry_function_first_is_accepted(self) -> None:
        text = """
            static int helper(void);

            int main(void)
            {
                return helper();
            }

            int sample_value(void)
            {
                return 0;
            }

            static int helper(void)
            {
                return sample_value();
            }
            """
        self.assertEqual(findings(text), [])

    def test_wmain_is_entry_function(self) -> None:
        text = """
            int sample_value(void)
            {
                return 0;
            }

            int wmain(void)
            {
                return 0;
            }
            """
        self.assertEqual(findings(text), [(6, "error", "non-topdown-order")])

    def test_call_direction_is_not_checked(self) -> None:
        text = """
            static int is_odd(int value);
            static int is_even(int value);

            int sample_parity(int value)
            {
                return is_even(value);
            }

            static int is_odd(int value)
            {
                return value == 0 ? 0 : is_even(value - 1);
            }

            static int is_even(int value)
            {
                return value == 0 ? 1 : is_odd(value - 1);
            }
            """
        self.assertEqual(findings(text), [])


class ConditionalCompilationTest(unittest.TestCase):
    def test_duplicated_definitions_in_branches(self) -> None:
        text = """
            static int platform_value(void);

            int sample_value(void)
            {
                return platform_value();
            }

            #if defined(PLATFORM_LINUX)
            static int platform_value(void)
            {
                return 1;
            }
            #else
            static int platform_value(void)
            {
                return 2;
            }
            #endif
            """
        self.assertEqual(findings(text), [])

    def test_each_duplicated_main_definition_is_evaluated(self) -> None:
        text = """
            static int helper(void);

            static int helper(void)
            {
                return 0;
            }

            #if defined(PLATFORM_LINUX)
            int sample_value(void)
            {
                return helper();
            }
            #else
            int sample_value(void)
            {
                return helper();
            }
            #endif
            """
        self.assertEqual(findings(text), [
            (9, "error", "non-topdown-order"),
            (14, "error", "non-topdown-order"),
        ])

    def test_prototype_in_any_branch_counts(self) -> None:
        text = """
            #if defined(PLATFORM_WINDOWS)
            static int windows_only(void);
            #endif

            int sample_value(void)
            {
                return 0;
            }

            #if defined(PLATFORM_WINDOWS)
            static int windows_only(void)
            {
                return 0;
            }
            #endif
            """
        self.assertEqual(findings(text), [])

    def test_whole_file_condition(self) -> None:
        text = """
            #include <cplat/base/platform.h>

            #if defined(PLATFORM_LINUX)

                #include <unistd.h>

            static int helper(void);

            int sample_value(void)
            {
                return helper();
            }

            static int helper(void)
            {
                return 0;
            }

            #elif defined(PLATFORM_WINDOWS)
                #pragma warning(disable : 4206)
            #endif
            """
        self.assertEqual(findings(text), [])


class OrderMismatchTest(unittest.TestCase):
    def test_declaration_order_differs(self) -> None:
        text = """
            static void first(void);
            static void second(void);

            void sample_run(void)
            {
            }

            static void second(void)
            {
            }

            static void first(void)
            {
            }
            """
        self.assertEqual(findings(text), [(1, "warning", "order-mismatch")])

    def test_warning_does_not_fail(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = make_repository(Path(directory), {
                "app/sample/prod/src/sample.c": source(
                    """
                    static void b(void);
                    static void a(void);

                    static void a(void)
                    {
                    }

                    static void b(void)
                    {
                    }
                    """
                ),
            })
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                self.assertEqual(CHECKER.main([str(path / "app/sample/prod")]), 0)
            self.assertIn("sample.c:1: warning: [order-mismatch]", output.getvalue())
            self.assertIn("対象 1 ファイル、エラー 0 件、警告 1 件", output.getvalue())


class ParseErrorTest(unittest.TestCase):
    def test_unbalanced_braces(self) -> None:
        text = """
            int sample_run(void)
            {
                if (1)
                {
                    return 0;
            }
            """
        self.assertEqual(findings(text), [(1, "error", "parse-error")])


def make_repository(root: Path, files: dict[str, str]) -> Path:
    """一時ディレクトリに Git リポジトリを作り、ファイルを追跡させる。"""
    run_git(root, "init", "-q")
    for name, content in files.items():
        path = root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")
        run_git(root, "add", "-f", name)
    return root


def run_git(root: Path, *arguments: str) -> None:
    subprocess.run(["git", "-C", str(root), *arguments], check=True, capture_output=True)


class TargetSelectionTest(unittest.TestCase):
    BAD = source(
        """
        static void helper(void)
        {
        }
        """
    )

    def test_only_tracked_regular_files_under_app_prod(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = make_repository(Path(directory), {
                "app/sample/prod/src/tracked.c": self.BAD,
                "app/sample/prod/src/ignored.c": self.BAD,
                "app/sample/test/src/test_only.c": self.BAD,
                ".gitignore": "ignored.c\nuntracked.c\n",
            })
            prod = root / "app/sample/prod/src"
            (prod / "untracked.c").write_text(self.BAD, encoding="utf-8")
            (prod / "new.c").write_text(self.BAD, encoding="utf-8")
            selector = CHECKER.TargetSelector()
            targets, excluded = CHECKER.collect_targets([str(root / "app")], selector)
            self.assertEqual([path.name for path in targets], ["tracked.c"])
            self.assertEqual(excluded, [])
            self.assertEqual(selector.exclusion_reason(prod / "ignored.c"), "ignore 対象です")
            self.assertEqual(selector.exclusion_reason(prod / "new.c"), "Git で追跡されていません")
            self.assertEqual(
                selector.exclusion_reason(root / "app/sample/test/src/test_only.c"),
                "app/<name>/prod/ 配下ではありません",
            )

    def test_symbolic_link_is_excluded(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = make_repository(Path(directory), {"app/sample/prod/src/real.c": self.BAD})
            link = root / "app/sample/prod/lib/link.c"
            link.parent.mkdir(parents=True)
            try:
                os.symlink(root / "app/sample/prod/src/real.c", link)
            except OSError as error:
                self.skipTest(f"[Windows] シンボリック リンクを作成できません: {error}")
            run_git(root, "add", "-f", "app/sample/prod/lib/link.c")
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                self.assertEqual(CHECKER.main([str(link)]), 0)
            self.assertIn("link.c: 対象外のため検査しません (シンボリック リンクです)", output.getvalue())
            self.assertIn("対象 0 ファイル", output.getvalue())
            targets, _ = CHECKER.collect_targets([str(root / "app")], CHECKER.TargetSelector())
            self.assertEqual([path.name for path in targets], ["real.c"])

    def test_main_reports_errors_with_japanese_messages(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = make_repository(Path(directory), {
                "app/sample/prod/src/sample.c": "/* 日本語のコメント。 */\n" + self.BAD,
            })
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                self.assertEqual(CHECKER.main([str(root / "app/sample/prod/src/sample.c")]), 1)
            self.assertIn(
                "sample.c:2: error: [missing-prototype] static 関数 helper の前方プロトタイプ宣言が、"
                "最初の関数定義より前にありません",
                output.getvalue(),
            )
            self.assertIn("対象 1 ファイル、エラー 1 件、警告 0 件", output.getvalue())


if __name__ == "__main__":
    unittest.main()
