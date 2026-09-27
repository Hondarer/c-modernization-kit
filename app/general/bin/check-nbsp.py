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
        ["submodule", "foreach", "--quiet", "--recursive", "printf '%s\\0' \"$PWD\""],
        repo_root,
    )
    if output is None or not output:
        return []

    paths = []
    for raw_path in output.split(b"\0"):
        if not raw_path:
            continue
        path = raw_path.decode("utf-8", errors="replace")
        paths.append(Path(path))
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


def git_text_attrs(repo_root, rel_paths):
    """Return git text attributes for rel_paths."""
    if not rel_paths:
        return {}

    input_data = b"\0".join(path.encode("utf-8") for path in rel_paths) + b"\0"
    try:
        result = subprocess.run(
            ["git", "check-attr", "-z", "--stdin", "text"],
            cwd=repo_root,
            check=True,
            input=input_data,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
    except (FileNotFoundError, subprocess.CalledProcessError):
        return {}

    attrs = {}
    parts = result.stdout.split(b"\0")
    for index in range(0, len(parts) - 2, 3):
        raw_path = parts[index]
        raw_value = parts[index + 2]
        if not raw_path:
            continue
        path = raw_path.decode("utf-8", errors="replace")
        value = raw_value.decode("utf-8", errors="replace")
        attrs[path] = value
    return attrs


def is_git_attr_binary(text_attr):
    """Return True when git attributes explicitly mark the path binary."""
    return text_attr == "unset"


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
    """Check one file and return the number of NBSP occurrences and binary state."""
    try:
        data = path.read_bytes()
    except OSError as error:
        print(f"{display_path}: read failed: {error}", file=sys.stderr)
        return 1, False

    if is_binary(data):
        return 0, True

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

    return count, False


def check_git(root, force):
    """Check files in root git repository and its submodules."""
    total = 0
    checked_count = 0
    skipped_binaries = []
    for repo_root in iter_repositories(root):
        if force:
            rel_paths = force_git_paths(repo_root)
        else:
            rel_paths = normal_git_paths(repo_root)
        text_attrs = git_text_attrs(repo_root, rel_paths)

        for rel_path in rel_paths:
            path = repo_root / rel_path
            if not path.is_file():
                continue
            try:
                display_path = path.relative_to(root).as_posix()
            except ValueError:
                display_path = path.as_posix()
            if is_git_attr_binary(text_attrs.get(rel_path)):
                skipped_binaries.append((display_path, "git attributes"))
                continue
            count, is_binary_file = check_file(display_path, path)
            total += count
            if is_binary_file:
                skipped_binaries.append((display_path, "NUL byte"))
            else:
                checked_count += 1
    return total, checked_count, skipped_binaries


def check_filesystem(root):
    """Check files under root outside git."""
    total = 0
    checked_count = 0
    skipped_binaries = []
    for rel_path in force_filesystem_paths(root):
        path = root / rel_path
        if path.is_file():
            count, is_binary_file = check_file(rel_path, path)
            total += count
            if is_binary_file:
                skipped_binaries.append((rel_path, "NUL byte"))
            else:
                checked_count += 1
    return total, checked_count, skipped_binaries


def print_force_summary(checked_count, skipped_binaries):
    """Print force mode scan summary."""
    git_attr_count = sum(1 for _, reason in skipped_binaries if reason == "git attributes")
    nul_byte_count = sum(1 for _, reason in skipped_binaries if reason == "NUL byte")
    print(f"INFO: Checked text files: {checked_count}")
    print(f"INFO: Skipped binary files: {len(skipped_binaries)}")
    print(f"INFO: Skipped by git attributes: {git_attr_count}")
    print(f"INFO: Skipped by NUL byte: {nul_byte_count}")
    for path, reason in skipped_binaries:
        print(f"INFO: Skipped binary file: {path} ({reason})")


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
        count, checked_count, skipped_binaries = check_filesystem(root)
    else:
        count, checked_count, skipped_binaries = check_git(root, args.force)

    if args.force:
        print_force_summary(checked_count, skipped_binaries)

    if count > 0:
        print(f"ERROR: NBSP (U+00A0) が {count} 件見つかりました。", file=sys.stderr)
        return 1

    print("INFO: NBSP が混入したテキスト ファイルは存在しませんでした。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
