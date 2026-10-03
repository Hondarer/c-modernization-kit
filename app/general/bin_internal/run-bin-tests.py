#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""ワークスペース内の bin_test/ にあるスクリプトのテストを実行する。

各 bin_test/ ディレクトリで、次のテストを実行する。

- test_*.py: unittest で実行する
- *_selftest.py: Python で直接実行し、終了コードで合否を判定する
  BIN_TEST_PLATFORM = "linux" または "windows" を定義した場合は、その OS だけで実行する
- *_selftest.ps1: Windows だけで PowerShell により実行する
- test_*.js: Node.js で実行し、終了コードで合否を判定する

bin_test/ の親ディレクトリに .venv がある場合は、その Python を使う。
作業ディレクトリは、bin_test/ を含む Git リポジトリのルートとする。

ツールやファイルの不足によるスキップは、検証していないのに成功に見えるため失敗として扱う。
OS に依存するスキップは、理由を [Linux] または [Windows] で始め、別の OS で起きた場合だけ認める。
"""

import argparse
import json
import re
import os
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path


SKIP_DIRS = {".git", ".venv", "node_modules", "obj", "pages"}
PLATFORM_TAGS = {"[Linux]": "linux", "[Windows]": "windows"}

# unittest を子プロセスで実行し、スキップの理由を JSON で受け取る。
UNITTEST_HARNESS = r"""
import json, sys, unittest
directory, report = sys.argv[1], sys.argv[2]
suite = unittest.defaultTestLoader.discover(directory, pattern="test_*.py", top_level_dir=directory)
result = unittest.TextTestRunner(stream=sys.stdout, verbosity=1).run(suite)
with open(report, "w", encoding="utf-8") as handle:
    json.dump({
        "run": result.testsRun,
        "failures": len(result.failures) + len(result.errors) + len(result.unexpectedSuccesses),
        "skipped": [[test.id(), reason] for test, reason in result.skipped],
    }, handle, ensure_ascii=False)
