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
├── testfw/                           # テストフレームワーク (サブモジュール)
│   ├── cmnd/                        # テスト支援コマンド
│   ├── include/                     # フレームワーク提供のモック (stdio等)
│   ├── include_override/            # オーバーライド用ヘッダー
│   ├── libsrc/                      # フレームワーク提供のモック実装
│   └── makefiles/                   # Makefile テンプレート
├── test/                             # テストコード (本プロジェクト固有)
│   ├── include/                     # プロジェクト固有のモックヘッダー
│   │   ├── mock_calc.h             # calcHandlerのモック
│   │   └── mock_calcbase.h         # add関数のモック
│   ├── libsrc/                      # プロジェクト固有のモック実装
│   │   ├── mock_calc/              # calcHandler モックライブラリ
│   │   │   ├── mock_calc.cc
│   │   │   └── mock_calcHandler.cc
│   │   └── mock_calcbase/          # add モックライブラリ
│   │       ├── mock_add.cc
│   │       └── mock_calcbase.cc
│   └── src/                         # テストコード
│       └── calc/
│           ├── libcalcbaseTest/    # ライブラリ関数のテスト
│           │   └── addTest/
│           │       └── addTest.cc
│           └── main/                # main関数を含むプログラムのテスト
│               ├── addTest/
│               │   └── addTest.cc
│               └── calcTest/
│                   └── calcTest.cc
└── prod/                             # テスト対象のソースコード
    └── calc/
        ├── include/
        │   ├── libcalcbase.h       # 静的リンク用API
        │   └── libcalc.h           # 動的リンク用API
        ├── libsrc/
        │   └── calcbase/
        │       └── add.c           # add関数の実装
        └── src/
            └── add/
                └── add.c           # addコマンドのmain関数
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
#ifndef _MOCK_CALCBASE_H_
#define _MOCK_CALCBASE_H_

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
    MOCK_METHOD(int, add, (int, int));

    Mock_calcbase();
    ~Mock_calcbase();
};

// グローバルなモックインスタンスへのポインタ
extern Mock_calcbase *_mock_calcbase;

#endif // _MOCK_CALCBASE_H_
```

#### ポイント

- **MOCK_METHOD マクロ**: Google Mock の機能で、関数のシグネチャを定義します
- **extern宣言**: モックインスタンスをグローバルに共有するための宣言
- **C++クラス**: C言語の関数をC++のモッククラスでラップします

### ステップ2: モッククラスの実装

`test/libsrc/mock_xxxxx/mock_xxxxx.cc` にモッククラスの実装を作成します。

#### 例: mock_calcbase.cc

```cpp
#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_calcbase.h>

using namespace testing;

// グローバルインスタンスの実体
Mock_calcbase *_mock_calcbase = nullptr;

