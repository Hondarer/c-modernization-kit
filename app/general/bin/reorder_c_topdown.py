#!/usr/bin/env python3
"""C のソースの関数をトップ ダウンの順に並べ替え、並べ替えの前後の等価性を検証する。

規則の正本は app/general/docs/coding-guideline.md の「関数の記載順と前方宣言」節である。

並べ替え:
    ファイル スコープの要素を、直前のコメントと空行を含めた単位で扱い、次の順に並べる。
    1. 関数定義以外の要素 (前処理指令、型、変数など)。元の相対順序を保つ
    2. static 関数の前方プロトタイプ宣言。定義の宣言部から生成し、定義と同じ順に置く。
       変数より前に置くため、最後の型・マクロなどの要素の直後へ入れる
    3. static でない関数の定義。入口関数 (main、wmain) を最初にする
    4. static 関数の定義。元の相対順序を保つ
    前処理の分岐をまたいで要素を移さない。関数定義をすべて含む分岐 (ファイル全体を囲む条件) は、
    その内側を 1 つのファイルとして扱う。関数定義を含む部分的な分岐は、分岐全体を 1 つの単位として移し、
    前方プロトタイプ宣言は同じ条件の分岐に入れて宣言の区分に置く。
    自動で並べ替えられない構造を検出した場合は、ファイルを変更せずに理由を表示し、終了コード 1 を返す。

等価性の検証 (--verify、および --in-place の書き込み前):
    - コメントと空白を除いたトークン列で比較し、ファイル スコープの要素 (関数定義、変数、型、マクロ、
      前処理指令) と、各要素を囲む前処理の条件が改修前後で一致する
    - 改修後にだけある要素は static 関数の前方プロトタイプ宣言だけであり、宣言部のトークン列が
      対応する定義の宣言部と一致する。宣言の条件は、定義の条件と同じか、その外側である
    - コメントの文字列の集合が一致する

既定は差分の表示だけとし、--in-place を指定した場合にファイルを書き換える。
"""

from __future__ import annotations

import argparse
from collections import Counter
from dataclasses import dataclass, field
import difflib
import os
from pathlib import Path
import re
import sys

# 埋め込み版 Python (._pth) はスクリプトのディレクトリを sys.path に入れないため、明示的に追加する。
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import check_c_topdown as topdown  # noqa: E402


CONDITIONAL_OPEN = ("if", "ifdef", "ifndef")
CONDITIONAL_BRANCH = ("elif", "elifdef", "elifndef", "else")
MOVABLE_DIRECTIVES = ("define", "include")


class Unsupported(Exception):
    """自動で並べ替えられない構造。"""


@dataclass
class Branch:
    directive: topdown.Item
    nodes: list = field(default_factory=list)


@dataclass
class Group:
    """#if から #endif までの前処理の分岐。"""

    branches: list[Branch]
    end: topdown.Item | None = None

    @property
    def start(self) -> int:
        return self.branches[0].directive.start

    @property
    def stop(self) -> int:
        assert self.end is not None
        return self.end.end


@dataclass
class Unit:
    """並べ替えの単位 (直前のコメントと空行を含む)。"""

    kind: str  # other / variable / proto / main / helper
    text: str
    node: object
    entry: bool = False


def build_tree(items: list[topdown.Item]) -> list:
    """要素の並びを、前処理の分岐の木にする。"""
    root: list = []
    stack: list[Group] = []

    def current() -> list:
        return stack[-1].branches[-1].nodes if stack else root

    for item in items:
        if item.kind == "directive" and item.directive in CONDITIONAL_OPEN:
            group = Group(branches=[Branch(item)])
            current().append(group)
            stack.append(group)
        elif item.kind == "directive" and item.directive in CONDITIONAL_BRANCH:
            if not stack:
                raise Unsupported(f"{item.line} 行目の #{item.directive} に対応する #if がありません")
            stack[-1].branches.append(Branch(item))
        elif item.kind == "directive" and item.directive == "endif":
            if not stack:
                raise Unsupported(f"{item.line} 行目の #endif に対応する #if がありません")
            stack.pop().end = item
        else:
            current().append(item)
    if stack:
        raise Unsupported(f"{stack[-1].start} の #if が閉じていません")
    return root


