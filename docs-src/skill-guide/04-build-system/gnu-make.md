# GNU Make

## 概要

GNU Make はビルドプロセスを自動化するツールです。`makefile` にビルドルールを記述することで、ソースファイルの変更を検出し必要な部分だけを再コンパイルできます。C プロジェクトのビルド自動化として長年広く使われています。

このリポジトリは GNU Make をビルドシステムの中核として使用しています。トップレベルの `makefile` から `prod/calc/`・`prod/calc.net/`・`test/` 配下の各 `makefile` を呼び出す階層構造になっており、`makefw/` サブモジュールが提供するテンプレート makefile を各サブディレクトリから利用しています。`make` コマンド一つでライブラリ・実行ファイル・テストのビルドをまとめて実行できます。

`makefw/` サブモジュールは、パスと言語に基づいてテンプレートを自動選択する仕組みを持ち、Linux / Windows の差異を吸収します。`makepart.mk` ファイルによるカスタマイズも可能です。

## 習得目標

- [ ] `makefile` のルール構文 (ターゲット・依存関係・レシピ) を理解できる
- [ ] `make`・`make clean`・`make all` などの基本コマンドを実行できる
- [ ] 変数定義 (`CC`・`CFLAGS`・`LDFLAGS` など) を読み取れる
- [ ] パターンルール (`%.o: %.c`) の意味を理解できる
- [ ] 自動変数 (`$@`・`$<`・`$^`) の意味を理解できる
- [ ] `ifeq`・`ifdef` などの条件分岐を読み取れる
- [ ] このリポジトリのトップレベル `makefile` から各サブ makefile の呼び出し構造を読み取れる

## 学習マテリアル

### 公式ドキュメント

- [GNU Make マニュアル](https://www.gnu.org/software/make/manual/make.html) - GNU Make の公式マニュアル (英語)
  - [ルールの書き方](https://www.gnu.org/software/make/manual/make.html#Rules) - ターゲット・依存関係・レシピの基本
  - [変数の使い方](https://www.gnu.org/software/make/manual/make.html#Using-Variables) - 変数定義と展開
  - [パターンルール](https://www.gnu.org/software/make/manual/make.html#Pattern-Rules) - `%.o: %.c` の説明

### チュートリアル・入門

- [サルでも分かるGit入門 - makefile 入門](https://www.backlog.com/ja/git-tutorial/) - 関連する日本語コンテンツ (参考)

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

主要コマンド:

```bash
# トップレベルからすべてをビルド
make

# クリーンアップ
make clean

# ドキュメント生成
make doxy
make docs
```

ファイル構成:

| ファイル                                         | 役割                               |
|--------------------------------------------------|------------------------------------|
| `makefile` (トップレベル)                        | 全体のビルド制御                   |
| `prod/calc/libsrc/calcbase/makefile`             | libcalcbase 静的ライブラリのビルド |
| `prod/calc/libsrc/calc/makefile`                 | libcalc 動的ライブラリのビルド     |
| `prod/calc/src/add/makefile`                     | add 実行ファイルのビルド           |
| `test/src/calc/libcalcbaseTest/addTest/makefile` | add テストのビルド                 |
| `makefw/makefiles/`                              | テンプレート makefile 群           |

カスタマイズ:
各サブディレクトリに `makepart.mk` を配置することで、テンプレートの動作をカスタマイズできます。

### 関連ドキュメント

- [ビルド設計](../../build-design.md) - このリポジトリのビルド構成の詳細
- [makepart.mk フックの設計](../../makepart-hooks-design.md) - `makepart.mk` カスタマイズ機構の詳細
- [GCC / MSVC ツールチェイン (スキルガイド)](gcc-toolchain.md) - コンパイラとリンカのオプション
