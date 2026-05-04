# AGENTS.md

## 重要事項

- 自動ステージング、コミット禁止。指示があるまでステージング、コミットは行わないこと。
- 思考の断片は英語でもよいが、ユーザーに気づきを与えたり報告する際は日本語を用いること。
- コミットメッセージは日本語で。
- 対話にあたって、寄せる、詰める、閉じる、乗せる、入れる、整える、潰す、片付ける、仕上げる などの言い回しは避け、正確な表現を心がける。
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

## app/ ライブラリのインクルード規則

`app/` 配下の C ライブラリは、以下の 3 層構造でヘッダーを管理する。

| ディレクトリ | 用途 | インクルード形式 |
|---|---|---|
| `prod/include/` | 公開 API（ライブラリ利用者向け） | `<lib/file.h>` |
| `prod/include_internal/` | ライブラリ内部共有ヘッダー（`.c` をまたいで参照） | `<lib/subdir/file.h>` |
| `prod/libsrc/` | ソースファイル（`.c`）のみ。ヘッダーは置かない | — |

### `_internal` サフィックスの付与ルール

- 同名の公開ヘッダーが存在する場合のみ `_internal` を付与する（例: `console.h` が公開にあるため `console_internal.h`）
- 対応する公開ヘッダーが存在しない場合はサフィックスなし（例: `path_format.h`）

### `makepart.mk` の INCDIR ルール

- `INCDIR` に追加するのは `include/` と `include_internal/` のみとする
- `libsrc/` のサブディレクトリは `INCDIR` に追加しない

## 注意点

- 作成するファイルは Linux (GCC)、Windows (MSVC) のクロスプラットフォームで動作することを念頭に置いて作業すること。
- Windows では GNU Make が POSIX シェルで動く前提です。必要に応じて `Start-VSCode-With-Env.cmd` から環境を整えること。
- ドキュメント生成と公開は `framework/doxyfw` と `framework/docsfw` の連携で成り立つため、出力パスやスクリプト名を変更する際は両方を確認すること。
- テスト関連の変更では `framework/testfw` とその配下の `gtest` サブモジュールの役割を理解し区別すること。
- full make には時間がかかる。そのため、make は修正箇所に対して最小限の実施で確認し、問題が外部にあると判断した場合に限り、ユーザーに問い合わせのうえで全体の make clean, make を行うこと。
