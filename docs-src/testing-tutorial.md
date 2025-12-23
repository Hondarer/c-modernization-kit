# C言語テスト実践チュートリアル

このチュートリアルでは、Linux/Windows クロスプラットフォーム環境において、Google Test フレームワークを使用して C言語プログラムをテストする方法を詳しく説明します。

## 目次

1. [テストフレームワークの概要](#テストフレームワークの概要)
2. [プロジェクト構造](#プロジェクト構造)
3. [環境構築](#環境構築)
   - [Linux 環境](#linux-環境)
   - [Windows 環境](#windows-環境)
4. [モックの作成方法](#モックの作成方法)
5. [関数のテスト方法](#関数のテスト方法)
6. [main関数を含むプログラムのテスト](#main関数を含むプログラムのテスト)
7. [Makefileの作成](#makefileの作成)
8. [テストの実行](#テストの実行)
9. [実践例](#実践例)
10. [ベストプラクティス](#ベストプラクティス)
11. [トラブルシューティング](#トラブルシューティング)
    - [共通の問題](#共通の問題)
    - [Windows 固有の問題](#windows-固有の問題)

---

## テストフレームワークの概要

本プロジェクトでは、Google Test (gtest/gmock) を使用してC言語プログラムのユニットテストを行います。
Linux では GCC、Windows では MSVC を使用したクロスプラットフォーム開発環境をサポートしています。

### 主な特徴

- **クロスプラットフォーム対応**: Linux (GCC) と Windows (MSVC) の両方でシームレスに動作
- **C言語とC++の統合**: C言語で書かれたコードをC++ (Google Test) でテスト
- **モックフレームワーク**: Google Mock を使用した依存関係のモック化
- **リンカラップ機能**: Linux では `-Wl,--wrap`、Windows では適切なリンカオプションを使用した関数の置き換え
- **テストフェーズの明確化**: Arrange, Pre-Assert, Act, Assert の4フェーズでテストを構造化

### テストの4フェーズ

本フレームワークでは、テストコードを以下の4つのフェーズに分けて記述します:

1. **Arrange [状態]**: テストの初期状態を設定
2. **Pre-Assert [Pre-Assert確認] [Pre-Assert手順]**: モックの期待動作を設定
3. **Act [手順]**: テスト対象のメソッドを実行
4. **Assert [確認]**: 実行結果を検証

---

## プロジェクト構造

```
c-modernization-kit/
+-- testfw/                           # テストフレームワーク (サブモジュール)
|   +-- cmnd/                        # テスト支援コマンド
|   +-- include/                     # フレームワーク提供のモック (stdio等)
|   +-- include_override/            # オーバーライド用ヘッダー
|   +-- libsrc/                      # フレームワーク提供のモック実装
|   +-- docs-src/                    # テストフレームワークドキュメント
+-- makefw/                           # Make ビルドフレームワーク (サブモジュール)
|   +-- makefiles/                   # Makefile テンプレート
|       +-- prepare.mk              # 準備処理
|       +-- makemain.mk             # ビルドルール生成
+-- test/                             # テストコード (本プロジェクト固有)
|   +-- include/                     # プロジェクト固有のモックヘッダー
|   |   +-- mock_calc.h             # calcHandlerのモック
|   |   +-- mock_calcbase.h         # add, subtract, multiply, divide のモック
|   +-- libsrc/                      # プロジェクト固有のモック実装
|   |   +-- mock_calc/              # calcHandler モックライブラリ
|   |   |   +-- Makefile            # 標準テンプレート
|   |   |   +-- makepart.mk         # プロジェクト固有の設定
|   |   |   +-- mock_calc.cc
|   |   |   +-- mock_calcHandler.cc
|   |   +-- mock_calcbase/          # calcbase 関数モックライブラリ
|   |       +-- Makefile            # 標準テンプレート
|   |       +-- makepart.mk         # プロジェクト固有の設定
|   |       +-- mock_calcbase.cc
|   |       +-- mock_add.cc
|   |       +-- mock_subtract.cc
|   |       +-- mock_multiply.cc
|   |       +-- mock_divide.cc
|   +-- src/                         # テストコード
|       +-- calc/
|           +-- libcalcbaseTest/    # ライブラリ関数のテスト
|           |   +-- addTest/
|           |       +-- Makefile    # 標準テンプレート
|           |       +-- makepart.mk # プロジェクト固有の設定
|           |       +-- addTest.cc
|           +-- main/                # main関数を含むプログラムのテスト
|               +-- addTest/
|               |   +-- Makefile    # 標準テンプレート
|               |   +-- makepart.mk # プロジェクト固有の設定
|               |   +-- addTest.cc
|               +-- calcTest/
|                   +-- Makefile    # 標準テンプレート
|                   +-- makepart.mk # プロジェクト固有の設定
|                   +-- calcTest.cc
+-- prod/                             # テスト対象のソースコード
    +-- calc/
        +-- include/
        |   +-- libcalcbase.h       # 静的リンク用API
        |   +-- libcalc.h           # 動的リンク用API
        |   +-- libcalc_const.h     # 定数定義
        +-- libsrc/
        |   +-- calcbase/
        |       +-- add.c           # add関数の実装
        |       +-- subtract.c      # subtract関数の実装
        |       +-- multiply.c      # multiply関数の実装
        |       +-- divide.c        # divide関数の実装
        +-- src/
            +-- add/
            |   +-- add.c           # addコマンドのmain関数
            +-- calc/
                +-- calc.c          # calcコマンドのmain関数
```

---

## 環境構築

### Linux 環境

#### 必要なパッケージ

```bash
# Debian/Ubuntu系
sudo apt-get install -y \
    build-essential \
    g++ \
    make \
    libgtest-dev \
    libgmock-dev \
    gcovr \
    lcov

# Red Hat/CentOS系
sudo dnf install -y \
    gcc \
    g++ \
    make \
    gtest-devel \
    gmock-devel \
    python3-pip

sudo pip3 install gcovr
```

### Windows 環境

#### 必要なツール

- **Visual Studio Build Tools** (または Visual Studio) - MSVC コンパイラとリンカ
- **GNU Make** - Make ビルドシステム (make.exe)
- **Git for Windows** - MinGW ツールチェーンを含む
- **Google Test/Mock** - テストフレームワーク

#### 環境設定スクリプト

Windows では、コマンドプロンプトで以下のスクリプトを実行してから VS Code を起動します:

```cmd
REM 環境設定 (重要: この順序を維持すること)
call Add-MinGW-Path.cmd
call Add-VSBT-Env-x64.cmd
code
```

**重要**: スクリプトの実行順序が重要です:

1. **Add-MinGW-Path.cmd**: Git for Windows の MinGW ツールへのパスを設定
2. **Add-VSBT-Env-x64.cmd**: Visual Studio Build Tools の環境変数を設定

この順序により、MSVC の `link.exe` が MinGW の `link` コマンドより優先されます。

> **注意**: `Add-VSBT-Env-x64.cmd` は、Visual Studio の環境設定スクリプトと適宜読み替えてください。
> 一般的には、x64 Native Tools Command Prompt for VS で使用される `vcvars64.bat` に相当します。

### サブモジュールの初期化

```bash
# Linux / Windows 共通
cd /path/to/c-modernization-kit
git submodule update --init --recursive
```

### ワークスペースルートの設定

プロジェクトのルートディレクトリに `.workspaceRoot` ファイルを作成します:

```bash
# Linux
touch .workspaceRoot

# Windows (PowerShell)
New-Item .workspaceRoot -ItemType File

# Windows (コマンドプロンプト)
type nul > .workspaceRoot
```

このファイルにより、Makefile がプロジェクトルートを自動検出できます。

---

## モックの作成方法

モックを作成することで、テスト対象のコードが依存する関数の動作を制御できます。

### ステップ1: モックヘッダーファイルの作成

`test/include/mock_xxxxx.h` にモックヘッダーを作成します。

#### 例: mock_calcbase.h

```cpp
#ifndef _MOCK_CALCBASE_H
#define _MOCK_CALCBASE_H

#include <stdio.h>
#include <gmock/gmock.h>

// テスト対象のヘッダーをインクルード
#include <libcalcbase.h>

// モッククラスの定義
class Mock_calcbase
{
public:
    // MOCK_METHOD マクロで関数をモック化
    // 構文: MOCK_METHOD(戻り値の型, 関数名, (引数リスト));
    MOCK_METHOD(int, add, (int, int, int *));
    MOCK_METHOD(int, subtract, (int, int, int *));
    MOCK_METHOD(int, multiply, (int, int, int *));
    MOCK_METHOD(int, divide, (int, int, int *));

    Mock_calcbase();
    ~Mock_calcbase();
};

// グローバルなモックインスタンスへのポインタ
extern Mock_calcbase *_mock_calcbase;

#endif // _MOCK_CALCBASE_H
```

#### ポイント

- **MOCK_METHOD マクロ**: Google Mock の機能で、関数のシグネチャを定義します
- **extern宣言**: モックインスタンスをグローバルに共有するための宣言
- **C++クラス**: C言語の関数をC++のモッククラスでラップします

### ステップ2: モッククラスの実装

`test/libsrc/mock_xxxxx/mock_xxxxx.cc` にモッククラスの実装を作成します。

#### 例: mock_calcbase.cc

```cpp
#include <testfw.h>
#include <mock_calcbase.h>

// グローバルインスタンスの実体
Mock_calcbase *_mock_calcbase = nullptr;

// コンストラクタ: デフォルト動作を設定
Mock_calcbase::Mock_calcbase()
{
    // デフォルト動作を設定 (オプション)
    ON_CALL(*this, add(_, _, _))
        .WillByDefault(Invoke([](int a, int b, int *result) {
            *result = a + b;
            return CALC_SUCCESS;
        })); // モックの既定の挙動を定義する例

    ON_CALL(*this, subtract(_, _, _))
        .WillByDefault(Return(CALC_SUCCESS)); // 一般的にはモックの既定の挙動は NOP にしておき、テストプログラムで具体的な挙動を決める

    ON_CALL(*this, multiply(_, _, _))
        .WillByDefault(Return(CALC_SUCCESS));

    ON_CALL(*this, divide(_, _, _))
        .WillByDefault(Return(CALC_SUCCESS));

    _mock_calcbase = this;
}

// デストラクタ: グローバルポインタをクリア
Mock_calcbase::~Mock_calcbase()
{
    _mock_calcbase = nullptr;
}
```

### ステップ3: モック関数の実装

`test/libsrc/mock_xxxxx/mock_関数名.cc` にC言語関数のモック実装を作成します。

#### 例: mock_add.cc

```cpp
#include <testfw.h>
#include <mock_calcbase.h>

// C言語の関数として実装
// この関数がテスト時に本物の add() 関数の代わりに呼ばれます
// WEAK_ATR 属性により、リンク時に弱いシンボルとして扱われる
WEAK_ATR int add(int a, int b, int *result)
{
    int rtc = 0;

    // モックインスタンスが存在する場合はモックを呼び出す
    if (_mock_calcbase != nullptr)
    {
        rtc = _mock_calcbase->add(a, b, result);
    }

    // トレース出力 (デバッグ用)
    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %d, 0x%p", __func__, a, b, (void *)result);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> %d, %d\n", *result, rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
```

#### ポイント

- **WEAK_ATR 属性**: リンク時に弱いシンボルとして扱われ、実装がない場合にモック関数が使用される
- **C言語関数**: `extern "C"` は不要 (`.cc` ファイルでも関数名が C++ にならない)
- **モックインスタンスチェック**: `_mock_calcbase != nullptr` でモックの有無を確認
- **トレース機能**: デバッグ時に関数呼び出しを確認できる

### モックライブラリの Makefile

`test/libsrc/mock_xxxxx/` ディレクトリに以下のファイルを作成します。

#### Makefile (標準テンプレート)

`test/libsrc/mock_xxxxx/Makefile` は標準テンプレートをそのまま使用します。

```makefile
# Makefile テンプレート
# すべての最終階層 Makefile で使用する標準テンプレート
# 本ファイルの編集は禁止する。makepart.mk を作成して拡張・カスタマイズすること。

# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (ビルドテンプレートより前に include)
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

##### makepart.mk の内容は、このタイミングで処理される #####

# ビルドテンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makemain.mk
```

#### makepart.mk (プロジェクト固有の設定)

`test/libsrc/mock_xxxxx/makepart.mk` にプロジェクト固有の設定を記述します。

```makefile
# 出力先ディレクトリ (ライブラリの場合のみ必要)
OUTPUT_DIR := $(WORKSPACE_FOLDER)/test/lib

# ソースファイル (必要に応じて追加)
SRCS := \
	mock_calcbase.cc \
	mock_add.cc \
	mock_subtract.cc \
	mock_multiply.cc \
	mock_divide.cc
```

---

## 関数のテスト方法

通常の関数 (main を含まない) のテスト方法を説明します。

### テストコードの基本構造

```cpp
#include <testfw.h>
#include <mock_stdio.h>
#include <libcalcbase.h>

// テストフィクスチャクラス
class addTest : public Test
{
};

// テストケース
TEST_F(addTest, test_1_add_2)
{
    // Arrange (状態設定)
    int result;

    // Pre-Assert (期待動作設定)
    // - モックの期待動作を設定 (この例では不要)

    // Act (実行)
    int rtc = add(1, 2, &result); // [手順] - add(1, 2, &result) を呼び出す。

    // Assert (検証)
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(3, result);         // [確認] - 結果が 3 であること。
}
```

### 実践例: add関数のテスト

`test/src/calc/libcalcbaseTest/addTest/addTest.cc`

```cpp
#include <testfw.h>
#include <mock_stdio.h>
#include <libcalcbase.h>

class addTest : public Test
{
};

// 正の数の加算テスト
TEST_F(addTest, test_1_add_2)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = add(1, 2, &result); // [手順] - add(1, 2, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(3, result);         // [確認] - 結果が 3 であること。
}

// 交換法則のテスト
TEST_F(addTest, test_2_add_1)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = add(2, 1, &result); // [手順] - add(2, 1, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(3, result);         // [確認] - 結果が 3 であること。
}

// NULLポインタのテスト
TEST_F(addTest, test_null_result)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(1, 2, NULL); // [手順] - add(1, 2, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERROR, rtc); // [確認] - 戻り値が CALC_ERROR であること。
}
```

---

## main関数を含むプログラムのテスト

main関数を含むプログラムをテストするには、リンカラップ機能を使用します。

### リンカラップ機能とは

GCCの `-Wl,--wrap=main` オプションを使用すると:

- `main` 関数は `__wrap_main` として定義される
- 元の `main` 関数は `__real_main` として参照可能になる
- `gtest_wrapmain` ライブラリが `__wrap_main` を提供し、Google Test を起動

### テストコードの構造

```cpp
#include <testfw.h>
#include <mock_stdio.h>
#include <mock_calcbase.h>

class addTest : public Test
{
};

TEST_F(addTest, less_argc)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"addTest", "1"}; // [状態] - main() に与える引数を、"1" **(不足)** とする。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_NE(0, rtc); // [確認] - main() の戻り値が 0 以外であること。
}

TEST_F(addTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calcbase mock_calcbase;
    int argc = 3;
    const char *argv[] = {"addTest", "1", "2"}; // [状態] - main() に与える引数を、"1", "2" とする。

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, add(1, 2, _))
        .WillOnce([](int, int, int *result) {
            *result = 3;
            return CALC_SUCCESS;
        }); // [Pre-Assert確認] - add(1, 2, &result) が 1 回呼び出されること。
            // [Pre-Assert手順] - add(1, 2, &result) にて result に 3 を設定し、CALC_SUCCESS を返す。

    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
        .WillOnce(DoDefault()); // [Pre-Assert確認] - printf() が 1 回呼び出され、内容が "3\n" であること。

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - main() の戻り値が 0 であること。
}
```

### NiceMock とは

`NiceMock<T>` は、未設定のモック呼び出しを許容するラッパーです。

- **NiceMock**: 未設定の呼び出しを許容 (警告なし)
- **NaggyMock**: 未設定の呼び出しで警告 (テストは通る)
- **StrictMock**: 未設定の呼び出しでテスト失敗

```cpp
// 例: stdio のモックは多数の関数があるため、
// 全てを EXPECT_CALL で設定するのは煩雑
// NiceMock を使うことで、必要な呼び出しだけを検証できる
NiceMock<Mock_stdio> mock_stdio;
```

---

## Makefileの作成

### テストコード用 Makefile

テストディレクトリに以下のファイルを作成します。

#### Makefile (標準テンプレート)

`test/src/calc/libcalcbaseTest/addTest/Makefile` は標準テンプレートをそのまま使用します。

```makefile
# Makefile テンプレート
# すべての最終階層 Makefile で使用する標準テンプレート
# 本ファイルの編集は禁止する。makepart.mk を作成して拡張・カスタマイズすること。

# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (ビルドテンプレートより前に include)
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

##### makepart.mk の内容は、このタイミングで処理される #####

# ビルドテンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makemain.mk
```

#### makepart.mk (プロジェクト固有の設定)

`test/src/calc/libcalcbaseTest/addTest/makepart.mk` にプロジェクト固有の設定を記述します。

```makefile
# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/libsrc/calcbase/add.c
```

### main関数テスト用 Makefile

#### Makefile (標準テンプレート)

`test/src/calc/main/addTest/Makefile` は標準テンプレートをそのまま使用します。

```makefile
# Makefile テンプレート
# すべての最終階層 Makefile で使用する標準テンプレート
# 本ファイルの編集は禁止する。makepart.mk を作成して拡張・カスタマイズすること。

# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (ビルドテンプレートより前に include)
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

##### makepart.mk の内容は、このタイミングで処理される #####

# ビルドテンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makemain.mk
```

#### makepart.mk (プロジェクト固有の設定)

`test/src/calc/main/addTest/makepart.mk` にプロジェクト固有の設定を記述します。

```makefile
# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/src/add/add.c

# エントリーポイントの変更
# テスト対象のソースファイルにある main() は直接実行されず、
# テストコード内から __real_main() 経由で実行される
USE_WRAP_MAIN := 1

# ライブラリの指定
LIBS += mock_calcbase mock_libc
```

### Makefile のポイント

#### Makefile と makepart.mk の分離

- **Makefile**: 標準テンプレート（すべてのテストディレクトリで共通）
- **makepart.mk**: プロジェクト固有の設定を記述

この分離により、ビルドシステムの更新が容易になり、保守性が向上します。

#### ワークスペースフォルダの検出

Makefile テンプレート内で `find-up` 関数を使用して `.workspaceRoot` ファイルを検出し、プロジェクトルートを特定します。

```makefile
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))
```

#### テンプレートの読み込み順序

```makefile
# 1. prepare.mk を先に読み込む
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

# 2. makepart.mk の内容が処理される (変数設定など)

# 3. makemain.mk を最後に読み込む
include $(WORKSPACE_FOLDER)/makefw/makefiles/makemain.mk
```

#### main関数のラップ

- `USE_WRAP_MAIN := 1`: main関数をラップして `__real_main` として呼び出し可能にする
  - Linux では `-Wl,--wrap=main` オプションが自動的に設定される
  - Windows では適切なリンカオプションが自動的に設定される

#### ライブラリの指定

- `LIBS`: リンクするライブラリ (プレフィックス `-l` なしで指定)
  - `mock_xxxxx`: モックライブラリ
  - `mock_libc`: 標準C関数のモック (stdio等)

例:
```makefile
LIBS += mock_calcbase mock_libc
```

---

## テストの実行

### 全テストの実行

プロジェクトルートから:

```bash
# Linux / Windows 共通
cd test
make clean    # クリーンビルド
make          # ビルド
make test     # テスト実行
```

> **Windows の注意**: コマンドプロンプトで環境設定スクリプトを実行してから make を実行してください。

### 個別テストの実行

特定のテストディレクトリで:

```bash
# Linux / Windows 共通
cd test/src/calc/libcalcbaseTest/addTest
make test
```

### 特定のテストケースのみ実行

フィルター機能を使用:

```bash
# Linux
# 方法1: 環境変数で指定
export GTEST_FILTER=*test_1_add_2*
make test
export -n GTEST_FILTER  # フィルターを解除

# 方法2: makeコマンドに指定
make test GTEST_FILTER=*test_1_add_2*
```

```cmd
REM Windows (コマンドプロンプト)
REM 方法1: 環境変数で指定
set GTEST_FILTER=*test_1_add_2*
make test
set GTEST_FILTER=

REM 方法2: makeコマンドに指定
make test GTEST_FILTER=*test_1_add_2*
```

```powershell
# Windows (PowerShell)
# 方法1: 環境変数で指定
$env:GTEST_FILTER="*test_1_add_2*"
make test
Remove-Item Env:\GTEST_FILTER

# 方法2: makeコマンドに指定
make test GTEST_FILTER=*test_1_add_2*
```

### カバレッジレポートの生成

```bash
# Linux
cd test
make test  # テスト実行 (カバレッジデータ収集)

# gcovr でカバレッジレポート生成
gcovr --exclude-unreachable-branches
```

> **注意**: Windows でのコードカバレッジ取得は、使用するツールによって手順が異なります。
> MSVC 環境では、Visual Studio のコードカバレッジツールまたはサードパーティツールの使用を検討してください。

### テスト出力例

```
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from addTest
[ RUN      ] addTest.test_1_add_2
[       OK ] addTest.test_1_add_2 (0 ms)
[ RUN      ] addTest.test_2_add_1
[       OK ] addTest.test_2_add_1 (0 ms)
[----------] 2 tests from addTest (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```

---

## 実践例

### 例1: calcHandler 関数のモックを使用したテスト

`test/src/calc/main/calcTest/calcTest.cc`

```cpp
#include <testfw.h>
#include <mock_stdio.h>
#include <mock_calc.h>

using namespace testing;

class calcTest : public Test
{
};

// 引数不足のテスト
TEST_F(calcTest, less_argc)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"calcTest", "1"}; // [状態] - main() に与える引数を、"1" **(不足)** とする。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_NE(0, rtc); // [確認] - main() の戻り値が 0 以外であること。
}

// 正常系のテスト
TEST_F(calcTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calc mock_calc;
    int argc = 4;
    const char *argv[] = {"calcTest", "1", "+", "2"}; // [状態] - main() に与える引数を、"1", "+", "2" とする。

    // Pre-Assert
    EXPECT_CALL(mock_calc, calcHandler(CALC_KIND_ADD, 1, 2, _))
        .WillOnce([](int, int, int, int *result) {
            *result = 3;
            return CALC_SUCCESS;
        }); // [Pre-Assert確認] - calcHandler(CALC_KIND_ADD, 1, 2, &result) が 1 回呼び出されること。
            // [Pre-Assert手順] - calcHandler(CALC_KIND_ADD, 1, 2, &result) にて result に 3 を設定し、CALC_SUCCESS を返す。

    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
        .WillOnce(DoDefault()); // [Pre-Assert確認] - printf() が 1 回呼び出され、内容が "3\n" であること。

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - main() の戻り値が 0 であること。
}
```

このテストの makepart.mk:

```makefile
# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/src/calc/calc.c

# エントリーポイントの変更
USE_WRAP_MAIN := 1

# ライブラリの指定
LIBS += mock_calc mock_libc
```

### 例2: モックの高度な使い方

#### 呼び出し回数の検証

```cpp
TEST_F(MyTest, call_times_check)
{
    Mock_calcbase mock;

    // 期待: add が正確に 2 回呼ばれること
    EXPECT_CALL(mock, add(_, _))
        .Times(2)
        .WillRepeatedly(Return(0));

    // 実際の呼び出し
    add(1, 2);
    add(3, 4);
    // テスト終了時に自動検証される
}
```

#### 呼び出し順序の検証

```cpp
TEST_F(MyTest, call_order_check)
{
    Mock_calcbase mock;
    InSequence seq;  // 順序を保証

    EXPECT_CALL(mock, add(1, 2)).Times(1);
    EXPECT_CALL(mock, add(3, 4)).Times(1);

    // この順序で呼ばれる必要がある
    add(1, 2);
    add(3, 4);
}
```

#### 引数の詳細な検証

```cpp
TEST_F(MyTest, argument_check)
{
    Mock_stdio mock_stdio;

    // 第4引数が "Hello" という文字列であることを検証
    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("Hello\n")))
        .Times(1);

    printf("%s", "", "", "Hello\n");
}
```

#### 複数回呼び出しでの戻り値変化

```cpp
TEST_F(MyTest, multiple_returns)
{
    Mock_calcbase mock;

    EXPECT_CALL(mock, add(_, _))
        .WillOnce(Return(10))      // 1回目は 10
        .WillOnce(Return(20))      // 2回目は 20
        .WillRepeatedly(Return(0)); // 3回目以降は 0

    EXPECT_EQ(10, add(1, 2));  // 1回目
    EXPECT_EQ(20, add(3, 4));  // 2回目
    EXPECT_EQ(0, add(5, 6));   // 3回目以降
}
```

---

## ベストプラクティス

### 1. テストコードの構造化

4つのフェーズを明確にコメントで記述:

```cpp
TEST_F(MyTest, test_case)
{
    // Arrange
    // - テストデータの準備

    // Pre-Assert
    // - モックの期待動作設定

    // Act
    // - テスト対象の実行

    // Assert
    // - 結果の検証
}
```

### 2. テストケースの命名規則

テストケース名は具体的かつ分かりやすく:

- **Good**: `test_add_positive_numbers`
- **Good**: `test_main_with_insufficient_args`
- **Bad**: `test1`, `test_normal`

### 3. モックのスコープ

モックオブジェクトはテストケース内で宣言:

```cpp
TEST_F(MyTest, test_case)
{
    // このスコープ内でモックが有効
    Mock_calcbase mock;

    // テストコード
}
// スコープを抜けるとモックが破棄される
```

### 4. NiceMock の活用

複雑なモック (stdio等) は NiceMock を使用:

```cpp
// 多数の関数を持つモックは NiceMock で簡潔に
NiceMock<Mock_stdio> mock_stdio;

// 検証したい呼び出しのみ EXPECT_CALL で指定
EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("result\n")))
    .WillOnce(DoDefault());
```

### 5. テストの独立性

各テストケースは独立して実行可能にする:

- テストケース間で状態を共有しない
- グローバル変数の使用は最小限に
- SetUp/TearDown を活用

```cpp
class MyTest : public Test
{
protected:
    void SetUp() override
    {
        // 各テストケース実行前の初期化
    }

    void TearDown() override
    {
        // 各テストケース実行後のクリーンアップ
    }
};
```

### 6. エラーケースのテスト

正常系だけでなく異常系もテスト:

```cpp
// 正常系
TEST_F(MyTest, normal_case) { /* ... */ }

// 境界値
TEST_F(MyTest, boundary_case) { /* ... */ }

// エラーケース
TEST_F(MyTest, error_case) { /* ... */ }

// NULL ポインタ
TEST_F(MyTest, null_pointer_case) { /* ... */ }
```

### 7. コメントの活用

テストの意図を日本語コメントで明確に:

```cpp
// Act
int rtc = add(1, 2); // [手順] - add(1, 2) を呼び出す。

// Assert
EXPECT_EQ(3, rtc); // [確認] - 戻り値が 3 であること。
```

### 8. Makefileの保守性

Makefile と makepart.mk を分離:

- **Makefile**: 標準テンプレート（すべてのディレクトリで共通、編集禁止）
- **makepart.mk**: プロジェクト固有の設定（TEST_SRCS, LIBS, USE_WRAP_MAIN など）

共通処理はフレームワークに集約:

- `makefw/makefiles/prepare.mk`: 準備処理
- `makefw/makefiles/makemain.mk`: ビルドルール生成

この分離により、ビルドシステムの更新が容易になり、保守性が向上します。

### 9. カバレッジの確認

定期的にカバレッジを確認:

```bash
make test
gcovr --exclude-unreachable-branches --html --html-details -o coverage.html
```

### 10. CI/CD への統合

テストを自動化:

```bash
#!/bin/bash
# test-runner.sh (Linux)

cd test
make clean
make
make test

if [ $? -ne 0 ]; then
    echo "Tests failed!"
    exit 1
fi

echo "All tests passed!"
```

```cmd
@echo off
REM test-runner.bat (Windows)

cd test
make clean
make
make test

if %ERRORLEVEL% neq 0 (
    echo Tests failed!
    exit /b 1
)

echo All tests passed!
```

### 11. クロスプラットフォーム開発のヒント

異なるプラットフォームでコードを保守する際のベストプラクティス:

#### プラットフォーム固有のコードを最小限に

- できるだけ標準C言語の機能のみを使用
- プラットフォーム固有の処理は条件付きコンパイルで分離

```c
#ifdef _WIN32
    // Windows 固有の処理
#else
    // Linux 固有の処理
#endif
```

#### ビルドシステムの活用

- makefw サブモジュールがプラットフォーム検出を自動化
- 各プラットフォームに適したコンパイラオプションを自動設定
- Makefile は共通のテンプレートを使用

#### 定期的なクロスプラットフォームテスト

- 両方のプラットフォームで定期的にテストを実行
- CI/CD パイプラインで両環境をテスト
- プラットフォーム固有の問題を早期に発見

#### ドキュメントの明確化

- プラットフォーム固有の手順を明記
- 環境設定の要件を文書化
- トラブルシューティング情報を共有

---

## トラブルシューティング

### 共通の問題

#### リンクエラー: undefined reference

**原因**: 必要なライブラリがリンクされていない

**解決策**: Makefile の `LIBS` に追加

```makefile
LIBS += -lmock_calcbase -ltest_com
```

#### 多重定義エラー: multiple definition

**原因**: 同じシンボルが複数回定義されている

**解決策**:
- モック関数の実装ファイルを確認
- `TEST_SRCS` に同じソースを重複して追加していないか確認

#### テストが実行されない

**原因**: `__real_main` が未定義

**解決策**: makepart.mk に `USE_WRAP_MAIN := 1` を追加

```makefile
USE_WRAP_MAIN := 1
```

これにより、ビルドシステムが自動的にプラットフォームに応じた適切なリンカオプションを設定します。

#### モックが呼ばれない

**原因**: リンク順序の問題

**解決策**:
1. モックライブラリを makepart.mk の LIBS に追加
2. `TEST_SRCS` に実体のソースを含めない

```makefile
# モックを使う場合は実体のソースを含めない
# TEST_SRCS := $(WORKSPACE_FOLDER)/prod/calc/libsrc/calcbase/add.c  # NG

# モックライブラリをリンク (プレフィックス -l なし)
LIBS += mock_calcbase  # OK
```

### Windows 固有の問題

#### 環境変数が設定されていないエラー

**原因**: Visual Studio Build Tools の環境変数が設定されていない

**解決策**: コマンドプロンプトで環境設定スクリプトを実行

```cmd
call Add-MinGW-Path.cmd
call Add-VSBT-Env-x64.cmd
```

#### link.exe が見つからないエラー

**原因**: 環境設定スクリプトの実行順序が間違っている

**解決策**: 正しい順序でスクリプトを実行

```cmd
REM 正しい順序
call Add-MinGW-Path.cmd      # 1. MinGW を先に
call Add-VSBT-Env-x64.cmd    # 2. VSBT を後に
```

この順序により、MSVC の `link.exe` が MinGW の `link` より優先されます。

#### ビルドエラー: コンパイラオプションの違い

**原因**: GCC と MSVC ではコンパイラオプションが異なる

**解決策**: makefw サブモジュールのビルドフレームワークが自動的にプラットフォームを検出し、
適切なコンパイラオプションを設定します。Makefile の設定を確認してください。

---

## 参考資料

- [GoogleTest User's Guide](https://google.github.io/googletest/)
- [GoogleMock for Dummies](https://github.com/google/googletest/blob/main/docs/gmock_for_dummies.md)
- testfw サブモジュール内のドキュメント:
  - `testfw/docs-src/how-to-mock.md`
  - `testfw/docs-src/how-to-test.md`
  - `testfw/docs-src/about-test-phase.md`
  - `testfw/docs-src/how-to-expect.md`
  - `testfw/docs-src/how-to-extern.md`

---

## まとめ

このチュートリアルでは、以下の内容を説明しました:

1. **クロスプラットフォーム対応**: Linux (GCC) と Windows (MSVC) の両方でシームレスに動作するテスト環境
2. **環境構築**: Linux と Windows それぞれの環境設定手順
3. **テストフレームワークの構造**: Google Test を使用した C言語のテスト環境
4. **モックの作成**: ヘッダー、クラス、関数の3段階でのモック実装
   - WEAK_ATR 属性を使用したモック関数の実装
   - ON_CALL を使用したデフォルト動作の設定
5. **関数のテスト**: 通常の関数のユニットテスト方法
   - ステータスコードとポインタ経由の結果取得パターン
6. **main関数のテスト**: リンカラップ機能を使用した main関数のテスト
   - `USE_WRAP_MAIN := 1` による自動設定
7. **Makefileの作成**: Makefile と makepart.mk の分離による保守性の向上
   - 標準テンプレート（Makefile）とプロジェクト固有の設定（makepart.mk）の分離
   - makefw サブモジュールによるクロスプラットフォーム対応
8. **実践例**: 実際のコードを使った具体的なテスト例
9. **トラブルシューティング**: プラットフォーム固有の問題と解決方法

これらの知識を活用して、Linux/Windows 両対応の堅牢で保守性の高いテストコードを作成してください。
