# クロスプラットフォームビルドシステムの実装

## 概要

このドキュメントは、c-modernization-kit プロジェクトのクロスプラットフォームビルドシステムの設計、実装、および使用方法を説明します。

本プロジェクトは、Linux/Windows クロスプラットフォームビルドシステムを実現しています。GCC と MSVC (Microsoft Visual C++) との両方に対応し、単一の Makefile で Linux と Windows の両環境でビルドできます。

## 前提条件

### Linux 環境

標準的な開発ツールが必要です:

- GCC コンパイラ
- GNU Make

### Windows 環境

注: 環境変数は VS Code 起動時に自動設定済みの前提です。以下の環境が利用可能である必要があります：

1. ポータブル版 Visual Studio Build Tools
   - MSVC コンパイラ (`cl.exe`)
   - MSVC リンカー (`link.exe`)

2. Git for Windows (MinGW)
   - GNU Make (`make.exe`)
   - 各種 Unix コマンド

3. 環境設定の実行順序 (手動設定時のみ)

   ```cmd
   call Add-MinGW-Path.cmd
   call Add-VSBT-Env-x64.cmd
   ```

## ビルド方法

### ビルド実行

prod/ ディレクトリでビルドします:

Windows:

```cmd
cd prod
make
```

Linux:

```bash
cd prod
make
```

このコマンドで以下がビルドされます:

ライブラリ / Libraries:

| ライブラリ | Windows | Linux | 説明 |
|-----------|---------|-------|------|
| libcalcbase | `prod/calc/lib/calcbase.lib` | `prod/calc/lib/libcalcbase.a` | 基本計算関数ライブラリ (静的ライブラリ) |
| libcalc | `prod/calc/lib/calc.dll` + `calc.lib` | `prod/calc/lib/libcalc.so` | 計算ハンドラーライブラリ (動的ライブラリ、calcbase を内部に静的リンク) |

コマンド / Commands:

| コマンド | Windows | Linux | リンクライブラリ |
|---------|---------|-------|----------------|
| add | `prod/calc/bin/add.exe` | `prod/calc/bin/add` | calcbase のみ |
| calc | `prod/calc/bin/calc.exe` | `prod/calc/bin/calc` | calc のみ |
| shared-and-static-calc | `prod/calc/bin/shared-and-static-calc.exe` | `prod/calc/bin/shared-and-static-calc` | calc + calcbase (両方) |

.NET コマンド / .NET Commands:

| コマンド | Windows | Linux | 依存ライブラリ |
|---------|---------|-------|--------------|
| CalcApp | `prod/calc.net/bin/CalcApp.exe` | `prod/calc.net/bin/CalcApp` | CalcLib (libcalc の .NET ラッパー) |

重要:
- libcalc は動的ライブラリとして実装されており、calcbase を内部に静的リンクします
- shared-and-static-calc は、コマンドにおいて動的ライブラリと静的ライブラリの両方をリンクする実装例です
- 実行ファイルの出力先は `prod/calc/src/makepart.mk` および `prod/calc.net/src/makepart.mk` で `OUTPUT_DIR` として設定されています

### テストのビルドと実行

testfw/ ディレクトリおよび test/ ディレクトリでテストをビルド・実行します:

Windows:

```cmd
cd testfw
make
cd ..\test
make
```

Linux:

```bash
cd testfw
make
cd ../test
make
```

テストは Google Test フレームワークを使用しています:

- 単体テスト: ライブラリ関数の単体テスト (`test/src/calc/libcalcbaseTest/`) および コマンドの単体テスト (`test/src/calc/main/`)
- モックライブラリ: テスト用のモック実装 (`test/libsrc/mock_*/`)

### クリーンアップ

**プロダクションコードのクリーンアップ:**

```bash
cd prod/calc
make clean
```

**テストコードのクリーンアップ:**

```bash
cd test
make clean
```

## ファイル構成

### プロジェクト構造

