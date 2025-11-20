# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

このファイルは、このリポジトリでコードを扱う際の Claude Code (claude.ai/code) への指針を提供します。

## プロジェクト概要

これは Doxygen を使用したドキュメント生成のサンプルプロジェクトです。C 言語のサンプルソースコードから高品質な日本語ドキュメントを生成するための設定とコードの例を提供します。

Doxygen フレームワーク機能はサブモジュール `doxyfw` として統合されています。

### 主要コンポーネント

- `prod/calc/` - サンプル C ソースコード (計算ライブラリとメインプログラム)
- `doxyfw/` - Doxygen ドキュメント生成フレームワーク (git サブモジュール)
- `docsfw/` - Markdown ドキュメント発行フレームワーク (git サブモジュール)
- `testfw/` - テストフレームワーク (git サブモジュール)
- `Doxyfile.part` - プロジェクト固有の Doxygen 設定

## サブモジュール

本プロジェクトは、以下のサブモジュールを使用しています。

### doxyfw

Doxygen ドキュメント生成機能を提供するフレームワークです。

`doxyfw` サブモジュールは以下の機能を提供します:

- Doxygen 基本設定ファイル
- Markdown 変換用の Doxybook2 設定・テンプレート
- XML 前処理・後処理スクリプト
- 日本語ドキュメント出力用のカスタムテンプレート

詳細な使用方法や設定については、`doxyfw/CLAUDE.md` を参照してください。

### docsfw

Markdown から HTML や docx を Pandoc で生成するためのフレームワークです。

`docsfw` サブモジュールは以下の機能を提供します。

- Pandoc を使用した Markdown から HTML/docx への変換
- PlantUML 図の統合
- 多言語ドキュメント対応
- カスタムスタイルとテンプレート

詳細な使用方法については、`docsfw/README.md` を参照してください。

### testfw

Google Test を使用した C コードのテストフレームワークです。

`testfw` サブモジュールは以下の機能を提供します:

- テスト実行支援スクリプト
- モック機能 (標準 C ライブラリのモック)
- Google Test ラッパー
- テストレポート生成機能

詳細な使用方法については、`testfw/README.md` を参照してください。

## 主要コマンド

### ドキュメント生成

```bash
cd doxyfw && make docs
```

### クリーンアップ

```bash
cd doxyfw && make clean
```

## プロジェクト構造

```text
doxygen-sample/                          # このプロジェクト
├── doxyfw/                           # Doxygen フレームワーク (git submodule)
│   ├── Doxyfile                     # Doxygen 基本設定
│   ├── doxybook-config.json         # Doxybook2 設定
│   ├── templates/                   # カスタム日本語テンプレート群
│   ├── docs-src/                    # フレームワーク技術ドキュメント
│   ├── Makefile                     # ドキュメント生成用 Makefile
│   └── CLAUDE.md                    # フレームワーク詳細ドキュメント
├── docsfw/                           # Markdown 発行フレームワーク (git submodule)
│   ├── bin/                         # Pandoc 実行スクリプト
│   ├── lib/                         # フィルタ・変換ライブラリ
│   ├── styles/                      # カスタムスタイル
│   └── README.md                    # フレームワーク詳細ドキュメント
├── testfw/                           # テストフレームワーク (git submodule)
│   ├── cmnd/                        # テスト実行スクリプト群
│   ├── include/                     # テスト用ヘッダーファイル
│   ├── libsrc/                      # テスト用ライブラリソース
│   └── README.md                    # テストフレームワーク詳細ドキュメント
├── Doxyfile.part                     # Doxygen プロジェクト固有設定
├── prod/calc/                        # サンプル C プロジェクト
│   ├── include/libcalc.h            # ライブラリヘッダー (関数宣言、定数定義)
│   ├── libsrc/calc/                 # ライブラリ実装
│   │   ├── add.c                   # add 関数の実装
│   │   └── calcHandler.c           # calcHandler 関数の実装
│   └── src/add/                     # メインプログラム
│       └── add.c                   # add コマンドのメインプログラム
├── test/                             # テストコード
│   └── src/calc/                    # 計算ライブラリのテスト
├── docs/doxygen/                     # 生成される HTML ドキュメント
├── docs-src/doxybook/                # 生成される Markdown ドキュメント
└── xml/                              # Doxygen XML 中間ファイル
```

## サンプルソースコード

### 計算ライブラリ (libcalc)

基本的な整数演算を提供するライブラリのサンプルです。

- `prod/calc/include/libcalc.h` - ライブラリヘッダー (関数宣言、CALC_KIND_ADD 定数定義)
- `prod/calc/libsrc/calc/add.c` - add 関数の実装 (2つの整数を加算)
- `prod/calc/libsrc/calc/calcHandler.c` - calcHandler 関数の実装 (演算種別に基づく計算ハンドラー)

### メインプログラム

ライブラリを使用するサンプルプログラムです。

- `prod/calc/src/add/add.c` - add コマンドのメインプログラム (コマンドライン引数から2つの整数を受け取り加算結果を出力)

## 現在対応中の作業

このレポジトリは、当初 Linux 専用として構築されました。  
現在、Windows との相互運用を行えるよう、対応中です。

## claude 起動時の重要事項

Claude Code 起動時、OS が Linux か Windows かを強く意識するようにしてください。  
このレポジトリはクロスプラットフォーム対応であり、環境の違いを意識する必要があります。

### Windows にてあらかじめ実施しなくてはいけない作業

Claude Code が Windows 対応を進めるにあたり、以下の前提を把握しておいてください。  
現段階では、make コマンドを実施するにあたって、Claude Code は以下に留意する必要があります。

#### GNU Make

GNU Make (make.exe) へのパスはすでに通っています。

#### ポータブル版 Visual Studio Build Tools のパス設定

ポータブル版 Visual Studio Build Tools はあらかじめインストールされています。  
呼び出しを行えるようにするには、

```cmd
Add-VSBT-Env-x64.cmd
```

を実行する必要があります。Add-VSBT-Env-x64.cmd より、パスを含む環境変数が設定されます。  
call 命令で実行しないと、環境変数を取り込めない点に注意が必要です。

#### Git for Windows のパス設定

Git for Windows はあらかじめインストールされています。  
MinGW の各種コマンドの呼び出しを行えるようにするには、

```cmd
Add-MinGW-Path.cmd
```

を実行する必要があります。Add-MinGW-Path.cmd より、パスを含む環境変数が設定されます。
call 命令で実行しないと、環境変数を取り込めない点に注意が必要です。

#### 環境設定の実行順序

**重要**: 環境設定スクリプトは、以下の順序で実行する必要があります。

```cmd
call Add-MinGW-Path.cmd
call Add-VSBT-Env-x64.cmd
```

この順序で実行することで、MSVC の `link.exe` が MinGW の `link` コマンドより優先されます。逆の順序で実行すると、リンク時に MinGW の `link` が呼び出され、ビルドが失敗します。