// コンストラクタ: デフォルト動作を設定
Mock_calcbase::Mock_calcbase()
{
    // デフォルト動作は設定しない (テストケースで個別に設定)
    // または、共通のデフォルト動作を設定:
    // ON_CALL(*this, add(_, _))
    //     .WillByDefault(Invoke([](int a, int b) { return a + b; }));

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
#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_calcbase.h>

using namespace testing;

// C言語の関数として実装
// この関数がテスト時に本物の add() 関数の代わりに呼ばれます
int add(int a, int b)
{
    int rtc = 0;

    // モックインスタンスが存在する場合はモックを呼び出す
    if (_mock_calcbase != nullptr)
    {
        rtc = _mock_calcbase->add(a, b);
    }

    // トレース出力 (デバッグ用)
    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s %d, %d", __func__, a, b);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> %d\n", rtc);
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

- **C言語関数**: `extern "C"` は不要 (`.cc` ファイルでも関数名が C++ にならない)
- **モックインスタンスチェック**: `_mock_calcbase != nullptr` でモックの有無を確認
- **トレース機能**: デバッグ時に関数呼び出しを確認できる

### モックライブラリの Makefile

`test/libsrc/mock_xxxxx/Makefile` を作成します。

```makefile
# ワークスペースのディレクトリ
WORKSPACE_FOLDER := $(shell \
    dir=`pwd`; \
    while [ "$$dir" != "/" ]; do \
        if [ -f "$$dir/.workspaceRoot" ]; then \
            echo $$dir; \
            break; \
        fi; \
        dir=$$(dirname $$dir); \
    done \
)

# 準備処理
include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk

# ライブラリ名
TARGET_LIB := libmock_calcbase.a

# ソースファイル
SRCS := \
	mock_calcbase.cc \
	mock_add.cc

# ライブラリ生成用の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/testfw/makefiles/makelib.mk
```

---

## 関数のテスト方法

通常の関数 (main を含まない) のテスト方法を説明します。

### テストコードの基本構造

```cpp
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <test_com.h>
#include <mock_stdio.h>

#include <libcalcbase.h>

using namespace testing;

// テストフィクスチャクラス
class addTest : public Test
{
};

// テストケース
TEST_F(addTest, test_1_add_2)
{
    // Arrange (状態設定)
    // - テストに必要な初期状態を準備

    // Pre-Assert (期待動作設定)
    // - モックの期待動作を設定 (この例では不要)

    // Act (実行)
    int rtc = add(1, 2); // [手順] - add(1, 2) を呼び出す。

    // Assert (検証)
    EXPECT_EQ(3, rtc); // [確認] - 戻り値が 3 であること。
}
```

### 実践例: add関数のテスト

`test/src/calc/libcalcbaseTest/addTest/addTest.cc`

```cpp
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <test_com.h>
#include <mock_stdio.h>

#include <libcalcbase.h>

using namespace testing;

class addTest : public Test
{
};

// 正の数の加算テスト
TEST_F(addTest, test_1_add_2)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(1, 2); // [手順] - add(1, 2) を呼び出す。

    // Assert
    EXPECT_EQ(3, rtc); // [確認] - 戻り値が 3 であること。
}

// 交換法則のテスト
TEST_F(addTest, test_2_add_1)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(2, 1); // [手順] - add(2, 1) を呼び出す。

    // Assert
    EXPECT_EQ(3, rtc); // [確認] - 戻り値が 3 であること。
}

// ゼロとの加算テスト
TEST_F(addTest, test_add_zero)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(5, 0); // [手順] - add(5, 0) を呼び出す。

    // Assert
    EXPECT_EQ(5, rtc); // [確認] - 戻り値が 5 であること。
}

// 負の数の加算テスト
TEST_F(addTest, test_add_negative)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = add(-3, 5); // [手順] - add(-3, 5) を呼び出す。

    // Assert
    EXPECT_EQ(2, rtc); // [確認] - 戻り値が 2 であること。
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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gtest_wrapmain.h>  // main関数ラップ用

#include <mock_stdio.h>
#include <mock_calcbase.h>

using namespace testing;

class addTest : public Test
{
};

TEST_F(addTest, less_argc)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"addTest", "1"}; // [状態] - 引数が不足

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // 元のmain関数を呼び出す

    // Assert
    EXPECT_EQ(1, rtc); // [確認] - エラーコード 1 が返ること
}

TEST_F(addTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calcbase mock_calcbase;
    int argc = 3;
    const char *argv[] = {"addTest", "1", "2"}; // [状態] - 正常な引数

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, add(1, 2))
        .WillOnce(Return(3)); // [Pre-Assert確認] - add(1, 2) が呼ばれ、3 を返す

    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
        .WillOnce(DoDefault()); // [Pre-Assert確認] - printf で "3\n" が出力される

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main関数を実行

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - 正常終了
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

`test/src/calc/libcalcbaseTest/addTest/Makefile`

```makefile
# ワークスペースのディレクトリ
WORKSPACE_FOLDER := $(shell \
    dir=`pwd`; \
    while [ "$$dir" != "/" ]; do \
        if [ -f "$$dir/.workspaceRoot" ]; then \
            echo $$dir; \
            break; \
        fi; \
        dir=$$(dirname $$dir); \
    done \
)

# 準備処理 (Makefile テンプレートより前に include)
include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk

# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/libsrc/calcbase/add.c

# リンクするライブラリの追加
# -ltest_com: テストフレームワークの共通機能
LIBS += -ltest_com

# src の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
```

### main関数テスト用 Makefile

`test/src/calc/main/addTest/Makefile`

```makefile
WORKSPACE_FOLDER := $(shell \
    dir=`pwd`; \
    while [ "$$dir" != "/" ]; do \
        if [ -f "$$dir/.workspaceRoot" ]; then \
            echo $$dir; \
            break; \
        fi; \
        dir=$$(dirname $$dir); \
    done \
)

include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk

# テスト対象のソースファイル (main関数を含む)
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/src/add/add.c

# リンクオプションの追加
# -Wl,--wrap=main により、main を __wrap_main に、
# 元のmainを __real_main に変更
LDCOMFLAGS += -Wl,--wrap=main

# ライブラリの追加
# -lgtest_wrapmain: __wrap_main() 経由でのテスト実施
# -lmock_libc: 標準C関数のモック (stdio等)
# -lmock_calcbase: add関数のモック
# -ltest_com: テストフレームワークの共通機能
LIBS += -lgtest_wrapmain -lmock_libc -lmock_calcbase -ltest_com

include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
```

### Makefile のポイント

#### ワークスペースフォルダの検出

```makefile
WORKSPACE_FOLDER := $(shell \
    dir=`pwd`; \
    while [ "$$dir" != "/" ]; do \
        if [ -f "$$dir/.workspaceRoot" ]; then \
            echo $$dir; \
            break; \
        fi; \
        dir=$$(dirname $$dir); \
    done \
)
```

`.workspaceRoot` ファイルを検出してプロジェクトルートを特定します。

#### テンプレートの読み込み順序

```makefile
# 1. prepare.mk を先に読み込む
include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk

# 2. 変数を設定

# 3. makesrc.mk または makelib.mk を最後に読み込む
include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
```

#### リンクオプション

- `LDCOMFLAGS`: リンカに渡すオプション
  - `-Wl,--wrap=main`: main関数のラップ
  - `-Wl,--wrap=関数名`: 他の関数のラップ

#### ライブラリの指定

- `LIBS`: リンクするライブラリ
  - `-lgtest_wrapmain`: main関数ラップ用
  - `-lmock_xxxxx`: モックライブラリ
  - `-ltest_com`: テスト共通機能

---

## テストの実行

### 全テストの実行

プロジェクトルートから:

```bash
# Linux / Windows 共通
cd test
make clean    # クリーンビルド
make all      # ビルド
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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gtest_wrapmain.h>

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
    const char *argv[] = {"calcTest", "1"}; // [状態] - 引数が不足

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv);

    // Assert
    EXPECT_EQ(1, rtc); // [確認] - エラーコード 1
}

