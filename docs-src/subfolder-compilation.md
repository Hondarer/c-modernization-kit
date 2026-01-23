# C 言語のサブディレクトリ対応

## 概要

このドキュメントは、makefw および testfw フレームワークにおける C 言語ソースコードのサブディレクトリ対応について説明します。

大規模な C 言語プロジェクトでは、ソースコードを機能別にサブディレクトリに分割することが一般的です。  
本フレームワークは、以下の観点からサブディレクトリ対応を提供します。

1. **ライブラリ**: サブディレクトリに配置したソースファイルを単一のライブラリにリンク
2. **コマンド (実行ファイル)**: サブディレクトリに配置したソースファイルを単一の実行ファイルにリンク
3. **テスト**: サブディレクトリごとのテストコードとテスト対象ソースの管理

## サブディレクトリ対応の仕様

### NO_LINK 変数

```makefile
# サブフォルダはコンパイルのみ
NO_LINK = 1
```

**動作:**

- `NO_LINK = 1` を設定したディレクトリでは、ソースファイルのコンパイルのみが行われます
- リンク処理 (ライブラリ生成や実行ファイル生成) はスキップされます
- コンパイル結果のオブジェクトファイル (`.o` / `.obj`) は、各サブディレクトリの `obj/` に配置されます

### オブジェクトファイルの自動収集

親ディレクトリ (リンクを行うディレクトリ) では、サブディレクトリのオブジェクトファイルが自動的に収集されます。

**makelibsrc_c_cpp.mk / makesrc_c_cpp.mk より:**

```makefile
# サブディレクトリの obj ディレクトリを再帰的に検索して、オブジェクトファイルを収集
ifeq ($(OS),Windows_NT)
    SUBDIR_OBJS := $(shell find . -type d -name obj -not -path "./obj" -exec find {} -maxdepth 1 -type f -name "*.obj" \; 2>/dev/null)
else
    SUBDIR_OBJS := $(shell find . -type d -name obj -not -path "./obj" -exec find {} -maxdepth 1 -type f -name "*.o" \; 2>/dev/null)
endif
OBJS += $(SUBDIR_OBJS)
```

これにより、親ディレクトリでリンクを実行すると、サブディレクトリのオブジェクトファイルも含めてリンクされます。

### 再帰的 make 処理

`makemain.mk` は、Makefile を含むサブディレクトリを自動検出し、再帰的に make を実行します。

```makefile
# サブディレクトリの検出 (GNUmakefile/makefile/Makefileを含むディレクトリのみ)
SUBDIRS := $(sort $(dir $(wildcard */GNUmakefile */makefile */Makefile)))

# サブディレクトリの再帰的make処理
ifneq ($(SUBDIRS),)
    .PHONY: $(SUBDIRS)
    $(SUBDIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

    # 主要なターゲットにサブディレクトリ依存を追加 (サブディレクトリを先に処理)
    default build clean test run restore rebuild: $(SUBDIRS)
endif
```

## ライブラリのサブディレクトリ対応

### ディレクトリ構造

```text
prod/subfolder-sample/
+-- lib/                                    # ビルド済みライブラリ出力先
|   +-- liblibsubfolder-sample.so
+-- libsrc/
    +-- Makefile                            # 再帰ビルド用
    +-- makepart.mk
    +-- libsubfolder-sample/
        +-- Makefile                        # ライブラリ本体 (リンク実行)
        +-- makepart.mk                     # LIB_TYPE = shared 設定
        +-- func.c                          # ルートのソースファイル
        +-- obj/
        |   +-- func.o
        +-- subfolder_a/
        |   +-- Makefile                    # サブディレクトリ (NO_LINK = 1)
        |   +-- makepart.mk
        |   +-- func_a.c
        |   +-- obj/
        |       +-- func_a.o
        +-- subfolder_b/
            +-- Makefile                    # サブディレクトリ (NO_LINK = 1)
            +-- makepart.mk
            +-- func_b.c
            +-- obj/
                +-- func_b.o
```

### 設定ファイルの内容

**libsubfolder-sample/makepart.mk (親ディレクトリ):**

```makefile
ifeq ($(OS),Windows_NT)
    # Windows
    CFLAGS   += /DSUBFOLDER_SAMPLE_EXPORTS
    CXXFLAGS += /DSUBFOLDER_SAMPLE_EXPORTS
endif

# 動的ライブラリとして生成
LIB_TYPE = shared
```