```text
c-modernization-kit/
+-- makefw/                              # Makefile フレームワーク (testfw から切り出し)
|   +-- makefiles/
|   |   +-- makelibsrc.mk               # ライブラリビルド用共通テンプレート
|   |   +-- makesrc.mk                  # 実行ファイルビルド用共通テンプレート
|   |   +-- prepare.mk                  # 準備処理
|   |   +-- _*.mk                       # 内部処理用ファイル
|   +-- docs-src/                       # フレームワーク技術ドキュメント
+-- prod/calc/
|   +-- Makefile                        # トップレベル Makefile (再帰ビルド)
|   +-- lib/                            # ビルド済みライブラリ
|   +-- bin/                            # ビルド済み実行ファイル
|   +-- libsrc/
|   |   +-- Makefile                    # libsrc 配下の再帰ビルド
|   |   +-- makepart.mk                # ライブラリ共通設定
|   |   +-- calcbase/
|   |   |   +-- Makefile                # calcbase ビルド定義 (静的ライブラリ)
|   |   +-- calc/
|   |       +-- Makefile                # calc ビルド定義 (動的ライブラリ)
|   |       +-- makepart.mk            # calc 固有設定 (LIB_TYPE=shared)
|   +-- src/
|       +-- Makefile                    # src 配下の再帰ビルド
|       +-- makepart.mk                # 実行ファイル出力先設定 (OUTPUT_DIR)
|       +-- add/
|       |   +-- Makefile                # add コマンドビルド定義
|       +-- calc/
|       |   +-- Makefile                # calc コマンドビルド定義
|       +-- shared-and-static-calc/
|           +-- Makefile                # shared-and-static-calc ビルド定義
+-- prod/calc.net/
|   +-- lib/                            # ビルド済みライブラリ
|   +-- bin/                            # ビルド済み実行ファイル
|   +-- libsrc/CalcLib/                 # .NET ライブラリソース
|   +-- src/
|       +-- makepart.mk                # 実行ファイル出力先設定 (OUTPUT_DIR)
|       +-- CalcApp/                    # .NET アプリケーションソース
+-- test/
    +-- Makefile                        # テストトップレベル Makefile
    +-- makepart.mk                    # テスト共通設定
    +-- libsrc/
    |   +-- Makefile                    # テスト用ライブラリ配下の再帰ビルド
    |   +-- mock_calcbase/
    |   |   +-- Makefile                # calcbase モックライブラリ
    |   +-- mock_calc/
    |       +-- Makefile                # calc モックライブラリ
    +-- src/
        +-- Makefile                    # テスト配下の再帰ビルド
        +-- calc/
            +-- libcalcbaseTest/
            |   +-- addTest/
            |       +-- Makefile        # add 関数単体テスト
            +-- main/
                +-- addTest/
                |   +-- Makefile        # add コマンド統合テスト
                +-- calcTest/
                |   +-- Makefile        # calc コマンド統合テスト
                +-- shared-and-static-calcTest/
                    +-- Makefile        # shared-and-static-calc 統合テスト
```

**設計方針**:

- 単一の `Makefile` で Linux/Windows 両対応
- makefw テンプレートが OS 判定を自動処理
- 各 Makefile は最小限の設定のみを記述

## 主な機能

### 1. makefw フレームワークによる統合

`makefw` は testfw から Makefile 機能を切り出したフレームワークです。共通テンプレートにより、Windows/Linux の差異を吸収します。

**Makefile の基本形 (ライブラリ・実行ファイル共通):**

```makefile
# ワークスペース検索
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理を include
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

##### makepart.mk の内容は、このタイミングで処理される #####

# ビルドテンプレートを include (ディレクトリパスに基づいて自動選択)
include $(WORKSPACE_FOLDER)/makefw/makefiles/makemain.mk
```

`makemain.mk` は、カレントディレクトリのパスを判定して、自動的に適切なテンプレートを選択します:

- パスに `/libsrc/` を含む → ライブラリ用テンプレート (`makelibsrc_c_cpp.mk` または `makelibsrc_dotnet.mk`)
- パスに `/src/` を含む → 実行ファイル用テンプレート (`makesrc_c_cpp.mk` または `makesrc_dotnet.mk`)
- `.csproj` ファイルの有無により、C/C++ 用か .NET 用かを判定

### 2. makepart.mk による追加設定

各ライブラリ/コマンド固有の設定は `makepart.mk` に記述できます。

**calc/makepart.mk の例:**

```makefile
# ライブラリの指定
LIBS += calcbase

ifeq ($(OS),Windows_NT)
    # Windows
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DCALC_EXPORTS
    CXXFLAGS += /DCALC_EXPORTS
endif

# 生成されるライブラリを動的ライブラリ (shared) とする
# 未指定の場合 (デフォルト) は static
LIB_TYPE = shared
```

