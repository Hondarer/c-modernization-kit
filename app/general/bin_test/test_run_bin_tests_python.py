"""発行処理が選択した Python をスクリプトのテストでも使う。"""

import importlib.util
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch


SCRIPT = Path(__file__).resolve().parents[1] / "bin_internal/run-bin-tests.py"
SPEC = importlib.util.spec_from_file_location("run_bin_tests", SCRIPT)
runner = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(runner)


class PythonSelectionTest(unittest.TestCase):
    def test_explicit_python_wins_over_existing_local_venv(self):
        with tempfile.TemporaryDirectory(prefix="test python space ") as temporary:
            root = Path(temporary)
            executable = root / ".venv/Scripts/python.exe"
            executable.parent.mkdir(parents=True)
            executable.touch()
            with patch.dict(os.environ, {"BIN_TEST_PYTHON": "/selected python/python"}):
                self.assertEqual(runner.python_for(root / "bin_test"), "/selected python/python")

    def test_without_override_existing_venv_remains_supported(self):
        with tempfile.TemporaryDirectory(prefix="test python space ") as temporary:
            root = Path(temporary)
            executable = root / ".venv/bin/python"
            executable.parent.mkdir(parents=True)
            executable.touch()
            with patch.dict(os.environ, {"BIN_TEST_PYTHON": ""}):
                self.assertEqual(runner.python_for(root / "bin_test"), str(executable))


class ChildEnvironmentTest(unittest.TestCase):
    def test_child_python_excludes_script_directory_like_embedded_python(self):
        # 埋め込み版 Python (._pth) と同じ条件で、同じディレクトリの import 漏れを検出する。
        environment = runner.child_environment()
        self.assertEqual(environment["PYTHONSAFEPATH"], "1")
        self.assertEqual(environment["PYTHONIOENCODING"], "utf-8")


class AssuredSkipTest(unittest.TestCase):
    """保証済みサブモジュールの bin_test/ を実行しないことを確認する。"""

    def make_workspace(self, temporary, answer):
        root = Path(temporary)
        resolver = runner.assured_resolver(root)
        resolver.parent.mkdir(parents=True)
        # resolve_app_deps.sh --assured の代わりに、決めた判定結果を返す。
        with open(resolver, "w", encoding="utf-8", newline="\n") as handle:
            handle.write('#!/bin/bash\nprintf "%s\\n" "$*" >> "$(dirname "$0")/calls.log"\necho {}\n'.format(answer))
        submodule = root / "framework/sample"
        (submodule / "bin_test").mkdir(parents=True)
        return root, submodule

    def test_without_stamp_runs_tests_without_resolver(self):
        with tempfile.TemporaryDirectory(prefix="assured space ") as temporary:
            root, submodule = self.make_workspace(temporary, 1)
            self.assertFalse(runner.is_assured(submodule, root))
            self.assertFalse((runner.assured_resolver(root).parent / "calls.log").exists())

    def test_assured_submodule_is_skipped(self):
        with tempfile.TemporaryDirectory(prefix="assured space ") as temporary:
            root, submodule = self.make_workspace(temporary, 1)
            (root / "framework/sample.assured.stamp").write_text("0123abcd\n", encoding="utf-8")
            self.assertTrue(runner.is_assured(submodule, root))
            calls = (runner.assured_resolver(root).parent / "calls.log").read_text(encoding="utf-8")
            self.assertIn("--assured", calls)
            outcomes = runner.run_directory(submodule / "bin_test", root, allow_skips=False)
            self.assertEqual(len(outcomes), 1)
            self.assertTrue(outcomes[0].passed)
            self.assertIn("assured skip", outcomes[0].notes[0])

    def test_unassured_stamp_runs_tests(self):
        with tempfile.TemporaryDirectory(prefix="assured space ") as temporary:
            root, submodule = self.make_workspace(temporary, 0)
            (root / "framework/sample.assured.stamp").write_text("0123abcd\n", encoding="utf-8")
            self.assertFalse(runner.is_assured(submodule, root))

    def test_workspace_itself_is_never_assured(self):
        with tempfile.TemporaryDirectory(prefix="assured space ") as temporary:
            root, _ = self.make_workspace(temporary, 1)
            (root.parent / (root.name + ".assured.stamp")).write_text("0123abcd\n", encoding="utf-8")
            try:
                self.assertFalse(runner.is_assured(root, root))
            finally:
                (root.parent / (root.name + ".assured.stamp")).unlink()


if __name__ == "__main__":
    unittest.main()
