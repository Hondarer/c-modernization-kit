# AGENTS.md

## 対象

この app は、統合ワークスペースの構成、CI/CD、スキル同期に固有の文書とスキルを管理します。

## 作業別の参照先

- 対象の目的を確認する場合は [README.md](README.md)
- ビルドや構成の運用を変更する場合は [ワークスペース作業ガイド](docs/workspace-agent-workflow.md) の該当節
- CI を変更する場合は [GitHub Actions](docs/github-actions.md) の該当ジョブ
- CI の失敗または警告を修正する場合は `fix-ci-warnings` スキルと [CI エラーと警告の確認と修正](docs/ci-warning-remediation.md)
- スキルの配置や同期を変更する場合は [スキル同期](docs/skill-sync.md)

## 注意点

- 全 app に共通する規範やスキルは `app/general` に配置してください。
- 個別 app または framework の実装規則を、この app の文書へ複製しないでください。
- CI の変更では GitHub Actions と Jenkins の同等性を確認してください。