この設定により、calc は Windows では DLL、Linux では .so として自動的にビルドされます。

**prod/calc/src/makepart.mk の例 (実行ファイルの出力先設定):**

```makefile
OUTPUT_DIR := $(WORKSPACE_FOLDER)/prod/calc/bin
```

この設定により、`prod/calc/src/` 配下のすべての実行ファイルは `prod/calc/bin/` に出力されます。

**test/makepart.mk の例:**

```makefile
ifneq ($(OS),Windows_NT)
    # Linux
    # 詳細な警告レベル設定 (gcc)
    CFLAGS=\
        -Wall \
        -Wextra \
        # ... (その他の警告オプション)
    CXXFLAGS=\
        -Wall \
        -Wextra \
        # ... (その他の警告オプション)
else
    # Windows
    CFLAGS      =
    CXXFLAGS    =
    LDFLAGS     =
endif

# テストフレームワークのライブラリ参照を追加する
ifneq ($(OS),Windows_NT)
    # Linux: TARGET_ARCH (e.g., linux-el8-x64)
    LIBSDIR += $(WORKSPACE_FOLDER)/testfw/lib/$(TARGET_ARCH)
else
    # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows-x64/md)
    LIBSDIR += $(WORKSPACE_FOLDER)/testfw/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif
LIBSDIR += \
    $(WORKSPACE_FOLDER)/test/lib

# テストフレームワークをリンクする
LINK_TEST = 1

# テスト関連ライブラリは、すべて静的リンクとする
ifeq ($(OS),Windows_NT)
    # Windows
    CFLAGS   += /DCALC_STATIC
    CXXFLAGS += /DCALC_STATIC
endif
```

`LINK_TEST = 1` を設定することで、Google Test フレームワークが自動的にリンクされます。

### 3. 静的ライブラリの自動組み込み

`LIB_TYPE=shared` の場合、動的ライブラリ (DLL/.so) は依存する静的ライブラリを自動的に内部リンクします。

**calc/Makefile の例:**

```makefile
# (ワークスペース検索、prepare.mk include は省略)

# makemain.mk を include (自動的に makelibsrc_c_cpp.mk が選択される)
include $(WORKSPACE_FOLDER)/makefw/makefiles/makemain.mk
```

**calc/makepart.mk:**

```makefile
# ライブラリの指定
LIBS += calcbase

# ... (CFLAGS, CXXFLAGS の設定)

# 動的ライブラリとして固定
LIB_TYPE = shared
```

**動作:**

- `LIBS` に指定された `calcbase` をライブラリ検索パスから検索
  - Windows: `calcbase.lib` を検索
  - Linux: `libcalcbase.a` を検索
- 静的ライブラリが見つかった場合は DLL/.so に静的リンク
- 見つからない場合は動的リンクフラグとして保持

### 4. クロスプラットフォーム対応

makefw テンプレートが Windows (MSVC) と Linux (GCC) の両方に自動対応します。

**makefw/makefiles/prepare.mk 内の OS 判定:**

```makefile
ifneq ($(OS),Windows_NT)
    # Linux (gcc/g++)
    ifeq ($(origin CC),default)
        CC = gcc
    endif
    ifeq ($(origin CXX),default)
        CXX = g++
    endif
    ifeq ($(origin LD),default)
        LD = g++
    endif
    ifeq ($(origin AR),default)
        AR = ar
    endif
else
    # Windows (MSVC)
    ifeq ($(origin CC),default)
        CC = cl
    endif
    ifeq ($(origin CXX),default)
        CXX = cl
    endif
    ifeq ($(origin LD),default)
        LD = link  # (MSVC の link.exe)
    endif
    ifeq ($(origin AR),default)
        AR = lib
    endif
endif
```

各 Makefile はテンプレートを include するだけで、OS 固有の処理は不要です。

### 5. ライブラリ構成

本プロジェクトでは、以下の構成でライブラリをビルドします：

| ライブラリ | LIB_TYPE 設定 | Windows | Linux | 説明 |
|-----------|----------|---------|-------|------|
| libcalcbase | 未設定 (→ static) | `.lib` | `.a` | 静的ライブラリ |
| libcalc | `shared` (makepart.mk で指定) | `.dll` + `.lib` | `.so` | 動的ライブラリ + インポートライブラリ |

