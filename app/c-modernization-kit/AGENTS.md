# AGENTS.md

## 対象

この app は、このリポジトリが採用する app の組み合わせと、リポジトリ固有の CI/CD を説明する文書とスキルを管理します。

## 作業別の参照先

- 対象の目的を確認する場合は [README.md](README.md)
- CI を変更する場合は [GitHub Actions](docs/github-actions.md) の該当ジョブ
- CI の失敗または警告を修正する場合は `fix-ci-warnings` スキルと [CI エラーと警告の確認と修正](docs/ci-warning-remediation.md)

## 注意点

- 全 app に共通する規範やスキルは `app/general` に配置してください。
- 個別 app または framework の実装規則を、この app の文書へ複製しないでください。
- CI の変更では GitHub Actions と Jenkins の同等性を確認してください。
