# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

このファイルは、このリポジトリでコードを扱う際の Claude Code (claude.ai/code) への指針を提供します。

## プロジェクト概要

これは Doxygen を使用したドキュメント生成のサンプルプロジェクトです。C 言語のサンプルソースコードから高品質な日本語ドキュメントを生成するための設定とコードの例を提供します。

Doxygen フレームワーク機能はサブモジュール `doxyfw` として統合されています。

### 主要コンポーネント

- `prod/calc/` - サンプル C ソースコード (計算ライブラリとメインプログラム)
- `doxyfw/` - Doxygen ドキュメント生成フレームワーク (git サブモジュール)
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

基本的な整数演算を提供するライブラリのサンプルです。`@ingroup calc_api` を使用した API グループ化の例を示しています。

- `prod/calc/include/libcalc.h` - ライブラリヘッダー (関数宣言、CALC_KIND_ADD 定数定義、`@ingroup` による API 分類)
- `prod/calc/libsrc/calc/add.c` - add 関数の実装 (2つの整数を加算)
- `prod/calc/libsrc/calc/calcHandler.c` - calcHandler 関数の実装 (演算種別に基づく計算ハンドラー)

### メインプログラム

ライブラリを使用するサンプルプログラムです。

- `prod/calc/src/add/add.c` - add コマンドのメインプログラム (コマンドライン引数から2つの整数を受け取り加算結果を出力)