**subfolder_a/makepart.mk (サブディレクトリ):**

```makefile
# サブフォルダはコンパイルのみ
NO_LINK = 1
```

**subfolder_b/makepart.mk (サブディレクトリ):**

```makefile
# サブフォルダはコンパイルのみ
NO_LINK = 1
```

### ビルドの流れ

1. `make` を `libsubfolder-sample/` で実行
2. `makemain.mk` がサブディレクトリ `subfolder_a/` と `subfolder_b/` を検出
3. 各サブディレクトリで `make` が再帰的に実行される
4. サブディレクトリでは `NO_LINK = 1` によりコンパイルのみ実行
5. 親ディレクトリで全オブジェクトファイルを収集してライブラリを生成

**生成されるライブラリ:**

```text
prod/subfolder-sample/lib/liblibsubfolder-sample.so
```

このライブラリには、`func.o`、`func_a.o`、`func_b.o` が含まれます。

## コマンド (実行ファイル) のサブディレクトリ対応

### ディレクトリ構造

```text
prod/subfolder-sample/
+-- bin/                                    # ビルド済み実行ファイル出力先
|   +-- sample-app
+-- src/
    +-- Makefile                            # 再帰ビルド用
    +-- makepart.mk
    +-- sample-app/
        +-- Makefile                        # 実行ファイル本体 (リンク実行)
        +-- sample-app.h                    # ヘッダーファイル
        +-- main.c                          # メインソースファイル
        +-- obj/
        |   +-- main.o
        +-- subfolder_a/
        |   +-- Makefile                    # サブディレクトリ (NO_LINK = 1)
        |   +-- makepart.mk
        |   +-- helper_a.c
        |   +-- obj/
        |       +-- helper_a.o
        +-- subfolder_b/
            +-- Makefile                    # サブディレクトリ (NO_LINK = 1)
            +-- makepart.mk
            +-- helper_b.c
            +-- obj/
                +-- helper_b.o
```

### ソースコードの例

**main.c:**

```c
#include <stdio.h>
#include "sample-app.h"

int main(void)
{
    int a = 10;
    int b = 20;

    printf("Testing subfolder make for src\n");
    printf("helper_a(%d) = %d\n", a, helper_a(a));
    printf("helper_b(%d) = %d\n", b, helper_b(b));
    printf("helper_a(%d) + helper_b(%d) = %d\n", a, b, helper_a(a) + helper_b(b));

    return 0;
}
```

**subfolder_a/helper_a.c:**

```c
#include "../sample-app.h"

int helper_a(int value)
{
    return value * 2;
}
```

### 設定ファイルの内容

**subfolder_a/makepart.mk:**

```makefile
# サブフォルダはコンパイルのみ
NO_LINK = 1
```

### ビルドの流れ

ライブラリと同様に、サブディレクトリのオブジェクトファイルが親ディレクトリに収集されてリンクされます。

**生成される実行ファイル:**

```text
prod/subfolder-sample/bin/sample-app
```

## テストのサブディレクトリ対応

### ディレクトリ構造

```text
test/src/subfolder-sample/
+-- Makefile                                # 再帰ビルド用
+-- makepart.mk
+-- subfolder-sampleTest/
    +-- Makefile                            # テスト本体 (リンク・テスト実行)
    +-- makepart.mk                         # TEST_SRCS 設定 (ルートのテスト対象)
    +-- subfolder-sampleTest.cc             # ルートのテストコード
    +-- bin/
    |   +-- subfolder-sampleTest            # テスト実行ファイル
    +-- obj/
    |   +-- func.o                          # テスト対象ソースのオブジェクト
    |   +-- subfolder-sampleTest.o          # テストコードのオブジェクト
    +-- results/                            # テスト結果出力先
    |   +-- all_tests/                      # 全体テスト結果
    |   |   +-- summary.log
    |   |   +-- coverage.xml
    |   |   +-- func.c.gcov.txt
    |   |   +-- func_a.c.gcov.txt
    |   |   +-- func_b.c.gcov.txt
    |   +-- subfolder_sampleTest.test_func/
    |   |   +-- results.log
    |   |   +-- func.c.gcov.txt
    |   +-- subfolder_sampleTest_a.test_func_a/
    |   |   +-- results.log
    |   |   +-- func_a.c.gcov.txt
    |   +-- subfolder_sampleTest_b.test_func_b/
    |       +-- results.log
    |       +-- func_b.c.gcov.txt
    +-- subfolder_a/
    |   +-- Makefile                        # サブディレクトリ (NO_LINK = 1)
    |   +-- makepart.mk                     # TEST_SRCS 設定 (サブディレクトリのテスト対象)
    |   +-- subfolder-sampleTest_a.cc       # サブディレクトリのテストコード
    |   +-- obj/
    |       +-- func_a.o
    |       +-- subfolder-sampleTest_a.o
    +-- subfolder_b/
        +-- Makefile                        # サブディレクトリ (NO_LINK = 1)
        +-- makepart.mk                     # TEST_SRCS 設定 (サブディレクトリのテスト対象)
        +-- subfolder-sampleTest_b.cc       # サブディレクトリのテストコード
        +-- obj/
            +-- func_b.o
            +-- subfolder-sampleTest_b.o
```