**LIB_TYPE 変数:**

- 未設定の場合はデフォルトで `static`
- `makepart.mk` で `LIB_TYPE = shared` を指定すると動的ライブラリとしてビルド

## 設計思想

### 技術的な対処方針

#### OS 判定による条件分岐

Makefile 内で OS を判定し、コンパイラやツールを切り替えます。

```makefile
ifeq ($(OS),Windows_NT)
    # Windows 環境
    CC := cl
    LD := link
    AR := lib
    OBJEXT := .obj
    EXEEXT := .exe
else
    # Linux 環境
    CC := gcc
    LD := gcc
    AR := ar
    OBJEXT := .o
    EXEEXT :=
endif
```

`$(OS)` 環境変数は、Windows では `Windows_NT` が設定されます。Linux では通常未定義です。

#### コンパイラとツールチェーンの違い

| 項目 | Linux (GCC) | Windows (MSVC) |
|:-----|:------------|:---------------|
| C コンパイラ | gcc | cl.exe |
| C++ コンパイラ | g++ | cl.exe |
| リンカー | gcc/g++ | link.exe |
| 静的ライブラリ生成 | ar | lib.exe |
| 静的ライブラリ拡張 | .a | .lib |
| 共有ライブラリ拡張 | .so | .dll |
| 実行ファイル拡張 | なし | .exe |
| オブジェクト拡張 | .o | .obj |

#### MinGW 環境の活用

Windows 環境では、Git for Windows に付属する MinGW 環境を活用します。これにより：

- `bash`, `pwd`, `dirname` などの Linux コマンドが使える
- `sh` コマンドで既存のシェルスクリプトを実行できる
- makefw/cmnd/ 配下のシェルスクリプトがそのまま動作する
- doxyfw の Makefile がそのまま動作する

### makefw フレームワーク

makefw は testfw から Makefile 関連機能を切り出したフレームワークです：

- **testfw**: テスト実行、モック、Google Test 統合に特化
- **makefw**: クロスプラットフォームビルドシステムに特化

この分離により、ビルドシステムとテストフレームワークの責務が明確になりました。

### ライブラリ構成

本プロジェクトでは、以下の構成を採用しています：

- **libcalcbase**: 静的ライブラリ (LIB_TYPE 変数未設定、デフォルトで static)
- **libcalc**: 動的ライブラリ (makepart.mk で `LIB_TYPE = shared` を指定)

この構成により、以下のメリットがあります：

1. calc.dll/.so が calcbase を内部に静的リンクする
2. 依存関係が単純化され、配布時に calc.dll/.so のみを配置すれば動作
3. Windows と Linux で同様の動作を実現

### 実装ファイル

**フレームワーク:**

- `makefw/makefiles/prepare.mk` - 準備処理
- `makefw/makefiles/makemain.mk` - テンプレート自動選択
    - `makefw/makefiles/makelibsrc_c_cpp.mk` - C/C++ ライブラリビルド用テンプレート
    - `makefw/makefiles/makesrc_c_cpp.mk` - C/C++ 実行ファイルビルド用テンプレート
    - `makefw/makefiles/makelibsrc_dotnet.mk` - .NET ライブラリビルド用テンプレート
    - `makefw/makefiles/makesrc_dotnet.mk` - .NET 実行ファイルビルド用テンプレート

**ライブラリ:**

- `prod/calc/libsrc/calcbase/Makefile` - calcbase ビルド定義 (LIB_TYPE 未設定 → static)
- `prod/calc/libsrc/calc/Makefile` - calc ビルド定義
- `prod/calc/libsrc/calc/makepart.mk` - calc 固有設定 (LIB_TYPE = shared)

**コマンド:**

- `prod/calc/src/add/Makefile` - add コマンド (calcbase のみリンク)
- `prod/calc/src/calc/Makefile` - calc コマンド (calc のみリンク)
- `prod/calc/src/shared-and-static-calc/Makefile` - shared-and-static-calc コマンド (calc + calcbase をリンク)

**テスト:**

