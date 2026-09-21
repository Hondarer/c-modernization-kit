#!/usr/bin/env python3
"""app 直下の .gitignore が正本と一致していることを検査する。

各 app は .gitignore の実体を持ち、テンプレートから生成する仕組みがない。
そのため app ごとに内容がずれても気付けず、実際に 6 app がずれていた。

正本は bin/app-gitignore.template とし、全 app へ同じ内容を配る。
app が生成しないものの規則も残す。生成物の有無は app ごとに変わるが、
規則を app ごとに削ると、何がずれで何が意図かを判別できなくなるためである。

使い方:
  check-app-gitignore.py --check   差分のある app を報告する (CI 向け)
  check-app-gitignore.py --write   正本の内容を全 app へ書き出す
"""

from __future__ import annotations

import argparse
import difflib
import sys
from pathlib import Path

#: 正本の置き場所。ワークスペース ルートからの相対パス。
TEMPLATE_RELATIVE = "bin/app-gitignore.template"

#: 検査の対象とする app の置き場所。
APP_DIR_RELATIVE = "app"

#: ワークスペース ルートを示す目印。
WORKSPACE_MARKER = ".workspaceRoot"


def find_workspace_root(start: Path) -> Path | None:
    """目印をたどってワークスペース ルートを返す。"""
    for directory in [start.resolve()] + list(start.resolve().parents):
        if (directory / WORKSPACE_MARKER).is_file():
            return directory
    return None


def collect_targets(workspace_root: Path) -> list[Path]:
    """検査の対象となる app 直下の .gitignore を、名前の昇順で返す。

    サブモジュールの app も対象とする。.gitignore はそれぞれのリポジトリで
    管理されるが、ワークスペースの利用者から見た内容は同じであるべきため。
    """
    app_root = workspace_root / APP_DIR_RELATIVE
    return sorted(
        (path / ".gitignore")
        for path in app_root.iterdir()
        if path.is_dir() and (path / ".gitignore").is_file()
    )


def report_difference(workspace_root: Path, target: Path, expected: str) -> None:
    """正本との差分を、ワークスペース ルートからの相対パスで報告する。"""
    relative = target.relative_to(workspace_root)
    current = target.read_text(encoding="utf-8")
    print(f"差分あり: {relative}", file=sys.stderr)
    for line in difflib.unified_diff(
        expected.splitlines(keepends=True),
        current.splitlines(keepends=True),
        fromfile=TEMPLATE_RELATIVE,
        tofile=str(relative),
    ):
        sys.stderr.write(line)


def main(argv: list[str] | None = None) -> int:
    """コマンドの入口。"""
    parser = argparse.ArgumentParser(description="app 直下の .gitignore を正本と照合します。")
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--check", action="store_true", help="差分のある app を報告する")
    mode.add_argument("--write", action="store_true", help="正本の内容を全 app へ書き出す")
    args = parser.parse_args(argv)

    workspace_root = find_workspace_root(Path(__file__).parent)
    if workspace_root is None:
        print(f"エラー: {WORKSPACE_MARKER} が見つかりません。", file=sys.stderr)
        return 1

    template = workspace_root / TEMPLATE_RELATIVE
    if not template.is_file():
        print(f"エラー: 正本がありません: {TEMPLATE_RELATIVE}", file=sys.stderr)
        return 1

    expected = template.read_text(encoding="utf-8")
    targets = collect_targets(workspace_root)
    if not targets:
        print(f"エラー: {APP_DIR_RELATIVE} 配下に .gitignore が見つかりません。", file=sys.stderr)
        return 1

    differing = [target for target in targets if target.read_text(encoding="utf-8") != expected]

    if args.check:
        for target in differing:
            report_difference(workspace_root, target, expected)
        if differing:
            print(
                f"\n{len(differing)} 件の app で .gitignore が正本と異なります。"
                f"{Path(__file__).name} --write で揃えてください。",
                file=sys.stderr,
            )
            return 1
        print(f"OK: {len(targets)} 件の app の .gitignore は正本と一致しています。")
        return 0

    for target in differing:
        # 改行コードは正本に合わせて LF で書き出す。
        with target.open("w", encoding="utf-8", newline="\n") as handle:
            handle.write(expected)
        print(f"更新: {target.relative_to(workspace_root)}")

    if not differing:
        print(f"変更はありません。{len(targets)} 件の app はすでに正本と一致しています。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
