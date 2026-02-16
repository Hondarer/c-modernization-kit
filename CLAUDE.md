# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

このファイルは、このリポジトリでコードを扱う際の Claude Code (claude.ai/code) への指針を提供します。

## プロジェクト概要

レガシー C コードのモダナイゼーションのための統合フレームワークです。C 言語のサンプルソースコードから高品質なドキュメントを生成し、自動テストを実行し、.NET からの呼び出しをサポートするための設定とコードの例を提供します。

主要なフレームワーク機能はサブモジュールとして統合されています:
- `doxyfw` - Doxygen ドキュメント生成フレームワーク
- `docsfw` - Markdown ドキュメント発行フレームワーク
- `testfw` - Google Test ベースのテストフレームワーク
- `makefw` - Make ビルドフレームワーク

### 主要コンポーネント

- `prod/calc/` - サンプル C ソースコード (計算ライブラリとメインプログラム)
- `prod/calc.net/` - .NET ラッパーとアプリケーション (C ライブラリの .NET からの呼び出し例)
- `test/` - テストコード (Google Test ベース)
- `doxyfw/` - Doxygen ドキュメント生成フレームワーク (git サブモジュール)
- `docsfw/` - Markdown ドキュメント発行フレームワーク (git サブモジュール)
- `testfw/` - テストフレームワーク (git サブモジュール)
- `makefw/` - Make ビルドフレームワーク (git サブモジュール)
- `Doxyfile.part.calc` - C プロジェクト用 Doxygen 設定
- `Doxyfile.part.calc.net` - .NET プロジェクト用 Doxygen 設定
- `c-modernization-kit.sln` - Visual Studio ソリューションファイル
- `Directory.Build.props` - .NET プロジェクト共通設定

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

### makefw

Make ビルドシステムのフレームワークです。

`makefw` サブモジュールは以下の機能を提供します:

- テンプレート自動選択機能 (パスと言語に基づく)
- C/C++ および .NET プロジェクトのビルドサポート
- クロスプラットフォームビルド (Linux, Windows)
- makepart.mk によるカスタマイズ機構