- `test/makepart.mk` - テスト共通設定 (テストフレームワークリンク、警告レベル設定)
- `test/libsrc/mock_calcbase/Makefile` - calcbase モックライブラリ
- `test/libsrc/mock_calc/Makefile` - calc モックライブラリ
- `test/src/calc/libcalcbaseTest/addTest/Makefile` - add 関数単体テスト
- `test/src/calc/main/addTest/Makefile` - add コマンド統合テスト
- `test/src/calc/main/calcTest/Makefile` - calc コマンド統合テスト
- `test/src/calc/main/shared-and-static-calcTest/Makefile` - shared-and-static-calc 統合テスト

## 実装の特徴

| 項目 | 説明 |
|------|------|
| クロスプラットフォーム | 単一の Makefile で Windows/Linux 両対応 |
| OS 判定 | makefw テンプレートが自動処理 |
| ライブラリ構成 | makepart.mk で柔軟に設定可能 |
| 依存関係解決 | `LIBS` 変数による明示的な指定 |
| 静的リンク自動化 | 動的ライブラリビルド時に静的ライブラリを自動検索・リンク |
| テストフレームワーク統合 | `LINK_TEST = 1` で Google Test を自動リンク |
| モック機能 | testfw のインクルードオーバーライド機能によるモック |

## デバッグ

makefw テンプレートには `debug` ターゲットがあり、設定された変数を表示できます：

**calcbase のデバッグ:**

Windows:

```cmd
cd prod\calc\libsrc\calcbase
make debug
```

Linux:

```bash
cd prod/calc/libsrc/calcbase
make debug
```

出力例 (Windows の場合):

```text
TARGET = calcbase.lib
LIB_TYPE = static
OS = Windows_NT
LIBS =
STATIC_LIBS =
OBJS = obj/add.obj
```

**calc のデバッグ:**

Windows:

```cmd
cd prod\calc\libsrc\calc
make debug
```

Linux:

```bash
cd prod/calc/libsrc/calc
make debug
```

出力例 (Windows の場合、makepart.mk で LIB_TYPE=shared 設定済み):

```text
TARGET = calc.dll
LIB_TYPE = shared
OS = Windows_NT
LIBS = calcbase
STATIC_LIBS = C:/path/to/prod/calc/lib/calcbase.lib
OBJS = obj/calcHandler.obj
```

## 現在の制限事項

1. **ライブラリ構成の固定**
   - libcalcbase は静的ライブラリとして実装 (makepart.mk で変更可能だが、依存関係上推奨しない)
   - libcalc は動的ライブラリとして実装 (makepart.mk で `LIB_TYPE = shared` 設定済み)

2. **testfw 機能との分離**
   - makefw はビルドシステムのみを提供
   - テスト機能 (inject, filter, モック) は testfw が提供

3. **ライブラリ検索パス**
   - `LIBSDIR` で指定されたパスから検索
   - prepare.mk により、以下のデフォルトパスが設定されます：
     - `$(WORKSPACE_FOLDER)/prod/calc/lib`
     - `$(WORKSPACE_FOLDER)/test/lib`

## 今後の拡張可能性

1. **より高度な依存関係解決**
   - `-L` オプションの追加サポート
   - システムライブラリパスの自動検索

2. **ビルド最適化**
   - 並列ビルドのさらなる最適化
   - インクリメンタルビルドの改善

3. **プラットフォーム拡張**
   - macOS サポート
   - その他のコンパイラサポート (Clang など)

## 実装のベストプラクティスと重要なノート

このセクションでは、クロスプラットフォーム対応の実装で得られた重要な知見とベストプラクティスを説明します。

### 環境設定スクリプト

Windows 環境では、環境設定スクリプトを実行する必要があります。

```cmd
call Add-MinGW-Path.cmd
call Add-VSBT-Env-x64.cmd
```

### UTF-8 ソースコードへの対応

ソースコードが UTF-8 で記述されている場合、MSVC のコンパイル時に `/utf-8` オプションを追加する必要があります。

```makefile
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /I$(WORKSPACE_FOLDER)/prod/calc/include
```

このオプションを指定しないと、以下の警告が発生します：

```text
warning C4819: The file contains a character that cannot be represented
in the current code page (932). Save the file in Unicode format to prevent data loss
```

### PDB ファイルの適切な配置

MSVC のデバッグ情報ファイル (PDB) は、以下のように配置します。

#### ライブラリの PDB

