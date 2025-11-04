# クロスプラットフォーム対応設計

## はじめに

本ドキュメントは、doxygen-sample プロジェクト全体を、Linux 環境から Windows 環境でもビルド可能にするためのクロスプラットフォーム対応の設計を示します。

現在、prod と test 配下のビルドシステムは Linux 専用として構築されており、GCC/G++ と Linux のシェルコマンドに依存しています。Windows 環境でビルドを実現するため、Git for Windows に付属している MinGW のコマンド、GNU Make、および、ポータブル版 Visual Studio Build Tools (MSVC) を活用したクロスプラットフォーム対応を行います。

### 設計の目標

- prod, test 配下のソースコードを Linux と Windows の両環境でビルドできるようにする
- doxyfw でのドキュメント生成を Linux と Windows の両環境で実行できるようにする
- testfw と doxyfw サブモジュール自体をクロスプラットフォーム対応する (他のプロジェクトでも利用可能にする)
- 既存の Linux 環境でのビルドを完全に維持する (動作に変更を加えない)
- Windows 環境では MSVC (cl.exe, lib.exe, link.exe) を使用する
- Git for Windows に付属している MinGW 環境のシェルコマンドを活用して、既存スクリプトの互換性を保つ

### 設計の基本方針

- testfw サブモジュールの Makefile テンプレートに OS 判定による条件分岐を追加する
- doxyfw サブモジュールの Makefile は MinGW の bash を前提とし、そのまま維持する
- prod と test の makeflags.mk に OS 判定による条件分岐を追加する
- Linux 環境では既存の動作を完全に維持する (条件分岐の else 節に既存コードを配置)
- PowerShell での互換処理は作成しない (MinGW の bash で既存スクリプトを実行)

## 現状分析

### プロジェクト全体の構造

プロジェクトは、以下のサブモジュールと構成で構築されています。

```text
doxygen-sample/
|-- .workspaceRoot       (ワークスペースルートマーカー)
|-- doxyfw/              (Doxygen フレームワーク: git submodule)
|   |-- Makefile        (ドキュメント生成)
|   |-- Doxyfile        (Doxygen 基本設定)
|   |-- doxybook-config.json
|   +-- templates/      (カスタムテンプレート、前処理・後処理スクリプト)
|-- testfw/              (テストフレームワーク: git submodule)
|   |-- makefiles/      (ビルド用 Makefile テンプレート)
|   |   |-- prepare.mk  (前処理: defines 取得、makeflags.mk 読み込み)
|   |   |-- makelibsrc.mk (ライブラリビルド)
|   |   +-- makesrc.mk  (実行ファイルビルド、テスト実行)
|   +-- cmnd/           (シェルスクリプト群)
|-- prod/                (プロダクションコード)
|   |-- Makefile        (トップレベル)
|   |-- makeflags.mk    (共通コンパイラフラグ)
|   +-- calc/           (計算ライブラリのサンプル)
|       |-- Makefile
|       |-- include/libcalc.h
|       |-- libsrc/     (ライブラリソース)
|       |   |-- Makefile
|       |   |-- makeflags.mk (TARGETDIR 設定)
|       |   +-- calc/
|       |       |-- Makefile (testfw/makefiles/makelibsrc.mk を使用)
|       |       |-- add.c
|       |       +-- calcHandler.c
|       +-- src/        (アプリケーションソース)
|           |-- Makefile
|           |-- makeflags.mk (libcalc へのリンク設定)
|           +-- add/
|               |-- Makefile (testfw/makefiles/makesrc.mk を使用)
|               +-- add.c
+-- test/                (テストコード)
    |-- Makefile        (トップレベル)
    |-- makeflags.mk    (共通コンパイラフラグ、LINK_TEST=1)
    |-- libsrc/         (テスト用ライブラリ)
    |   |-- Makefile
    |   |-- makeflags.mk
    |   +-- mock_calc/
    |       +-- Makefile (testfw/makefiles/makelibsrc.mk を使用)
    +-- src/            (テストソース)
        |-- Makefile
        +-- calc/
            |-- Makefile
            +-- libcalc/
                +-- addTest/
                    +-- Makefile (testfw/makefiles/makesrc.mk を使用)
```

### testfw サブモジュールの役割

testfw サブモジュールは、以下の Makefile テンプレートを提供します。

#### testfw/makefiles/prepare.mk

各 Makefile から最初に include され、以下の処理を行います。

- `c_cpp_properties.json` から defines を取得し、Makefile 変数に設定
- `testfw/cmnd/get_defines.sh` を実行
- `testfw/cmnd/get_files_lang.sh` でファイルエンコーディングを取得
- ワークスペースルートから Makefile のディレクトリまでの `makeflags.mk` を検索し、親から子の順に include

#### testfw/makefiles/makelibsrc.mk

ライブラリをビルドするためのテンプレートです。

- 静的ライブラリ (.a) または共有ライブラリ (.so) を生成
- GCC/G++ でコンパイル、ar でアーカイブ作成
- オブジェクトファイル拡張子: .o
- シンボリックリンク、コピー、inject、filter の処理に対応
- 依存関係の自動生成 (.d ファイル)