詳細な使用方法については、`makefw/README.md` を参照してください。

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
c-modernization-kit/                          # このプロジェクト
+-- doxyfw/                                # Doxygen フレームワーク (git submodule)
|   +-- Doxyfile                          # Doxygen 基本設定
|   +-- doxybook-config.json              # Doxybook2 設定
|   +-- templates/                        # カスタムテンプレート群
|   +-- docs-src/                         # フレームワーク技術ドキュメント
|   +-- makefile                          # ドキュメント生成用 makefile
|   +-- CLAUDE.md                         # フレームワーク詳細ドキュメント
+-- docsfw/                                # Markdown 発行フレームワーク (git submodule)
|   +-- bin/                              # Pandoc 実行スクリプト
|   +-- lib/                              # フィルタ・変換ライブラリ
|   +-- styles/                           # カスタムスタイル
|   +-- README.md                         # フレームワーク詳細ドキュメント
+-- testfw/                                # テストフレームワーク (git submodule)
|   +-- cmnd/                             # テスト実行スクリプト群
|   +-- include/                          # テスト用ヘッダーファイル
|   +-- libsrc/                           # テスト用ライブラリソース
|   +-- README.md                         # テストフレームワーク詳細ドキュメント
+-- makefw/                                # Make ビルドフレームワーク (git submodule)
|   +-- makefiles/                        # ビルド用テンプレート
|   +-- README.md                         # フレームワーク詳細ドキュメント
+-- Doxyfile.part.calc                     # C プロジェクト用 Doxygen 設定
+-- Doxyfile.part.calc.net                 # .NET プロジェクト用 Doxygen 設定
+-- c-modernization-kit.sln                # Visual Studio ソリューションファイル
+-- Directory.Build.props                  # .NET プロジェクト共通設定
+-- makefile                               # トップレベル makefile
+-- prod/                                  # プロダクションコード
|   +-- calc/                             # サンプル C プロジェクト
|   |   +-- include/                      # ライブラリヘッダー
|   |   |   +-- libcalc.h                # 動的リンク用ヘッダー (calcHandler 関数)
|   |   |   +-- libcalcbase.h            # 静的リンク用ヘッダー (add, subtract, multiply, divide 関数)
|   |   |   +-- libcalc_const.h          # 定数定義 (CALC_KIND_ADD など)
|   |   +-- libsrc/                       # ライブラリ実装
|   |   |   +-- calcbase/                # 基本計算関数 (静的ライブラリ)
|   |   |   |   +-- add.c               # add 関数の実装
|   |   |   |   +-- subtract.c          # subtract 関数の実装
|   |   |   |   +-- multiply.c          # multiply 関数の実装
|   |   |   |   +-- divide.c            # divide 関数の実装
|   |   |   +-- calc/                    # 計算ハンドラー (動的ライブラリ)
|   |   |       +-- calcHandler.c        # calcHandler 関数の実装
|   |   +-- src/                          # メインプログラム
|   |   |   +-- add/add.c                # add コマンド (calcbase を静的リンク)
|   |   |   +-- calc/calc.c              # calc コマンド (calc を動的リンク)
|   |   |   +-- shared-and-static-calc/  # 動的・静的両方をリンクする例
|   |   |       +-- shared-and-static-calc.c
|   |   +-- lib/                          # ビルド済みライブラリ
|   |   +-- bin/                          # ビルド済み実行ファイル
|   +-- calc.net/                         # .NET プロジェクト
|   |   +-- libsrc/CalcLib/               # C ライブラリの .NET ラッパー
|   |   |   +-- CalcLib.csproj           # ライブラリプロジェクト
|   |   |   +-- CalcLibrary.cs           # メインライブラリクラス
|   |   |   +-- CalcKind.cs              # 列挙型定義
|   |   |   +-- CalcResult.cs            # 結果クラス
|   |   |   +-- CalcException.cs         # 例外クラス
|   |   |   +-- Internal/NativeMethods.cs   # P/Invoke 定義
|   |   +-- src/CalcApp/                  # サンプルアプリケーション
|   |   |   +-- CalcApp.csproj           # アプリケーションプロジェクト
|   |   |   +-- Program.cs               # メインプログラム
|   |   |   +-- ModuleInitializer.cs     # モジュール初期化
|   |   +-- lib/                          # ビルド済みライブラリ
|   |   +-- bin/                          # ビルド済み実行ファイル
|   +-- tutorial/                         # チュートリアル用サンプル
|       +-- include/                      # ヘッダーファイル
|       +-- libsrc/samplestatic/          # サンプル静的ライブラリ
|       +-- lib/                          # ビルド済みライブラリ
+-- test/                                  # テストコード
|   +-- src/
|   |   +-- calc/                         # 計算ライブラリのテスト
|   |   |   +-- main/                     # メインプログラムのテスト
|   |   |   |   +-- addTest/             # add コマンドのテスト
|   |   |   |   +-- calcTest/            # calc コマンドのテスト
|   |   |   |   +-- shared-and-static-calcTest/   # shared-and-static-calc コマンドのテスト
|   |   |   +-- libcalcbaseTest/         # libcalcbase ライブラリのテスト
|   |   |       +-- addTest/             # add 関数のテスト
|   |   |       +-- subtractTest/        # subtract 関数のテスト
|   |   |       +-- multiplyTest/        # multiply 関数のテスト
|   |   |       +-- divideTest/          # divide 関数のテスト
|   |   +-- tutorial/                     # テストのチュートリアル
|   +-- libsrc/                           # テスト用モック実装
|       +-- mock_calcbase/                # calcbase ライブラリのモック
|       +-- mock_calc/                    # calc ライブラリのモック
+-- docs/                                  # 生成されるドキュメント
|   +-- doxygen/                          # HTML ドキュメント (Doxygen)
|   +-- ja/html/                          # 日本語版 HTML
|   +-- en/html/                          # 英語版 HTML
+-- docs-src/                              # ドキュメントソース
|   +-- doxybook/                         # 生成される Markdown (Doxybook2)
|   |   +-- calc/                         # C プロジェクトのドキュメント
|   |   +-- calc.net/                     # .NET プロジェクトのドキュメント
|   +-- *.md                              # 追加ドキュメント
|       +-- github-actions.md             # GitHub Actions 設定
|       +-- dotnet-relwithdebinfo.md      # .NET RelWithDebInfo ビルド
|       +-- testing-tutorial.md           # テストチュートリアル
|       +-- vscode-variables.md           # VS Code 変数リファレンス
+-- xml/                                   # Doxygen XML 中間ファイル
```

## サンプルソースコード

### 計算ライブラリ

基本的な整数演算を提供するライブラリのサンプルです。

#### ヘッダーファイル

- `prod/calc/include/libcalc_const.h` - 定数定義 (CALC_SUCCESS, CALC_KIND_ADD など)
- `prod/calc/include/libcalcbase.h` - 静的リンク用ヘッダー (add, subtract, multiply, divide 関数)
- `prod/calc/include/libcalc.h` - 動的リンク用ヘッダー (calcHandler 関数)

#### libcalcbase (静的ライブラリ)

- `prod/calc/libsrc/calcbase/add.c` - add 関数の実装 (2つの整数を加算)
- `prod/calc/libsrc/calcbase/subtract.c` - subtract 関数の実装 (2つの整数を減算)
- `prod/calc/libsrc/calcbase/multiply.c` - multiply 関数の実装 (2つの整数を乗算)
- `prod/calc/libsrc/calcbase/divide.c` - divide 関数の実装 (2つの整数を除算)

#### libcalc (動的ライブラリ)

- `prod/calc/libsrc/calc/calcHandler.c` - calcHandler 関数の実装 (演算種別に基づく計算ハンドラー)

### メインプログラム

ライブラリを使用するサンプルプログラムです。

- `prod/calc/src/add/add.c` - add コマンド (calcbase を静的リンク)
- `prod/calc/src/calc/calc.c` - calc コマンド (calc を動的リンク)
- `prod/calc/src/shared-and-static-calc/shared-and-static-calc.c` - 動的・静的両方をリンクする例

### .NET プロジェクト

C ライブラリを .NET から利用するための実装例です。

#### CalcLib (.NET ラッパーライブラリ)

- `prod/calc.net/libsrc/CalcLib/CalcLibrary.cs` - メインライブラリクラス (C ライブラリへの .NET インターフェース)
- `prod/calc.net/libsrc/CalcLib/CalcKind.cs` - 計算種別の列挙型定義
- `prod/calc.net/libsrc/CalcLib/CalcResult.cs` - 計算結果を格納するクラス
- `prod/calc.net/libsrc/CalcLib/CalcException.cs` - 計算エラー用の例外クラス
- `prod/calc.net/libsrc/CalcLib/Internal/NativeMethods.cs` - P/Invoke による C ライブラリの呼び出し定義

#### CalcApp (.NET アプリケーション)

- `prod/calc.net/src/CalcApp/Program.cs` - メインプログラム (CalcLib を使用した計算アプリケーション)
- `prod/calc.net/src/CalcApp/ModuleInitializer.cs` - モジュール初期化処理 (ネイティブライブラリのロード設定)

## Claude Code 起動時の重要事項

Claude Code 起動時、OS が Linux か Windows かを強く意識するようにしてください。  
このレポジトリはクロスプラットフォーム対応であり、環境の違いを意識する必要があります。

### Windows にてあらかじめ実施しなくてはいけない作業

Claude Code が Windows 対応を進めるにあたり、以下の前提を把握しておいてください。  
現段階では、make コマンドを実施するにあたって、Claude Code は以下に留意する必要があります。

#### GNU Make

GNU Make (make.exe) へのパスはすでに通っています。

#### 開発環境のセットアップ

ポータブル版 Visual Studio Build Tools と Git for Windows はあらかじめインストールされています。
MinGW PATH と VSBT 環境変数を設定するには、`Start-VSCode-With-Env.ps1` を利用します。

環境変数の設定のみを行う場合は、`-EnvOnly` パラメータを指定してドットソースで実行します。

```powershell
. .\Start-VSCode-With-Env.ps1 -EnvOnly
```

VS Code を起動する場合は、パラメータなしで実行します。VS Code は子プロセスとして起動するため、設定した環境変数を継承します。

```powershell
.\Start-VSCode-With-Env.ps1
```

各種パス (Git for Windows, MSVC ToolSet, Windows SDK など) は候補ディレクトリを自動走査して検出されます。  
明示的にパスを設定する場合は、スクリプト内のユーザー設定値セクションを編集してください。
