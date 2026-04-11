# update_template_makefiles.py — 設計ノート

## 背景

### makefw フレームワークとテンプレート makefile

本プロジェクトは `makefw` サブモジュールを利用したビルドシステムを採用している。
ビルド対象 (ライブラリ・実行ファイル・テスト) が置かれた **最終階層ディレクトリ** には、
`framework/makefw/makefiles/__template.mk` と同一内容の `makefile` を配置する規則になっている。

```makefile
# makefile テンプレート
# すべての最終階層 makefile で使用する標準テンプレート
# 本ファイルの編集は禁止する。makepart.mk を作成して拡張・カスタマイズすること。

# ワークスペースのディレクトリ
find-up = \
    ...
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

include $(WORKSPACE_FOLDER)/framework/makefw/makefiles/prepare.mk

##### makepart.mk の内容は、このタイミングで処理される #####

include $(WORKSPACE_FOLDER)/framework/makefw/makefiles/makemain.mk
```

テンプレート自体にビルドロジックは含まない。
ビルド設定は各ディレクトリの `makepart.mk` に記述し、テンプレートが `prepare.mk` 経由で
読み込む仕組みになっている。これにより、ビルドロジックを `makepart.mk` に集約し、
テンプレートを汚さずにカスタマイズできる。

### 2 種類の makefile

ワークスペース配下には、用途の異なる 2 種類の `makefile` が存在する。

| 種別 | 特徴 | 例 |
|------|------|----|
| **テンプレート由来** | 先頭行 `# makefile テンプレート`。最終階層に配置。 | `prod/calc/libsrc/calcbase/makefile` |
| **手書き (SUBDIRS 形式)** | `SUBDIRS = \` または `SUBDIRS = $(wildcard */)` で始まる。中間ディレクトリに配置。再帰 make でサブディレクトリを呼び出す。 | `prod/calc/makefile` |

テンプレート由来の makefile は内容を変更してはならず、
`framework/makefw/makefiles/__template.mk` と常に同一であるべきである。

### テンプレートのドリフト問題

`makefw` サブモジュールが更新されると `__template.mk` の内容が変わる。
しかしテンプレート由来の各 `makefile` は個別ファイルとして管理されているため、
手動で更新しなければ古いまま残り続ける (テンプレートのドリフト)。

Linux ではシンボリック リンクを利用すればよいが、このレポジトリは Windows との相互運用も行うため、
実体の配置が必要となっており、この問題を解消する手段が求められる。

`update_template_makefiles.py` はこの問題を解消するために作成された。

## テンプレート判定の仕組み

### 先頭行マーカーによる識別

スクリプトはファイルの **先頭行が `# makefile テンプレート` で始まるかどうか** のみで
テンプレート由来かどうかを判定する。

```python
def is_template_makefile(path: Path) -> bool:
    with path.open(encoding="utf-8") as f:
        first_line = f.readline()
    return first_line.startswith("# makefile テンプレート")
```

### 対象外ファイル

先頭行が `# makefile テンプレート` で始まらないファイルは先頭行が異なるため対象外となる。

## スクリプトの動作概要

### アルゴリズム

1. スクリプトファイルの場所から上方向に `.workspaceRoot` を探し、
   ワークスペースルートを特定する。
2. `framework/makefw/makefiles/__template.mk` の内容を読み込む。
3. ワークスペース配下の `makefile` を再帰的に列挙し、アルファベット順に処理する。
4. 各 `makefile` について先頭行を検査し、テンプレートでなければスキップ。
5. テンプレートであれば現在の内容と `__template.mk` を比較する。
   - **同一** → `[スキップ]` と表示して次へ。
   - **異なる** → `--dry-run` なら `[対象]` と表示のみ。通常実行なら内容を置き換えて `[更新]` と表示。

### `--dry-run` モード

`--dry-run` オプションを付けると、ファイルへの書き込みを一切行わず、
更新されるはずのファイルの一覧だけを表示する。実際に更新する前の確認に使う。

```
[対象]     prod/calc/libsrc/calcbase/makefile
[スキップ] test/calc/src/main/addTest/makefile (既に最新)
...
合計: N 件が更新対象, M 件はスキップ予定 (--dry-run モード)
```

## 使い方

```bash
# 更新対象の確認のみ（変更なし）
python3 bin/update_template_makefiles.py --dry-run

# 実際に置き換え
python3 bin/update_template_makefiles.py
```