#### testfw/makefiles/makesrc.mk

実行ファイルをビルドし、テストを実行するためのテンプレートです。

- 実行ファイルを生成
- GCC/G++ でコンパイル・リンク
- オブジェクトファイル拡張子: .o
- テストライブラリ (gtest, gmock, gcov) へのリンク
- カバレッジ取得 (gcov, lcov, gcovr)
- シンボリックリンク、コピー、inject、filter の処理に対応

### doxyfw サブモジュールの役割

doxyfw サブモジュールは、Doxygen ドキュメント生成機能を提供します。

#### doxyfw/Makefile

`make docs` コマンドで以下の処理を実行します。

1. Doxygen で C ソースから XML と HTML を生成
2. `templates/preprocess.sh` で XML を前処理
3. Doxybook2 で XML を Markdown に変換
4. `templates/postprocess.sh` で Markdown を後処理

シェルコマンド (`command -v`, `mkdir -p`, `mktemp`, `cat`, `rm -rf`, `cp -rp`) を使用しています。

### prod と test の makeflags.mk

prod/makeflags.mk と test/makeflags.mk は、共通のコンパイラフラグを定義します。

- `CCOMFLAGS`: C コンパイラフラグ (GCC 用: -Wall, -Wextra, -g, -std=c99 など)
- `CPPCOMFLAGS`: C++ コンパイラフラグ (GCC 用: -Wall, -Wextra, -g など)
- `LDCOMFLAGS`: リンカフラグ
- `LIBSDIR`: ライブラリディレクトリ
- test/makeflags.mk のみ: `LINK_TEST = 1` (テストライブラリをリンク)

### Linux 依存箇所の特定

以下の箇所が Linux 環境に依存しています。

#### シェルコマンド

```{.makefile caption="testfw/makefiles/makelibsrc.mk, makesrc.mk"}
SHELL := /bin/bash
```

```{.makefile caption="WORKSPACE_FOLDER 検索"}
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

使用されている主なシェルコマンド

- `pwd` (カレントディレクトリの取得)
- `dirname` (親ディレクトリの取得)
- `ln -s` (シンボリックリンクの作成)
- `mkdir -p` (ディレクトリの作成)
- `mktemp` (一時ファイルの作成)
- `nkf` (文字コード変換)
- `command -v` (コマンドの存在確認)

#### コンパイラとツールチェーン

```{.makefile caption="testfw/makefiles/makelibsrc.mk, makesrc.mk"}
CC := gcc
CPP := g++
```

```{.makefile caption="testfw/makefiles/makelibsrc.mk (アーカイブ作成)"}
ar rvs $@ $(OBJS)
```

```{.makefile caption="testfw/makefiles/makesrc.mk (リンク)"}
$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(TEST_LIBS)
```

使用されているツール

- `gcc` (C コンパイラ)
- `g++` (C++ コンパイラ)
- `ar` (アーカイブ作成)
- 出力: `.a` (静的ライブラリ), `.so` (共有ライブラリ), 拡張子なし (実行ファイル)
- オブジェクトファイル: `.o`

#### testfw のシェルスクリプト

```{.makefile caption="testfw/makefiles/prepare.mk"}
DEFINES := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_defines.sh)
FILES_LANG := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_files_lang.sh)
INCDIR := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_include_paths.sh)
```

依存しているスクリプト

- `testfw/cmnd/get_defines.sh` (defines の取得)
- `testfw/cmnd/get_files_lang.sh` (ファイルエンコーディングの取得)
- `testfw/cmnd/get_include_paths.sh` (include パスの取得)

#### doxyfw のシェルコマンド

```{.makefile caption="doxyfw/Makefile"}
command -v doxygen
mkdir -p ../docs/doxygen
mktemp
cat Doxyfile ../Doxyfile.part > $$TEMP_DOXYFILE
rm -f $$TEMP_DOXYFILE
```

## Windows 対応の課題

### コンパイラとツールチェーンの違い

| 項目               | Linux (GCC)           | Windows (MSVC)         |
|:-------------------|:----------------------|:-----------------------|
| C コンパイラ       | gcc                   | cl.exe                 |
| C++ コンパイラ     | g++                   | cl.exe                 |
| リンカー           | gcc/g++               | link.exe               |
| 静的ライブラリ生成 | ar                    | lib.exe                |
| 静的ライブラリ拡張 | .a                    | .lib                   |
| 共有ライブラリ拡張 | .so                   | .dll                   |
| 実行ファイル拡張   | なし                  | .exe                   |
| オブジェクト拡張   | .o                    | .obj                   |

MSVC は GCC とコンパイラオプションの書式が異なります。

```text
GCC: -Wall -Wextra -g -fPIC -std=c99 -Iinclude -Llib -lfoo
MSVC: /W4 /Zi /TC /Iinclude foo.lib /link /LIBPATH:lib
```

### シェルコマンドの違い

Linux のシェルコマンド (`/bin/bash`) は、Windows のネイティブコマンドプロンプト (cmd.exe) では利用できません。

ただし、本プロジェクトでは、Git for Windows の MinGW 環境を利用可能とするため、`bash` をはじめ、`pwd`, `dirname`, `ln`, `mkdir`, `mktemp` などの Linux コマンドを使用できます。

```{.cmd caption="Add-MinGW-Path.cmd"}
REM Git for Windows の MinGW コマンドを利用可能にする
```

この環境設定により、既存のシェルスクリプトや Makefile 内のシェルコマンドを、ほぼそのまま利用できます。

### パスと環境の違い

| 項目                     | Linux                | Windows                |
|:-------------------------|:---------------------|:-----------------------|
| パス区切り文字           | /                    | \ (GNU Make では / も可) |
| ルートディレクトリ       | /                    | C:\, D:\ など          |
| 環境変数参照 (シェル)    | $VAR, ${VAR}         | %VAR% (cmd), $env:VAR (PowerShell) |
| 行末                     | LF                   | CRLF (ただし MinGW は LF 対応) |

GNU Make は Windows でもパス区切りに `/` を使えるため、この点は大きな問題にはなりません。

### testfw と doxyfw への影響範囲

testfw と doxyfw サブモジュールは、複数のプロジェクトで共有されています。他のプロジェクトもクロスプラットフォーム対応が必要なため、testfw と doxyfw 自体をクロスプラットフォーム対応する方針とします。

- testfw: Makefile テンプレートに OS 判定を追加し、Linux と Windows の両環境で動作するようにする
- doxyfw: MinGW の bash を前提とし、既存のシェルコマンドをそのまま利用する

## 対処方針

### OS 判定による条件分岐

Makefile 内で OS を判定し、コンパイラやツールを切り替えます。

```{.makefile caption="OS 判定の例"}
ifeq ($(OS),Windows_NT)
    # Windows 環境
    CC := cl
    AR := lib
    LIBEXT := .lib
    OBJEXT := .obj
    EXEEXT := .exe
