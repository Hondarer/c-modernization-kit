# Git 基礎

## 概要

Git は分散型バージョン管理システムです。ソースコードの変更履歴を記録し、複数人での並行開発を可能にします。Word ファイルによる手動バージョン管理 (`仕様書_v1_最終版_修正2.docx` のような管理) と比べ、誰がいつどのような変更を加えたかを正確に追跡できます。

このリポジトリはすべての成果物を Git で管理しています。C ソースコード、makefile、テストコード、ドキュメントソースのすべてが Git の追跡対象です。また、`doxyfw`・`docsfw`・`testfw`・`makefw` の 4 つのサブモジュールを含む構成のため、通常の `git clone` に加えてサブモジュールの初期化が必要です。

Git の基本操作を習得することは、このリポジトリを利用するための最初のステップです。コミット・ブランチ・マージの概念を理解すれば、変更の追跡と協調開発ができるようになります。

## 習得目標

- [ ] `git init` / `git clone` でリポジトリを作成・取得できる
- [ ] `git add` / `git commit` で変更を記録できる
- [ ] `git status` / `git log` / `git diff` で状態を確認できる
- [ ] `git branch` / `git checkout` / `git switch` でブランチを作成・切り替えできる
- [ ] `git merge` / `git rebase` でブランチを統合できる
- [ ] `git pull` / `git push` でリモートリポジトリと同期できる
- [ ] `.gitignore` でトラッキング対象外ファイルを設定できる

## 学習マテリアル

### 公式ドキュメント

- [Pro Git (日本語版)](https://git-scm.com/book/ja/v2) - Git の公式書籍。無料でオンライン公開されており、入門から応用まで網羅
  - [第 2 章 Git の基本](https://git-scm.com/book/ja/v2/Git-の基本-Git-リポジトリの取得) - `init`・`clone`・`commit`・`log`
  - [第 3 章 Git のブランチ機能](https://git-scm.com/book/ja/v2/Git-のブランチ機能-ブランチとは) - ブランチとマージ

### チュートリアル・入門

- [サルでも分かる Git 入門](https://www.backlog.com/ja/git-tutorial/) - 図解でわかりやすく解説された日本語チュートリアル (Backlog 提供)

### 日本語コンテンツ

- [GitHub Docs - Git 入門](https://docs.github.com/ja/get-started/getting-started-with-git) - GitHub 公式の Git 入門ガイド (日本語)

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

リポジトリをクローンするには、サブモジュールを含めて取得する必要があります。

```bash
# サブモジュールを含めてクローン
git clone --recurse-submodules <リポジトリURL>

# すでにクローン済みの場合、サブモジュールを初期化
git submodule update --init --recursive
```

`.gitignore` の設定例

```gitignore
# ビルド出力
prod/calc/lib/
prod/calc/bin/
prod/calc.net/lib/
prod/calc.net/bin/

# Doxygen 生成ファイル
docs/doxygen/
xml/
```

### 関連ドキュメント

- [Git サブモジュール](git-submodules.md) - このリポジトリで使用するサブモジュールの詳細
- [GitHub ワークフロー](github-workflow.md) - チーム開発のための PR とレビュー
