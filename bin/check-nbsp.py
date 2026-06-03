#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Detect NBSP (U+00A0) in text files."""

import argparse
import os
import subprocess
import sys
from pathlib import Path


NBSP_BYTES = b"\xc2\xa0"
NUL_BYTE = b"\x00"
SKIP_DIRS = {
    ".git",
    "bin",
    "coverage",
    "lib",
    "node_modules",
    "obj",
    "pages",
    "results",
    "xml",
    "xml_org",
}


def configure_stdio():
    """Use UTF-8 for Japanese messages on Windows."""
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    if hasattr(sys.stderr, "reconfigure"):
        sys.stderr.reconfigure(encoding="utf-8")


def run_git(args, cwd):
    """Run git and return stdout bytes, or None if the command fails."""
    try:
        result = subprocess.run(
            ["git", *args],
            cwd=cwd,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
    except (FileNotFoundError, subprocess.CalledProcessError):
        return None
    return result.stdout


def git_root(cwd):
    """Return the git work tree root for cwd, or None outside a work tree."""
    output = run_git(["rev-parse", "--show-toplevel"], cwd)
    if output is None:
        return None
    return Path(output.decode("utf-8", errors="replace").strip())


def git_submodule_paths(repo_root):
    """Return recursive submodule roots under repo_root."""
    output = run_git(
        ["submodule", "foreach", "--quiet", "--recursive", "printf '%s\\0' \"$sm_path\""],
        repo_root,
    )
    if output is None or not output:
        return []

    paths = []
    for raw_path in output.split(b"\0"):
        if not raw_path:
            continue
        rel_path = raw_path.decode("utf-8", errors="replace")
        paths.append(repo_root / rel_path)
    return paths


def parse_status_paths(output):
    """Parse `git status --porcelain -z` output and return tracked changed paths."""
    paths = []
    records = output.split(b"\0")
    index = 0

    while index < len(records):
        record = records[index]
        index += 1
        if not record:
            continue

        if len(record) < 4:
            continue

        status = record[:2].decode("ascii", errors="replace")
        path_bytes = record[3:]

        if status == "??" or status == "!!":
            continue

        if status[0] == "D" and status[1] in (" ", "D"):
            continue
        if status[1] == "D" and status[0] in (" ", "D"):
            continue

        paths.append(path_bytes.decode("utf-8", errors="replace"))

        if status[0] in ("R", "C") or status[1] in ("R", "C"):
            index += 1

    return paths


def normal_git_paths(repo_root):
    """Return tracked files changed in repo_root."""
    output = run_git(["status", "--porcelain", "-z", "--untracked-files=no"], repo_root)
    if output is None:
        return []
    return parse_status_paths(output)


def force_git_paths(repo_root):
    """Return all tracked files in repo_root."""
    output = run_git(["ls-files", "-z"], repo_root)
    if output is None:
        return []
    return [path.decode("utf-8", errors="replace") for path in output.split(b"\0") if path]


def force_filesystem_paths(root):
    """Yield files under root outside git."""
    for current_root, dirnames, filenames in os.walk(root):
        dirnames[:] = [dirname for dirname in dirnames if dirname not in SKIP_DIRS]
        current_path = Path(current_root)
        for filename in filenames:
            path = current_path / filename
            try:
                yield path.relative_to(root).as_posix()
            except ValueError:
                yield path.as_posix()


def iter_repositories(root):
    """Yield root and recursive submodule repositories."""
    yield root
    for submodule_path in git_submodule_paths(root):
        if submodule_path.is_dir():
            yield submodule_path


def is_binary(data):
    """Return True when data looks binary."""
    return NUL_BYTE in data


def line_column(data, byte_index):
    """Return 1-based line and column for byte_index."""
    line = data.count(b"\n", 0, byte_index) + 1
    line_start = data.rfind(b"\n", 0, byte_index)
    if line_start < 0:
        line_start = -1
    column = len(data[line_start + 1:byte_index].decode("utf-8", errors="replace")) + 1
    return line, column


def check_file(display_path, path):
    """Check one file and return the number of NBSP occurrences."""
    try:
        data = path.read_bytes()
    except OSError as error:
        print(f"{display_path}: read failed: {error}", file=sys.stderr)
        return 1

    if is_binary(data):
        return 0

    count = 0
    start = 0
    while True:
        index = data.find(NBSP_BYTES, start)
        if index < 0:
            break
        line, column = line_column(data, index)
        print(f"{display_path}:{line}:{column}: NBSP (U+00A0) found")
        count += 1
        start = index + len(NBSP_BYTES)

    return count


def check_git(root, force):
    """Check files in root git repository and its submodules."""
    total = 0
    for repo_root in iter_repositories(root):
        if force:
            rel_paths = force_git_paths(repo_root)
        else:
            rel_paths = normal_git_paths(repo_root)

        for rel_path in rel_paths:
            path = repo_root / rel_path
            if not path.is_file():
                continue
            try:
                display_path = path.relative_to(root).as_posix()
            except ValueError:
                display_path = path.as_posix()
            total += check_file(display_path, path)
    return total


def check_filesystem(root):
    """Check files under root outside git."""
    total = 0
    for rel_path in force_filesystem_paths(root):
        path = root / rel_path
        if path.is_file():
            total += check_file(rel_path, path)
    return total


def main():
    configure_stdio()

    parser = argparse.ArgumentParser(description="Detect NBSP (U+00A0) in text files.")
    parser.add_argument(
        "--force",
        action="store_true",
        help="Check all tracked files in git, or all files under cwd outside git.",
    )
    args = parser.parse_args()

    cwd = Path.cwd()
    root = git_root(cwd)
    if root is None:
        if not args.force:
            print("INFO: Git 管理下ではないため、NBSP チェックをスキップします。")
            return 0
        root = cwd
        count = check_filesystem(root)
    else:
        count = check_git(root, args.force)

    if count > 0:
        print(f"ERROR: NBSP (U+00A0) が {count} 件見つかりました。", file=sys.stderr)
        return 1

    print("INFO: NBSP チェックは成功しました。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
