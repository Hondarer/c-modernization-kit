# AGENTS.md

## 対象

この app は、すべての app に共通する規範、設計、運用手順、チュートリアル、スキルを管理します。

## 参照先

作業に関係する文書の該当節を参照してください。

- [README.md](README.md)
- [文書一覧](docs/README.md)
- [AGENTS とスキルの設計指針](docs/agents-and-skills-guideline.md)
- ビルドや構成の運用を変更する場合は [ワークスペース作業ガイド](docs/workspace-agent-workflow.md) の該当節
- スキルの配置や同期を変更する場合は [スキル同期](docs/skill-sync.md)

## 注意点

- 個別 app 固有の設定、パス、例外を正本として記載しないでください。
- framework の詳細仕様は複製せず、framework 側の正本を参照してください。
- 全 app 向けのスキルは `.agents/skills/` に配置し、詳細を通常文書へ集約してください。
