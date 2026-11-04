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


if __name__ == "__main__":
    unittest.main()
