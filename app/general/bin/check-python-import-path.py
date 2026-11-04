#!/usr/bin/env python3
"""同じディレクトリのモジュールを import する Python に、sys.path の追加があることを検査する。

埋め込み版 Python (Windows embeddable package) は ._pth ファイルで sys.path を固定し、
スクリプトのあるディレクトリを sys.path に入れない。そのため、同じディレクトリの
モジュールを import するスクリプトは、通常の Python では動くのに埋め込み版では
ModuleNotFoundError になる。

この検査は、Git 管理下の .py (サブモジュールを含む) のうち、同じディレクトリの
モジュールを import するものに、次のどちらかの行が import より前にあることを確かめる。

    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    sys.path.insert(0, str(Path(__file__).resolve().parent))

規範は app/general/docs/python-import-path-guideline.md を参照。

使い方:
  check-python-import-path.py              ワークスペース全体を検査する
  check-python-import-path.py PATH ...     指定したファイルまたはディレクトリだけを検査する
"""

from __future__ import annotations

import argparse
import ast
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

#: ワークスペース ルートを示す目印。
WORKSPACE_MARKER = ".workspaceRoot"

#: 自分のディレクトリを表す式として認める書き方 (ast.unparse の結果)。
SELF_DIR_EXPRESSIONS = {
    "os.path.dirname(os.path.abspath(__file__))",
    "os.path.dirname(os.path.realpath(__file__))",
    "str(Path(__file__).resolve().parent)",
    "str(Path(__file__).absolute().parent)",
    "str(pathlib.Path(__file__).resolve().parent)",
}

GUIDELINE = "app/general/docs/python-import-path-guideline.md"


@dataclass
class Finding:
    path: Path
    line: int
    module: str
    reason: str


def find_workspace_root(start: Path) -> Path | None:
    """目印をたどってワークスペース ルートを返す。"""
    for directory in [start.resolve()] + list(start.resolve().parents):
        if (directory / WORKSPACE_MARKER).is_file():
            return directory
    return None


def tracked_python_files(root: Path) -> list[Path]:
    """root 配下で Git 管理下の .py を、サブモジュールを含めて返す。"""
    result = subprocess.run(
        ["git", "-C", str(root), "ls-files", "-z", "--recurse-submodules", "--", "*.py"],
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True,
    )
    names = [name for name in result.stdout.decode("utf-8").split("\0") if name]
    return sorted(root / name for name in names if (root / name).is_file())


def is_sibling_module(directory: Path, name: str) -> bool:
    """name が directory 直下のモジュール (.py またはパッケージ) かどうか。"""
    return (directory / f"{name}.py").is_file() or (directory / name / "__init__.py").is_file()


def module_level_assignments(tree: ast.Module) -> dict[str, str]:
    """モジュール直下の単純な代入 (NAME = 式) を、名前から式の文字列へ対応付ける。"""
    values: dict[str, str] = {}
    for node in tree.body:
        if isinstance(node, ast.Assign) and len(node.targets) == 1 and isinstance(node.targets[0], ast.Name):
            values[node.targets[0].id] = ast.unparse(node.value)
    return values


def is_self_dir(expression: ast.expr, assignments: dict[str, str]) -> bool:
    """式が自分のディレクトリを表すかどうか。NAME の場合は代入元の式で判定する。"""
    text = ast.unparse(expression)
    if isinstance(expression, ast.Name):
        text = assignments.get(expression.id, text)
    return text in SELF_DIR_EXPRESSIONS


def self_dir_insert_lines(tree: ast.Module) -> list[int]:
    """モジュール直下で自分のディレクトリを sys.path に追加している行番号を返す。"""
    assignments = module_level_assignments(tree)
    lines = []
    for node in tree.body:
        if not (isinstance(node, ast.Expr) and isinstance(node.value, ast.Call)):
            continue
        call = node.value
        if ast.unparse(call.func) not in ("sys.path.insert", "sys.path.append") or not call.args:
            continue
        if is_self_dir(call.args[-1], assignments):
            lines.append(node.lineno)
    return lines


def sibling_imports(tree: ast.Module, directory: Path) -> list[tuple[int, str, bool]]:
    """同じディレクトリのモジュールを import する箇所を (行, モジュール名, モジュール直下か) で返す。"""
    top_level = {id(node) for node in tree.body}
    found = []
    for node in ast.walk(tree):
        if isinstance(node, ast.Import):
            names = [alias.name for alias in node.names]
        elif isinstance(node, ast.ImportFrom) and node.level == 0 and node.module:
            names = [node.module]
        else:
            continue
        for name in names:
            top = name.split(".")[0]
            if is_sibling_module(directory, top):
                found.append((node.lineno, top, id(node) in top_level))
    return found


def check_file(path: Path) -> list[Finding]:
    """1 ファイルを検査し、規範に合わない import を返す。"""
    try:
        tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
    except (SyntaxError, UnicodeDecodeError) as error:
        return [Finding(path, getattr(error, "lineno", 0) or 0, "", f"解析できません: {error}")]

    imports = sibling_imports(tree, path.parent)
    if not imports:
        return []
    inserts = self_dir_insert_lines(tree)
    findings = []
    for line, module, at_top_level in imports:
        if not inserts:
            findings.append(Finding(path, line, module, "自分のディレクトリを sys.path に追加していません"))
        elif at_top_level and min(inserts) > line:
            findings.append(Finding(path, line, module, "sys.path への追加が import より後にあります"))
    return findings


def collect_targets(arguments: list[str], root: Path) -> list[Path]:
    """引数で指定したファイルまたはディレクトリから、検査対象の .py を集める。"""
    if not arguments:
        return tracked_python_files(root)
    targets: list[Path] = []
    for argument in arguments:
        path = Path(argument)
        if path.is_dir():
            targets.extend(tracked_python_files(path))
        elif path.suffix == ".py":
            targets.append(path)
    return targets


def main(argv: list[str] | None = None) -> int:
    """コマンドの入口。"""
    parser = argparse.ArgumentParser(
        description="同じディレクトリのモジュールを import する Python に、sys.path の追加があることを検査します。"
    )
    parser.add_argument("paths", nargs="*", help="検査するファイルまたはディレクトリ (省略時はワークスペース全体)")
    args = parser.parse_args(argv)

    root = find_workspace_root(Path(__file__).parent) or Path.cwd()
    findings: list[Finding] = []
    targets = collect_targets(args.paths, root)
    for target in targets:
        findings.extend(sorted(check_file(target), key=lambda finding: finding.line))

    for finding in findings:
        try:
            shown = finding.path.resolve().relative_to(root.resolve())
        except ValueError:
            shown = finding.path
        suffix = f" ({finding.module})" if finding.module else ""
        print(f"{shown}:{finding.line}: {finding.reason}{suffix}", file=sys.stderr)

    if findings:
        print(
            f"エラー: {len(findings)} 件の import が埋め込み版 Python で失敗します。"
            f"import より前に次の行を追加してください。規範: {GUIDELINE}\n"
            "    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))",
            file=sys.stderr,
        )
        return 1
    print(f"OK: {len(targets)} ファイルを検査しました。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
