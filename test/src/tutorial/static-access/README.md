# static-access テストプログラム

## 概要

このテストプログラムは、C 言語の static 変数に対するテスト手法を示すサンプルです。

通常、static 変数はファイルスコープ内でのみアクセス可能であり、外部のテストコードから直接アクセスすることができません。本テストでは、testfw の inject 機能を使用して、テスト対象ソースコードの static 変数にアクセスする方法を説明します。

## テスト対象

- sample-static-lib/samplestatic.c
  - `static int static_int` - ファイルスコープの static 変数
  - `int samplestatic(void)` - static 変数の値を返す関数

## テストプログラムの構成

### テストケース (static-access.cc)

```cpp
TEST_F(test_static_access, test)
{
    // Arrange
    // inject 機能を使用して static 変数に値を設定
    set_static_int(123); // [準備] - static_int に 123 を設定する。

    // Pre-Assert

    // Act
    int rtc = samplestatic(); // [手順] - samplestatic() を呼び出す。

    // Assert
    EXPECT_EQ(123, rtc); // [確認] - 戻り値が設定した値 123 であること。
}
```

### inject ヘッダー (samplestatic.inject.h)

テスト対象ソースファイルの先頭に結合される追加ヘッダーです。

```c
extern void set_static_int(int);
```

このヘッダーをテストプログラムが参照することで、テストプログラムから static 変数にアクセスするための関数を呼び出せるようになります。

### inject ソース (samplestatic.inject.c)

テスト対象ソースファイルの末尾に結合される追加ソースです。

```c
void set_static_int(int set_value)
{
    static_int = set_value;
}
```

この関数は、static 変数へのアクセサー (setter) として機能します。テスト対象ソースファイルと同一ファイル内にコンパイルされるため、static 変数にアクセスできます。

### Makefile

```makefile
# テスト対象のソースファイル
TEST_SRCS := \
    sample-static-lib/samplestatic.c
```

`TEST_SRCS` 変数にテスト対象のソースファイルを指定します。makefw のビルドシステムが自動的に inject ファイルを検出し、結合処理を行います。

## inject 機能の仕組み

1. ビルド時の処理:
   - テスト対象ソースファイル (samplestatic.c)
   - inject ヘッダー (samplestatic.inject.h) - ソースの先頭に結合
   - inject ソース (samplestatic.inject.c) - ソースの末尾に結合

2. 結合後のイメージ:
   ```c
   // samplestatic.inject.h の内容が挿入される

   #include "samplestatic.h"

   static int static_int;  // この変数にアクセスしたい

   int samplestatic(void)
   {
       return static_int;
   }

   // samplestatic.inject.c の内容が挿入される
   void set_static_int(int set_value)
   {
       static_int = set_value;  // 同一ファイル内なのでアクセス可能
   }
   ```

3. テストコードからの利用:
   - テストコードは `samplestatic.inject.h` をインクルード
   - `set_static_int()` 関数を呼び出して static 変数を操作
   - `samplestatic()` 関数を呼び出して結果を検証

## テストの実行内容

このテストプログラムは以下の処理を行います:

1. Arrange (準備): inject 機能の `set_static_int()` 関数を使用して、static 変数 `static_int` に値 123 を設定
2. Act (実行): テスト対象関数 `samplestatic()` を呼び出し、戻り値を取得
3. Assert (検証): 戻り値が設定した値 (123) と一致することを確認

## inject 機能の特長

- カプセル化を維持: 本番コードの static 変数を外部に公開する必要がない
- テスト専用のアクセス: テスト時のみ static 変数にアクセス可能
- 既存コードの変更不要: テスト対象のソースコードを変更せずにテスト可能

## ビルドと実行

```bash
# テストのビルドと実行
cd test/src/tutorial/static-access
make test
```

## 参考

- testfw フレームワークの詳細: `testfw/README.md`
- テスト対象ソース: `sample-static-lib/samplestatic.c`
- テスト対象ヘッダー: `prod/tutorial/include/samplestatic.h`
