# AGENTS.md

## 重要事項

- 自動ステージング、コミット禁止。指示があるまでステージング、コミットは行わないこと。  
  エージェント作業にあわせてユーザーがステージングする場合はある。エージェントは Git の状態参照のみが許可されており、自動アンステージも禁止する。  
  作業の過程でスタッシュする場合もユーザーの許可を得ること。
- 思考の断片は英語でもよいが、ユーザーに気づきを与えたり報告する際は日本語を用いること。
- コミット メッセージは日本語で。
- 対話にあたって、寄せる、詰める、閉じる、乗せる、入れる、整える、潰す、片付ける、仕上げる などの言い回しは避け、正確な表現を心がける。
- サブモジュール配下のファイルを参照、編集する場合は、サブモジュール配下の AGENTS.md もあわせて参照すること。
- `app/{サブフォルダー}` 配下のファイルを参照、編集する場合は、`app/{サブフォルダー}` 配下の AGENTS.md もあわせて参照すること。  
  `app/{サブフォルダー}` はサブモジュールの場合もあれば通常のサブフォルダーの場合もあるが、等しく `app/` 以下の別カテゴリのソース ファイル群として扱う。
- `app/{サブフォルダー}` 配下のコードでは `goto` を使用しない。既存コードを変更する際に `goto` があった場合は、`goto` の除去を行う。
- `app/{サブフォルダー}` 配下のコードでは三項演算子を使用しない。既存コードを変更する際に三項演算子があった場合は、三項演算子の除去を行う。

## リポジトリ概要

レガシ C コードのモダナイゼーションを題材に、ビルド、テスト、Doxygen、Markdown 発行、.NET 呼び出し例をまとめた統合ワークスペースです。

主要な git ルートは以下です。

- `framework/doxyfw` - Doxygen と Doxybook2 を使うドキュメント生成フレームワーク
- `framework/docsfw` - Pandoc ベースの Markdown 発行フレームワーク
- `framework/testfw` - Google Test ベースのテスト フレームワーク
- `framework/makefw` - C/C++ と .NET 向けの Make テンプレート群

## 作業時の入口

- `makefile` - ルートの入口。`make`、`make test`、`make doxy`、`make docs`、`make clean` を提供する
- `app/calc/` - C のサンプル アプリ、ライブラリ、テスト、Doxygen 設定
- `app/calc.net/` - .NET ラッパー、アプリ、テスト、ソリューション
- `Directory.Build.props` - .NET 共通設定
- `framework/*` - 各フレームワークの独立した git ルート。変更前に各ルートの `AGENTS.md` と `README.md` を確認する
- `Start-VSCode-With-Env.cmd` - Windows で GNU Make と MSVC の環境を整えて VS Code を起動する

## make ファイル関連の指針

- makepart.mk, makechild.mk, makelocal.mk など、make ファイルの断片については、空ファイルは不要。  
  see: `framework/makefw/docs/makeparts.md`

## make コマンド実行の指針

**フル ビルドは時間がかかる。以下の指針に従うこと:**

1. **修正箇所の特定** - どのモジュール (`app/` 配下のサブディレクトリ) を変更したかを明確にする
2. **局所的なテスト** - 変更したモジュール配下で `cd app/MODULE && make test` を実行し、そのモジュールのみビルド・テストする
3. **clean + 再 make は高コスト** - make clean が必要になるのはファイル パスの変更などが伴う場合だけであり、依存ファイルの内容変更であれば make だけで事足りるため、むやみに make clean しない。
4. **全体テストが必要な場合** - 以下の場合は全体テストをユーザーに相談してから実行する:
   - ローカル テストで外部依存エラーが検出された場合
   - 複数モジュールにまたがる変更をした場合
   - ライブラリ層 (framework/) を変更した場合

また、Windows において、make 終了時にプロセス終了を検知できないケースがある。出力を定期的に確認し、make 完了を検出すること。

## 主要コマンド

```bash
make
make test
make doxy
make cleandocs
make docs
make clean
make skills
```

## Markdown 記述の注意点

ドキュメント (`.md`) 内でファイル ツリーや階層構造を記述するとき、罫線文字 (`└` `├` `─` など、Unicode Ambiguous) は使用しない。日本語フォントはこれらに全角グリフを割り当てるため、コンソールやプレビューでインデントがずれる。ツリー構造には ASCII 記号 (`+` `-` `|`) を使うこと。  
背景: [east-asian-ambiguous-width.md](framework/docsfw/docs/east-asian-ambiguous-width.md)

## ソース ファイル作成時の注意点

新規ファイル作成時は `clang-format`、変更時は `git-clang-format` を利用してソースの整形を行うこと。  
この際、ネストの整形によって Doxygen コメントの字下げが崩れることがあるため、整形後にはコメントの字下げチェックも実施すること。

コーディング規範 (整数型の選択、関数引数の異常入力対応など) は [コーディング規範](docs/c-modernization-kit/coding-guideline.md) に従うこと。

## app/ ライブラリのインクルード規則

`app/{サブフォルダ}` 配下の C ライブラリは、以下の 3 層構造でヘッダーを管理する。

| ディレクトリ | 用途 | インクルード形式 |
|---|---|---|
| `prod/include/` | 公開 API (ライブラリ利用者向け) | `<lib/file.h>` |
| `prod/include_internal/` | ライブラリ内部共有ヘッダー (`.c` をまたいで参照) | `<lib/subdir/file.h>` |
| `prod/libsrc/` | ソース ファイル (`.c`) のみ。ヘッダーは置かない | — |

### _internal サフィックスの付与ルール

- 同名の公開ヘッダーが存在する場合のみ `_internal` を付与する (例: `console.h` が公開にあるため `console_internal.h`)
- 対応する公開ヘッダーが存在しない場合はサフィックスなし (例: `path_format.h`)

### makepart.mk の INCDIR ルール

- `INCDIR` に追加するのは `include/` と `include_internal/` のみとする
- `libsrc/` のサブディレクトリは `INCDIR` に追加しない

## 注意点

- 作成するファイルは Linux (GCC)、Windows (MSVC) のクロスプラットフォームで動作することを念頭に置いて作業すること。
- Windows では GNU Make が POSIX シェルで動く前提です。必要に応じて `Start-VSCode-With-Env.cmd` から環境を整えること。
- ドキュメント生成と公開は `framework/doxyfw` と `framework/docsfw` の連携で成り立つため、出力パスやスクリプト名を変更する際は両方を確認すること。
- テスト関連の変更では `framework/testfw` とその配下の `gtest` サブモジュールの役割を理解し区別すること。
- `bin/` 配下の Python スクリプトで日本語を出力するときは、モジュール レベルに以下を追加すること。  
  Windows のデフォルト `sys.stdout.encoding` は `cp932` であり、出力が文字化けする。

  ```python
  sys.stdout.reconfigure(encoding="utf-8")
  sys.stderr.reconfigure(encoding="utf-8")
  ```

- `TEST_SRCS` / `ADD_SRCS` に指定したソース ファイルは、`make test` 時にビルド ディレクトリへシンボリック リンクまたはコピーとして取り込まれる。ビルド ディレクトリ内のファイルを直接変更しても次回 `make test` で上書きされるため、`prod/` の実体ファイルを変更すること。  
  see: `framework/makefw/docs/makeparts.md` の「TEST_SRCS / ADD_SRCS の留意事項」