def node_items(node) -> list[topdown.Item]:
    if isinstance(node, topdown.Item):
        return [node]
    result = []
    for branch in node.branches:
        result.append(branch.directive)
        for child in branch.nodes:
            result.extend(node_items(child))
    result.append(node.end)
    return result


def node_start(node) -> int:
    return node.start


def node_stop(node) -> int:
    return node.end if isinstance(node, topdown.Item) else node.stop


def definitions_in(node) -> list[topdown.Item]:
    return [item for item in node_items(node) if item.kind == "function"]


def find_regions(nodes: list, start: int, stop: int) -> list[tuple[list, int, int]]:
    """並べ替える範囲 (要素の並び、開始位置、終了位置) を求める。

    関数定義を含む要素が 1 つの分岐だけであれば、その分岐の内側を 1 つのファイルとして扱う。
    """
    holders = [node for node in nodes if definitions_in(node)]
    if not holders:
        return []
    if len(holders) == 1 and isinstance(holders[0], Group):
        group = holders[0]
        regions = []
        for index, branch in enumerate(group.branches):
            following = group.branches[index + 1].directive if index + 1 < len(group.branches) else group.end
            regions.extend(find_regions(branch.nodes, branch.directive.end, following.start))
        return regions
    return [(nodes, start, stop)]


