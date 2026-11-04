#!/usr/bin/env python3
"""check-python-import-path.py の単体テスト。"""

from __future__ import annotations

import importlib.util
from pathlib import Path
import os
import subprocess
import sys
import tempfile
import textwrap
import unittest


SCRIPT_PATH = Path(__file__).resolve().parent.parent / "bin/check-python-import-path.py"
SPEC = importlib.util.spec_from_file_location("check_python_import_path", SCRIPT_PATH)
assert SPEC is not None
assert SPEC.loader is not None
CHECKER = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = CHECKER
SPEC.loader.exec_module(CHECKER)

INSERT = "sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))"


class CheckPythonImportPathTest(unittest.TestCase):
    def setUp(self) -> None:
        self._temporary = tempfile.TemporaryDirectory()
        self.root = Path(self._temporary.name)
        (self.root / "helper.py").write_text("VALUE = 1\n", encoding="utf-8")

    def tearDown(self) -> None:
        self._temporary.cleanup()

    def _check(self, source: str) -> list:
        path = self.root / "main.py"
        path.write_text(textwrap.dedent(source), encoding="utf-8")
        return CHECKER.check_file(path)

    def test_sibling_import_without_insert_is_reported(self) -> None:
        findings = self._check("import sys\nfrom helper import VALUE\n")
        self.assertEqual([(2, "helper")], [(f.line, f.module) for f in findings])

    def test_insert_before_import_is_accepted(self) -> None:
        self.assertEqual([], self._check(f"import os\nimport sys\n{INSERT}\nimport helper\n"))

    def test_insert_after_import_is_reported(self) -> None:
        findings = self._check(f"import os\nimport sys\nimport helper\n{INSERT}\n")
        self.assertEqual(["sys.path への追加が import より後にあります"], [f.reason for f in findings])

    def test_pathlib_and_variable_forms_are_accepted(self) -> None:
        self.assertEqual([], self._check(
            "import sys\nfrom pathlib import Path\n"
            "sys.path.insert(0, str(Path(__file__).resolve().parent))\nimport helper\n"
        ))
        self.assertEqual([], self._check(
            "import os\nimport sys\nHERE = os.path.dirname(os.path.abspath(__file__))\n"
            "sys.path.insert(0, HERE)\nimport helper\n"
        ))

    def test_other_directory_insert_does_not_count(self) -> None:
        findings = self._check(
            "import os\nimport sys\n"
            "sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'lib'))\nimport helper\n"
        )
        self.assertEqual(1, len(findings))

    def test_lazy_import_needs_module_level_insert(self) -> None:
        self.assertEqual(1, len(self._check("def run():\n    import helper\n")))
        self.assertEqual([], self._check(f"import os\nimport sys\n\ndef run():\n    import helper\n\n{INSERT}\n"))

    def test_non_sibling_import_is_ignored(self) -> None:
        self.assertEqual([], self._check("import json\nimport os.path\n"))

    def test_package_directory_counts_as_sibling(self) -> None:
        package = self.root / "pkg"
        package.mkdir()
        (package / "__init__.py").write_text("", encoding="utf-8")
        self.assertEqual(1, len(self._check("from pkg import thing\n")))

    def test_fixed_script_runs_without_script_directory_in_sys_path(self) -> None:
        # PYTHONSAFEPATH=1 は、埋め込み版 Python と同じくスクリプトのディレクトリを sys.path に入れない。
        path = self.root / "main.py"
        environment = dict(os.environ, PYTHONSAFEPATH="1")
        command = [sys.executable, str(path)]

        path.write_text("import helper\n", encoding="utf-8")
        broken = subprocess.run(command, cwd=str(self.root.parent), env=environment,
                                capture_output=True, text=True, encoding="utf-8", check=False)
        if sys.version_info < (3, 11):
            self.skipTest("PYTHONSAFEPATH は Python 3.11 以降で有効です。")
        self.assertIn("ModuleNotFoundError", broken.stderr)

        path.write_text(f"import os\nimport sys\n{INSERT}\nimport helper\n", encoding="utf-8")
        fixed = subprocess.run(command, cwd=str(self.root.parent), env=environment,
                               capture_output=True, text=True, encoding="utf-8", check=False)
        self.assertEqual(0, fixed.returncode, fixed.stderr)
        self.assertEqual([], CHECKER.check_file(path))


if __name__ == "__main__":
    unittest.main()
