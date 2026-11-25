#!/usr/bin/env python3
"""reorder_c_topdown.py の単体テスト。"""

from __future__ import annotations

import contextlib
import importlib.util
import io
from pathlib import Path
import subprocess
import sys
import tempfile
import textwrap
import unittest


BIN_DIRECTORY = Path(__file__).resolve().parent.parent / "bin"


def load(name: str):
    spec = importlib.util.spec_from_file_location(name, BIN_DIRECTORY / f"{name}.py")
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


CHECKER = load("check_c_topdown")
REORDER = load("reorder_c_topdown")


def source(text: str) -> str:
    return textwrap.dedent(text).lstrip("\n")


class ReorderTestCase(unittest.TestCase):
    def reorder(self, text: str) -> str:
        """並べ替え、結果がチェッカーと等価性の検証に合格し、再実行で変わらないことを確かめる。"""
        before = source(text)
        after = REORDER.reorder_text(before)
        self.assertEqual(REORDER.verify_texts(before, after), [])
        self.assertEqual([finding.format() for finding in CHECKER.check_text(after, "sample.c")], [])
        self.assertEqual(REORDER.reorder_text(after), after)
        return after


class ReorderTest(ReorderTestCase):
    def test_bottom_up_command(self) -> None:
        after = self.reorder(
            """
            #include <stdio.h>

            #define SAMPLE_LIMIT 3

            /* 使い方を表示する。 */
            static void print_usage(void)
            {
                puts("usage");
            }

            static int run(int count)
            {
                return count < SAMPLE_LIMIT ? 0 : 1;
            }

            int sample_value(void)
            {
                return 1;
            }

            int main(void)
            {
                print_usage();
                return run(sample_value());
            }
            """
        )
        self.assertEqual(after, source(
            """
            #include <stdio.h>

            #define SAMPLE_LIMIT 3

            static void print_usage(void);
            static int run(int count);

            int main(void)
            {
                print_usage();
                return run(sample_value());
            }

            int sample_value(void)
            {
                return 1;
            }

            /* 使い方を表示する。 */
            static void print_usage(void)
            {
                puts("usage");
            }

            static int run(int count)
            {
                return count < SAMPLE_LIMIT ? 0 : 1;
            }
            """
        ))

    def test_prototypes_precede_variables_that_use_helpers(self) -> None:
        after = self.reorder(
            """
            #include "sample.h"

            static int handle(int value)
            {
                return value;
            }

            static const sample_handler handlers[] = {handle};

            void sample_run(void)
            {
                handlers[0](0);
            }
            """
        )
        self.assertEqual(after, source(
            """
            #include "sample.h"

            static int handle(int value);

            static const sample_handler handlers[] = {handle};

            void sample_run(void)
            {
                handlers[0](0);
            }

            static int handle(int value)
            {
                return value;
            }
            """
        ))

    def test_existing_prototypes_are_kept_in_definition_order(self) -> None:
        after = self.reorder(
            """
            static void second(void);
            static void first(void) __attribute__((unused));

            static void first(void)
            {
            }

            static void second(void)
            {
            }

            void sample_run(void)
            {
                first();
                second();
            }
            """
        )
        self.assertEqual(after, source(
            """
            static void first(void) __attribute__((unused));
            static void second(void);

            void sample_run(void)
            {
                first();
                second();
            }

            static void first(void)
            {
            }

            static void second(void)
            {
            }
            """
        ))

    def test_multiline_header_and_static_inline(self) -> None:
        after = self.reorder(
            """
            static inline int
            add(int left, /* 左辺 */
                int right)
            {
                return left + right;
            }

            int sample_add(int value)
            {
                return add(value, 1);
            }
            """
        )
        self.assertIn("static inline int\nadd(int left,\n    int right);\n", after)
        self.assertEqual(after.count("/* 左辺 */"), 1)

    def test_whole_file_condition(self) -> None:
        after = self.reorder(
            """
            #include <cplat/base/platform.h>

            #if defined(PLATFORM_LINUX)

                #include <unistd.h>

            static int helper(void)
            {
                return 0;
            }

            int sample_value(void)
            {
                return helper();
            }

            #elif defined(PLATFORM_WINDOWS)
                #pragma warning(disable : 4206)
            #endif
            """
        )
        self.assertEqual(after, source(
            """
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
        ))

    def test_partial_condition_is_moved_as_a_unit(self) -> None:
        after = self.reorder(
            """
            #include "sample.h"

            #if defined(PLATFORM_WINDOWS)
            static int platform_value(void)
            {
                return 2;
            }
            #else
            static int platform_value(void)
            {
                return 1;
            }
            #endif /* PLATFORM_ */

            int sample_value(void)
            {
                return platform_value();
            }
            """
        )
        self.assertEqual(after, source(
            """
            #include "sample.h"

            #if defined(PLATFORM_WINDOWS)
            static int platform_value(void);
            #else
            static int platform_value(void);
            #endif /* PLATFORM_ */

            int sample_value(void)
            {
                return platform_value();
            }

            #if defined(PLATFORM_WINDOWS)
            static int platform_value(void)
            {
                return 2;
            }
            #else
            static int platform_value(void)
            {
                return 1;
            }
            #endif /* PLATFORM_ */
            """
        ))

    def test_mixed_condition_is_split(self) -> None:
        after = self.reorder(
            """
            static void install(void)
            {
            }

            #if defined(_WIN32)
            int sample_attach(void)
            {
                install();
                return 1;
            }
            #else
            static void on_load(void)
            {
                install();
            }
            #endif

            int sample_value(void)
            {
                return 0;
            }
            """
        )
        self.assertEqual(after, source(
            """
            static void install(void);
            #if defined(_WIN32)
            #else
            static void on_load(void);
            #endif

            #if defined(_WIN32)
            int sample_attach(void)
            {
                install();
                return 1;
            }
            #endif

            int sample_value(void)
            {
                return 0;
            }

            static void install(void)
            {
            }

            #if defined(_WIN32)
            #else
            static void on_load(void)
            {
                install();
            }
            #endif
            """
        ))

    def test_compliant_file_is_unchanged(self) -> None:
        text = source(
            """
            static int helper(void);

            int sample_value(void)
            {
                return helper();
            }

            static int helper(void)
            {
                return 0;
            }
            """
        )
        self.assertEqual(REORDER.reorder_text(text), text)


class UnsupportedTest(unittest.TestCase):
    def test_pragma_between_functions(self) -> None:
        text = source(
            """
            static void helper(void)
            {
            }

            #pragma warning(push)
            int sample_value(void)
            {
                return 0;
            }
            #pragma warning(pop)
            """
        )
        with self.assertRaisesRegex(REORDER.Unsupported, "#pragma"):
            REORDER.reorder_text(text)

    def test_variable_before_type_uses_helper(self) -> None:
        text = source(
            """
            static void helper(void)
            {
            }

            static void (*callback)(void) = helper;

            #define SAMPLE_LATE 1

            void sample_run(void)
            {
                callback();
            }
            """
        )
        with self.assertRaisesRegex(REORDER.Unsupported, "helper"):
            REORDER.reorder_text(text)


class VerifyTest(unittest.TestCase):
    BEFORE = source(
        """
        #include "sample.h"

        /* 補助。 */
        static int helper(int value)
        {
            return value + 1;
        }

        #if defined(PLATFORM_LINUX)
        int sample_value(void)
        {
            return helper(1);
        }
        #endif
        """
    )

    def test_reordered_text_is_equivalent(self) -> None:
        self.assertEqual(REORDER.verify_texts(self.BEFORE, REORDER.reorder_text(self.BEFORE)), [])

    def test_changed_token_is_detected(self) -> None:
        after = REORDER.reorder_text(self.BEFORE).replace("value + 1", "value + 2")
        problems = REORDER.verify_texts(self.BEFORE, after)
        self.assertEqual(len(problems), 2)
        self.assertIn("改修前の 4 行目の要素が、同じ条件の下で改修後にありません", problems[0])

    def test_changed_comment_is_detected(self) -> None:
        after = REORDER.reorder_text(self.BEFORE).replace("/* 補助。 */", "/* 補助関数。 */")
        self.assertEqual(REORDER.verify_texts(self.BEFORE, after), [
            "改修後にないコメント: /* 補助。 */",
            "改修前にないコメント: /* 補助関数。 */",
        ])

    def test_element_moved_across_condition_is_detected(self) -> None:
        after = self.BEFORE.replace("#endif\n", "").replace("#if defined(PLATFORM_LINUX)\n", "")
        problems = REORDER.verify_texts(self.BEFORE, after)
        self.assertEqual(len(problems), 2)
        self.assertIn("同じ条件の下で", problems[0])

    def test_prototype_must_match_definition(self) -> None:
        after = self.BEFORE.replace("/* 補助。 */", "static int helper(long value);\n\n/* 補助。 */")
        problems = REORDER.verify_texts(self.BEFORE, after)
        self.assertEqual(len(problems), 1)
        self.assertIn("前方プロトタイプ宣言が、同じ条件の定義の宣言部と一致しません", problems[0])

    def test_prototype_outside_branch_is_accepted(self) -> None:
        before = source(
            """
            #if defined(PLATFORM_LINUX)
            static int helper(void)
            {
                return 1;
            }
            #else
            static int helper(void)
            {
                return 2;
            }
            #endif
            """
        )
        after = "static int helper(void);\n\n" + before
        self.assertEqual(REORDER.verify_texts(before, after), [])


class MainTest(unittest.TestCase):
    BOTTOM_UP = source(
        """
        /* 日本語のコメント。 */
        static void helper(void)
        {
        }

        void sample_run(void)
        {
            helper();
        }
        """
    )

    def make_repository(self, root: Path) -> Path:
        path = root / "app/sample/prod/src/sample.c"
        path.parent.mkdir(parents=True)
        path.write_text(self.BOTTOM_UP, encoding="utf-8")
        subprocess.run(["git", "-C", str(root), "init", "-q"], check=True, capture_output=True)
        subprocess.run(["git", "-C", str(root), "add", "-f", "app"], check=True, capture_output=True)
        return path

    def test_diff_in_place_and_verify(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = self.make_repository(Path(directory))
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                self.assertEqual(REORDER.main([str(path.parent)]), 0)
            self.assertIn("+static void helper(void);", output.getvalue())
            self.assertIn("対象 1 ファイル、変更 1 ファイル、並べ替えられない 0 ファイル", output.getvalue())
            self.assertEqual(path.read_text(encoding="utf-8"), self.BOTTOM_UP)

            before = Path(directory) / "before.c"
            before.write_text(self.BOTTOM_UP, encoding="utf-8")
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                self.assertEqual(REORDER.main(["--in-place", str(path)]), 0)
                self.assertEqual(REORDER.main(["--verify", str(before), str(path)]), 0)
                self.assertEqual(CHECKER.main([str(path)]), 0)
            self.assertIn("sample.c: 並べ替えました", output.getvalue())
            self.assertIn("等価性の検証に合格しました", output.getvalue())
            self.assertIn("/* 日本語のコメント。 */", path.read_text(encoding="utf-8"))

            changed = path.read_text(encoding="utf-8").replace("helper();", "helper(); helper();")
            path.write_text(changed, encoding="utf-8")
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                self.assertEqual(REORDER.main(["--verify", str(before), str(path)]), 1)
            self.assertIn("等価性の検証に失敗しました", output.getvalue())

    def test_unsupported_file_is_not_changed(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = self.make_repository(Path(directory))
            text = self.BOTTOM_UP + "#pragma once_more\n\nvoid sample_last(void)\n{\n}\n"
            path.write_text(text, encoding="utf-8")
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                self.assertEqual(REORDER.main(["--in-place", str(path)]), 1)
            self.assertIn("自動で並べ替えられません", output.getvalue())
            self.assertEqual(path.read_text(encoding="utf-8"), text)


if __name__ == "__main__":
    unittest.main()