else
    # Linux 環境
    CC := gcc
    AR := ar
    LIBEXT := .a
    OBJEXT := .o
    EXEEXT :=
endif
```

`$(OS)` 環境変数は、Windows では `Windows_NT` が設定されます。Linux では通常未定義です。

### MinGW 環境の活用

CLAUDE.md に記載されているように、Windows 環境では以下のスクリプトを **この順序で** 実行して、MinGW コマンドと MSVC を利用可能にします。

```{.cmd caption="呼び出し例 (重要: この順序で実行)"}
call Add-MinGW-Path.cmd
call set-vsbt-env-x64.bat
```

**注意**: この順序で実行することで、MSVC の `link.exe` が MinGW の `link` コマンドより PATH の優先順位が高くなります。逆の順序で実行すると、リンク時にエラーが発生します。

これにより、以下が可能になります。

- `bash`, `pwd`, `dirname` などの Linux コマンドが使える
- `sh` コマンドで既存のシェルスクリプトを実行できる
- `cl.exe`, `link.exe`, `lib.exe` が利用できる
- testfw/cmnd/ 配下のシェルスクリプトがそのまま動作する
- doxyfw の Makefile がそのまま動作する

### MSVC との統合

#### コンパイラフラグの変換

GCC と MSVC ではコンパイラフラグが異なるため、makeflags.mk に条件分岐を追加して、OS に応じたフラグを設定します。

| 目的                 | GCC                | MSVC              |
|:---------------------|:-------------------|:------------------|
| 警告レベル           | -Wall -Wextra      | /W4               |
| デバッグ情報         | -g                 | /Zi               |
| 最適化               | -O2                | /O2               |
| C 標準バージョン     | -std=c99           | /TC               |
| C++ 標準バージョン   | -std=c++17         | /std:c++17        |
| インクルードパス     | -Ipath             | /Ipath            |
| ライブラリパス       | -Lpath             | (リンカオプション) |
| ライブラリ指定       | -lfoo              | foo.lib           |

#### ライブラリ生成の変換

GCC の `ar` コマンドと MSVC の `lib.exe` は、呼び出し方が異なります。

```{.makefile caption="GCC (ar)"}
ar rvs libfoo.a foo.o bar.o
```

```{.makefile caption="MSVC (lib.exe)"}
lib /OUT:foo.lib foo.obj bar.obj
```

既存の testfw Makefile テンプレートを修正し、OS 判定によって適切なコマンドを実行します。

#### リンクの変換

GCC と MSVC では、リンクの方法が異なります。

```{.makefile caption="GCC (gcc/g++)"}
gcc -o program program.o -Llib -lfoo
```

```{.makefile caption="MSVC (link.exe)"}
link /OUT:program.exe program.obj foo.lib /LIBPATH:lib
```

testfw/makefiles/makesrc.mk を修正し、OS 判定によって適切なコマンドを実行します。

#### オブジェクトファイルとライブラリの拡張子

| コンパイラ | オブジェクト | 静的ライブラリ | 実行ファイル |
|:-----------|:-------------|:---------------|:-------------|
| GCC        | .o           | .a             | なし         |
| MSVC       | .obj         | .lib           | .exe         |

Makefile 内で、拡張子も OS に応じて切り替えます。

## 実装設計

### 変更が必要なファイルの全体像

以下のファイルを変更します。

#### testfw サブモジュール

- `testfw/makefiles/prepare.mk` (OS 判定追加、Linux は変更なし)
- `testfw/makefiles/makelibsrc.mk` (OS 判定でコンパイラ、アーカイバ、拡張子切り替え)
- `testfw/makefiles/makesrc.mk` (OS 判定でコンパイラ、リンカ、拡張子切り替え)

#### doxyfw サブモジュール

- `doxyfw/Makefile` (MinGW の bash を前提とし、変更不要)

#### メインプロジェクト

- `prod/makeflags.mk` (OS 判定でコンパイラフラグ切り替え)
- `test/makeflags.mk` (OS 判定でコンパイラフラグ切り替え)

### testfw/makefiles/prepare.mk の変更

OS 判定を追加しますが、Linux 環境では既存の動作を完全に維持します。

```{.makefile caption="testfw/makefiles/prepare.mk (変更後のイメージ)"}
# OS 判定 (Windows では追加処理が必要な場合のための準備)
ifeq ($(OS),Windows_NT)
    # Windows 環境
    # 現時点では追加処理なし (MinGW の bash を利用)
