# GitHub ワークフロー

## 概要

GitHub は Git リポジトリをホストするプラットフォームであり、チーム開発を支援する多くの機能を提供します。Pull Request（PR）によるコードレビュー、Issues によるタスク管理、GitHub Actions による自動化が主要な機能です。

このリポジトリは GitHub でホストされており、GitHub Actions による CI/CD パイプラインを使用しています。PRのマージ時に自動ビルド・テスト・ドキュメント公開が実行されます。チームでこのリポジトリを活用するには、GitHub Flow の基本的なワークフローを理解することが重要です。

PR ベースの開発は「レビューなしに main ブランチに直接コミットする」従来の開発スタイルからの転換です。コードレビューをプロセスに組み込むことで、バグの早期発見とナレッジ共有が実現できます。

## 習得目標

- [ ] GitHub アカウントを作成し、リポジトリをフォーク・クローンできる
- [ ] feature ブランチを作成し、変更をプッシュできる
- [ ] Pull Request を作成し、説明を記述できる
- [ ] PR のレビューコメントに応答し、変更を追加できる
- [ ] Issues を作成し、PR とリンクできる
- [ ] GitHub Flow（feature branch → PR → review → merge）を説明できる
- [ ] PR がマージされたときに CI が実行されることを確認できる

## 学習マテリアル

### 公式ドキュメント

- [GitHub Docs（日本語）](https://docs.github.com/ja) — GitHub の公式ドキュメント
  - [GitHub Flow](https://docs.github.com/ja/get-started/using-github/github-flow) — GitHub 推奨の開発ワークフロー
  - [Pull Request の作成](https://docs.github.com/ja/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request) — PR の基本操作
  - [Issues の作成](https://docs.github.com/ja/issues/tracking-your-work-with-issues/creating-an-issue) — タスク管理の基本

### チュートリアル・入門

- [GitHub Skills](https://skills.github.com/) — GitHub 公式のハンズオン学習コース（英語）
  - Introduction to GitHub、Review Pull Requests など対話型コースが無料で利用可能

### 日本語コンテンツ

- [サルでも分かるGit入門 — プルリクエスト](https://www.backlog.com/ja/git-tutorial/) — 図解によるプルリクエストの説明

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

典型的な開発ワークフロー:

```bash
# 1. 作業ブランチを作成
git switch -c feature/add-new-test

# 2. 変更をコミット
git add test/src/calc/libcalcbaseTest/addTest/addTest.cpp
git commit -m "テスト: add 関数の境界値テストを追加"

# 3. リモートにプッシュ
git push -u origin feature/add-new-test

# 4. GitHub 上で Pull Request を作成
```

PR のタイトル・説明の例:

```
タイトル: feat: divide 関数のゼロ除算テストを追加

説明:
## 変更内容
- test/src/calc/libcalcbaseTest/divideTest/ にゼロ除算のテストケースを追加
- 期待される戻り値 CALC_ERR_DIVIDE_BY_ZERO を検証

## 関連 Issues
Fixes #12
```

### 関連ドキュメント

- [GitHub Actions 設定](../../github-actions.md) — このリポジトリの CI/CD パイプライン詳細
- [GitHub Actions（スキルガイド）](../06-ci-cd/github-actions.md) — GitHub Actions の学習マテリアル
