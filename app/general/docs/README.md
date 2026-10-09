# general

C モダナイゼーション フレームワークを利用するワークスペース全体で共有する開発規範、設計指針、および運用手順をまとめたドキュメント群です。

C/C++ コーディング規約やテスト方針、AI エージェントとスキルの設計、ビルド環境の共通ルールを提供します。

## 重要な文書

### 開発とコーディングの規範

ソース コードの作成や機能仕様を記述する際に参照します。

- [コーディング規範](coding-guideline.md) - C/C++ 言語の命名規則、設計、移植性の指針
- [Python の文字コード指定規範](python-text-encoding-guideline.md) - ファイルと子プロセスのテキスト入出力、既定値への依存を検出する検証
- [機能仕様の記載規範](functional-spec-guideline.md) - 機能仕様書の構成と記載要件
- [Doxygen コメント](doxygen-comment-guideline.md) - app 共通の Doxygen コメント配置と契約記述

### ビルドとテストの指針

アプリケーションのビルド構成やテストを作成する際に参照します。

- [ビルド設計](build-design.md) - ライブラリ構成と依存関係の基本設計
- [単体テストの作成](testing-tutorial.md) - app 向けの単体テスト作成チュートリアル
- [期待確認の件数対応の棚卸し](test-confirmation-count-inventory.md) - 既存テストの回数指定と確認結果
- [共有ライブラリのモック](shared-library-mock-guideline.md) - 第三者共有ライブラリのモック作成指針
- [VS Code の環境変数](vscode-variables.md) - デバッグや実行に必要な環境変数の構成

### エージェントとワークスペース運用

AI エージェントの活用やワークスペースの管理を行う際に参照します。

- [ワークスペース作業ガイド](workspace-agent-workflow.md) - ビルド、テスト、CI の共通作業手順
- [AGENTS.md とスキルの設計指針](agents-and-skills-guideline.md) - AI エージェントの指示ファイルとスキルの設計
- [スキル同期](skill-sync.md) - 各フレームワークのスキルをワークスペースへ同期する手順
- [Markdown 一括スタイル確認](markdown-style-bulk-check.md) - ドキュメント全体の表記確認と一括整形手順

## 文書一覧

\toc depth=-1 exclude-basedir=true
