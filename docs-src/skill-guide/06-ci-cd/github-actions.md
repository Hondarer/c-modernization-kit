# GitHub Actions

## 概要

GitHub Actions は GitHub に組み込まれた CI/CD（継続的インテグレーション/継続的デリバリー）プラットフォームです。リポジトリへのプッシュや PR の作成をトリガーに、自動ビルド・テスト・デプロイなどのワークフローを実行できます。YAML ファイルでワークフローを定義し、`.github/workflows/` ディレクトリに配置します。

このリポジトリは GitHub Actions を使用して、C コードのビルド・テスト・カバレッジ計測・ドキュメント生成・GitHub Pages へのデプロイを自動化しています。PR のマージ前に自動テストを実行することで、コードの品質を継続的に維持できます。

GitHub Actions のワークフローを理解することで、CI の実行状況の確認、失敗時の原因調査、新たな自動化の追加ができるようになります。

## 習得目標

- [ ] ワークフロー YAML ファイルの基本構造（`on`・`jobs`・`steps`）を読み取れる
- [ ] `push`・`pull_request` トリガーを設定できる
- [ ] `uses` でサードパーティ Action を使用できる（`actions/checkout` など）
- [ ] `run` でシェルコマンドを実行するステップを書ける
- [ ] GitHub Actions の実行ログを確認して失敗原因を調査できる
- [ ] `env` 環境変数と `secrets` を使用できる
- [ ] matrix build（複数 OS・バージョンの並行テスト）の概念を理解できる

## 学習マテリアル

### 公式ドキュメント

- [GitHub Actions ドキュメント（日本語）](https://docs.github.com/ja/actions) — GitHub Actions の公式ドキュメント
  - [クイックスタート](https://docs.github.com/ja/actions/writing-workflows/quickstart) — 最初のワークフロー作成
  - [ワークフロー構文リファレンス](https://docs.github.com/ja/actions/writing-workflows/workflow-syntax-for-github-actions) — YAML の完全リファレンス
  - [トリガーイベント](https://docs.github.com/ja/actions/writing-workflows/choosing-when-your-workflow-runs/events-that-trigger-workflows) — `on` で使えるイベント一覧

### チュートリアル・入門

- [GitHub Skills — GitHub Actions 入門](https://skills.github.com/) — 対話型ハンズオンコース（英語）

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

このリポジトリのワークフロー（`.github/workflows/` 配下）が担当する処理:

| ジョブ | 内容 |
|-------|------|
| build | C コードのビルド（Linux / Windows） |
| test | Google Test の実行・結果収集 |
| coverage | gcov / lcov によるカバレッジ計測 |
| docs | Doxygen・Pandoc によるドキュメント生成 |
| deploy | GitHub Pages へのドキュメント公開 |

ワークフロー YAML の基本構造:

```yaml
name: CI

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - name: チェックアウト（サブモジュール含む）
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: C コードのビルド
        run: make

      - name: テストの実行
        run: make test
```

### 関連ドキュメント

- [GitHub Actions 設定](../../github-actions.md) — このリポジトリの CI/CD 設定の詳細
- [GitHub Pages（スキルガイド）](github-pages.md) — 生成ドキュメントの公開
- [GitHub ワークフロー（スキルガイド）](../01-version-control/github-workflow.md) — PR ベースの開発フロー