class Reorderer:
    def __init__(self, text: str) -> None:
        self.text = text
        self.parsed = topdown.scan(text)
        if self.parsed.error is not None:
            raise Unsupported(self.parsed.error)
        self.source = self.parsed.source
        self.tree = build_tree(self.parsed.items)
        self.contexts = item_contexts(self.parsed.items, self.source)

    def run(self) -> str:
        if not topdown.check_text(self.text, ""):
            return self.text
        result = self.text
        regions = find_regions(self.tree, 0, len(self.text))
        for nodes, start, stop in sorted(regions, key=lambda region: region[1], reverse=True):
            begin = self.line_after(start) if start > 0 else 0
            end = stop if stop == len(self.text) else self.line_start(stop)
            result = result[:begin] + self.reorder_region(nodes, begin, end) + result[end:]
        return result

    def line_after(self, offset: int) -> int:
        """offset を含む行の次の行の先頭 (行末のコメントを含む)。"""
        end = offset
        for comment_start, comment_end in self.source.comments:
            if comment_start >= end and self.text.count("\n", end, comment_start) == 0:
                end = max(end, comment_end)
        newline = self.text.find("\n", end)
        return len(self.text) if newline < 0 else newline + 1

    def line_start(self, offset: int) -> int:
        return self.text.rfind("\n", 0, offset) + 1

    def reorder_region(self, nodes: list, begin: int, end: int) -> str:
        units = self.make_units(nodes, begin)
        tail = self.text[self.unit_end(nodes[-1]) if nodes else begin:end]
        first_definition = next(index for index, unit in enumerate(units) if unit.kind in ("main", "helper"))
        for unit in units[first_definition:]:
            if unit.kind in ("other", "variable"):
                for item in node_items(unit.node):
                    if (item.kind == "directive" and item.directive not in MOVABLE_DIRECTIVES
                            and item.directive not in CONDITIONAL_OPEN + CONDITIONAL_BRANCH + ("endif",)):
                        raise Unsupported(f"{item.line} 行目の前処理指令 #{item.directive} が関数定義の間にあります")

        prefix = [unit for unit in units if unit.kind in ("other", "variable")]
        protos = [unit for unit in units if unit.kind == "proto"]
        mains = [unit for unit in units if unit.kind == "main" and unit.entry]
        mains += [unit for unit in units if unit.kind == "main" and not unit.entry]
        helpers = [unit for unit in units if unit.kind == "helper"]

        definition_units = mains + helpers
        first_definition_offset = min(node_start(unit.node) for unit in definition_units)
        declared = self.declared_names(definition_units, first_definition_offset)
        block = self.prototype_block(protos, helpers, declared)

        insert_at = 0
        for index, unit in enumerate(prefix):
            if unit.kind == "other":
                insert_at = index + 1
        helper_names = {item.name for unit in helpers for item in definitions_in(unit.node)}
        for unit in prefix[:insert_at]:
            items = node_items(unit.node)
            if all(item.kind == "directive" for item in items):
                continue
            for item in items:
                if item.kind == "directive":
                    continue
                used = helper_names & set(topdown.tokenize(self.source.code[item.start:item.end]))
                if used:
                    raise Unsupported(
                        f"{item.line} 行目の要素が static 関数 {', '.join(sorted(used))} を参照しており、"
                        "前方プロトタイプ宣言より前に置かれます"
                    )

        ordered: list[tuple[str, str]] = []  # (種類, 文字列)
        for unit in prefix[:insert_at]:
            ordered.append(("keep", unit.text))
        if block:
            ordered.append(("block", block))
        for index, unit in enumerate(prefix[insert_at:]):
            ordered.append(("separate" if index == 0 and block else "keep", unit.text))
        for unit in mains + helpers:
            ordered.append(("separate", unit.text))

        pieces = []
        for index, (kind, piece) in enumerate(ordered):
            if index == 0:
                leading = re.match(r"(?:[ \t]*\n)*", units[0].text).group(0)
                piece = leading + strip_leading_blank_lines(piece)
            elif kind in ("separate", "block"):
                piece = "\n" + strip_leading_blank_lines(piece)
            pieces.append(piece)
        return "".join(pieces) + tail

    def make_units(self, nodes: list, begin: int) -> list[Unit]:
        units: list[Unit] = []
        position = begin
        index = 0
        while index < len(nodes):
            node = nodes[index]
            unit_end = self.unit_end(node)
            if node_start(node) < position:
                raise Unsupported(f"{self.source.line_of(node_start(node))} 行目で、複数の要素が同じ行にあります")
            if isinstance(node, Group) and len({item.is_static for item in definitions_in(node)}) > 1:
                units.extend(self.split_group(node, self.text[position:node_start(node)]))
            else:
                units.append(self.classify(node, self.text[position:unit_end]))
            position = unit_end
            index += 1
        return units

    def split_group(self, group: Group, leading: str) -> list[Unit]:
        """static 関数と static でない関数の定義が混在する分岐を、同じ条件の 2 つの分岐に分ける。

        static でない関数の定義と関数定義以外の要素を前の分岐に、static 関数の定義を後の分岐に置く。
        """
        main_branches: list[tuple[topdown.Item, list[str], list]] = []
        helper_branches: list[tuple[topdown.Item, list[str], list]] = []
        for index, branch in enumerate(group.branches):
            following = group.branches[index + 1].directive if index + 1 < len(group.branches) else group.end
            main_side: tuple[topdown.Item, list[str], list] = (branch.directive, [], [])
            helper_side: tuple[topdown.Item, list[str], list] = (branch.directive, [], [])
            position = self.line_after(branch.directive.end - 1)
            for child in branch.nodes:
                end = self.unit_end(child)
                if node_start(child) < position:
                    raise Unsupported(f"{self.source.line_of(node_start(child))} 行目で、複数の要素が同じ行にあります")
                kinds = {item.is_static for item in definitions_in(child)}
                if len(kinds) > 1:
                    raise Unsupported(
                        f"{self.source.line_of(node_start(child))} 行目の入れ子の分岐に、"
                        "static 関数と static でない関数の定義が混在しています"
                    )
                side = helper_side if kinds == {True} else main_side
                side[1].append(self.text[position:end])
                side[2].append(child)
                position = end
            main_side[1].append(self.text[position:self.line_start(following.start)])
            main_branches.append(main_side)
            helper_branches.append(helper_side)

        units = []
        for kind, branches, prefix in (("main", main_branches, leading), ("helper", helper_branches, "")):
            while len(branches) > 1 and not branches[-1][2]:
                branches.pop()
            text = prefix + "".join(
                self.directive_line(directive) + "".join(pieces) for directive, pieces, _ in branches
            ) + self.directive_line(group.end)
            node = Group(branches=[Branch(directive, children) for directive, _, children in branches], end=group.end)
            entry = any(item.name in topdown.ENTRY_NAMES for item in definitions_in(node))
            units.append(Unit(kind, text, node, entry=entry and kind == "main"))
        return units

    def unit_end(self, node) -> int:
        return self.line_after(node_stop(node) - 1)

    def classify(self, node, text: str) -> Unit:
        definitions = definitions_in(node)
        if definitions:
            if any(item.has_inner_directive for item in definitions):
                raise Unsupported(f"{definitions[0].line} 行目の関数定義の途中に前処理指令があります")
            statics = {item.is_static for item in definitions}
            if len(statics) > 1:
                raise Unsupported(
                    f"{self.source.line_of(node_start(node))} 行目の分岐に、static 関数と static でない関数の定義が混在しています"
                )
            if statics == {True}:
                return Unit("helper", text, node)
            entry = any(item.name in topdown.ENTRY_NAMES for item in definitions)
            return Unit("main", text, node, entry=entry)
        if isinstance(node, topdown.Item) and node.kind == "declaration":
            if node.is_function_declaration and node.is_static:
                return Unit("proto", text, node)
            if is_variable(self.source.code[node.start:node.end]) and not node.is_function_declaration:
                return Unit("variable", text, node)
        return Unit("other", text, node)

    def declared_names(self, definition_units: list[Unit], first_definition: int) -> dict[str, list[tuple]]:
        """既にある static 関数のプロトタイプ宣言の名前と条件。関数定義の単位の中の宣言は除く。"""
        inside = {id(item) for unit in definition_units for item in node_items(unit.node)}
        declared: dict[str, list[tuple]] = {}
        for item in self.parsed.items:
            if (item.kind == "declaration" and item.is_function_declaration and item.is_static
                    and id(item) not in inside):
                declared.setdefault(item.name, []).append(self.contexts[id(item)])
        return declared

    def is_declared(self, declared: dict[str, list[tuple]], definition: topdown.Item) -> bool:
        context = self.contexts[id(definition)]
        return any(context[: len(found)] == found for found in declared.get(definition.name, []))

    def prototype_block(self, protos: list[Unit], helpers: list[Unit], declared: dict[str, list[tuple]]) -> str:
        by_name: dict[str, Unit] = {}
        for unit in protos:
            by_name.setdefault(unit.node.name, unit)
        used: set[int] = set()
        lines: list[str] = []
        for unit in helpers:
            if isinstance(unit.node, topdown.Item):
                existing = by_name.get(unit.node.name)
                if existing is not None:
                    if id(existing) not in used:
                        used.add(id(existing))
                        lines.append(strip_leading_blank_lines(existing.text))
                elif not self.is_declared(declared, unit.node):
                    lines.append(self.prototype(unit.node) + "\n")
            else:
                skeleton = self.prototype_skeleton(unit.node, declared)
                if skeleton:
                    lines.append(skeleton)
        unused = [strip_leading_blank_lines(unit.text) for unit in protos if id(unit) not in used]
        pieces = unused + lines
        # コメントを伴う宣言は、前置コメントの規則に合わせて前後を空行で区切る
        result = ""
        for index, piece in enumerate(pieces):
            if index > 0 and (starts_with_comment(piece) or starts_with_comment(pieces[index - 1])):
                result += "\n"
            result += piece
        return result

    def prototype(self, definition: topdown.Item) -> str:
        header = self.text[definition.start:definition.header_end]
        for comment_start, comment_end in reversed(self.source.comments):
            if definition.start <= comment_start < definition.header_end:
                header = (header[: comment_start - definition.start].rstrip(" \t")
                          + header[comment_end - definition.start:])
        return header.rstrip() + ";"

    def prototype_skeleton(self, group: Group, declared: dict[str, list[tuple]]) -> str:
        """部分的な分岐の中の static 関数の前方プロトタイプ宣言を、同じ条件の分岐に入れて作る。"""
        body: list[str] = []
        has_prototype = False
        for index, branch in enumerate(group.branches):
            branch_lines = []
            for child in branch.nodes:
                if isinstance(child, Group):
                    nested = self.prototype_skeleton(child, declared)
                    if nested:
                        branch_lines.append(nested)
                elif child.kind == "function" and not self.is_declared(declared, child):
                    branch_lines.append(self.prototype(child) + "\n")
            has_prototype = has_prototype or bool(branch_lines)
            body.append((self.directive_line(branch.directive), branch_lines))
        if not has_prototype:
            return ""
        # 末尾の空の分岐 (#else、#elif) は、ほかの分岐の条件を変えないため省く
        while len(body) > 1 and not body[-1][1]:
            body.pop()
        result = "".join(line + "".join(lines) for line, lines in body)
        return result + self.directive_line(group.end)

    def directive_line(self, item: topdown.Item) -> str:
        return self.text[self.line_start(item.start):self.line_after(item.end - 1)]


