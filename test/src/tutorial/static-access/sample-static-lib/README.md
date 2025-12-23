# sample-static-lib

## 概要

このディレクトリには、static 変数へのテストアクセス手法を示すためのサンプルライブラリソースコードが含まれています。

親ディレクトリ (`test/src/tutorial/static-access`) は、testfw の inject 機能を使用して C 言語の static 変数をテストする方法を実践的に説明するチュートリアルです。  
本ディレクトリはそのテスト対象となるライブラリを提供します。

## ファイル構成

- `samplestatic.h` - ライブラリのヘッダーファイル
- `samplestatic.c` - ライブラリの実装ファイル

## ライブラリ機能

### samplestatic() 関数

```c
int samplestatic(void);
```

内部の static 変数 `static_int` の値を返す関数です。

#### 実装の特徴

- ファイルスコープの `static int static_int` 変数を持つ
- 外部から直接アクセスできない static 変数の値を返す
- テストフレームワークの inject 機能によるテストの対象となる

## static 変数とテストの課題

C 言語の static 変数は、通常ファイルスコープ内でのみアクセス可能です。このため、以下の課題があります。

- 外部のテストコードから直接アクセスできない
- 値の設定や検証が困難
- テストのために static を削除すると、カプセル化が損なわれる

## testfw の inject 機能による解決

本チュートリアルでは、testfw の inject 機能を使用してこの課題を解決します。

1. **inject ヘッダー** (`../samplestatic.inject.h`)
   - テスト対象ソースの先頭に結合される
   - setter 関数のプロトタイプ宣言を提供

2. **inject ソース** (`../samplestatic.inject.c`)
   - テスト対象ソースの末尾に結合される
   - static 変数へのアクセサー関数を実装
   - 同一ファイル内にコンパイルされるため static 変数にアクセス可能

3. **テストコード** (`../static-access.cc`)
   - inject ヘッダーをインクルード
   - setter 関数を使用して static 変数を操作
   - samplestatic() 関数を呼び出して結果を検証

## コードの変更が不要

本番コードである `samplestatic.c` と `samplestatic.h` は、テストのために変更する必要がありません。  
inject 機能により、既存コードの構造とカプセル化を維持したままテストが可能です。

## チュートリアルの詳細

このライブラリを使用したテストの詳細については、親ディレクトリの README.md を参照してください。

- チュートリアル全体の説明: `../README.md`
- テストの実行方法
- inject 機能の仕組み
- テストコードの解説

## 参考

- testfw フレームワーク: `../../../../testfw/README.md`
- テストコード: `../static-access.cc`
- inject ヘッダー: `../samplestatic.inject.h`
- inject ソース: `../samplestatic.inject.c`
