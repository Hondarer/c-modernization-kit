# general

このディレクトリは、すべての app に共通する規範、設計、運用手順、スキルを管理します。

統合ターミナルから直接使う共通コマンドは `bin/`、環境設定、スキル同期、検査などの内部処理は `bin_internal/`、それらのテストは `bin_test/` に配置します。

- [作業規則](AGENTS.md)
- [文書一覧](docs/README.md)
- [AGENTS とスキルの設計指針](docs/agents-and-skills-guideline.md)
- [コーディング規範](docs/coding-guideline.md)
- [テスト チュートリアル](docs/testing-tutorial.md)
- [VS Code 環境変数](docs/vscode-variables.md)
- [ワークスペース作業ガイド](docs/workspace-agent-workflow.md)
- [スキル同期](docs/skill-sync.md)

個別 app 固有の情報は、その app の `AGENTS.md`、README.md、`docs/` に記載します。  
さらに狭い範囲だけに適用する規則は、対象ディレクトリの `AGENTS.md` に記載できます。