def starts_with_comment(text: str) -> bool:
    return text.lstrip().startswith(("/*", "//"))


def strip_leading_blank_lines(text: str) -> str:
    return re.sub(r"\A(?:[ \t]*\n)+", "", text)


def is_variable(code: str) -> bool:
    """関数宣言でない宣言が、変数 (型の定義や静的表明でない) であるかを判定する。"""
    tokens = topdown.tokenize(code)
    if not tokens or tokens[0] in ("typedef", "static_assert", "_Static_assert"):
        return False
    if tokens[0] in ("struct", "union", "enum") and len(tokens) > 1:
        if len(tokens) <= 3 and tokens[-1] == ";":
            return False
        brace = 1 if tokens[1] == "{" else 2 if len(tokens) > 2 and tokens[2] == "{" else -1
        if brace >= 0:
            depth = 0
            for index in range(brace, len(tokens)):
                if tokens[index] == "{":
                    depth += 1
                elif tokens[index] == "}":
                    depth -= 1
                    if depth == 0:
                        return tokens[index + 1:] != [";"]
    return True


def item_contexts(items: list[topdown.Item], source: topdown.Source) -> dict[int, tuple]:
    """各要素を囲む前処理の条件 (開始の指令と分岐の指令のトークン列の組の並び) を求める。"""
    nocomment = without_comments(source)
    contexts: dict[int, tuple] = {}
    stack: list[tuple[tuple, tuple]] = []
    for item in items:
        if item.kind == "directive":
            tokens = tuple(topdown.tokenize(nocomment[item.start:item.end].replace("\\\n", " ")))
            if item.directive in CONDITIONAL_OPEN:
                stack.append((tokens, tokens))
                contexts[id(item)] = tuple(stack[:-1])
                continue
            if item.directive in CONDITIONAL_BRANCH and stack:
                stack[-1] = (stack[-1][0], tokens)
                contexts[id(item)] = tuple(stack[:-1])
                continue
            if item.directive == "endif" and stack:
                stack.pop()
                contexts[id(item)] = tuple(stack)
                continue
        contexts[id(item)] = tuple(stack)
    return contexts


