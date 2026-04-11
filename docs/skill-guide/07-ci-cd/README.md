# CI/CD (ステップ 5 - 自動化・拡張)

CI/CD ツールを使ったビルド・テスト・ドキュメント生成の自動化と、静的サイトへのドキュメント公開を学びます。
GitHub Actions によるクラウド CI/CD から、Jenkins を使ったオンプレミス構成まで、環境に応じた自動化手法を習得します。

## スキルガイド一覧

| スキルガイド     | 内容                                           |
|------------------|------------------------------------------------|
| [GitHub Actions](github-actions.md) | 自動ビルド・テスト・デプロイのワークフロー定義 |
| [GitHub Pages](github-pages.md)     | 生成ドキュメントの静的サイト公開               |
| [Jenkins](jenkins.md)               | Oracle Linux 8 への Jenkins 導入、ビルドジョブ構成、ドキュメント公開 |
| [Podman](podman.md)                 | rootless Podman と Oracle Linux 開発コンテナの利用 |

Table: スキルガイド一覧

## このリポジトリとの関連

- `.github/workflows/` - GitHub Actions ワークフロー定義ファイル
- `docs/` - GitHub Pages で公開されるドキュメント出力先
- `docs-src/github-actions.md` - このリポジトリの CI/CD 設定の詳細解説

## 次のステップ

CI/CD を習得したら、[開発環境・.NET 連携](../08-dev-environment/README.md) に進んでください。