ライブラリファイル (.lib) と同じディレクトリに配置します。

```makefile
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /FS /Fd$(OUTPUT_DIR)/calc.pdb /I...
```

生成結果：

```text
prod/calc/lib/
+-- calc.lib
+-- calc.pdb
```

#### 実行ファイルの PDB

実行ファイルと同じディレクトリに配置します。コンパイル時の中間 PDB は obj ディレクトリに配置します。

```makefile
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /Fd$(OBJDIR)/add.pdb /I...
LDFLAGS := /DEBUG /PDB:$(OUTPUT_DIR)/add.pdb /LIBPATH:...
```

生成結果：

```text
prod/calc/src/add/
+-- add.exe
+-- add.pdb     (リンク時の PDB、実行ファイルと同じ場所)
+-- obj/
    +-- add.obj
    +-- add.pdb (コンパイル時の PDB、obj ディレクトリ)
```

### /FS オプションの追加

複数のソースファイルが同じ PDB ファイルに同時に書き込む場合、`/FS` オプションを追加して同期を保証する必要があります。

```makefile
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /FS /Fd$(OUTPUT_DIR)/calc.pdb /I...
```

このオプションを指定しないと、以下のエラーが発生することがあります：

```text
fatal error C1041: cannot open program database 'calc.pdb';
if multiple CL.EXE write to the same .PDB file, please use /FS
```

### WORKSPACE_FOLDER のパス変換

Makefile 内で Unix スタイルのパス (`/d/Users/...`) を取得した場合、MSVC はこのパスを認識できません。`cygpath -w` を使用して Windows スタイルのパス (`D:\Users\...`) に変換する必要があります。

```makefile
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

### ディレクトリの依存関係

PDB ファイルを OUTPUT_DIR に出力する場合、コンパイルルールに OUTPUT_DIR の依存関係を追加する必要があります。

```makefile
$(OBJDIR)/%$(OBJEXT): %.c | $(OBJDIR) $(OUTPUT_DIR)
    $(CC) $(CFLAGS) /c /Fo$@ $<
```

これにより、コンパイル前に OUTPUT_DIR が作成されることが保証されます。

### clean ターゲットの実装

Windows 環境では、clean ターゲットで PDB ファイルも削除する必要があります。

```makefile
.PHONY: clean
clean:
    rm -rf $(OBJDIR)
    rm -f $(OUTPUT_DIR)/$(TARGET)
ifeq ($(OS),Windows_NT)
    rm -f $(OUTPUT_DIR)/*.pdb
endif
```

obj ディレクトリの削除により、コンパイル時の PDB と ILK ファイルが削除されます。OUTPUT_DIR に配置されたリンク時の PDB ファイルのみ、明示的に削除します。

## まとめと今後の展望

### 採用した方針

本プロジェクトのクロスプラットフォーム対応では、以下の方針を採用しました：

- **OS 判定による条件分岐**: コンパイラとツールを切り替え
- **MinGW 環境の活用**: 既存のシェルスクリプトとシェルコマンドを利用
- **makefw フレームワーク**: testfw から切り出し、クロスプラットフォーム対応
- **既存コードの保護**: Linux 環境では既存の動作を完全に維持

### メリット

- **再利用性**: makefw がクロスプラットフォーム対応され、他のプロジェクトでも利用できる
- **保守性**: Linux と Windows で同じ Makefile を使用できるため、メンテナンス性が向上
- **互換性**: MinGW 環境により、既存のシェルスクリプトとシェルコマンドを活用できる
- **ネイティブ性**: MSVC を使用することで、Windows ネイティブなバイナリを生成できる
- **安定性**: Linux の動作は完全に維持される

### 注意点

- **makefw の共有**: makefw は複数のプロジェクトで共有されるため、変更時には慎重に行う
- **コンパイラフラグ**: MSVC と GCC でコンパイラフラグが異なるため、makepart.mk の条件分岐を慎重に設定
- **環境設定順序**: Windows では環境設定スクリプトを必ず正しい順序で実行
- **MinGW 必須**: doxyfw は MinGW の bash を前提としているため、Windows では MinGW 環境が必須
- **テストライブラリ**: テストを実行する場合、Windows 版の Google Test ライブラリが必要
- **文字コード**: ソースコードが UTF-8 の場合、MSVC では `/utf-8` オプションが必要