def without_comments(source: topdown.Source) -> str:
    chars = list(source.text)
    for start, end in source.comments:
        for position in range(start, end):
            if chars[position] != "\n":
                chars[position] = " "
    return "".join(chars)


def reorder_text(text: str) -> str:
    """並べ替えた結果を返す。並べ替えられない場合は Unsupported を送出する。"""
    return Reorderer(text).run()


@dataclass
class Element:
    context: tuple
    tokens: tuple
    line: int
    item: topdown.Item
    header: tuple = ()


def elements(text: str) -> tuple[list[Element], Counter]:
    parsed = topdown.scan(text)
    if parsed.error is not None:
        raise Unsupported(parsed.error)
    nocomment = without_comments(parsed.source)
    contexts = item_contexts(parsed.items, parsed.source)
    result = []
    for item in parsed.items:
        if item.kind == "directive" and item.directive in CONDITIONAL_OPEN + CONDITIONAL_BRANCH + ("endif",):
            continue
        body = nocomment[item.start:item.end]
        if item.kind == "directive":
            body = body.replace("\\\n", " ")
        header = ()
        if item.kind == "function":
            header = tuple(topdown.tokenize(nocomment[item.start:item.header_end]))
        result.append(Element(contexts[id(item)], tuple(topdown.tokenize(body)), item.line, item, header))
    comments = Counter(text[start:end] for start, end in parsed.source.comments)
    return result, comments


