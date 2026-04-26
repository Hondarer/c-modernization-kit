---
name: create-unit-test
description: |
  app 配下の C テストを新規作成するときに使うスキルです。
  関数単体テストと main() を含むテストの作り分け、
  makepart.mk の設定、EXPECT_CALL と NiceMock の基本形を案内します。
when_to_use: |
  - app/<name>/test/src 配下で新しい C テストを追加するとき
  - 関数単体テストと main() テストの配置や書き方を確認したいとき
---

# app 向け C テスト作成

このスキルは `app/<name>/test/` 配下の Google Test 作成を対象にします。
mock を新規追加する作業は `create-mock` を使い、この文書ではテストコードと `makepart.mk` の作り方に絞ります。

## テストの種類と単体テストの位置付け

`test/` 配下には目的の異なる複数の種類のテストが含まれます。

| 種類 | 定義 | 配置先 |
|------|------|--------|
| **関数単体テスト** | テスト対象のソース ファイル 1 本のカバレッジを、依存関数の mock によって充足するテスト | `test/src/lib<lib>Test/<name>Test/` |
|  └ **`main()` テスト** | `main()` を対象とする関数単体テストの一形態 | `test/src/.../<name>Test/` |
| **組み合わせテスト・総合テスト** | 複数のコンポーネントを実際にリンクして動作を検証するテスト | 対象に特化したディレクトリ |

**単体テストの核心**: mock が依存関数を差し替えるため、`subtract.c` のテストで `add.c` の実装は不要です。`makepart.mk` の `TEST_SRCS` に対象ファイル 1 本だけを指定し、依存する関数を `LIBS += mock_<lib>` で差し替えます。

組み合わせテスト・総合テストは、単体テストとは別に対象に特化したテストとして個別に用意します。このスキルが扱う主な対象は関数単体テストです。`main()` を対象とする場合も関数単体テストですが、`main()` という性質上の追加手続きがあります。

先に次を確認します。

- 対象の関数が `main()` かどうか (`main()` の場合は関数単体テストに追加の手続きが必要)
- 既存の近いテスト配置があるか
- 依存する関数の mock がすでにあるか
  - アプリケーション関数 (`calcHandler`, `add` 等) の mock が不足する場合は `create-mock` を使う
  - 標準ライブラリ関数 (`printf`, `fopen` 等) の mock が不足する場合は `create-testfw-mock` を使い、`framework/testfw/libsrc/` 配下にモックを追加する

参照先:

- `framework/testfw/docs/how-to-test.md`
- `framework/testfw/docs/how-to-expect.md`
- `framework/testfw/docs/about-test-phase.md`
- `docs/testing-tutorial.md`

## 配置の決め方

- 関数単体テストは `test/src/lib<lib>Test/<name>Test/` を優先します
- `main()` テストは `test/src/main/<name>Test/` を使います
- `makefile` は近い既存テストからそのまま揃え、差分は `makepart.mk` だけに入れます

`app/calc` の規範例:

- 関数単体: `app/calc/test/src/libcalcbaseTest/addTest/`
- `main()` テスト: `app/calc/test/src/main/addTest/`

## 関数単体テスト

規範例は `app/calc/test/src/libcalcbaseTest/addTest/addTest.cc` です。

方針:

- `#include <testfw.h>` を先頭に置きます
- 被テスト関数のヘッダーを直接 include します
- 依存先を差し替える必要がある場合だけ `Mock_<lib>` を生成し、`EXPECT_CALL` を書きます
- Act では対象関数を直接呼び、Assert で戻り値と出力引数を確認します

`makepart.mk` の基本形:

```makefile
# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/calcbase/add.c
```

- `USE_WRAP_MAIN := 1` は入れません
- `LIBS += ...` は mock や補助ライブラリが必要なときだけ追加します

## `main()` テスト

`main()` テストは関数単体テストの一形態です。`makepart.mk` や `EXPECT_CALL` の基本方針は関数単体テストと同じです。`main()` である以上、テストコードから直接 `main()` を呼び出せないため、以下の追加の手続きが必要です。

規範例は `app/calc/test/src/main/addTest/addTest.cc` を正とします。

追加の手続き:

- `USE_WRAP_MAIN := 1` を `makepart.mk` に指定し、`main()` を `__real_main()` として再公開します
- `argv` を組み立て、Act では `__real_main(argc, (char **)&argv)` を呼びます
- Assert では `main()` の戻り値を確認し、必要なら `printf` などの出力も検証します
- 出力確認があるときは `#include <mock_stdio.h>` を追加します
- `Mock_stdio` は未設定呼び出しを許容したいので `NiceMock<Mock_stdio>` を基本にします

`makepart.mk` の基本形:

```makefile
# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/add/add.c

# テスト対象のソースファイルにある main() は直接実行されず、
# テストコード内から __real_main() 経由で実行される
USE_WRAP_MAIN := 1

# ライブラリの指定
LIBS += mock_calcbase mock_libc com_util
```

- `USE_WRAP_MAIN := 1` を必ず入れます
- `mock_libc` は `mock_stdio.h` を使うなら通常必要です
- `__real_main` は `framework/testfw/include/wrapmain.h` から利用されます

