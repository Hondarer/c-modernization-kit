#!/usr/bin/env python3
"""
update_template_makefiles.py - テンプレート由来の makefile を最新版に更新する。

【対象】
  prod/ と test/ 配下の makefile で、先頭行が
  "# makefile テンプレート" で始まるファイル。

  先頭行がそれ以外のファイル (手書き makefile 等) は対象外。

【使い方】
  python3 bin/update_template_makefiles.py [--dry-run]

  --dry-run  ファイルを変更せず、更新対象のリストのみ表示する。
"""

import sys
import argparse
from pathlib import Path


TEMPLATE_HEADER = "# makefile テンプレート"
TEMPLATE_REL_PATH = "framework/makefw/makefiles/__template.mk"
SEARCH_DIRS = ["prod", "test"]


def find_workspace_root(start: Path) -> Path:
    """スクリプトの配置場所から上方向に .workspaceRoot を探してルートを返す。"""
    current = start.resolve()
    while True:
        if (current / ".workspaceRoot").exists():
            return current
        parent = current.parent
        if parent == current:
            raise RuntimeError(".workspaceRoot が見つかりません。")
        current = parent


def is_template_makefile(path: Path) -> bool:
    """ファイルの先頭行が TEMPLATE_HEADER で始まるか判定する。"""
    try:
        with path.open(encoding="utf-8") as f:
            first_line = f.readline()
        return first_line.startswith(TEMPLATE_HEADER)
    except (OSError, UnicodeDecodeError):
        return False


def main():
    parser = argparse.ArgumentParser(
        description="テンプレート由来の makefile を最新版に更新する。"
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="ファイルを変更せず、更新対象のリストのみ表示する。",
    )
    args = parser.parse_args()

    script_dir = Path(__file__).parent
    workspace = find_workspace_root(script_dir)
    template_path = workspace / TEMPLATE_REL_PATH

    if not template_path.exists():
        print(f"エラー: テンプレートファイルが見つかりません: {template_path}", file=sys.stderr)
        sys.exit(1)

    template_content = template_path.read_text(encoding="utf-8")

    updated = 0
    skipped = 0

    for search_dir_name in SEARCH_DIRS:
        search_dir = workspace / search_dir_name
        if not search_dir.is_dir():
            continue
        for makefile in sorted(search_dir.rglob("makefile")):
            if not is_template_makefile(makefile):
                continue
            rel = makefile.relative_to(workspace)
            current_content = makefile.read_text(encoding="utf-8")
            if current_content == template_content:
                print(f"[スキップ] {rel} (既に最新)")
                skipped += 1
            else:
                if args.dry_run:
                    print(f"[対象]     {rel}")
                else:
                    makefile.write_text(template_content, encoding="utf-8")
                    print(f"[更新]     {rel}")
                updated += 1

    if args.dry_run:
        print(f"\n合計: {updated} 件が更新対象, {skipped} 件はスキップ予定 (--dry-run モード)")
    else:
        print(f"\n完了: {updated} 件更新, {skipped} 件スキップ")


if __name__ == "__main__":
    main()