def verify_texts(before: str, after: str) -> list[str]:
    """並べ替えの前後の等価性を検証し、問題の説明の一覧を返す (空なら等価)。"""
    try:
        before_elements, before_comments = elements(before)
        after_elements, after_comments = elements(after)
    except Unsupported as error:
        return [f"解析できません: {error}"]
    problems: list[str] = []
    before_counter = Counter((element.context, element.tokens) for element in before_elements)
    after_counter = Counter((element.context, element.tokens) for element in after_elements)
    missing = before_counter - after_counter
    extra = after_counter - before_counter
    for element in before_elements:
        key = (element.context, element.tokens)
        if missing[key] > 0:
            missing[key] -= 1
            problems.append(f"改修前の {element.line} 行目の要素が、同じ条件の下で改修後にありません: {summary(element)}")
    definitions = [element for element in after_elements if element.item.kind == "function" and element.item.is_static]
    for element in after_elements:
        key = (element.context, element.tokens)
        if extra[key] <= 0:
            continue
        extra[key] -= 1
        item = element.item
        if item.kind == "declaration" and item.is_function_declaration and item.is_static:
            matched = any(
                definition.tokens and definition.header + (";",) == element.tokens
                and definition.context[: len(element.context)] == element.context
                for definition in definitions
            )
            if matched:
                continue
            problems.append(f"改修後の {element.line} 行目の前方プロトタイプ宣言が、同じ条件の定義の宣言部と一致しません: {summary(element)}")
            continue
        problems.append(f"改修後の {element.line} 行目の要素が、同じ条件の下で改修前にありません: {summary(element)}")
    for comment in sorted(set(before_comments) - set(after_comments)):
        problems.append(f"改修後にないコメント: {comment.splitlines()[0][:80]}")
    for comment in sorted(set(after_comments) - set(before_comments)):
        problems.append(f"改修前にないコメント: {comment.splitlines()[0][:80]}")
    return problems


def summary(element: Element) -> str:
    return " ".join(element.tokens[:12]) + (" ..." if len(element.tokens) > 12 else "")


def reorder_file(path: Path, in_place: bool) -> str:
    """1 つのファイルを処理し、結果の種類 (unchanged / changed / unsupported) を返す。"""
    text = path.read_text(encoding="utf-8")
    try:
        result = reorder_text(text)
    except Unsupported as error:
        print(f"{path}: 自動で並べ替えられません: {error}")
        return "unsupported"
    if result == text:
        return "unchanged"
    problems = verify_texts(text, result)
    if problems:
        print(f"{path}: 並べ替えの結果が等価性の検証に合格しないため、変更しません")
        for problem in problems:
            print(f"    {problem}")
        return "unsupported"
    if in_place:
        path.write_text(result, encoding="utf-8", newline="\n")
        print(f"{path}: 並べ替えました")
    else:
        diff = difflib.unified_diff(
            text.splitlines(keepends=True), result.splitlines(keepends=True), str(path), str(path),
        )
        sys.stdout.writelines(diff)
    return "changed"


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="C のソースの関数をトップ ダウンの順に並べ替えます。")
    parser.add_argument("paths", nargs="*", help="対象のファイルまたはディレクトリ")
    parser.add_argument("--in-place", action="store_true", help="ファイルを書き換える (既定は差分の表示だけ)")
    parser.add_argument("--verify", nargs=2, metavar=("BEFORE", "AFTER"), help="改修前後のファイルの等価性を検証する")
    args = parser.parse_args(argv)

    if args.verify:
        if args.paths or args.in_place:
            parser.error("--verify は、ほかの引数と同時に指定できません")
        before, after = (Path(name).read_text(encoding="utf-8") for name in args.verify)
        problems = verify_texts(before, after)
        for problem in problems:
            print(problem)
        print(f"{args.verify[1]}: 等価性の検証に{'失敗しました' if problems else '合格しました'}")
        return 1 if problems else 0
    if not args.paths:
        parser.error("対象のファイルまたはディレクトリを指定してください")

    targets, excluded = topdown.collect_targets(args.paths, topdown.TargetSelector())
    for path, reason in excluded:
        print(f"{path}: 対象外のため処理しません ({reason})")
    results = Counter(reorder_file(path, args.in_place) for path in targets)
    print(
        f"対象 {len(targets)} ファイル、変更 {results['changed']} ファイル、"
        f"並べ替えられない {results['unsupported']} ファイル"
    )
    return 1 if results["unsupported"] else 0


if __name__ == "__main__":
    sys.exit(main())
