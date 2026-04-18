# AGENTS.md

## 重要事項

- 自動ステージング、コミット禁止。指示があるまでステージング、コミットは行わないこと。
- 思考の断片は英語でもよいが、ユーザーに気づきを与えたり報告する際は日本語を用いること。
- コミットメッセージは日本語で。
- サブモジュール配下のファイルを参照、編集する場合は、サブモジュール配下の AGENTS.md もあわせて参照すること。

## リポジトリ概要

レガシー C コードのモダナイゼーションを題材に、ビルド、テスト、Doxygen、Markdown 発行、.NET 呼び出し例をまとめた統合ワークスペースです。

主要な git ルートは以下です。

- `framework/doxyfw` - Doxygen と Doxybook2 を使うドキュメント生成フレームワーク
- `framework/docsfw` - Pandoc ベースの Markdown 発行フレームワーク
- `framework/testfw` - Google Test ベースのテストフレームワーク
- `framework/makefw` - C/C++ と .NET 向けの Make テンプレート群

## 作業時の入口

- `makefile` - ルートの入口。`make`、`make test`、`make doxy`、`make docs`、`make clean` を提供する
- `app/calc/` - C のサンプルアプリ、ライブラリ、テスト、Doxygen 設定
- `app/calc.net/` - .NET ラッパー、アプリ、テスト、ソリューション
- `Directory.Build.props` - .NET 共通設定
- `framework/*` - 各フレームワークの独立した git ルート。変更前に各ルートの `AGENTS.md` と `README.md` を確認する
- `Start-VSCode-With-Env.cmd` - Windows で GNU Make と MSVC の環境を整えて VS Code を起動する

## 主要コマンド

```bash
make
make test
make doxy
make docs
make clean
```

## 注意点

- Windows では GNU Make が POSIX シェルで動く前提です。必要に応じて `Start-VSCode-With-Env.cmd` から環境を整えること。
- ドキュメント生成と公開は `framework/doxyfw` と `framework/docsfw` の連携で成り立つため、出力パスやスクリプト名を変更する際は両方を確認すること。
- テスト関連の変更では `framework/testfw` とその配下の `gtest` サブモジュールの役割を混同しないこと。
