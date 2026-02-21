# バージョン管理 (ステップ 2 - 必須基盤)

Git を使ったバージョン管理はすべての現代的な開発の基盤です。
チームでコードを共有・管理するために、最初に習得すべきスキルをまとめています。

## スキルガイド一覧

| スキルガイド          | 内容                                              |
|-----------------------|---------------------------------------------------|
| [Git 基礎](git-basics.md)                 | init/clone/commit/branch/merge の基本操作 |
| [Git サブモジュール](git-submodules.md)   | サブモジュールの追加・更新・運用方法              |
| [GitHub ワークフロー](github-workflow.md) | PR・Issues・コードレビューによるチーム開発        |

## このリポジトリとの関連

- `.gitmodules` - サブモジュール構成 (`doxyfw` / `docsfw` / `testfw` / `makefw`)
- `git submodule update --init --recursive` - サブモジュールの初期化

## 次のステップ

バージョン管理を習得したら、[ステップ 3 - ビルド理解](../03-c-language/README.md) に進んでください。
