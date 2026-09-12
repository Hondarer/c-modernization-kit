# GitHub Actions

## 概要

GitHub Actions は GitHub に組み込まれた CI/CD (継続的インテグレーション/継続的デリバリー) プラットフォームです。リポジトリへのプッシュや PR の作成をトリガーに、自動ビルド・テスト・デプロイなどのワークフローを実行できます。YAML ファイルでワークフローを定義し、`.github/workflows/` ディレクトリに配置します。

対象ワークスペースは GitHub Actions を使用して、C コードのビルド・テスト・カバレッジ計測・ドキュメント生成・GitHub Pages へのデプロイを自動化しています。PR のマージ前に自動テストを実行することで、コードの品質を継続的に維持できます。

GitHub Actions のワークフローを理解することで、CI の実行状況の確認、失敗時の原因調査、新たな自動化の追加ができるようになります。

## 習得目標

- [ ] ワークフロー YAML ファイルの基本構造 (`on`・`jobs`・`steps`) を読み取ることができます。
- [ ] `push`・`pull_request` トリガーを設定できます。
- [ ] `uses` でサード パーティー Action を使用できます (`actions/checkout` など)。
- [ ] `run` でシェル コマンドを実行するステップを記述できます。
- [ ] GitHub Actions の実行ログを確認して失敗原因を調査できます。
- [ ] `env` 環境変数と `secrets` を使用できます。
- [ ] matrix build (複数 OS・バージョンの並行テスト) の概念を理解できます。

## 学習マテリアル

### 公式ドキュメント

- [GitHub Actions ドキュメント (日本語)](https://docs.github.com/ja/actions) - GitHub Actions の公式ドキュメント
    - [クイック スタート](https://docs.github.com/ja/actions/writing-workflows/quickstart) - 最初のワークフロー作成
    - [ワークフロー構文リファレンス](https://docs.github.com/ja/actions/writing-workflows/workflow-syntax-for-github-actions) - YAML の完全リファレンス
    - [トリガー イベント](https://docs.github.com/ja/actions/writing-workflows/choosing-when-your-workflow-runs/events-that-trigger-workflows) - `on` で使えるイベント一覧

### チュートリアル・入門

- [GitHub Skills - GitHub Actions 入門](https://skills.github.com/) - 対話型ハンズオン コース (英語)

## 対象ワークスペースとの関連

### 使用箇所 (具体的なファイル・コマンド)

対象ワークスペースのワークフロー (`.github/workflows/` 配下) が担当する処理:

| ジョブ | 内容 |
|-------|------|
| build | C コードのビルド (Linux / Windows) |
| test | Google Test の実行・結果収集 |
| coverage | gcov / lcov によるカバレッジ計測 |
| docs | Doxygen・Pandoc によるドキュメント生成 |
| deploy | GitHub Pages へのドキュメント公開 |

Table: GitHub Actions ジョブ一覧

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
      - name: チェックアウト(サブモジュール含む)
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: C コードのビルド
        run: make

      - name: テストの実行
        run: make test
```

対象ワークスペースの実ワークフローでは、全ジョブ共通の `env` で `MAKEFW_HOME`、`DOCSFW_HOME`、`DOXYFW_HOME`、`TESTFW_HOME` を設定します。  
`MAKEFW_HOME` は `make` 系ターゲットで必須で、未設定だと `MAKEFW_HOME is required. Export MAKEFW_HOME before running make` で停止します。`make docs` は `DOCSFW_HOME`、`make doxy` は `DOXYFW_HOME`、`make` / `make test` は `TESTFW_HOME` を参照します。  
具体的なジョブ、トリガー、成果物は、展開先の CI/CD 仕様書に記録します。

### 関連ドキュメント

- [GitHub Pages (スキル ガイド)](github-pages.md) - 生成ドキュメントの公開
- [GitHub ワークフロー (スキル ガイド)](../02-version-control/github-workflow.md) - PR ベースの開発フロー
