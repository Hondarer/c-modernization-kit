# Git サブモジュール

## 概要

Git サブモジュールは、ひとつの Git リポジトリの中に別の Git リポジトリを組み込む機能です。共通のフレームワークやライブラリを複数のプロジェクトで共有しつつ、それぞれのバージョンを独立して管理できます。

このリポジトリは `doxyfw`・`docsfw`・`testfw`・`makefw` の 4 つのサブモジュールを使用しています。これらはドキュメント生成・テスト・ビルドのフレームワークとして機能し、メインリポジトリから独立したリポジトリとして管理されています。サブモジュールを理解することで、フレームワークの更新と本体の更新を分離して扱えるようになります。

通常の `git clone` ではサブモジュールの内容は取得されません。サブモジュールを含むリポジトリを正しく扱うには、専用のコマンドが必要です。

## 習得目標

- [ ] サブモジュールとは何か、使う理由を説明できる
- [ ] `git clone --recurse-submodules` でサブモジュールごとクローンできる
- [ ] `git submodule update --init --recursive` でサブモジュールを初期化できる
- [ ] `git submodule update --remote` でサブモジュールを最新版に更新できる
- [ ] `.gitmodules` ファイルの内容を読み、サブモジュールの設定を理解できる
- [ ] サブモジュールの変更をメインリポジトリにコミットできる

## 学習マテリアル

### 公式ドキュメント

- [Pro Git - Git ツール: サブモジュール (日本語)](https://git-scm.com/book/ja/v2/Git-ツール-サブモジュール) - サブモジュールの公式解説。基本操作から注意点まで詳細に説明
- [GitHub Docs - サブモジュールについて](https://docs.github.com/ja/get-started/getting-started-with-git/about-git-subtree-merges) - GitHub 公式の補足説明

### チュートリアル・入門

- [サルでも分かるGit入門 - サブモジュール](https://www.backlog.com/ja/git-tutorial/) - 図解入りの入門説明

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

このリポジトリの `.gitmodules` に定義されたサブモジュールは以下の通りです。

| サブモジュール | パス      | 役割                                   |
|----------------|-----------|----------------------------------------|
| `doxyfw`       | `doxyfw/` | Doxygen ドキュメント生成フレームワーク |
| `docsfw`       | `docsfw/` | Markdown 発行フレームワーク (Pandoc)   |
| `testfw`       | `testfw/` | Google Test テストフレームワーク       |
| `makefw`       | `makefw/` | Make ビルドフレームワーク              |

Table: サブモジュール一覧

基本的な操作コマンド:

```bash
# 初回クローン (サブモジュールを含む)
git clone --recurse-submodules <URL>

# クローン後にサブモジュールを初期化
git submodule update --init --recursive

# サブモジュールをリモートの最新版に更新
git submodule update --remote

# サブモジュールの状態確認
git submodule status
```

### 関連ドキュメント

- [Git 基礎](git-basics.md) - Git の基本操作
- [GitHub ワークフロー](github-workflow.md) - チーム開発のワークフロー