else
    # Linux 環境 (既存コードをそのまま維持)
endif

# c_cpp_properties.json から defines を得る
DEFINES := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_defines.sh)
$(foreach define, $(DEFINES), $(eval $(define) = 1))

# ソースファイルのエンコード指定から LANG を得る
FILES_LANG := $(shell sh $(WORKSPACE_FOLDER)/testfw/cmnd/get_files_lang.sh)

# makeflags.mk の検索と読み込み
MAKEFLAGS_MK := $(shell \
    dir=`pwd`; \
    while [ "$$dir" != "/" ]; do \
        if [ -f "$$dir/makeflags.mk" ]; then \
            echo "$$dir/makeflags.mk"; \
        fi; \
        if [ -f "$$dir/.workspaceRoot" ]; then \
            break; \
        fi; \
        dir=$$(dirname $$dir); \
    done \
)

MAKEFLAGS_MK := $(foreach mkfile, $(shell seq $(words $(MAKEFLAGS_MK)) -1 1), $(word $(mkfile), $(MAKEFLAGS_MK)))
$(foreach makeflags, $(MAKEFLAGS_MK), $(eval include $(makeflags)))
```

prepare.mk は、MinGW の bash を前提とし、既存のシェルコマンドをそのまま利用します。

### testfw/makefiles/makelibsrc.mk の変更

OS 判定による条件分岐を追加し、Linux と Windows で異なるコンパイラとツールを使用します。

#### OS 判定とツール設定

```{.makefile caption="testfw/makefiles/makelibsrc.mk (OS 判定部分)"}
ifeq ($(OS),Windows_NT)
    # Windows 環境 (MSVC)
    ifneq ($(CC),cl)
        CC := cl
    endif
    CPP := cl
    AR := lib
    OBJEXT := .obj
    LIBEXT := .lib
else
    # Linux 環境 (GCC) - 既存コードをそのまま維持
    ifneq ($(CC),g++)
        CC := gcc
    endif
    CPP := g++
    AR := ar
    OBJEXT := .o
    LIBEXT := .a
endif
```

#### ライブラリターゲット名の設定

```{.makefile caption="testfw/makefiles/makelibsrc.mk (ターゲット名設定)"}
ifeq ($(TARGETDIR),)
    TARGETDIR := $(WORKSPACE_FOLDER)/test/lib
endif

ifeq ($(TARGET),)
    ifeq ($(BUILD),shared)
        ifeq ($(OS),Windows_NT)
            TARGET := $(shell basename `pwd`).dll
        else
            TARGET := lib$(shell basename `pwd`).so
        endif
    else
        ifeq ($(OS),Windows_NT)
            TARGET := $(shell basename `pwd`)$(LIBEXT)
        else
            TARGET := lib$(shell basename `pwd`)$(LIBEXT)
        endif
    endif
endif
```

#### ビルドルール

```{.makefile caption="testfw/makefiles/makelibsrc.mk (ビルドルール)"}
SRCS_C := $(wildcard *.c)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(patsubst %.c, %$(OBJEXT), $(SRCS_C))))