"""


def configure_stdio():
    """Windows でも日本語を UTF-8 で出力する。"""
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")


def current_platform():
    return "windows" if os.name == "nt" else "linux"


def find_bin_test_dirs(root):
    found = []
    for directory, subdirs, _ in os.walk(root):
        subdirs[:] = sorted(name for name in subdirs if name not in SKIP_DIRS)
        if Path(directory).name == "bin_test":
            found.append(Path(directory))
    return found


def repository_root(directory):
    result = subprocess.run(
        ["git", "-C", str(directory), "rev-parse", "--show-toplevel"],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, encoding="utf-8", check=False,
    )
    return Path(result.stdout.strip()) if result.returncode == 0 else directory.parent


def python_for(directory):
    venv = directory.parent / ".venv"
    for candidate in (venv / "Scripts" / "python.exe", venv / "bin" / "python"):
        if candidate.is_file():
            return str(candidate)
    return sys.executable


def child_environment():
    # 子プロセスの標準出力だけを UTF-8 にする。open() の既定は変えず、
    # 文字コードを指定していない読み書きの不具合を隠さない。
    environment = os.environ.copy()
    environment["PYTHONIOENCODING"] = "utf-8"
    return environment


def is_expected_skip(reason):
    for tag, platform in PLATFORM_TAGS.items():
        if reason.startswith(tag):
            return platform != current_platform()
    return False


class Outcome:
    def __init__(self, label):
        self.label = label
        self.passed = True
        self.notes = []
        self.seconds = 0.0


def declared_platform(path):
    """*_selftest.py が BIN_TEST_PLATFORM で宣言した実行対象の OS を返す。"""
    match = re.search(r"^BIN_TEST_PLATFORM\s*=\s*[\"'](linux|windows)[\"']",
                      path.read_text(encoding="utf-8", errors="replace"), re.M)
    return match.group(1) if match else None


def run_command(command, cwd, label):
    outcome = Outcome(label)
    started = time.monotonic()
    print("\n=== {}".format(label), flush=True)
    result = subprocess.run(command, cwd=str(cwd), env=child_environment(), check=False)
    outcome.seconds = time.monotonic() - started
    if result.returncode != 0:
        outcome.passed = False
        outcome.notes.append("exit code {}".format(result.returncode))
    return outcome


def run_unittest(directory, cwd, label, allow_skips):
    outcome = Outcome(label)
    started = time.monotonic()
    print("\n=== {}".format(label), flush=True)
    with tempfile.TemporaryDirectory(prefix="bin-test-") as temporary:
        report_path = Path(temporary) / "report.json"
        result = subprocess.run(
            [python_for(directory), "-c", UNITTEST_HARNESS, str(directory), str(report_path)],
            cwd=str(cwd), env=child_environment(), check=False,
        )
        outcome.seconds = time.monotonic() - started
        if not report_path.is_file():
            outcome.passed = False
            outcome.notes.append("unittest did not finish (exit code {})".format(result.returncode))
            return outcome
        report = json.loads(report_path.read_text(encoding="utf-8"))
    if report["failures"]:
        outcome.passed = False
        outcome.notes.append("{} failed".format(report["failures"]))
    for test_id, reason in report["skipped"]:
        if is_expected_skip(reason):
            continue
        message = "unexpected skip: {} ({})".format(test_id, reason)
        outcome.notes.append(message)
        if not allow_skips:
            outcome.passed = False
    outcome.notes.insert(0, "{} tests".format(report["run"]))
    return outcome


def run_directory(directory, root, allow_skips):
    cwd = repository_root(directory)
    relative = directory.relative_to(root).as_posix()
    outcomes = []
    files = sorted(path for path in directory.iterdir() if path.is_file())
    if any(path.name.startswith("test_") and path.suffix == ".py" for path in files):
        outcomes.append(run_unittest(directory, cwd, relative + "/test_*.py", allow_skips))
    for path in files:
        label = relative + "/" + path.name
        if path.name.endswith("_selftest.py"):
            platform = declared_platform(path)
            if platform and platform != current_platform():
                # 宣言された OS 以外での不実行は、OS に依存する想定どおりのスキップとして扱う。
                outcome = Outcome(label)
                outcome.notes.append("expected skip: [{}] only".format(platform.capitalize()))
                outcomes.append(outcome)
                continue
            outcomes.append(run_command([python_for(directory), str(path)], cwd, label))
        elif path.name.endswith("_selftest.ps1"):
            # MSVC 向けの補助スクリプトを検証するため、Windows だけで実行する。
            if current_platform() == "windows":
                powershell = shutil.which("powershell") or "powershell"
                outcomes.append(run_command(
                    [powershell, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(path)], cwd, label))
        elif path.name.startswith("test_") and path.suffix == ".js":
            node = shutil.which("node")
            if node is None:
                outcome = Outcome(label)
                outcome.passed = allow_skips
                outcome.notes.append("unexpected skip: node is not found")
                outcomes.append(outcome)
            else:
                outcomes.append(run_command([node, str(path)], cwd, label))
    return outcomes


def main():
    configure_stdio()
    default_root = Path(__file__).resolve().parents[3]
    parser = argparse.ArgumentParser(description="bin_test/ のスクリプトのテストを実行します。")
    parser.add_argument("--workspace", type=Path, default=default_root, help="ワークスペースのルート")
    parser.add_argument("--allow-skips", action="store_true",
                        help="ツールの不足などによる想定外のスキップを失敗として扱わない")
    parser.add_argument("filters", nargs="*",
                        help="ワークスペースからの相対パスにこの文字列を含む bin_test/ だけを実行する")
    args = parser.parse_args()

    root = args.workspace.resolve()
    directories = [
        directory for directory in find_bin_test_dirs(root)
        if not args.filters or any(text in directory.relative_to(root).as_posix() for text in args.filters)
    ]
    if not directories:
        print("ERROR: bin_test/ が見つかりません。", file=sys.stderr)
        return 1

    outcomes = []
    for directory in directories:
        outcomes.extend(run_directory(directory, root, args.allow_skips))

    print("\n=== bin_test summary ({})".format(current_platform()))
    for outcome in outcomes:
        status = "PASSED" if outcome.passed else "FAILED"
        print("{:6} {:7.1f}s  {}".format(status, outcome.seconds, outcome.label))
        for note in outcome.notes:
            print("               {}".format(note))
    failed = [outcome for outcome in outcomes if not outcome.passed]
    print("\n{} of {} test groups failed.".format(len(failed), len(outcomes)) if failed
          else "\nAll {} test groups passed.".format(len(outcomes)))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