## `EXPECT_CALL` の書き方

- 戻り値だけでなく、出力引数を書き換える必要があるときは `WillOnce([](...) { ... })` を使います
- 文字列出力の確認は `StrEq("...\n")` を使います
- 呼び出し回数だけを厳密に見たい場合は `Times(n)` を付けます
- 1 回しか使わない期待値は、不要に `expected_*` 変数へ切り出さず `EXPECT_EQ(...)` へ即値で書きます。プラットフォームごとに値が違う場合は Assert 側を分岐します
- 共通の説明は `framework/testfw/docs/how-to-expect.md` を参照します

例:

```cpp
EXPECT_CALL(mock_calcbase, add(1, 2, _))
    .WillOnce([](int, int, int *result) {
        *result = 3;
        return CALC_SUCCESS;
    });

EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
    .WillOnce(DoDefault());
```

## プラットフォーム分岐

app 向けテストで OS 分岐が必要な場合は、`_WIN32` を直接使わず `app/<name>/prod/include/.../platform.h` の統一マクロを使います。
被テストヘッダーが `platform.h` を読んでいない場合だけ、テストコード側で明示的に include します。

```c
#if defined(PLATFORM_LINUX)
    /* Linux 向けテスト */
#elif defined(PLATFORM_WINDOWS)
    /* Windows 向けテスト */
#endif
```

- Linux / Windows の二択は `#if defined(PLATFORM_LINUX)` と `#elif defined(PLATFORM_WINDOWS)` を基本にし、Linux 側を先に記載します
- `#ifdef _WIN32` や `#ifndef _WIN32` を app test に持ち込まないようにします
- 既存テストに統一マクロの例がある場合は、その書き方に合わせます
- `results.log` 用のタグ付きコメントは、`#if` / `#elif` / `#else` の内側へ重複配置しません。共通化できる `[状態]` / `[Pre-Assert確認]` / `[Pre-Assert手順]` / `[確認]` は分岐の外に置きます
- 分岐内にタグ付きコメントを書くと、非アクティブ側の枝にある説明も evidence 生成で拾われ、多重に出力される場合があります

## コメントの書き方

`framework/testfw/docs/about-test-phase.md` のフェーズ分割に従います。
コメントはテストエビデンス生成の入力になるため、既存例に合わせて各フェーズを明示します。
各テスト ケースでは、フェーズごとに `// Arrange`、`// Pre-Assert`、`// Act`、`// Assert` のコメントを挿入します。

- Arrange フェーズの準備値には `[状態]`
- Pre-Assert フェーズの期待には `[Pre-Assert確認]` または `[Pre-Assert確認_正常系]` / `[Pre-Assert確認_異常系]`
- `WillOnce` の動作説明が必要なら `[Pre-Assert手順]`
- Act フェーズの実行には `[手順]`
- Assert フェーズの検証には `[確認]` または `[確認_正常系]` / `[確認_異常系]`

書式:

- タグの直後に ` - 説明文` を続けます
- `results.log` の要約に出したい文は、箇条書きとして読める簡潔な説明にします
- プラットフォーム分岐がある場合、tag 付きコメントは原則として分岐の外へ置きます。分岐内には tag なしの補足コメントだけを置くか、tag 文言を共通化して外へ寄せます
- `[状態]` / `[Pre-Assert確認]` / `[Pre-Assert手順]` / `[手順]` / `[確認]` が実際に存在しないフェーズは、`results.log` 上で空欄になって構いません
- ただしフェーズ見出しそのものは省略せず、処理がなくても `// Arrange` `// Pre-Assert` `// Act` `// Assert` を順に記載します
- 正常系と異常系を分けたいときだけサフィックス付きタグを使います。迷う場合はサフィックスなしで構いません
- フェーズ見出しは `// Arrange` `// Pre-Assert` `// Act` `// Assert` の形で統一します

例:

```cpp
// Arrange
int argc = 3;
const char *argv[] = {"addTest", "1", "2"}; // [状態] - main() に与える引数を、"1", "2" とする。

// Pre-Assert
EXPECT_CALL(mock_calcbase, add(1, 2, _))
    .WillOnce([](int, int, int *result) {
        *result = 3;
        return CALC_SUCCESS;
    }); // [Pre-Assert確認_正常系] - add(1, 2, &result) が 1 回呼び出されること。
        // [Pre-Assert手順] - add(1, 2, &result) にて result に 3 を設定し、CALC_SUCCESS を返す。

// Act
int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

// Assert
EXPECT_EQ(0, rtc); // [確認_正常系] - main() の戻り値が 0 であること。
```

## 実装時の確認項目

- 近い既存テストと同じディレクトリ構成に置かれていること
- `makefile` を編集せず `makepart.mk` だけで差分を表現していること
- 関数単体テストに `USE_WRAP_MAIN := 1` を入れていないこと
- `main()` テストで `USE_WRAP_MAIN := 1` を入れていること
- `EXPECT_CALL` が Act より前に書かれていること
- 新しい mock が必要なら `create-mock` を使うこと
