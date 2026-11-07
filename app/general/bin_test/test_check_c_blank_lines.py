#!/usr/bin/env python3
"""check_c_blank_lines.py の単体テスト。"""

from __future__ import annotations

import contextlib
import importlib.util
import io
from pathlib import Path
import sys
import tempfile
import textwrap
import unittest


SCRIPT_PATH = Path(__file__).resolve().parent.parent / "bin/check_c_blank_lines.py"
SPEC = importlib.util.spec_from_file_location("check_c_blank_lines", SCRIPT_PATH)
assert SPEC is not None
assert SPEC.loader is not None
CHECKER = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = CHECKER
SPEC.loader.exec_module(CHECKER)


def source(text: str) -> str:
    return textwrap.dedent(text).lstrip("\n")


def kinds(text: str) -> list[tuple[int, str]]:
    """検出結果を、1 始まりの行番号と種別の組で返す。"""
    return [(index + 1, kind) for index, kind in CHECKER.find_gaps(source(text))]


class GapDetectionTest(unittest.TestCase):
    def test_consecutive_blocks(self) -> None:
        text = """
            int f(int rc)
            {
                if (rc == 0)
                {
                    return 1;
                }
                if (rc == 1)
                {
                    return 2;
                }

                return 0;
            }
            """
        self.assertEqual(kinds(text), [(7, "after-block")])

    def test_statement_after_block(self) -> None:
        text = """
            void f(int *p)
            {
                if (p == NULL)
                {
                    return;
                }
                *p = 1;
                g();
            }
            """
        self.assertEqual(kinds(text), [(7, "after-block")])

    def test_do_while_end_is_block_end(self) -> None:
        text = """
            void f(void)
            {
                do
                {
                    g();
                } while (h() != 0);
                g();
                g();
            }
            """
        self.assertEqual(kinds(text), [(7, "after-block")])

    def test_block_after_statement(self) -> None:
        text = """
            void f(int a)
            {
                g();
                if (a != 0)
                {
                    g();
                }
            }
            """
        self.assertEqual(kinds(text), [(4, "before-block")])

    def test_block_after_assignment_judged_in_condition_is_exempt(self) -> None:
        text = """
            int f(void)
            {
                int rc;

                rc = g();
                if (rc != 0)
                {
                    return rc;
                }

                ctx->state = h();
                while (ctx->state == 0)
                {
                    ctx->state = h();
                }

                for (int i = g(); i < 3; i++)
                {
                    g();
                }

                return 0;
            }
            """
        self.assertEqual(kinds(text), [])

    def test_block_after_assignment_not_in_condition(self) -> None:
        text = """
            int f(void)
            {
                int rc = g();
                if (h() != 0)
                {
                    return rc;
                }

                return 0;
            }
            """
        self.assertEqual(kinds(text), [(4, "before-block")])

    def test_do_is_not_exempt(self) -> None:
        text = """
            void f(void)
            {
                int deadline = g();
                do
                {
                    h();
                } while (now() < deadline);
            }
            """
        self.assertEqual(kinds(text), [(4, "before-block")])

    def test_preceding_comment(self) -> None:
        text = """
            void f(void)
            {
                /* ブロックの先頭は対象外。 */
                g();
                /* 前の処理と切り離す。 */
                g();
                /*
                 * 複数行の前置コメント。
                 */
                /* 別のコメントの直後は対象外。 */
                g(); /* 後置コメントは対象外 */
                switch (h())
                {
                case 0:
                    /* ラベルの直後は対象外。 */
                    break;
                default:
                    break;
                }
            }
            """
        self.assertEqual(kinds(text), [(5, "comment"), (7, "comment"), (12, "before-block")])

    def test_comment_after_block(self) -> None:
        text = """
            void f(int a)
            {
                if (a != 0)
                {
                    g();
                }
                /* ブロックの後の前置コメント。 */
                g();
            }
            """
        self.assertEqual(kinds(text), [(7, "comment")])

    def test_comment_before_else_is_reported_and_not_fixed(self) -> None:
        text = source(
            """
            void f(int a)
            {
                if (a == 0)
                {
                    g();
                }
                /* else 側の説明 */
                else
                {
                    g();
                }
            }
            """
        )
        self.assertEqual([(index + 1, kind) for index, kind in CHECKER.find_gaps(text)], [(7, "comment-before-else")])
        self.assertEqual(CHECKER.insert_gaps(text, CHECKER.find_gaps(text)), text)

    def test_comment_inside_parentheses_is_exempt(self) -> None:
        text = """
            void f(void)
            {
                g(1,
                  /* 引数の説明 */
                  2);
            }
            """
        self.assertEqual(kinds(text), [])

    def test_final_return(self) -> None:
        text = """
            int f(int a)
            {
                int b = a + 1;
                int c = b + 1;
                return c;
            }
            """
        self.assertEqual(kinds(text), [(5, "final-return")])

    def test_final_return_in_short_function_is_exempt(self) -> None:
        text = """
            int f(const struct s *c)
            {
                int state = c->state;
                return state;
            }

            int g(void)
            {
                return 0;
            }

            int h(void)
            {
                g();
                g();

                /* 前置コメントの直後は対象外。 */
                return 0;
            }
            """
        self.assertEqual(kinds(text), [])

    def test_final_return_counts_statements_per_preprocessor_branch(self) -> None:
        text = """
            void *f(void *p, int order)
            {
            #if defined(COMPILER_GCC)
                return g(p, order);
            #elif defined(COMPILER_MSVC)
                (void)order;
                return h(p);
            #endif
            }
            """
        self.assertEqual(kinds(text), [])

    def test_final_return_after_block_in_short_function(self) -> None:
        text = """
            int f(int rc)
            {
                if (rc != 0)
                {
                    return 1;
                }
                return 0;
            }
            """
        self.assertEqual(kinds(text), [(7, "after-block")])

    def test_final_return_after_label_is_exempt(self) -> None:
        text = """
            int f(void)
            {
                int ret = g();

                g();
            cleanup:
                return ret;
            }
            """
        self.assertEqual(kinds(text), [])

    def test_else_and_outer_brace_and_label_are_exempt(self) -> None:
        text = """
            void f(int a)
            {
                if (a == 0)
                {
                    g();
                }
                else if (a == 1)
                {
                    g();
                }
                else
                {
                    switch (a)
                    {
                    case 2:
                    {
                        g();
                    }
                    case 3:
                        break;
                    default:
                        break;
                    }
                }
            }
            """
        self.assertEqual(kinds(text), [])

    def test_preprocessor_and_strings_are_ignored(self) -> None:
        text = """
            #define M(x) \\
                do \\
                { \\
                    g(x); \\
                } while (0)

            void f(void)
            {
                const char *s = "{ /* not a comment */";
                char c = '}';
            #if defined(A)
                g();
            #endif
                g();
            }
            """
        self.assertEqual(kinds(text), [])

    def test_aggregate_and_function_end_are_not_blocks(self) -> None:
        text = """
            struct s
            {
                int a;
            };
            static const int table[] = {
                1,
                2,
            };
            int f(void)
            {
                return 0;
            }
            int g(void)
            {
                return 0;
            }
            """
        self.assertEqual(kinds(text), [])


