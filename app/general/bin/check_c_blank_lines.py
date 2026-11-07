#!/usr/bin/env python3
"""C / C++ ソースで、処理を区切る空行がない箇所を検出し、必要に応じて挿入する。

判定規則の正本は app/general/docs/coding-guideline.md の「処理を区切る空行」節である。

検出する種別:
    after-block   閉じ波括弧 (do 文の末尾を含む) の後に、同じ字下げで次の文が続く
    before-block  通常の文の後に、同じ字下げでブロックが続く
    comment       前置コメントの直前が空行でない
    final-return  関数末尾の return の直前が空行でない
    comment-before-else  閉じ波括弧と else の間にコメントがある (--fix では直さない)

既定では検出結果を出力し、1 件以上あれば終了コード 1 を返す。--fix を指定すると空行を挿入する。
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass, field
from pathlib import Path
import re
import sys


SOURCE_SUFFIXES = (".c", ".h", ".cc")
CONTROL_KEYWORDS = ("if", "else", "for", "while", "switch", "do")
CONTROL_RE = re.compile(r"^(if|for|while|switch|do)\b")
LABEL_RE = re.compile(r"^(case\b.*|default\s*|[A-Za-z_]\w*\s*):$")
DO_WHILE_END_RE = re.compile(r"^\}\s*while\s*\(.*\)\s*;$")


@dataclass
class Line:
    """1 行分の字句解析の結果。"""

    text: str
    code: str = ""  # コメントと文字列の中身を除いたコード部分
    indent: int = 0
    starts_in_comment: bool = False  # 行頭がブロック コメントの途中
    is_preprocessor: bool = False  # 前処理指令の行 (継続行を含む)
    paren_depth_at_start: int = 0
    closes: str | None = None  # 行頭の '}' が閉じたブロックの種類


@dataclass
class Block:
    """開いている波括弧の情報。"""

    kind: str  # function / control / aggregate
    header: str
    header_line: int
    is_do: bool = False


@dataclass
class Function:
    """関数本体の情報。"""

    open_line: int
    statements: int = 0
    last_statement_line: int = -1
    last_statement_start: int = -1
    last_is_return: bool = False


@dataclass
class Analysis:
    lines: list[Line]
    statement_start: dict[int, int] = field(default_factory=dict)  # 文の終わりの行 -> 文の開始行
    statement_text: dict[int, str] = field(default_factory=dict)  # 文の終わりの行 -> 文のコード
    block_headers: dict[int, tuple[str, str]] = field(default_factory=dict)  # 見出しの開始行 -> (種類, 見出し)
    final_returns: list[tuple[int, int]] = field(default_factory=list)  # (return の開始行, 関数の文の数)


def analyze(text: str) -> Analysis:
    """ソース全体を字句解析し、行ごとの情報と文、ブロック、関数の境界を求める。"""
    raw_lines = text.split("\n")
    lines = [Line(text=raw) for raw in raw_lines]
    result = Analysis(lines=lines)

    in_comment = False
    in_preprocessor = False
    paren_depth = 0
    stack: list[Block] = []
    functions: list[Function] = []
    pending = ""  # 直前の境界 (';'、'{'、'}') から続くコード
    pending_start = -1
    skip_semicolon_count = False  # do 文の末尾の ';' を文として数えない
    # 関数内の #if の分岐ごとに文を数えるため、分岐の開始時点の文の数を積む
    branch_counts: list[int] = []

    def current_function() -> Function | None:
        for block in reversed(stack):
            if block.kind == "function":
                return functions[-1]
        return None

    def at_function_body() -> bool:
        return bool(stack) and stack[-1].kind == "function"

    for index, line in enumerate(lines):
        raw = line.text
        line.indent = len(raw) - len(raw.lstrip(" "))
        line.starts_in_comment = in_comment
        line.paren_depth_at_start = paren_depth
        stripped = raw.strip()

        if not in_comment and (in_preprocessor or stripped.startswith("#")):
            if not in_preprocessor and at_function_body():
                directive = stripped[1:].lstrip().split(" ")[0]
                func = functions[-1]
                if directive.startswith("if"):
                    branch_counts.append(func.statements)
                elif directive in ("elif", "else") and branch_counts:
                    func.statements = branch_counts[-1]
                elif directive == "endif" and branch_counts:
                    branch_counts.pop()
            line.is_preprocessor = True
            in_preprocessor = raw.rstrip().endswith("\\")
            continue

        code_chars: list[str] = []
        i = 0
        quote = ""
        while i < len(raw):
            ch = raw[i]
            nxt = raw[i + 1] if i + 1 < len(raw) else ""
            if in_comment:
                if ch == "*" and nxt == "/":
                    in_comment = False
                    i += 2
                    continue
                i += 1
                continue
            if quote:
                if ch == "\\":
                    i += 2
                    continue
                if ch == quote:
                    quote = ""
                    code_chars.append(ch)
                i += 1
                continue
            if ch == "/" and nxt == "*":
                in_comment = True
                i += 2
                continue
            if ch == "/" and nxt == "/":
                break
            if ch in ("'", '"'):
                quote = ch
                code_chars.append(ch)
                i += 1
                continue

            code_chars.append(ch)
            if ch.isspace() and not pending.strip():
                i += 1
                continue
            if not pending.strip() and not ch.isspace():
                pending_start = index
            if ch == "(":
                paren_depth += 1
            elif ch == ")":
                paren_depth = max(0, paren_depth - 1)
            if ch == "{" and paren_depth == 0:
                header = pending.strip()
                kind = classify_block(header, stack)
                is_do = header == "do" or header.endswith(" do")
                if kind == "control" and at_function_body() and not header.startswith("else"):
                    func = current_function()
                    if func is not None:
                        func.statements += 1
                        func.last_is_return = False
                        func.last_statement_line = index
                if kind == "function":
                    functions.append(Function(open_line=index))
                header_line = pending_start if header else index
                result.block_headers.setdefault(header_line, (kind, header))
                stack.append(Block(kind=kind, header=header, header_line=header_line, is_do=is_do))
                pending = ""
                pending_start = -1
            elif ch == "}" and paren_depth == 0:
                closed = stack.pop() if stack else Block(kind="aggregate", header="", header_line=index)
                if code_chars and "".join(code_chars).strip() == "}" and line.closes is None:
                    line.closes = closed.kind
                if closed.kind == "function" and functions:
                    func = functions[-1]
                    if func.last_is_return:
                        result.final_returns.append((func.last_statement_start, func.statements))
                if closed.is_do:
                    skip_semicolon_count = True
                pending = ""
                pending_start = -1
            elif ch == ";" and paren_depth == 0:
                statement = (pending + ";").strip()
                start = pending_start if pending_start >= 0 else index
                result.statement_start[index] = start
                result.statement_text[index] = statement
                if at_function_body():
                    func = functions[-1]
                    if skip_semicolon_count:
                        skip_semicolon_count = False
                    else:
                        func.statements += 1
                    func.last_statement_line = index
                    func.last_statement_start = start
                    func.last_is_return = re.match(r"^return\b", statement) is not None
                elif skip_semicolon_count:
                    skip_semicolon_count = False
                pending = ""
                pending_start = -1
            else:
                pending += ch
            i += 1
        if quote:
            quote = ""
        line.code = "".join(code_chars).strip()
        if pending.strip():
            pending += " "
    return result


def classify_block(header: str, stack: list[Block]) -> str:
    """'{' の前の見出しから、ブロックの種類を決める。"""
    inside_function = any(block.kind == "function" for block in stack)
    if inside_function:
        if header == "" or CONTROL_RE.match(header) or header.startswith("else") or header.endswith(":"):
            return "control"
        return "aggregate"
    if "(" in header and "=" not in header.split("(")[0] and not re.match(r"^(struct|union|enum|typedef)\b", header):
        return "function"
    return "aggregate"


def assigned_lvalue(statement: str) -> str | None:
    """文が代入 (初期化子付きの宣言を含む) であれば、代入先の式を返す。"""
    depth = 0
    for pos, ch in enumerate(statement):
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        elif ch == "=" and depth == 0:
            before = statement[pos - 1] if pos > 0 else ""
            after = statement[pos + 1] if pos + 1 < len(statement) else ""
            # ==、!=、<=、>= は比較であり、<<= と >>= は代入である
            if after == "=":
                continue
            if before and before in "=!<>" and not statement[:pos].endswith(("<<", ">>")):
                continue
            left = statement[:pos].rstrip("+-*/%&|^<> \t")
            match = re.search(r"([A-Za-z_]\w*(?:\s*(?:->|\.)\s*[A-Za-z_]\w*|\s*\[[^\]]*\])*)\s*$", left)
            return re.sub(r"\s+", "", match.group(1)) if match else None
    return None


def block_condition(header: str) -> str | None:
    """if、while、switch の式と、for の条件を返す。do 文と、その他のブロックは None を返す。"""
    match = re.match(r"^(if|while|switch|for)\s*\(", header)
    if not match:
        return None
    start = header.index("(", match.start(1))
    depth = 0
    for pos in range(start, len(header)):
        if header[pos] == "(":
            depth += 1
        elif header[pos] == ")":
            depth -= 1
            if depth == 0:
                inner = header[start + 1 : pos]
                if match.group(1) == "for":
                    parts = split_top_level(inner, ";")
                    return parts[1] if len(parts) == 3 else inner
                return inner
    return header[start + 1 :]


def uses_lvalue(condition: str, lvalue: str) -> bool:
    """条件式に代入先の式が現れるかを、空白を除いて判定する。"""
    compact = re.sub(r"\s+", "", condition)
    return re.search(r"(?<![\w.>])" + re.escape(lvalue) + r"(?![\w\[])", compact) is not None


def split_top_level(text: str, separator: str) -> list[str]:
    parts = []
    depth = 0
    current = ""
    for ch in text:
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        if ch == separator and depth == 0:
            parts.append(current)
            current = ""
            continue
        current += ch
    parts.append(current)
    return parts


def is_comment_start(line: Line) -> bool:
    stripped = line.text.strip()
    return not line.starts_in_comment and not line.code and stripped.startswith(("/*", "//"))


def is_comment_line(line: Line) -> bool:
    stripped = line.text.strip()
    return line.starts_in_comment or (not line.code and stripped.startswith(("/*", "//", "*")))


def find_gaps(text: str) -> list[tuple[int, str]]:
    """空行を挿入すべき位置 (挿入後に空行の次になる行の 0 始まりの番号) と種別を返す。"""
    analysis = analyze(text)
    lines = analysis.lines
    gaps: dict[int, str] = {}

    def add(index: int, kind: str) -> None:
        gaps.setdefault(index, kind)

    final_return_counts = dict(analysis.final_returns)

    for index in range(1, len(lines)):
        cur = lines[index]
        prev = lines[index - 1]
        if not cur.text.strip() or not prev.text.strip():
            continue
        if cur.is_preprocessor or prev.is_preprocessor:
            continue
        if cur.starts_in_comment:
            continue

        # 前置コメントの直前
        if is_comment_start(cur):
            if cur.paren_depth_at_start > 0:
                continue
            # 閉じ波括弧と else の間のコメントは、空行の挿入では直せないため別の種別とする
            following = index
            while following < len(lines) and (is_comment_line(lines[following]) or not lines[following].text.strip()):
                following += 1
            if following < len(lines) and re.match(r"^else\b", lines[following].code):
                add(index, "comment-before-else")
                continue
            if is_comment_line(prev) or prev.code.endswith("{") or LABEL_RE.match(prev.code):
                continue
            add(index, "comment")
            continue

        # 閉じ波括弧の後
        if (prev.code == "}" and prev.closes == "control") or DO_WHILE_END_RE.match(prev.code):
            if cur.indent == prev.indent and not re.match(r"^(\}|else\b|while\b)", cur.code) and not LABEL_RE.match(cur.code):
                add(index, "after-block")
            continue

        # 通常の文の後のブロック
        header = analysis.block_headers.get(index)
        if header and header[0] == "control" and prev.code.endswith(";") and not prev.code.startswith("}"):
            start = analysis.statement_start.get(index - 1, index - 1)
            if lines[start].indent != cur.indent:
                continue
            # 直前の文で代入した値をブロックの条件で判定する場合は、取得と判定のまとまりとして詰める
            lvalue = assigned_lvalue(analysis.statement_text.get(index - 1, ""))
            condition = block_condition(header[1])
            if lvalue and condition is not None and uses_lvalue(condition, lvalue):
                continue
            add(index, "before-block")
            continue

        # 関数末尾の return
        if index in final_return_counts:
            if prev.code.endswith("{") or is_comment_line(prev) or LABEL_RE.match(prev.code):
                continue
            if final_return_counts[index] <= 2 and not prev.code.startswith("}"):
                continue
            add(index, "final-return")

    return sorted(gaps.items())


def insert_gaps(text: str, gaps: list[tuple[int, str]]) -> str:
    lines = text.split("\n")
    for index, kind in sorted(gaps, reverse=True):
        if kind == "comment-before-else":
            continue
        lines.insert(index, "")
    return "\n".join(lines)


def collect_files(paths: list[str]) -> list[Path]:
    files: list[Path] = []
    for name in paths:
        path = Path(name)
        if path.is_dir():
            files.extend(sorted(p for p in path.rglob("*") if p.is_file() and p.suffix in SOURCE_SUFFIXES))
        else:
            files.append(path)
    return files


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="処理を区切る空行がない箇所を検出し、必要に応じて挿入します。")
    parser.add_argument("paths", nargs="+", help="対象のファイルまたはディレクトリ")
    parser.add_argument("--fix", action="store_true", help="空行がない箇所に空行を挿入する")
    args = parser.parse_args(argv)

    found = 0
    for path in collect_files(args.paths):
        text = path.read_text(encoding="utf-8")
        gaps = find_gaps(text)
        if not gaps:
            continue
        if args.fix:
            path.write_text(insert_gaps(text, gaps), encoding="utf-8", newline="\n")
            print(f"{path}: {len(gaps)} 件の空行を挿入しました")
            continue
        lines = text.split("\n")
        for index, kind in gaps:
            print(f"{path}:{index + 1}: {kind}: {lines[index].strip()}")
        found += len(gaps)
    if args.fix:
        return 0
    return 1 if found else 0


if __name__ == "__main__":
    sys.exit(main())
