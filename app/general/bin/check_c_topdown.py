#!/usr/bin/env python3
"""C のソースが、関数をトップ ダウンの順に記載しているかを検査する。

判定規則の正本は app/general/docs/coding-guideline.md の「関数の記載順と前方宣言」節である。

検出する種別:
    missing-prototype  (error)   static 関数の定義に対応する static のプロトタイプ宣言が、
                                 ファイル内の最初の関数定義より前にない
    non-topdown-order  (error)   static でない関数の定義が、最初の static 関数の定義より後にある。
                                 または、入口関数 (main、wmain) がほかの関数定義より後にある
    order-mismatch     (warning) static 関数の前方プロトタイプ宣言の順序と、定義の順序が一致しない
    parse-error        (error)   波括弧や丸括弧の対応がとれず、ファイル スコープを解析できない

コメント、文字列リテラル、文字リテラルを除いたファイル スコープ (波括弧の深さ 0) の関数定義と関数宣言を解析する。
前処理指令の条件は評価せず、すべての分岐を 1 つの並びとして扱う。

対象は app/<name>/prod/ 配下の .c のうち、ファイルを含む Git リポジトリで追跡され、
ignore 対象でもシンボリック リンクでもないファイルである。引数を省略した場合は、ワークスペースの
app/*/prod/ 全体を走査する。エラーが 1 件以上あれば終了コード 1 を返す。

字句と構文の解析処理は、同じディレクトリの reorder_c_topdown.py と共有する。
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass, field
import os
from pathlib import Path
import re
import subprocess
import sys


WORKSPACE_ROOT = Path(__file__).resolve().parent.parent.parent.parent
ENTRY_NAMES = ("main", "wmain")
ATTRIBUTE_KEYWORDS = ("__attribute__", "__declspec")
NON_FUNCTION_NAMES = {
    "if", "for", "while", "switch", "return", "sizeof", "_Alignof", "alignof", "_Static_assert", "static_assert",
    "__attribute__", "__declspec", "_Pragma", "__pragma", "defined",
}
TOKEN_RE = re.compile(
    r'[A-Za-z_]\w*|\.?\d(?:[eEpP][+-]|[\w.])*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|->|\+\+|--|<<=|>>=|<<|>>|'
    r'<=|>=|==|!=|&&|\|\||[-+*/%&|^]=|##|\.\.\.|\S'
)


@dataclass
class Source:
    """コメント、文字列、文字リテラルを処理したソースの表現。"""

    text: str
    code: str  # コメントを空白に、文字列と文字リテラルの中身を空白に置き換えた同じ長さの文字列
    comments: list[tuple[int, int]] = field(default_factory=list)  # コメントの [開始, 終了) の位置
    line_starts: list[int] = field(default_factory=list)

    def line_of(self, offset: int) -> int:
        """位置を 1 始まりの行番号へ変換する。"""
        low, high = 0, len(self.line_starts) - 1
        while low < high:
            middle = (low + high + 1) // 2
            if self.line_starts[middle] <= offset:
                low = middle
            else:
                high = middle - 1
        return low + 1


@dataclass
class Item:
    """ファイル スコープの要素 (前処理指令、関数定義、宣言)。"""

    kind: str  # directive / function / declaration
    start: int
    end: int
    line: int
    name: str | None = None  # 関数定義と関数宣言の名前
    is_static: bool = False
    is_function_declaration: bool = False
    header_end: int = -1  # 関数定義の '{' の位置
    directive: str = ""  # 前処理指令の種類 (if、ifdef、else、endif、define、include など)
    has_inner_directive: bool = False  # 要素の途中に、ファイル スコープの前処理指令が挟まる


@dataclass
class Parsed:
    source: Source
    items: list[Item]
    error: str | None = None  # 解析できない場合の理由


@dataclass
class Finding:
    path: str
    line: int
    level: str  # error / warning
    rule: str
    message: str

    def format(self) -> str:
        return f"{self.path}:{self.line}: {self.level}: [{self.rule}] {self.message}"


def preprocess(text: str) -> Source:
    """コメントと文字列の中身を空白へ置き換えた、位置を保つ解析用の文字列を作る。"""
    code = list(text)
    comments: list[tuple[int, int]] = []
    length = len(text)
    i = 0
    while i < length:
        ch = text[i]
        nxt = text[i + 1] if i + 1 < length else ""
        if ch == "/" and nxt == "*":
            end = text.find("*/", i + 2)
            end = length if end < 0 else end + 2
            comments.append((i, end))
            for pos in range(i, end):
                if code[pos] != "\n":
                    code[pos] = " "
            i = end
            continue
        if ch == "/" and nxt == "/":
            end = i
            while end < length and text[end] != "\n":
                # 行末の '\' による継続行もコメントに含む
                if text[end] == "\\" and end + 1 < length and text[end + 1] == "\n":
                    end += 2
                    continue
                end += 1
            comments.append((i, end))
            for pos in range(i, end):
                if code[pos] != "\n":
                    code[pos] = " "
            i = end
            continue
        if ch in ("'", '"'):
            end = i + 1
            while end < length and text[end] != ch and text[end] != "\n":
                end += 2 if text[end] == "\\" else 1
            for pos in range(i + 1, min(end, length)):
                if code[pos] != "\n":
                    code[pos] = " "
            i = end + 1
            continue
        i += 1
    line_starts = [0] + [m.end() for m in re.finditer("\n", text)]
    return Source(text=text, code="".join(code), comments=comments, line_starts=line_starts)


def tokenize(code: str) -> list[str]:
    return TOKEN_RE.findall(code)


def matching_open(tokens: list[str], close_index: int) -> int:
    """閉じ丸括弧に対応する開き丸括弧の位置を返す。見つからなければ -1。"""
    depth = 0
    for pos in range(close_index, -1, -1):
        if tokens[pos] == ")":
            depth += 1
        elif tokens[pos] == "(":
            depth -= 1
            if depth == 0:
                return pos
    return -1


def analyze_declarator(code: str) -> tuple[str | None, bool, bool]:
    """宣言または定義の宣言部から、関数名、static の有無、関数の宣言部であるかを返す。

    戻り値の関数名は、宣言部が関数の宣言子でない場合と、関数を返す関数のような
    複雑な宣言子の場合に None とする。
    """
    tokens = tokenize(code)
    if not tokens or tokens[0] == "typedef":
        return None, False, False
    depth = 0
    for token in tokens:
        if token in ("(", "["):
            depth += 1
        elif token in (")", "]"):
            depth -= 1
        elif token == "=" and depth == 0:
            return None, False, False
    # 宣言子の後ろの属性 (__attribute__((...))、__declspec(...)) を除く
    while tokens and tokens[-1] == ")":
        open_index = matching_open(tokens, len(tokens) - 1)
        if open_index <= 0 or tokens[open_index - 1] not in ATTRIBUTE_KEYWORDS:
            break
        tokens = tokens[: open_index - 1]
    if not tokens or tokens[-1] != ")":
        return None, False, False
    open_index = matching_open(tokens, len(tokens) - 1)
    if open_index <= 0:
        return None, False, False
    name = tokens[open_index - 1]
    if not re.match(r"^[A-Za-z_]\w*$", name) or name in NON_FUNCTION_NAMES:
        return None, False, False
    # 属性の引数の中の名前を、関数名と取り違えない
    prefix = tokens[: open_index - 1]
    nesting = 0
    for token in prefix:
        if token == "(":
            nesting += 1
        elif token == ")":
            nesting -= 1
    if nesting != 0:
        return None, False, False
    return name, "static" in prefix, True


def scan(text: str) -> Parsed:
    """ファイル スコープの要素を、出現順に列挙する。"""
    source = preprocess(text)
    code = source.code
    items: list[Item] = []
    length = len(code)
    brace = 0
    paren = 0
    pending_start = -1
    pending_has_directive = False
    function_start = -1
    function_header_end = -1
    in_function = False
    i = 0
    line_start = True
    while i < length:
        ch = code[i]
        if line_start:
            j = i
            while j < length and code[j] in " \t":
                j += 1
            if j < length and code[j] == "#":
                end = j
                while end < length:
                    if code[end] == "\n":
                        # 継続行 ('\' で終わる行) は前処理指令に含む
                        back = end - 1
                        while back > j and code[back] in " \t\r":
                            back -= 1
                        if code[back] != "\\":
                            break
                    end += 1
                if brace == 0 and paren == 0:
                    keyword = re.match(r"#\s*(\w*)", code[j:end])
                    directive = keyword.group(1) if keyword else ""
                    if pending_start >= 0:
                        pending_has_directive = True
                    items.append(Item("directive", j, end, source.line_of(j), directive=directive))
                i = end
                line_start = False
                continue
        line_start = ch == "\n"
        if ch in "([":
            paren += 1
        elif ch in ")]":
            paren -= 1
            if paren < 0:
                return Parsed(source, items, f"{source.line_of(i)} 行目の閉じ括弧に対応する開き括弧がありません")
        elif ch == "{":
            if brace == 0 and paren == 0:
                start = pending_start if pending_start >= 0 else i
                name, is_static, is_function = analyze_declarator(code[start:i])
                if is_function and name is not None:
                    in_function = True
                    function_start = start
                    function_header_end = i
                else:
                    in_function = False
                if pending_start < 0:
                    pending_start = i
            brace += 1
        elif ch == "}":
            brace -= 1
            if brace < 0:
                return Parsed(source, items, f"{source.line_of(i)} 行目の閉じ波括弧に対応する開き波括弧がありません")
            if brace == 0 and paren == 0 and in_function:
                name, is_static, _ = analyze_declarator(code[function_start:function_header_end])
                items.append(Item(
                    "function", function_start, i + 1, source.line_of(function_start), name=name,
                    is_static=is_static, header_end=function_header_end, has_inner_directive=pending_has_directive,
                ))
                in_function = False
                pending_start = -1
                pending_has_directive = False
        elif ch == ";" and brace == 0 and paren == 0:
            start = pending_start if pending_start >= 0 else i
            name, is_static, is_function = analyze_declarator(code[start:i])
            items.append(Item(
                "declaration", start, i + 1, source.line_of(start), name=name, is_static=is_static,
                is_function_declaration=is_function, has_inner_directive=pending_has_directive,
            ))
            pending_start = -1
            pending_has_directive = False
        elif not ch.isspace() and pending_start < 0 and brace == 0:
            pending_start = i
        i += 1
    if brace != 0 or paren != 0:
        return Parsed(source, items, "ファイルの末尾で波括弧または丸括弧が閉じていません")
    if pending_start >= 0:
        return Parsed(source, items, f"{source.line_of(pending_start)} 行目からの要素が ';' または '}}' で終わっていません")
    return Parsed(source, items)


def check_text(text: str, path: str) -> list[Finding]:
    """1 つのファイルの内容を検査し、検出結果を返す。"""
    parsed = scan(text)
    if parsed.error is not None:
        return [Finding(path, 1, "error", "parse-error", parsed.error)]
    findings: list[Finding] = []
    definitions = [item for item in parsed.items if item.kind == "function" and item.name is not None]
    if not definitions:
        return findings
    first_definition = definitions[0].start
    prototypes = [
        item for item in parsed.items
        if item.kind == "declaration" and item.is_function_declaration and item.is_static
    ]
    declared_before = {item.name for item in prototypes if item.start < first_definition}

    helpers: dict[str, Item] = {}
    for item in definitions:
        if item.is_static:
            helpers.setdefault(item.name, item)
    for name, item in helpers.items():
        if name not in declared_before:
            findings.append(Finding(
                path, item.line, "error", "missing-prototype",
                f"static 関数 {name} の前方プロトタイプ宣言が、最初の関数定義より前にありません",
            ))

    static_definitions = [item for item in definitions if item.is_static]
    if static_definitions:
        first_static = static_definitions[0]
        for item in definitions:
            if not item.is_static and item.start > first_static.start:
                findings.append(Finding(
                    path, item.line, "error", "non-topdown-order",
                    f"主たる関数 {item.name} の定義が、static 関数 {first_static.name} の定義より後にあります",
                ))
    others = [item for item in definitions if item.is_static or item.name not in ENTRY_NAMES]
    if others:
        first_other = others[0]
        for item in definitions:
            if not item.is_static and item.name in ENTRY_NAMES and item.start > first_other.start:
                findings.append(Finding(
                    path, item.line, "error", "non-topdown-order",
                    f"入口関数 {item.name} の定義が、関数 {first_other.name} の定義より後にあります",
                ))

    declared_order: list[str] = []
    declared_line: dict[str, int] = {}
    for item in prototypes:
        if item.name in helpers and item.name not in declared_line:
            declared_order.append(item.name)
            declared_line[item.name] = item.line
    defined_order = [name for name in helpers if name in declared_line]
    for declared, defined in zip(declared_order, defined_order):
        if declared != defined:
            findings.append(Finding(
                path, declared_line[declared], "warning", "order-mismatch",
                f"前方プロトタイプ宣言の順序が定義の順序と一致しません (宣言 {declared}、定義 {defined})",
            ))
            break
    findings.sort(key=lambda finding: (finding.line, finding.rule))
    return findings


class TargetSelector:
    """対象ファイルの条件 (app/<name>/prod/ 配下、追跡済み、ignore 対象外、シンボリック リンクでない) を判定する。"""

    def __init__(self) -> None:
        self._repositories: dict[str, Path | None] = {}
        self._tracked: dict[Path, tuple[dict[str, str], set[str]]] = {}

    def exclusion_reason(self, path: Path) -> str | None:
        """対象外であればその理由を、対象であれば None を返す。"""
        absolute = Path(os.path.abspath(path))
        if absolute.suffix != ".c":
            return ".c ファイルではありません"
        if not is_under_app_prod(absolute):
            return "app/<name>/prod/ 配下ではありません"
        if absolute.is_symlink():
            return "シンボリック リンクです"
        repository = self._repository_of(absolute.parent)
        if repository is None:
            return "Git リポジトリの中にありません"
        modes, ignored = self._tracked_files(repository)
        relative = absolute.relative_to(repository).as_posix()
        if relative not in modes:
            return "Git で追跡されていません"
        if modes[relative] == "120000":
            return "シンボリック リンクです"
        if relative in ignored:
            return "ignore 対象です"
        return None

    def _repository_of(self, directory: Path) -> Path | None:
        key = str(directory)
        if key not in self._repositories:
            if (directory / ".git").exists():
                self._repositories[key] = directory
            elif directory.parent == directory:
                self._repositories[key] = None
            else:
                self._repositories[key] = self._repository_of(directory.parent)
        return self._repositories[key]

    def _tracked_files(self, repository: Path) -> tuple[dict[str, str], set[str]]:
        if repository not in self._tracked:
            staged = run_git(repository, ["ls-files", "-z", "-s"])
            modes: dict[str, str] = {}
            for entry in staged.split("\0"):
                if "\t" in entry:
                    meta, name = entry.split("\t", 1)
                    modes[name] = meta.split(" ", 1)[0]
            ignored = run_git(repository, ["ls-files", "-z", "-c", "-i", "--exclude-standard"])
            self._tracked[repository] = (modes, {name for name in ignored.split("\0") if name})
        return self._tracked[repository]


def run_git(repository: Path, arguments: list[str]) -> str:
    completed = subprocess.run(
        ["git", "-C", str(repository), *arguments], capture_output=True, check=True,
    )
    return completed.stdout.decode("utf-8")


def is_under_app_prod(path: Path) -> bool:
    parts = path.parts
    for index in range(len(parts) - 3):
        if parts[index] == "app" and parts[index + 2] == "prod":
            return True
    return False


def collect_targets(paths: list[str], selector: TargetSelector) -> tuple[list[Path], list[tuple[Path, str]]]:
    """対象ファイルと、明示的に指定された対象外のファイル (理由付き) を返す。"""
    if not paths:
        app_root = WORKSPACE_ROOT / "app"
        paths = [str(app / "prod") for app in sorted(app_root.iterdir()) if (app / "prod").is_dir()]
    targets: list[Path] = []
    excluded: list[tuple[Path, str]] = []
    for name in paths:
        path = Path(name)
        if path.is_dir() and not path.is_symlink():
            for directory, subdirectories, files in os.walk(path):
                subdirectories.sort()
                for file_name in sorted(files):
                    candidate = Path(directory) / file_name
                    if candidate.suffix == ".c" and selector.exclusion_reason(candidate) is None:
                        targets.append(candidate)
            continue
        reason = selector.exclusion_reason(path) if path.exists() or path.is_symlink() else "存在しません"
        if reason is None:
            targets.append(path)
        else:
            excluded.append((path, reason))
    return targets, excluded


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="C のソースが、関数をトップ ダウンの順に記載しているかを検査します。")
    parser.add_argument("paths", nargs="*", help="対象のファイルまたはディレクトリ (省略時はワークスペースの app/*/prod/)")
    args = parser.parse_args(argv)

    targets, excluded = collect_targets(args.paths, TargetSelector())
    for path, reason in excluded:
        print(f"{path}: 対象外のため検査しません ({reason})")
    errors = 0
    warnings = 0
    for path in targets:
        for finding in check_text(path.read_text(encoding="utf-8"), str(path)):
            print(finding.format())
            if finding.level == "error":
                errors += 1
            else:
                warnings += 1
    print(f"対象 {len(targets)} ファイル、エラー {errors} 件、警告 {warnings} 件")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