// 正常系のテスト
TEST_F(calcTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calc mock_calc;
    int argc = 4;
    const char *argv[] = {"calcTest", "1", "+", "2"}; // [状態] - "1" "+" "2"

    // Pre-Assert
    EXPECT_CALL(mock_calc, calcHandler(CALC_KIND_ADD, 1, 2))
        .WillOnce(Return(3)); // [Pre-Assert確認] - calcHandler が呼ばれ 3 を返す

    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
        .WillOnce(DoDefault()); // [Pre-Assert確認] - "3\n" が出力される

    // Act
    int rtc = __real_main(argc, (char **)&argv);

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - 正常終了
}
```

このテストの Makefile:

```makefile
WORKSPACE_FOLDER := $(shell \
    dir=`pwd`; \
    while [ "$$dir" != "/" ]; do \
        if [ -f "$$dir/.workspaceRoot" ]; then \
            echo $$dir; \
            break; \
        fi; \
        dir=$$(dirname $$dir); \
    done \
)

include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk

# calcコマンドのmain関数を含むソース
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/src/calc/calc.c

LDCOMFLAGS += -Wl,--wrap=main

LIBS += -lgtest_wrapmain -lmock_libc -lmock_calc -ltest_com

include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
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

共通処理はテンプレートに集約:

- `testfw/makefiles/prepare.mk`: 準備処理
- `testfw/makefiles/makesrc.mk`: テスト実行ファイル生成
- `testfw/makefiles/makelib.mk`: ライブラリ生成

個別のMakefileには最小限の設定のみ記述。

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
make all
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
make all
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

**解決策**: Makefile に `--wrap=main` を追加 (Linux)

```makefile
LDCOMFLAGS += -Wl,--wrap=main
LIBS += -lgtest_wrapmain
```

#### モックが呼ばれない

**原因**: リンク順序の問題

**解決策**:
1. モックライブラリを先にリンク
2. `TEST_SRCS` に実体のソースを含めない

```makefile
# モックを使う場合は実体のソースを含めない
# TEST_SRCS := $(WORKSPACE_FOLDER)/prod/calc/libsrc/calcbase/add.c  # NG

# モックライブラリをリンク
LIBS += -lmock_calcbase  # OK
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
5. **関数のテスト**: 通常の関数のユニットテスト方法
6. **main関数のテスト**: リンカラップ機能を使用した main関数のテスト
7. **Makefileの作成**: テンプレートを活用した効率的なビルド設定
8. **実践例**: 実際のコードを使った具体的なテスト例
9. **トラブルシューティング**: プラットフォーム固有の問題と解決方法

これらの知識を活用して、Linux/Windows 両対応の堅牢で保守性の高いテストコードを作成してください。