class FixTest(unittest.TestCase):
    def test_insert_gaps(self) -> None:
        text = source(
            """
            int f(int rc)
            {
                g();
                if (rc == 0)
                {
                    return 1;
                }
                g();
                /* 説明。 */
                g();
                g();
                return 0;
            }
            """
        )
        fixed = CHECKER.insert_gaps(text, CHECKER.find_gaps(text))
        expected = source(
            """
            int f(int rc)
            {
                g();

                if (rc == 0)
                {
                    return 1;
                }

                g();

                /* 説明。 */
                g();
                g();

                return 0;
            }
            """
        )
        self.assertEqual(fixed, expected)
        self.assertEqual(CHECKER.find_gaps(fixed), [])


class MainTest(unittest.TestCase):
    def test_check_and_fix_with_japanese(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "sample.c"
            path.write_text(source(
                """
                void f(void)
                {
                    g();
                    /* 日本語の前置コメント。 */
                    g();
                }
                """
            ), encoding="utf-8")

            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                self.assertEqual(CHECKER.main([directory]), 1)
            self.assertIn("sample.c:4: comment: /* 日本語の前置コメント。 */", output.getvalue())

            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(CHECKER.main(["--fix", str(path)]), 0)
                self.assertEqual(CHECKER.main([str(path)]), 0)
            self.assertIn("    g();\n\n    /* 日本語", path.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
