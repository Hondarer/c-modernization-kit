# GNU Make

## 概要

GNU Make はビルド プロセスを自動化するツールです。`makefile` にビルド ルールを記述することで、ソース ファイルの変更を検出し必要な部分だけを再コンパイルできます。C プロジェクトのビルド自動化として長年広く使われています。

対象ワークスペースは GNU Make をビルド システムの中核として使用しています。トップレベルの `makefile` から `app/example/prod/`・`app/example.net/prod/`・`app/example/test/` 配下の各 `makefile` を呼び出す階層構造になっており、`framework/makefw/` サブモジュールが提供するテンプレート makefile を各サブディレクトリから利用しています。`make` コマンド一つでライブラリ・実行ファイル・テストのビルドをまとめて実行できます。

`framework/makefw/` サブモジュールは、パスと言語に基づいてテンプレートを自動選択する仕組みを持ち、Linux / Windows の差異を吸収します。`makepart.mk` ファイルによるカスタマイズも可能です。

## 習得目標

- [ ] `makefile` のルール構文 (ターゲット・依存関係・レシピ) を理解できます。
- [ ] `make`・`make clean`・`make all` などの基本コマンドを実行できます。
- [ ] 変数定義 (`CC`・`CFLAGS`・`LDFLAGS` など) を読み取ることができます。
- [ ] パターン ルール (`%.o: %.c`) の意味を理解できます。
- [ ] 自動変数 (`$@`・`$<`・`$^`) の意味を理解できます。
- [ ] `ifeq`・`ifdef` などの条件分岐を読み取ることができます。
- [ ] 対象ワークスペースのトップレベル `makefile` から各サブ makefile の呼び出し構造を読み取ることができます。

## 学習マテリアル

### 公式ドキュメント

- [GNU Make マニュアル](https://www.gnu.org/software/make/manual/make.html) - GNU Make の公式マニュアル (英語)
    - [ルールの書き方](https://www.gnu.org/software/make/manual/make.html#Rules) - ターゲット・依存関係・レシピの基本
    - [変数の使い方](https://www.gnu.org/software/make/manual/make.html#Using-Variables) - 変数定義と展開
    - [パターン ルール](https://www.gnu.org/software/make/manual/make.html#Pattern-Rules) - `%.o: %.c` の説明

### チュートリアル・入門

- [サルでも分かる Git 入門 - makefile 入門](https://www.backlog.com/ja/git-tutorial/) - 関連する日本語コンテンツ (参考)

## 対象ワークスペースとの関連

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
| `app/example/prod/libsrc/examplebase/makefile`             | libexamplebase 静的ライブラリのビルド |
| `app/example/prod/libsrc/example/makefile`                 | libexample 動的ライブラリのビルド     |
| `app/example/prod/src/add/makefile`                     | add 実行ファイルのビルド           |
| `app/example/test/src/libexamplebaseTest/addTest/makefile` | add テストのビルド                 |
| `framework/makefw/makefiles/`                              | テンプレート makefile 群           |

Table: makefile ファイル構成一覧

カスタマイズ:  
各サブディレクトリに `makepart.mk` を配置することで、テンプレートの動作をカスタマイズできます。

### 関連ドキュメント

- [ビルド設計](../../build-design.md) - 対象ワークスペースのビルド構成の詳細
- [フック機能](../../../../../framework/makefw/docs/hooks.md) - `makelocal.mk` の pre/post フック
- [GCC / MSVC ツールチェーン (スキル ガイド)](gcc-toolchain.md) - コンパイラとリンカーのオプション