ifeq ($(OS),Windows_NT)
    # MSVC のビルドルール
    ifeq ($(BUILD),shared)
        $(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
            $(CC) /LD /Fe$@ $(OBJS)
    else
        $(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
            $(AR) /OUT:$@ $(OBJS)
    endif

    $(OBJDIR)/%$(OBJEXT): %.c $(OBJDIR)/%.d | $(OBJDIR)
        $(CC) $(CFLAGS) /c /Fo$@ $<
else
    # GCC のビルドルール (既存コードをそのまま維持)
    ifeq ($(BUILD),shared)
        $(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
            $(CC) -shared -o $@ $(OBJS)
    else
        $(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
            $(AR) rvs $@ $(OBJS)
    endif

    $(OBJDIR)/%$(OBJEXT): %.c $(OBJDIR)/%.d | $(OBJDIR)
        set -o pipefail; LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf
endif
```

### testfw/makefiles/makesrc.mk の変更

OS 判定による条件分岐を追加し、Linux と Windows で異なるコンパイラとリンカを使用します。

#### OS 判定とツール設定

```{.makefile caption="testfw/makefiles/makesrc.mk (OS 判定部分)"}
ifeq ($(OS),Windows_NT)
    # Windows 環境 (MSVC)
    ifneq ($(CC),cl)
        CC := cl
    endif
    CPP := cl
    LD := link
    OBJEXT := .obj
    EXEEXT := .exe
else
    # Linux 環境 (GCC) - 既存コードをそのまま維持
    ifneq ($(CC),g++)
        CC := gcc
    endif
    CPP := g++
    ifneq ($(LD),)
        LD := g++
    endif
    OBJEXT := .o
    EXEEXT :=
endif
```

#### 実行ファイルターゲット名の設定

```{.makefile caption="testfw/makefiles/makesrc.mk (ターゲット名設定)"}
ifeq ($(TARGETDIR),)
    TARGETDIR := .
endif

ifeq ($(TARGET),)
    TARGET := $(shell basename `pwd`)
endif

TARGET_WITH_EXT := $(TARGET)$(EXEEXT)
```

#### ビルドルール

```{.makefile caption="testfw/makefiles/makesrc.mk (ビルドルール)"}
SRCS_C := $(wildcard *.c)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(patsubst %.c, %$(OBJEXT), $(SRCS_C))))

ifeq ($(OS),Windows_NT)
    # MSVC のビルドルール
    ifndef NO_LINK
        $(TARGETDIR)/$(TARGET_WITH_EXT): $(OBJS) $(LIBSFILES) | $(TARGETDIR)
            $(LD) /OUT:$@ $(OBJS) $(LIBS) $(TEST_LIBS)
    else
        $(OBJS): $(LIBSFILES)
    endif

    $(OBJDIR)/%$(OBJEXT): %.c $(OBJDIR)/%.d | $(OBJDIR)
        $(CC) $(CFLAGS) /c /Fo$@ $<
else
    # GCC のビルドルール (既存コードをそのまま維持)
    ifndef NO_LINK
        $(TARGETDIR)/$(TARGET_WITH_EXT): $(OBJS) $(LIBSFILES) | $(TARGETDIR)
            set -o pipefail; LANG=$(FILES_LANG) $(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(TEST_LIBS) -fdiagnostics-color=always 2>&1 | nkf
    else
        $(OBJS): $(LIBSFILES)
    endif

    $(OBJDIR)/%$(OBJEXT): %.c $(OBJDIR)/%.d | $(OBJDIR)
        @set -o pipefail; if echo $(TEST_SRCS) | grep -q $(notdir $<); then \
            echo LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS_TEST) -coverage -D_IN_TEST_SRC_ -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
            LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS_TEST) -coverage -D_IN_TEST_SRC_ -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
        else \
            echo LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
            LANG=$(FILES_LANG) $(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $< -fdiagnostics-color=always 2>&1 | nkf; \
        fi
endif
```

### prod/makeflags.mk の変更

OS 判定による条件分岐を追加し、Linux 用と Windows 用のコンパイラフラグを分けます。

```{.makefile caption="prod/makeflags.mk (変更後のイメージ)"}
ifeq ($(OS),Windows_NT)
    # Windows 環境 (MSVC)
    CCOMFLAGS = /W4 /Zi /TC /nologo
    CPPCOMFLAGS = /W4 /Zi /EHsc /std:c++17 /nologo
    LDCOMFLAGS =
else
    # Linux 環境 (GCC) - 既存コードをそのまま維持
    CCOMFLAGS=\
        -Wall \
        -Wextra \
        -Wcast-align \
        -Wdisabled-optimization \
        -Wfloat-equal \
        -Wformat=2 \
        -Winit-self \
        -Winline \
        -Winvalid-pch \
        -Wmissing-format-attribute \
        -Wmissing-noreturn \
        -Wpacked \
        -Wpadded \
        -Wredundant-decls \
        -Wstrict-aliasing=2 \
        -Wswitch-default \
        -Wswitch-enum \
        -Wundef \
        -Wvariadic-macros \
        -Wshadow \
        -Wmissing-declarations \
        -Wmissing-prototypes \
        -Wstrict-prototypes \
        -Wsequence-point \
        -Wpointer-arith \
        -Wbad-function-cast \
        -Wunsafe-loop-optimizations \
        -Wnested-externs \
        -Wcast-qual \
        -Wint-to-pointer-cast \
        -Wpointer-to-int-cast \
        -Wimplicit-fallthrough=0 \
        -Wformat-overflow=0 \
        -Wunknown-pragmas \
        -W \
        -std=c99

    CPPCOMFLAGS=\
        -Wall \
        -Wextra \
        -Wcast-align \
        -Wdisabled-optimization \
        -Wfloat-equal \
        -Wformat=2 \
        -Winit-self \
        -Winline \
        -Winvalid-pch \
        -Wmissing-format-attribute \
        -Wmissing-noreturn \
        -Wpacked \
        -Wpadded \
        -Wredundant-decls \
        -Wstrict-aliasing=2 \
        -Wswitch-default \
        -Wswitch-enum \
        -Wundef \
        -Wvariadic-macros \
        -Wshadow \
        -Wmissing-declarations \
        -Wsequence-point \
        -Wpointer-arith \
        -Wunsafe-loop-optimizations \
        -Wcast-qual \
        -Wint-to-pointer-cast \
        -Wimplicit-fallthrough=0 \
        -Wformat-overflow=0 \
        -Wunknown-pragmas \
        -W

    LDCOMFLAGS =
endif

LIBSDIR =
```

### test/makeflags.mk の変更

prod/makeflags.mk と同様に、OS 判定による条件分岐を追加します。

```{.makefile caption="test/makeflags.mk (変更後のイメージ)"}
ifeq ($(OS),Windows_NT)
    # Windows 環境 (MSVC)
    CCOMFLAGS = /W4 /Zi /TC /nologo
    CPPCOMFLAGS = /W4 /Zi /EHsc /std:c++17 /nologo
    LDCOMFLAGS =
else
    # Linux 環境 (GCC) - 既存コードをそのまま維持
    CCOMFLAGS=\
        -Wall \
        -Wextra \
        ... (既存のフラグ) \
        -std=c99

    CPPCOMFLAGS=\
        -Wall \
        -Wextra \
        ... (既存のフラグ)

    LDCOMFLAGS =
endif

LIBSDIR =

# テストフレームワークをリンクする
LINK_TEST = 1
```

### doxyfw/Makefile の変更

doxyfw/Makefile は、MinGW の bash を前提とし、変更不要です。

既存のシェルコマンド (`command -v`, `mkdir -p`, `mktemp`, `cat`, `rm -f`, `cp -rp`) は、MinGW 環境でそのまま動作します。

## 実装手順

クロスプラットフォーム対応は、以下の手順で段階的に実装します。

### ステップ 1: testfw/makefiles/prepare.mk の更新

OS 判定を追加しますが、Linux では既存コードをそのまま維持します。

### ステップ 2: testfw/makefiles/makelibsrc.mk の更新

OS 判定による条件分岐を追加し、Linux と Windows で異なるコンパイラとツールを使用します。

- コンパイラ、アーカイバ、拡張子の切り替え
- ライブラリターゲット名の設定
- ビルドルールの分岐

### ステップ 3: testfw/makefiles/makesrc.mk の更新

OS 判定による条件分岐を追加し、Linux と Windows で異なるコンパイラとリンカを使用します。

- コンパイラ、リンカ、拡張子の切り替え
- 実行ファイルターゲット名の設定
- ビルドルールの分岐

### ステップ 4: prod/makeflags.mk の更新

OS 判定による条件分岐を追加し、Linux 用と Windows 用のコンパイラフラグを分けます。

### ステップ 5: test/makeflags.mk の更新

prod/makeflags.mk と同様に、OS 判定による条件分岐を追加します。

### ステップ 6: Linux 環境でのテスト

既存の Linux 環境で、変更後もビルドが成功することを確認します。

### ステップ 7: Windows 環境でのテスト

Windows 環境で、MSVC を使ったビルドが成功することを確認します。

## テストと検証

### Linux 環境でのテスト

既存の Linux 環境で、変更後もビルドが成功することを確認します。

#### prod のビルド確認

```{.bash caption="Linux での prod ビルド確認"}
cd prod
make clean
make all
```

生成物の確認

- `prod/calc/lib/libcalc.a` (静的ライブラリ)
- `prod/calc/src/add/add` (実行ファイル)

実行確認

```{.bash caption="実行確認"}
./prod/calc/src/add/add 10 20
```

期待される出力

```text
30
```

#### test のビルド確認

```{.bash caption="Linux での test ビルド確認"}
cd test
make clean
make all
make test
```

#### doxyfw のドキュメント生成確認

```{.bash caption="Linux での doxyfw 確認"}
cd doxyfw
make clean
make docs
```

生成物の確認

- `docs/doxygen/` (HTML ドキュメント)
- `docs-src/doxybook/` (Markdown ドキュメント)

### Windows 環境でのテスト

Windows 環境で、MSVC を使ったビルドが成功することを確認します。

#### 前提条件

```{.cmd caption="環境設定 (重要: この順序で実行)"}
call Add-MinGW-Path.cmd
call set-vsbt-env-x64.bat
```

#### prod のビルド確認

```{.cmd caption="Windows での prod ビルド確認"}
cd prod
make clean
make all
```

生成物の確認

- `prod/calc/lib/calc.lib` (静的ライブラリ)
- `prod/calc/lib/calc.pdb` (ライブラリのデバッグ情報)
- `prod/calc/src/add/add.exe` (実行ファイル)
- `prod/calc/src/add/add.pdb` (実行ファイルのデバッグ情報)

実行確認

```{.cmd caption="実行確認"}
prod\calc\src\add\add.exe 10 20
```

期待される出力

```text
30
```

#### test のビルド確認

```{.cmd caption="Windows での test ビルド確認"}
cd test
make clean
make all
make test
```

#### doxyfw のドキュメント生成確認

```{.cmd caption="Windows での doxyfw 確認"}
cd doxyfw
make clean
make docs
```

生成物の確認

- `docs/doxygen/` (HTML ドキュメント)
- `docs-src/doxybook/` (Markdown ドキュメント)

### エラー対応

ビルドエラーが発生した場合、以下の点を確認します。

#### コンパイラフラグの確認

MSVC と GCC でフラグの互換性がない場合があります。

- MSVC 固有の警告を抑制する (`/wd番号`)
- GCC 固有のフラグを MSVC で使わない (`-fPIC` など)

#### パスの確認

Windows 環境でパスにスペースが含まれる場合、引用符で囲みます。

```{.makefile caption="パスに引用符を追加"}
INCDIR := "C:/Program Files/include"
```

#### ライブラリ名の確認

GCC では `-lfoo` で `libfoo.a` または `libfoo.so` を検索しますが、MSVC では `foo.lib` を直接指定します。

```{.makefile caption="ライブラリ名の切り替え"}
ifeq ($(OS),Windows_NT)
    LIBS := calc.lib
else
    LIBS := -lcalc
endif
```

#### シンボリックリンクの確認

Windows では、管理者権限なしにシンボリックリンクを作成できない場合があります。testfw の inject/filter 機能がシンボリックリンクに依存している場合、コピーに変更する必要があるかもしれません。

## PoC 実装で得られた知見

Windows 用の PoC Makefile を実装して動作確認を行った結果、以下の知見が得られました。

### 環境設定スクリプトの実行順序

環境設定スクリプトは、**必ず以下の順序で実行する必要があります**。

```{.cmd caption="環境設定の正しい順序"}
call Add-MinGW-Path.cmd
call set-vsbt-env-x64.bat
```

この順序で実行することで、MSVC の `link.exe` が MinGW の `link` コマンドより PATH の優先順位が高くなります。

逆の順序で実行すると、リンク時に MinGW の `link` コマンドが呼び出され、以下のようなエラーが発生します。

```text
link: extra operand 'calc.lib'
Try 'link --help' for more information.
```

### UTF-8 ソースコードへの対応

ソースコードが UTF-8 で記述されている場合、MSVC のコンパイル時に `/utf-8` オプションを追加する必要があります。

```{.makefile caption="UTF-8 オプションの追加"}
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /I$(WORKSPACE_FOLDER)/prod/calc/include
```

このオプションを指定しないと、以下の警告が発生します。

```text
warning C4819: The file contains a character that cannot be represented
in the current code page (932). Save the file in Unicode format to prevent data loss
```

### PDB ファイルの適切な配置

MSVC のデバッグ情報ファイル (PDB) は、以下のように配置します。

#### ライブラリの PDB

ライブラリファイル (.lib) と同じディレクトリに配置します。

```{.makefile caption="ライブラリの PDB 配置"}
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /FS /Fd$(TARGETDIR)/calc.pdb /I...
```

生成結果

```text
prod/calc/lib/
├── calc.lib
└── calc.pdb
```

#### 実行ファイルの PDB

実行ファイルと同じディレクトリに配置します。コンパイル時の中間 PDB は obj ディレクトリに配置します。

```{.makefile caption="実行ファイルの PDB 配置"}
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /Fd$(OBJDIR)/add.pdb /I...
LDFLAGS := /DEBUG /PDB:$(TARGETDIR)/add.pdb /LIBPATH:...
```

生成結果

```text
prod/calc/src/add/
├── add.exe
├── add.pdb     (リンク時の PDB、実行ファイルと同じ場所)
└── obj/
    ├── add.obj
    └── add.pdb (コンパイル時の PDB、obj ディレクトリ)
```

### インクリメンタルリンク情報ファイル (ILK) の配置

MSVC のインクリメンタルリンク情報ファイル (.ilk) は、中間ファイルとして obj ディレクトリに配置します。

```{.makefile caption="ILK ファイルの配置"}
LDFLAGS := /DEBUG /PDB:$(TARGETDIR)/add.pdb /INCREMENTAL /ILK:$(OBJDIR)/add.ilk /LIBPATH:...
```

生成結果

```text
prod/calc/src/add/
├── add.exe
├── add.pdb
└── obj/
    ├── add.obj
    ├── add.pdb
    └── add.ilk (インクリメンタルリンク情報)
```

### WORKSPACE_FOLDER のパス変換

Makefile 内で Unix スタイルのパス (`/d/Users/...`) を取得した場合、MSVC はこのパスを認識できません。`cygpath -w` を使用して Windows スタイルのパス (`D:\Users\...`) に変換する必要があります。

```{.makefile caption="WORKSPACE_FOLDER のパス変換"}
WORKSPACE_FOLDER := $(shell \
    dir=`pwd`; \
    while [ "$$dir" != "/" ]; do \
        if [ -f "$$dir/.workspaceRoot" ]; then \
            cygpath -w "$$dir" 2>/dev/null || echo $$dir; \
            break; \
        fi; \
        dir=$$(dirname $$dir); \
    done \
)
```

`cygpath -w` が利用できない環境では、`echo $$dir` にフォールバックします。

### /FS オプションの追加

複数のソースファイルが同じ PDB ファイルに同時に書き込む場合、`/FS` オプションを追加して同期を保証する必要があります。

```{.makefile caption="/FS オプションの追加"}
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /FS /Fd$(TARGETDIR)/calc.pdb /I...
```

このオプションを指定しないと、以下のエラーが発生することがあります。

```text
fatal error C1041: cannot open program database 'calc.pdb';
if multiple CL.EXE write to the same .PDB file, please use /FS
```

### Makefile の変数定義順序

Makefile 内で `$(OBJDIR)` や `$(TARGETDIR)` を参照する場合、これらの変数は参照前に定義されている必要があります。

```{.makefile caption="変数定義の順序"}
# ディレクトリ設定 (OS 判定より前に定義)
OBJDIR := obj
TARGETDIR := $(WORKSPACE_FOLDER)/prod/calc/lib

# OS 判定
ifeq ($(OS),Windows_NT)
    # ここで $(OBJDIR) や $(TARGETDIR) を参照可能
    CFLAGS := ... /Fd$(TARGETDIR)/calc.pdb ...
endif
```

### ディレクトリの依存関係

PDB ファイルを TARGETDIR に出力する場合、コンパイルルールに TARGETDIR の依存関係を追加する必要があります。

```{.makefile caption="ディレクトリの依存関係"}
$(OBJDIR)/%$(OBJEXT): %.c | $(OBJDIR) $(TARGETDIR)
    $(CC) $(CFLAGS) /c /Fo$@ $<
```

これにより、コンパイル前に TARGETDIR が作成されることが保証されます。

### clean ターゲットの改善

Windows 環境では、clean ターゲットで PDB ファイルも削除する必要があります。ILK ファイルは obj ディレクトリに配置されるため、`rm -rf $(OBJDIR)` で削除されます。

#### ライブラリの clean ターゲット

```{.makefile caption="ライブラリの clean ターゲット"}
.PHONY: clean
clean:
    rm -rf $(OBJDIR)
    rm -f $(TARGETDIR)/$(TARGET)
ifeq ($(OS),Windows_NT)
    rm -f $(TARGETDIR)/*.pdb
endif
```

#### 実行ファイルの clean ターゲット

```{.makefile caption="実行ファイルの clean ターゲット"}
.PHONY: clean
clean:
    rm -rf $(OBJDIR)
    rm -f $(TARGETDIR)/$(TARGET)
ifeq ($(OS),Windows_NT)
    rm -f $(TARGETDIR)/*.pdb
endif
```

obj ディレクトリの削除により、コンパイル時の PDB と ILK ファイルが削除されます。TARGETDIR に配置されたリンク時の PDB ファイルのみ、明示的に削除します。

## まとめ

本設計では、testfw、doxyfw、prod、test の全体を Linux と Windows の両環境でビルド・動作可能にするため、以下の方針で対応します。

### 採用する方針

- OS 判定による条件分岐で、コンパイラとツールを切り替える
- MinGW 環境を活用し、既存のシェルスクリプトとシェルコマンドを利用する
- testfw サブモジュールの Makefile テンプレートに OS 判定を追加する
- doxyfw サブモジュールの Makefile は MinGW の bash を前提とし、変更不要
- prod/makeflags.mk と test/makeflags.mk に OS 判定を追加する
- Linux 環境では既存の動作を完全に維持する (条件分岐の else 節に既存コードを配置)

### メリット

- testfw と doxyfw がクロスプラットフォーム対応され、他のプロジェクトでも利用できる
- Linux と Windows で同じ Makefile を使用できるため、メンテナンス性が向上する
- MinGW 環境により、既存のシェルスクリプトとシェルコマンドを活用できる
- MSVC を使用することで、Windows ネイティブなバイナリを生成できる
- Linux の動作は完全に維持される (既存コードをそのまま維持)

### 注意点

- testfw の Makefile テンプレートは複数のプロジェクトで共有されるため、変更時には慎重に行う
- MSVC と GCC でコンパイラフラグが異なるため、makeflags.mk の条件分岐を慎重に設定する
- Windows 環境では、環境設定スクリプトを **必ず正しい順序** (`Add-MinGW-Path.cmd` → `set-vsbt-env-x64.bat`) で実行する必要がある
- doxyfw は MinGW の bash を前提としているため、Windows では MinGW 環境が必須
- テストライブラリ (gtest, gmock, gcov) の Windows 版が必要 (LINK_TEST=1 の場合)
- ソースコードが UTF-8 の場合、MSVC では `/utf-8` オプションが必要
- PDB ファイルと ILK ファイルを適切なディレクトリに配置する必要がある

### 次のステップ

設計に基づき、以下の順序で実装を進めます。

1. testfw/makefiles/prepare.mk の更新
2. testfw/makefiles/makelibsrc.mk の更新
3. testfw/makefiles/makesrc.mk の更新
4. prod/makeflags.mk の更新
5. test/makeflags.mk の更新
6. Linux 環境でのテスト (prod, test, doxyfw)
7. Windows 環境でのテスト (prod, test, doxyfw)

これにより、プロジェクト全体のクロスプラットフォーム対応が完了します。
