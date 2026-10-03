# c-modernization-kit

[Hondarer/c-modernization-kit](https://github.com/Hondarer/c-modernization-kit) の構成、および GitHub Actions を中心とした CI/CD の運用手順をまとめたドキュメント群です。

本フレームワークを別の目的に展開する場合は、共通層 (`app/general`) を残したまま、このディレクトリを削除できます。

## 重要な文書

### CI/CD パイプラインと運用

GitHub Actions による自動ビルド、テスト、および警告対応を行う際に参照します。

- [GitHub Actions](github-actions.md) - ワークフロー構成と CI パイプラインの仕様
- [CI エラーと警告の確認と修正](ci-warning-remediation.md) - CI 失敗時のログ確認、警告対処、および局所検証手順

### リリース手順

リポジトリ群のリリース タグ発行や配布物を管理する際に参照します。

- [リリース タグと GitHub Release](release-workflow.md) - 日付タグの発行と GitHub Release の一括作成手順

## 文書一覧

\toc depth=-1 exclude-basedir=true