### 設定ファイルの内容

**subfolder-sampleTest/makepart.mk (親ディレクトリ):**

```makefile
# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/subfolder-sample/libsrc/libsubfolder-sample/func.c
```

**subfolder_a/makepart.mk (サブディレクトリ):**

```makefile
# サブフォルダはコンパイルのみ
NO_LINK = 1

# テスト対象のソースファイル
# NOTE: 上位フォルダで TEST_SRCS を指定している場合、テスト対象ソースが重複しないように留意すること。
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_a/func_a.c
```

### テストコードの例

**subfolder-sampleTest.cc (親ディレクトリ):**

```cpp
#include <testfw.h>
#include <subfolder-sample.h>

class subfolder_sampleTest : public Test
{
};

TEST_F(subfolder_sampleTest, test_func)
{
    // Act
    int rtc = func(); // [手順] - func() を呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - func() から 0 が返されること。
}
```

**subfolder_a/subfolder-sampleTest_a.cc (サブディレクトリ):**

```cpp
#include <testfw.h>
#include <subfolder-sample.h>

class subfolder_sampleTest_a : public Test
{
};

TEST_F(subfolder_sampleTest_a, test_func_a)
{
    // Act
    int rtc = func_a(); // [手順] - func_a() を呼び出す。

    // Assert
    EXPECT_EQ(1, rtc); // [確認] - func_a() から 1 が返されること。
}
```

### TEST_SRCS の自動収集

テスト実行スクリプト (`exec_test_c_cpp.sh`) は、サブディレクトリの `makepart.mk` から `TEST_SRCS` を自動的に収集します。

```bash
# TEST_SRCS が空の場合、サブフォルダの makepart.mk から TEST_SRCS を収集
if [ -z "$TEST_SRCS" ]; then
    for makepart in $(find . -mindepth 2 -name "makepart.mk" 2>/dev/null); do
        subdir_test_srcs=$(grep -A10 "^TEST_SRCS" "$makepart" 2>/dev/null | \
            grep -v "^TEST_SRCS" | grep -v "^#" | grep -v "^--$" | \
            sed "s|\\\$(WORKSPACE_FOLDER)|$WORKSPACE_FOLDER|g" | \
            xargs 2>/dev/null)
        TEST_SRCS="$TEST_SRCS $subdir_test_srcs"
    done
fi
```

これにより、各サブディレクトリで個別にテスト対象を指定しつつ、全体のカバレッジレポートを生成できます。

## テストレポートの構造

`make test` を実行すると、`results/` ディレクトリにテストレポートが生成されます。

### results/ ディレクトリ構造

```text
results/
+-- all_tests/                              # 全体テスト結果
|   +-- summary.log                         # テストサマリー
|   +-- coverage.xml                        # 全体カバレッジ (Cobertura形式)
|   +-- func.c.gcov.txt                     # func.c のカバレッジ詳細
|   +-- func_a.c.gcov.txt                   # func_a.c のカバレッジ詳細
|   +-- func_b.c.gcov.txt                   # func_b.c のカバレッジ詳細
|   +-- lcov/                               # HTML カバレッジレポート (Linux)
+-- <テストクラス>.<テスト名>/              # 個別テスト結果
    +-- results.log                         # テスト実行ログ
    +-- <ソースファイル>.gcov.txt           # 個別テストのカバレッジ
```

