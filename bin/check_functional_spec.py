#!/usr/bin/env python3
"""app の機能仕様の要件 ID、UUID、下流成果物の参照を検査する。

ワークスペース内で `docs/functional-spec-guideline.md` と `docs/functional-spec/`
の両方を持つ app を検査対象とする。要件 ID の接頭辞、参照コメントのタグ、
カテゴリごとの主語は、各 app の `docs/functional-spec-guideline.md` から読み取る。
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass, field
import os
from pathlib import Path
import re
import sys


UUID_TEXT = (
    r"[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-"
    r"[89ab][0-9a-f]{3}-[0-9a-f]{12}"
)
KIND_TEXT = r"FUNC|QUAL|COMP|CONS"

GUIDELINE_NAME = "functional-spec-guideline.md"
SPEC_DIRECTORY_NAME = "functional-spec"

ID_CONFIG_HEADING = "## 要件 ID の構成"
SUBJECT_HEADING = "## カテゴリごとの主語"

ID_CONFIG_KEYS = {
    "要件 ID 接頭辞": "prefix",
    "参照コメント タグ": "tag",
    "機能要件表の見出し": "table_heading",
}
ID_CONFIG_ROW_RE = re.compile(r"^\| (?P<key>[^|]+?) \| `(?P<value>[^`]+)` \|$")
SUBJECT_ROW_RE = re.compile(r"^\| `(?P<category>[A-Z0-9_]+)` \| (?P<subject>.+?) \|$")
FENCE_RE = re.compile(r"^\s*(?:```|~~~)")
HEADING_RE = re.compile(r"^#{1,6} ")

PREFIX_RE = re.compile(r"^[A-Z][A-Z0-9_]*$")
TAG_RE = re.compile(r"^[a-z][a-z0-9-]*$")

TEST_DESCRIPTION_TEMPLATE = r"^\s*// (?!{tags}:).+\S\s*$"
TEST_MACRO_RE = re.compile(r"^\s*(?:TEST|TEST_F|TEST_P|TYPED_TEST)\s*\(")

C_SOURCE_SUFFIXES = {".c", ".h"}
CXX_SOURCE_SUFFIXES = {".cc", ".cpp", ".cxx", ".hh", ".hpp", ".hxx"}
SOURCE_SUFFIXES = C_SOURCE_SUFFIXES | CXX_SOURCE_SUFFIXES
SCAN_SUFFIXES = SOURCE_SUFFIXES | {".md"}
EXCLUDED_DIRECTORY_NAMES = {
    ".git",
    "coverage",
    "doxybook2_internal",
    "doxybook2_public",
    "lib",
    "node_modules",
    "obj",
    "pages",
    "results",
    "xml",
    "xml_org",
}


@dataclass(frozen=True)
class AppSpec:
    """app 固有規範から読み取った、機能仕様の検査設定。"""

    name: str
    root: Path
    guideline_path: Path
    prefix: str
    tag: str
    table_heading: str
    subjects: dict[str, str] = field(compare=False)

    @property
    def id_text(self) -> str:
        return rf"{self.prefix}-[A-Z0-9_]+-(?:{KIND_TEXT})-[0-9]{{3,}}"

    @property
    def id_re(self) -> re.Pattern[str]:
        return re.compile(
            rf"^{self.prefix}-(?P<category>[A-Z0-9_]+)-"
            rf"(?P<kind>{KIND_TEXT})-(?P<number>[0-9]{{3,}})$"
        )

    @property
    def canonical_row_re(self) -> re.Pattern[str]:
        return re.compile(
            rf"^\| `(?P<id>{self.id_text})` "
            rf"<!-- {self.tag}: uuid=(?P<uuid>{UUID_TEXT}) --> "
            r"\| (?P<body>.+) \|$"
        )

    @property
    def spec_directory(self) -> Path:
        return self.root / "docs" / SPEC_DIRECTORY_NAME


@dataclass(frozen=True)
class Patterns:
    """検査対象の全 app を合わせた、走査用の正規表現。"""

    id_find: re.Pattern[str]
    legacy_id: re.Pattern[str]
    markdown_ref: re.Pattern[str]
    source_block_ref: re.Pattern[str]
    source_line_ref: re.Pattern[str]
    test_description: re.Pattern[str]
    tag_find: re.Pattern[str]
    tag_by_prefix: dict[str, str]
    prefix_by_tag: dict[str, str]


@dataclass(frozen=True)
class Requirement:
    requirement_id: str
    uuid: str
    path: Path
    line: int


@dataclass(frozen=True)
class Reference:
    requirement_id: str
    uuid: str
    tag: str
    path: Path
    line: int


@dataclass(frozen=True)
class CheckResult:
    errors: list[str]
    requirement_count: int
    reference_count: int
    counts_by_app: dict[str, int]


def _relative(path: Path, root: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return path.as_posix()


def _error(path: Path, line: int, root: Path, message: str) -> str:
    return f"{_relative(path, root)}:{line}: {message}"


def _read_lines(path: Path, root: Path, errors: list[str]) -> list[str]:
    try:
        return path.read_text(encoding="utf-8").splitlines()
    except (OSError, UnicodeError) as exc:
        errors.append(_error(path, 1, root, f"ファイルを読み取れません: {exc}"))
        return []


def _section_lines(lines: list[str], heading: str) -> list[str]:
    """指定した見出しの直下から、次の見出しまでの行を返す。

    フェンス付きコード ブロックの内側は、書式の例示とみなして除外する。
    """
    collected: list[str] = []
    inside = False
    fenced = False
    for line in lines:
        if FENCE_RE.match(line):
            fenced = not fenced
            continue
        if fenced:
            continue
        if HEADING_RE.match(line):
            if line.rstrip() == heading:
                inside = True
                collected = []
                continue
            if inside:
                break
            continue
        if inside:
            collected.append(line)
    return collected


def _parse_guideline(
    guideline_path: Path, app_root: Path, root: Path, errors: list[str]
) -> AppSpec | None:
    """app 固有規範から、要件 ID の構成とカテゴリごとの主語を読み取る。"""
    lines = _read_lines(guideline_path, root, errors)
    if not lines:
        return None

    values: dict[str, str] = {}
    for line in _section_lines(lines, ID_CONFIG_HEADING):
        match = ID_CONFIG_ROW_RE.fullmatch(line)
        if match is None:
            continue
        key = ID_CONFIG_KEYS.get(match.group("key").strip())
        if key is not None:
            values[key] = match.group("value")

    missing = [name for name in ID_CONFIG_KEYS if ID_CONFIG_KEYS[name] not in values]
    if missing:
        errors.append(
            _error(
                guideline_path,
                1,
                root,
                f"「{ID_CONFIG_HEADING[3:]}」に次の項目が定義されていません: {'、'.join(missing)}",
            )
        )
        return None

    prefix = values["prefix"]
    tag = values["tag"]
    if PREFIX_RE.fullmatch(prefix) is None:
        errors.append(
            _error(guideline_path, 1, root, f"要件 ID 接頭辞の書式が不正です: {prefix}")
        )
        return None
    if TAG_RE.fullmatch(tag) is None:
        errors.append(
            _error(guideline_path, 1, root, f"参照コメント タグの書式が不正です: {tag}")
        )
        return None

    subjects: dict[str, str] = {}
    for line in _section_lines(lines, SUBJECT_HEADING):
        match = SUBJECT_ROW_RE.fullmatch(line)
        if match is None:
            continue
        subjects[match.group("category")] = match.group("subject").strip()

    if not subjects:
        errors.append(
            _error(
                guideline_path,
                1,
                root,
                f"「{SUBJECT_HEADING[3:]}」にカテゴリの主語が定義されていません",
            )
        )
        return None

    return AppSpec(
        name=app_root.name,
        root=app_root,
        guideline_path=guideline_path,
        prefix=prefix,
        tag=tag,
        table_heading=values["table_heading"],
        subjects=subjects,
    )


def _discover_apps(root: Path, errors: list[str]) -> list[AppSpec]:
    """機能仕様を持つ app を探索し、app 固有規範から設定を読み取る。"""
    apps: list[AppSpec] = []
    app_directory = root / "app"
    if not app_directory.is_dir():
        errors.append("app: app ディレクトリが存在しません")
        return apps

    for app_root in sorted(app_directory.iterdir()):
        if not app_root.is_dir():
            continue
        guideline_path = app_root / "docs" / GUIDELINE_NAME
        spec_directory = app_root / "docs" / SPEC_DIRECTORY_NAME
        if not guideline_path.is_file():
            continue
        if not spec_directory.is_dir():
            continue
        app = _parse_guideline(guideline_path, app_root, root, errors)
        if app is not None:
            apps.append(app)

    if not apps:
        errors.append("app: 機能仕様が配置されている app が存在しません")
        return apps

    by_prefix: dict[str, AppSpec] = {}
    by_tag: dict[str, AppSpec] = {}
    for app in apps:
        duplicate = by_prefix.get(app.prefix)
        if duplicate is not None:
            errors.append(
                _error(
                    app.guideline_path,
                    1,
                    root,
                    f"要件 ID 接頭辞が {duplicate.name} と重複しています: {app.prefix}",
                )
            )
        else:
            by_prefix[app.prefix] = app
        duplicate = by_tag.get(app.tag)
        if duplicate is not None:
            errors.append(
                _error(
                    app.guideline_path,
                    1,
                    root,
                    f"参照コメント タグが {duplicate.name} と重複しています: {app.tag}",
                )
            )
        else:
            by_tag[app.tag] = app

    return apps


def _build_patterns(apps: list[AppSpec]) -> Patterns:
    """全 app の接頭辞とタグをまとめた走査用の正規表現を構築する。"""
    prefixes = "|".join(re.escape(app.prefix) for app in apps)
    tags = "|".join(re.escape(app.tag) for app in apps)
    id_text = rf"(?:{prefixes})-[A-Z0-9_]+-(?:{KIND_TEXT})-[0-9]{{3,}}"
    tag_text = rf"(?:{tags})"

    return Patterns(
        id_find=re.compile(rf"\b{id_text}\b"),
        legacy_id=re.compile(
            rf"\b(?:{prefixes})-[A-Z0-9_]+-(?!(?:{KIND_TEXT})-)[0-9]{{3,}}\b"
        ),
        markdown_ref=re.compile(
            rf"`(?P<id>{id_text})` "
            rf"<!-- (?P<tag>{tag_text}): uuid=(?P<uuid>{UUID_TEXT}) -->"
        ),
        source_block_ref=re.compile(
            rf"^\s*/\* (?P<tag>{tag_text}): id=(?P<id>{id_text}); "
            rf"uuid=(?P<uuid>{UUID_TEXT}) \*/\s*$"
        ),
        source_line_ref=re.compile(
            rf"^\s*// (?P<tag>{tag_text}): id=(?P<id>{id_text}); "
            rf"uuid=(?P<uuid>{UUID_TEXT})\s*$"
        ),
        test_description=re.compile(TEST_DESCRIPTION_TEMPLATE.format(tags=tag_text)),
        tag_find=re.compile(rf"\b{tag_text}:"),
        tag_by_prefix={app.prefix: app.tag for app in apps},
        prefix_by_tag={app.tag: app.prefix for app in apps},
    )


def _is_test_path(path: Path, root: Path) -> bool:
    parts = path.relative_to(root).parts
    return len(parts) > 3 and parts[0] in {"app", "framework"} and parts[2] == "test"


def _is_cxx_source(path: Path) -> bool:
    return path.suffix.lower() in CXX_SOURCE_SUFFIXES


def _match_source_ref(
    path: Path, line: str, patterns: Patterns
) -> re.Match[str] | None:
    block_match = patterns.source_block_ref.fullmatch(line)
    if block_match is not None:
        return block_match
    if _is_cxx_source(path):
        return patterns.source_line_ref.fullmatch(line)
    return None


def _covered_id_spans(matches: list[re.Match[str]]) -> set[tuple[int, int]]:
    return {match.span("id") for match in matches}


def _check_unpaired_ids(
    line: str,
    valid_matches: list[re.Match[str]],
    path: Path,
    line_number: int,
    root: Path,
    patterns: Patterns,
    errors: list[str],
) -> None:
    covered = _covered_id_spans(valid_matches)
    for match in patterns.id_find.finditer(line):
        if match.span() not in covered:
            errors.append(
                _error(
                    path,
                    line_number,
                    root,
                    f"要件 ID {match.group(0)} に UUID が併記されていません",
                )
            )


def _check_legacy_ids(
    line: str,
    path: Path,
    line_number: int,
    root: Path,
    patterns: Patterns,
    errors: list[str],
) -> None:
    for match in patterns.legacy_id.finditer(line):
        errors.append(
            _error(
                path,
                line_number,
                root,
                f"旧形式の要件 ID が検出されました: {match.group(0)}",
            )
        )


def _load_requirements(
    app: AppSpec,
    root: Path,
    requirements_by_id: dict[str, Requirement],
    requirements_by_uuid: dict[str, Requirement],
    errors: list[str],
) -> tuple[set[Path], int]:
    """1 つの app の機能仕様から、要件 ID と UUID の正本を読み取る。"""
    spec_paths: set[Path] = set()
    loaded = 0
    canonical_row_re = app.canonical_row_re
    id_re = app.id_re
    row_prefix = f"| `{app.prefix}-"

    for path in sorted(app.spec_directory.glob("*.md")):
        if path.name == "README.md":
            continue
        spec_paths.add(path)
        category = path.stem.upper()
        lines = _read_lines(path, root, errors)
        if f"| 要件 ID | {app.table_heading} |" not in lines:
            errors.append(_error(path, 1, root, "機能要件表の見出しが存在しません"))
        if "|---|---|" not in lines:
            errors.append(_error(path, 1, root, "機能要件表の区切り行が存在しません"))
        if category not in app.subjects:
            errors.append(
                _error(
                    path,
                    1,
                    root,
                    f"カテゴリが app 固有規範の主語表に定義されていません: {category}",
                )
            )

        previous_numbers: dict[str, int] = {}
        row_count = 0
        for line_number, line in enumerate(lines, start=1):
            if not line.startswith(row_prefix):
                continue
            row_count += 1
            match = canonical_row_re.fullmatch(line)
            if match is None:
                errors.append(
                    _error(path, line_number, root, "機能要件行の書式が不正です")
                )
                continue

            requirement_id = match.group("id")
            requirement_uuid = match.group("uuid")
            id_match = id_re.fullmatch(requirement_id)
            assert id_match is not None
            if id_match.group("category") != category:
                errors.append(
                    _error(
                        path,
                        line_number,
                        root,
                        f"要件 ID のカテゴリが文書名と一致しません: {requirement_id}",
                    )
                )

            expected_subject = app.subjects.get(category)
            if expected_subject is None:
                pass
            elif not match.group("body").startswith(f"{expected_subject}は、"):
                errors.append(
                    _error(
                        path,
                        line_number,
                        root,
                        f"要件文の主語は「{expected_subject}」でなければなりません",
                    )
                )

            kind = id_match.group("kind")
            number = int(id_match.group("number"))
            previous_number = previous_numbers.get(kind, 0)
            if number <= previous_number:
                errors.append(
                    _error(
                        path,
                        line_number,
                        root,
                        f"{kind} の連番が掲載順に増加していません: {requirement_id}",
                    )
                )
            previous_numbers[kind] = number

            requirement = Requirement(
                requirement_id, requirement_uuid, path, line_number
            )
            duplicate_id = requirements_by_id.get(requirement_id)
            if duplicate_id is not None:
                errors.append(
                    _error(
                        path,
                        line_number,
                        root,
                        f"要件 ID が重複しています: {requirement_id} "
                        f"({_relative(duplicate_id.path, root)}:{duplicate_id.line})",
                    )
                )
            else:
                requirements_by_id[requirement_id] = requirement
                loaded += 1

            duplicate_uuid = requirements_by_uuid.get(requirement_uuid)
            if duplicate_uuid is not None:
                errors.append(
                    _error(
                        path,
                        line_number,
                        root,
                        f"UUID が重複しています: {requirement_uuid} "
                        f"({_relative(duplicate_uuid.path, root)}:{duplicate_uuid.line})",
                    )
                )
            else:
                requirements_by_uuid[requirement_uuid] = requirement

        if row_count == 0:
            errors.append(_error(path, 1, root, "機能要件が定義されていません"))

    if not spec_paths:
        errors.append(
            _error(app.guideline_path, 1, root, "機能仕様が存在しません")
        )
    return spec_paths, loaded


def _validate_reference(
    reference: Reference,
    requirements_by_uuid: dict[str, Requirement],
    root: Path,
    patterns: Patterns,
    errors: list[str],
) -> None:
    expected_prefix = patterns.prefix_by_tag.get(reference.tag)
    actual_prefix = reference.requirement_id.split("-", 1)[0]
    if expected_prefix is not None and expected_prefix != actual_prefix:
        errors.append(
            _error(
                reference.path,
                reference.line,
                root,
                f"要件 ID の接頭辞 {actual_prefix} に対するタグは "
                f"{patterns.tag_by_prefix.get(actual_prefix)} です: {reference.tag}",
            )
        )
        return

    requirement = requirements_by_uuid.get(reference.uuid)
    if requirement is None:
        errors.append(
            _error(
                reference.path,
                reference.line,
                root,
                f"正本に存在しない UUID です: {reference.uuid}",
            )
        )
        return
    if requirement.requirement_id != reference.requirement_id:
        errors.append(
            _error(
                reference.path,
                reference.line,
                root,
                f"UUID に対応する現在の要件 ID は "
                f"{requirement.requirement_id} です: {reference.requirement_id}",
            )
        )


def _scan_markdown(
    path: Path,
    lines: list[str],
    root: Path,
    patterns: Patterns,
    errors: list[str],
    canonical_row_re: re.Pattern[str] | None = None,
) -> list[Reference]:
    references: list[Reference] = []
    for line_number, line in enumerate(lines, start=1):
        if canonical_row_re is not None and canonical_row_re.fullmatch(line) is not None:
            continue
        _check_legacy_ids(line, path, line_number, root, patterns, errors)
        matches = list(patterns.markdown_ref.finditer(line))
        if patterns.tag_find.search(line) and not matches:
            errors.append(_error(path, line_number, root, "要件コメントの書式が不正です"))
        _check_unpaired_ids(line, matches, path, line_number, root, patterns, errors)
        references.extend(
            Reference(
                match.group("id"),
                match.group("uuid"),
                match.group("tag"),
                path,
                line_number,
            )
            for match in matches
        )
    return references


def _scan_source(
    path: Path,
    lines: list[str],
    root: Path,
    patterns: Patterns,
    errors: list[str],
) -> list[Reference]:
    references: list[Reference] = []
    for line_number, line in enumerate(lines, start=1):
        _check_legacy_ids(line, path, line_number, root, patterns, errors)
        match = _match_source_ref(path, line, patterns)
        matches = [match] if match is not None else []
        if patterns.tag_find.search(line) and match is None:
            errors.append(
                _error(path, line_number, root, "製品コードの要件コメントが不正です")
            )
        _check_unpaired_ids(line, matches, path, line_number, root, patterns, errors)
        if match is not None:
            references.append(
                Reference(
                    match.group("id"),
                    match.group("uuid"),
                    match.group("tag"),
                    path,
                    line_number,
                )
            )
    return references


def _scan_test(
    path: Path,
    lines: list[str],
    root: Path,
    patterns: Patterns,
    errors: list[str],
) -> list[Reference]:
    references: list[Reference] = []
    marker_indexes: list[int] = []
    for index, line in enumerate(lines):
        line_number = index + 1
        _check_legacy_ids(line, path, line_number, root, patterns, errors)
        match = patterns.source_line_ref.fullmatch(line)
        matches = [match] if match is not None else []
        if patterns.tag_find.search(line) and match is None:
            errors.append(
                _error(path, line_number, root, "テストの要件コメントが不正です")
            )
        _check_unpaired_ids(line, matches, path, line_number, root, patterns, errors)
        if match is not None:
            marker_indexes.append(index)
            references.append(
                Reference(
                    match.group("id"),
                    match.group("uuid"),
                    match.group("tag"),
                    path,
                    line_number,
                )
            )

    marker_index_set = set(marker_indexes)
    for index in marker_indexes:
        if index - 1 in marker_index_set:
            continue
        if (
            index == 0
            or patterns.test_description.fullmatch(lines[index - 1]) is None
        ):
            errors.append(
                _error(
                    path,
                    index + 1,
                    root,
                    "要件コメントの直前にテスト項目の説明が記述されていません",
                )
            )

        last_index = index
        while last_index + 1 in marker_index_set:
            last_index += 1
        if (
            last_index + 1 >= len(lines)
            or TEST_MACRO_RE.match(lines[last_index + 1]) is None
        ):
            errors.append(
                _error(
                    path,
                    last_index + 1,
                    root,
                    "要件コメントの直後にテスト マクロが記述されていません",
                )
            )
    return references


def _iter_downstream_files(
    root: Path, spec_paths: set[Path], guideline_paths: set[Path]
) -> list[Path]:
    """要件参照を走査する対象のファイルを列挙する。

    機能仕様の正本と、記法を例示する規範は対象から除外する。
    """
    paths: list[Path] = []
    for directory, subdirectories, filenames in os.walk(root):
        subdirectories[:] = sorted(
            name for name in subdirectories if name not in EXCLUDED_DIRECTORY_NAMES
        )
        for filename in filenames:
            path = Path(directory) / filename
            if path.suffix.lower() not in SCAN_SUFFIXES:
                continue
            if path in spec_paths or path in guideline_paths:
                continue
            paths.append(path)
    return sorted(paths)


def check_workspace(root: Path) -> CheckResult:
    root = root.resolve()
    errors: list[str] = []
    apps = _discover_apps(root, errors)
    if not apps:
        return CheckResult(
            errors=errors, requirement_count=0, reference_count=0, counts_by_app={}
        )

    patterns = _build_patterns(apps)
    requirements_by_id: dict[str, Requirement] = {}
    requirements_by_uuid: dict[str, Requirement] = {}
    counts_by_app: dict[str, int] = {}
    spec_paths: set[Path] = set()
    spec_paths_by_app: dict[str, set[Path]] = {}

    for app in apps:
        app_spec_paths, loaded = _load_requirements(
            app, root, requirements_by_id, requirements_by_uuid, errors
        )
        counts_by_app[app.name] = loaded
        spec_paths |= app_spec_paths
        spec_paths_by_app[app.name] = app_spec_paths

    references: list[Reference] = []
    for app in apps:
        canonical_row_re = app.canonical_row_re
        for path in sorted(spec_paths_by_app[app.name]):
            lines = _read_lines(path, root, errors)
            references.extend(
                _scan_markdown(
                    path,
                    lines,
                    root,
                    patterns,
                    errors,
                    canonical_row_re=canonical_row_re,
                )
            )

    guideline_paths = {app.guideline_path for app in apps}
    guideline_paths.add(root / "app" / "general" / "docs" / GUIDELINE_NAME)

    for path in _iter_downstream_files(root, spec_paths, guideline_paths):
        lines = _read_lines(path, root, errors)
        if path.suffix.lower() == ".md":
            references.extend(_scan_markdown(path, lines, root, patterns, errors))
        elif _is_test_path(path, root):
            references.extend(_scan_test(path, lines, root, patterns, errors))
        else:
            references.extend(_scan_source(path, lines, root, patterns, errors))

    for reference in references:
        _validate_reference(
            reference, requirements_by_uuid, root, patterns, errors
        )

    return CheckResult(
        errors=errors,
        requirement_count=len(requirements_by_id),
        reference_count=len(references),
        counts_by_app=counts_by_app,
    )


def _parse_args(argv: list[str]) -> argparse.Namespace:
    default_root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(
        description="app の機能仕様の要件 ID、UUID、下流成果物の参照を検査します。"
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=default_root,
        help="ワークスペースのルート ディレクトリ",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = _parse_args(sys.argv[1:] if argv is None else argv)
    result = check_workspace(args.root)
    if result.errors:
        for error in result.errors:
            print(error, file=sys.stderr)
        print(f"ERROR: {len(result.errors)} 件の問題が検出されました", file=sys.stderr)
        return 1

    breakdown = "、".join(
        f"{name} {count} 件" for name, count in sorted(result.counts_by_app.items())
    )
    print(
        f"OK: 要件 {result.requirement_count} 件 ({breakdown})、"
        f"下流参照 {result.reference_count} 件"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