### summary.log の内容例

```text
Test start on Sat Jan 24 07:55:31 JST 2026.
----
MD5 checksums of files in TEST_SRCS:
8c18e38566df7a9630b40ca18881a5d4  prod/subfolder-sample/libsrc/libsubfolder-sample/func.c
----
subfolder_sampleTest_a.test_func_a	PASSED
subfolder_sampleTest_b.test_func_b	PASSED
subfolder_sampleTest.test_func	PASSED
Test results:
----
Total tests	3
Passed		3
Warning(s)	0
Failed		0

------------------------------------------------------------------------------
                             Code Coverage Report
------------------------------------------------------------------------------
File                                       Lines    Exec  Cover   Missing
------------------------------------------------------------------------------
func.c                                         2       2   100%
func_a.c                                       2       2   100%
func_b.c                                       2       2   100%
------------------------------------------------------------------------------
TOTAL                                          6       6   100%
------------------------------------------------------------------------------
```

### 個別テスト結果 (results.log) の内容例

```text
Running test: subfolder_sampleTest.test_func on bin/subfolder-sampleTest
----
## テスト項目

### 状態

### 手順

- func() を呼び出す。

### 確認内容 (1)

- func() から 0 が返されること。
----
TEST_F(subfolder_sampleTest, test_func)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = func(); // [手順] - func() を呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - func() から 0 が返されること。
}
----
./bin/subfolder-sampleTest --gtest_filter=subfolder_sampleTest.test_func
Running main() from .../gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from subfolder_sampleTest
[ RUN      ] subfolder_sampleTest.test_func
[       OK ] subfolder_sampleTest.test_func (0 ms)
[----------] 1 test from subfolder_sampleTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

### カバレッジファイル (*.gcov.txt) の内容例

```text
        -:    0:Source:func.c
        -:    0:Graph:.../obj/func.gcno
        -:    0:Data:.../obj/func.gcda
        -:    0:Runs:1
        -:    0:Programs:1
        -:    1:#include <subfolder-sample.h>
        -:    2:
        1:    3:SUBFOLDER_SAMPLE_API int WINAPI func(void)
        -:    4:{
        1:    5:    return 0;
        -:    6:}
```

各行の先頭の数字は実行回数を示します:
- `-`: 実行対象外の行 (コメント、空行など)
- `1` 以上: 実行された回数

## サブディレクトリ対応のベストプラクティス

### 1. Makefile の配置

各サブディレクトリに Makefile を配置します。内容は標準テンプレートをそのまま使用します。

```makefile
# Makefile テンプレート
# すべての最終階層 Makefile で使用する標準テンプレート
# 本ファイルの編集は禁止する。makepart.mk を作成して拡張・カスタマイズすること。

find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

include $(WORKSPACE_FOLDER)/makefw/makefiles/makemain.mk
```

### 2. makepart.mk でのカスタマイズ

サブディレクトリでは必ず `NO_LINK = 1` を設定します。

```makefile
# サブフォルダはコンパイルのみ
NO_LINK = 1
```

テストの場合は、`TEST_SRCS` も設定します。

```makefile
# サブフォルダはコンパイルのみ
NO_LINK = 1

# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/.../subfolder_a/func_a.c
```

### 3. TEST_SRCS の重複回避

テストでは、親ディレクトリとサブディレクトリで `TEST_SRCS` が重複しないように注意してください。重複すると、同じソースファイルが複数回コンパイルされ、リンクエラーが発生します。

### 4. ヘッダーファイルの参照

サブディレクトリから親ディレクトリのヘッダーファイルを参照する場合は、相対パスを使用します。

```c
#include "../sample-app.h"
```

または、`makepart.mk` で `INCDIR` を設定します。

```makefile
INCDIR += $(WORKSPACE_FOLDER)/prod/subfolder-sample/src/sample-app
```

## まとめ

| 観点 | 設定 | 説明 |
|------|------|------|
| ライブラリ | `NO_LINK = 1` | サブディレクトリではコンパイルのみ、親でリンク |
| コマンド | `NO_LINK = 1` | サブディレクトリではコンパイルのみ、親でリンク |
| テスト | `NO_LINK = 1` + `TEST_SRCS` | サブディレクトリごとにテスト対象を指定 |
| テストレポート | `results/` | 個別テスト結果と全体カバレッジを出力 |
