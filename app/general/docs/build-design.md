# クロスプラットフォーム ビルド システムの実装

## 概要

このドキュメントは、本フレームワークを利用するクロスプラットフォーム ビルド システムの設計、実装、および使用方法を説明します。

対象プロジェクトは、Linux/Windows クロスプラットフォーム ビルド システムを実現しています。GCC と MSVC (Microsoft Visual C++) との両方に対応し、単一の makefile で Linux と Windows の両環境でビルドできます。

## 前提条件

### Linux 環境

標準的な開発ツールが必要です:

- GCC コンパイラ
- GNU Make

### Windows 環境

注: 環境変数は VS Code 起動時に自動設定済みの前提です。以下の環境が利用可能である必要があります:

1. ポータブル版 Visual Studio Build Tools
    - MSVC コンパイラ (`cl.exe`)
    - MSVC リンカー (`link.exe`)

2. Git for Windows (MinGW)
    - GNU Make (`make.exe`)
    - 各種 Unix コマンド

3. 環境設定の実行 (手動設定時のみ)

   ```powershell
   . .\Start-VSCode-With-Env.ps1 -EnvOnly
   ```

## ビルド方法

### ビルド実行

C 側は `app/example/prod/`、.NET 側は `app/example.net/prod/` を個別にビルドします:

Windows:

```cmd
cd app/example/prod
make
```

Linux:

```bash
cd app/example/prod
make
```

このコマンドで以下がビルドされます:

ライブラリ / Libraries:

| ライブラリ | Windows | Linux | 説明 |
|-----------|---------|-------|------|
| libexamplebase | `app/example/prod/lib/libexamplebase.lib` | `app/example/prod/lib/libexamplebase.a` | 基本計算関数ライブラリ (静的ライブラリ) |
| libexample | `app/example/prod/lib/libexample.dll` + `libexample.lib` | `app/example/prod/lib/libexample.so` | 計算ハンドラー ライブラリ (動的ライブラリ、examplebase を内部に静的リンク) |

コマンド / Commands:

| コマンド | Windows | Linux | リンク ライブラリ |
|---------|---------|-------|----------------|
| add | `app/example/prod/cbin/add.exe` | `app/example/prod/cbin/add` | examplebase のみ |
| example | `app/example/prod/cbin/example.exe` | `app/example/prod/cbin/example` | example のみ |
| shared-and-static-example | `app/example/prod/cbin/shared-and-static-example.exe` | `app/example/prod/cbin/shared-and-static-example` | example + examplebase (両方) |

.NET コマンド / .NET Commands:

| コマンド | Windows | Linux | 依存ライブラリ |
|---------|---------|-------|--------------|
| ExampleApp | `app/example.net/prod/cbin/ExampleApp.exe` | `app/example.net/prod/cbin/ExampleApp` | ExampleLib (libexample の .NET ラッパー) |

重要:

- libexample は動的ライブラリとして実装されており、examplebase を内部に静的リンクします
- shared-and-static-example は、コマンドにおいて動的ライブラリと静的ライブラリの両方をリンクする実装例です
- 実行ファイルの出力先は `app/example/prod/src/makepart.mk` および `app/example.net/prod/src/makepart.mk` で `OUTPUT_DIR` として設定されています

### テストのビルドと実行

framework/testfw/ ディレクトリおよび app/example/test/ ディレクトリでテストをビルド・実行します:

Windows:

```cmd
cd framework\testfw
make
cd ..\..\app\example\test
make
```

Linux:

```bash
cd framework/testfw
make
cd ../../app/example/test
make
```

テストは Google Test フレームワークを使用しています:

- 単体テスト: ライブラリ関数の単体テスト (`app/example/test/src/libexamplebaseTest/`) および コマンドの単体テスト (`app/example/test/src/main/`)
- モック ライブラリ: テスト用のモック実装 (`app/example/test/libsrc/mock_*/`)

### クリーンアップ

**プロダクション コードのクリーンアップ:**

```bash
cd app/example/prod
make clean
```

**テスト コードのクリーンアップ:**

```bash
cd app/example/test
make clean
```

## ファイル構成

### プロジェクト構造

```text
workspace/
+-- framework/makefw/                              # makefile フレームワーク (testfw から切り出し)
|   +-- makefiles/
|   |   +-- makelibsrc.mk               # ライブラリビルド用共通テンプレート
|   |   +-- makesrc.mk                  # 実行ファイルビルド用共通テンプレート
|   |   +-- prepare.mk                  # 準備処理
|   |   +-- _*.mk                       # 内部処理用ファイル
|   +-- docs/                           # フレームワーク技術ドキュメント
+-- app/example/prod/
|   +-- makefile                        # トップレベル makefile (再帰ビルド)
|   +-- lib/                            # ビルド済みライブラリ
|   +-- cbin/                           # ビルド済み実行ファイル
|   +-- libsrc/
|   |   +-- makefile                    # libsrc 配下の再帰ビルド
|   |   +-- makepart.mk                # ライブラリ共通設定
|   |   +-- examplebase/
|   |   |   +-- makefile                # examplebase ビルド定義 (静的ライブラリ)
|   |   +-- example/
|   |       +-- makefile                # example ビルド定義 (動的ライブラリ)
|   |       +-- makepart.mk            # example 固有設定 (LIB_TYPE=shared)
|   +-- src/
|       +-- makefile                    # src 配下の再帰ビルド
|       +-- makepart.mk                # 実行ファイル出力先設定 (OUTPUT_DIR)
|       +-- add/
|       |   +-- makefile                # add コマンドビルド定義
|       +-- example/
|       |   +-- makefile                # example コマンドビルド定義
|       +-- shared-and-static-example/
|           +-- makefile                # shared-and-static-example ビルド定義
+-- app/example.net/prod/
|   +-- lib/                            # ビルド済みライブラリ
|   +-- cbin/                           # ビルド済み実行ファイル
|   +-- libsrc/ExampleLib/                 # .NET ライブラリソース
|   +-- src/
|       +-- makepart.mk                # 実行ファイル出力先設定 (OUTPUT_DIR)
|       +-- ExampleApp/                    # .NET アプリケーションソース
+-- app/example/test/
    +-- makefile                        # テストトップレベル makefile
    +-- makepart.mk                    # テスト共通設定
    +-- libsrc/
    |   +-- makefile                    # テスト用ライブラリ配下の再帰ビルド
    |   +-- mock_examplebase/
    |   |   +-- makefile                # examplebase モックライブラリ
    |   +-- mock_example/
    |       +-- makefile                # example モックライブラリ
    +-- src/
        +-- makefile                    # テスト配下の再帰ビルド
        +-- example/
            +-- libexamplebaseTest/
            |   +-- addTest/
            |       +-- makefile        # add 関数単体テスト
            +-- main/
                +-- addTest/
                |   +-- makefile        # add コマンド統合テスト
                +-- exampleTest/
                |   +-- makefile        # example コマンド統合テスト
                +-- shared-and-static-exampleTest/
                    +-- makefile        # shared-and-static-example 統合テスト
```

**設計方針**:

- 単一の `makefile` で Linux/Windows 両対応
- makefw テンプレートが OS 判定を自動処理
- 各 makefile は最小限の設定のみを記述

## 主な機能

### makefw フレームワークによる統合

`makefw` は testfw から makefile 機能を切り出したフレームワークです。共通テンプレートにより、Windows/Linux の差異を吸収します。

**makefile の基本形 (ライブラリ・実行ファイル共通):**

```makefile
# ワークスペース検索
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_DIR := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理を include
include $(WORKSPACE_DIR)/framework/makefw/makefiles/prepare.mk

##### makepart.mk の内容は、このタイミングで処理される #####

# ビルド テンプレートを include (ディレクトリ パスに基づいて自動選択)
include $(WORKSPACE_DIR)/framework/makefw/makefiles/makemain.mk
```

`makemain.mk` は、カレント ディレクトリのパスを判定して、自動的に適切なテンプレートを選択します:

- パスに `/libsrc/` を含む → ライブラリ用テンプレート (`makelibsrc_c_cpp.mk` または `makelibsrc_dotnet.mk`)
- パスに `/src/` を含む → 実行ファイル用テンプレート (`makesrc_c_cpp.mk` または `makesrc_dotnet.mk`)
- `.csproj` ファイルの有無により、C/C++ 用か .NET 用かを判定

### makepart.mk による追加設定

各ライブラリ/コマンド固有の設定は `makepart.mk` に記述できます。

**example/makepart.mk の例:**

```makefile
# ライブラリの指定
LIBS += examplebase

ifeq ($(OS),Windows_NT)
    # Windows
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DEXAMPLE_EXPORTS
    CXXFLAGS += /DEXAMPLE_EXPORTS
endif

# 生成されるライブラリを動的ライブラリ (shared) とする
# 未指定の場合 (デフォルト) は static
LIB_TYPE = shared
```

この設定により、example は Windows では DLL、Linux では .so として自動的にビルドされます。

**app/example/prod/src/cmd/makepart.mk の例 (実行ファイルの出力先設定):**

```makefile
OUTPUT_DIR := $(WORKSPACE_DIR)/app/example/prod/cbin
```

この設定により、`app/example/prod/src/cmd/` 配下のすべての実行ファイルは `app/example/prod/cbin/` に出力されます。

**app/example/test/makepart.mk の例:**

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

# テスト フレームワークのライブラリ参照を追加する
ifneq ($(OS),Windows_NT)
    # Linux: TARGET_ARCH (e.g., linux_el8_x64, linux_el9_x64, linux_el10_x64)
    LIBSDIR += $(WORKSPACE_DIR)/framework/testfw/lib/$(TARGET_ARCH)
else
    # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows_x64/md)
    LIBSDIR += $(WORKSPACE_DIR)/framework/testfw/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif
LIBSDIR += \
    $(WORKSPACE_DIR)/app/example/test/lib

# テスト フレームワークをリンクする
LINK_TEST = 1

# テスト関連ライブラリは、すべて静的リンクとする
ifeq ($(OS),Windows_NT)
    # Windows
    CFLAGS   += /DEXAMPLE_STATIC
    CXXFLAGS += /DEXAMPLE_STATIC
endif
```

`LINK_TEST = 1` を設定することで、Google Test フレームワークが自動的にリンクされます。

### 静的ライブラリの自動組み込み

`LIB_TYPE=shared` の場合、動的ライブラリ (DLL/.so) は依存する静的ライブラリを自動的に内部リンクします。

**example/makefile の例:**

```makefile
# (ワークスペース検索、prepare.mk include は省略)

# makemain.mk を include (自動的に makelibsrc_c_cpp.mk が選択される)
include $(WORKSPACE_DIR)/framework/makefw/makefiles/makemain.mk
```

**example/makepart.mk:**

```makefile
# ライブラリの指定
LIBS += examplebase

# ... (CFLAGS, CXXFLAGS の設定)

# 動的ライブラリとして固定
LIB_TYPE = shared
```

**動作:**

- `LIBS` に指定された `examplebase` をライブラリ検索パスから検索
    - Windows: `libexamplebase.lib` を検索
    - Linux: `libexamplebase.a` を検索
- 静的ライブラリが見つかった場合は DLL/.so に静的リンク
- 見つからない場合は動的リンク フラグとして保持

### クロスプラットフォーム対応

makefw テンプレートが Windows (MSVC) と Linux (GCC) の両方に自動対応します。

**framework/makefw/makefiles/prepare.mk 内の OS 判定:**

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

各 makefile はテンプレートを include するだけで、OS 固有の処理は不要です。

### ライブラリ構成

対象プロジェクトでは、以下の構成でライブラリをビルドします:

| ライブラリ | LIB_TYPE 設定 | Windows | Linux | 説明 |
|-----------|----------|---------|-------|------|
| libexamplebase | 未設定 (→ static) | `.lib` | `.a` | 静的ライブラリ |
| libexample | `shared` (makepart.mk で指定) | `.dll` + `.lib` | `.so` | 動的ライブラリ + インポート ライブラリ |

**LIB_TYPE 変数:**

- 未設定の場合はデフォルトで `static`
- `makepart.mk` で `LIB_TYPE = shared` を指定すると動的ライブラリとしてビルド

## 設計思想

### 技術的な対処方針

#### OS 判定による条件分岐

makefile 内で OS を判定し、コンパイラやツールを切り替えます。

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

Windows 環境では、Git for Windows に付属する MinGW 環境を活用します。これにより:

- `bash`, `pwd`, `dirname` などの Linux コマンドを使用できます。
- `sh` コマンドで既存のシェル スクリプトを実行できます。
- framework/makefw/bin/ 配下のシェル スクリプトがそのまま動作します。
- doxyfw の makefile がそのまま動作します。

### makefw フレームワーク

makefw は testfw から makefile 関連機能を切り出したフレームワークです:

- **testfw**: テスト実行、モック、Google Test 統合に特化
- **makefw**: クロスプラットフォーム ビルド システムに特化

この分離により、ビルド システムとテスト フレームワークの責務が明確になりました。

### ライブラリ構成

対象プロジェクトでは、以下の構成を採用しています:

- **libexamplebase**: 静的ライブラリ (LIB_TYPE 変数未設定、デフォルトで static)
- **libexample**: 動的ライブラリ (makepart.mk で `LIB_TYPE = shared` を指定)

この構成により、以下のメリットがあります:

1. libexample.dll/.so が examplebase を内部に静的リンクします。
2. 依存関係が単純化され、配布時に libexample.dll/.so のみを配置すれば動作します。
3. Windows と Linux で同様の動作を実現します。

### 実装ファイル

**フレームワーク:**

- `framework/makefw/makefiles/prepare.mk` - 準備処理
- `framework/makefw/makefiles/makemain.mk` - テンプレート自動選択
    - `framework/makefw/makefiles/makelibsrc_c_cpp.mk` - C/C++ ライブラリ ビルド用テンプレート
    - `framework/makefw/makefiles/makesrc_c_cpp.mk` - C/C++ 実行ファイル ビルド用テンプレート
    - `framework/makefw/makefiles/makelibsrc_dotnet.mk` - .NET ライブラリ ビルド用テンプレート
    - `framework/makefw/makefiles/makesrc_dotnet.mk` - .NET 実行ファイル ビルド用テンプレート

**ライブラリ:**

- `app/example/prod/libsrc/examplebase/makefile` - examplebase ビルド定義 (LIB_TYPE 未設定 → static)
- `app/example/prod/libsrc/example/makefile` - example ビルド定義
- `app/example/prod/libsrc/example/makepart.mk` - example 固有設定 (LIB_TYPE = shared)

**コマンド:**

- `app/example/prod/src/add/makefile` - add コマンド (examplebase のみリンク)
- `app/example/prod/src/example/makefile` - example コマンド (example のみリンク)
- `app/example/prod/src/shared-and-static-example/makefile` - shared-and-static-example コマンド (example + examplebase をリンク)

**テスト:**

- `app/example/test/makepart.mk` - テスト共通設定 (テスト フレームワーク リンク、警告レベル設定)
- `app/example/test/libsrc/mock_examplebase/makefile` - examplebase モック ライブラリ
- `app/example/test/libsrc/mock_example/makefile` - example モック ライブラリ
- `app/example/test/src/libexamplebaseTest/addTest/makefile` - add 関数単体テスト
- `app/example/test/src/main/addTest/makefile` - add コマンド統合テスト
- `app/example/test/src/main/exampleTest/makefile` - example コマンド統合テスト
- `app/example/test/src/main/shared-and-static-exampleTest/makefile` - shared-and-static-example 統合テスト

## 実装の特徴

| 項目 | 説明 |
|------|------|
| クロスプラットフォーム | 単一の makefile で Windows/Linux 両対応 |
| OS 判定 | makefw テンプレートが自動処理 |
| ライブラリ構成 | makepart.mk で柔軟に設定可能 |
| 依存関係解決 | `LIBS` 変数による明示的な指定 |
| 静的リンク自動化 | 動的ライブラリ ビルド時に静的ライブラリを自動検索・リンク |
| テスト フレームワーク統合 | `LINK_TEST = 1` で Google Test を自動リンク |
| モック機能 | testfw のインクルード オーバーライド機能によるモック |

## デバッグ

makefw テンプレートには `debug` ターゲットがあり、設定された変数を表示できます:

**examplebase のデバッグ:**

Windows:

```cmd
cd prod\example\libsrc\examplebase
make debug
```

Linux:

```bash
cd app/example/prod/libsrc/examplebase
make debug
```

出力例 (Windows の場合):

```text
TARGET = libexamplebase.lib
LIB_TYPE = static
OS = Windows_NT
LIBS =
STATIC_LIBS =
OBJS = obj/add.obj
```

**example のデバッグ:**

Windows:

```cmd
cd prod\example\libsrc\example
make debug
```

Linux:

```bash
cd app/example/prod/libsrc/example
make debug
```

出力例 (Windows の場合、makepart.mk で LIB_TYPE=shared 設定済み):

```text
TARGET = libexample.dll
LIB_TYPE = shared
OS = Windows_NT
LIBS = examplebase
STATIC_LIBS = C:/path/to/app/example/prod/lib/libexamplebase.lib
OBJS = obj/exampleHandler.obj
```

## 現在の制限事項

1. **ライブラリ構成の固定**
    - libexamplebase は静的ライブラリとして実装 (makepart.mk で変更可能だが、依存関係上推奨しない)
    - libexample は動的ライブラリとして実装 (makepart.mk で `LIB_TYPE = shared` 設定済み)

2. **testfw 機能との分離**
    - makefw はビルド システムのみを提供
    - テスト機能 (inject, filter, モック) は testfw が提供

3. **ライブラリ検索パス**
    - `LIBSDIR` で指定されたパスから検索
    - prepare.mk により、以下のデフォルト パスが設定されます:
        - `$(WORKSPACE_DIR)/app/example/prod/lib`
        - `$(WORKSPACE_DIR)/app/example/test/lib`

## 今後の拡張可能性

1. **より高度な依存関係解決**
    - `-L` オプションの追加サポート
    - システム ライブラリ パスの自動検索

2. **ビルド最適化**
    - 並列ビルドのさらなる最適化
    - インクリメンタル ビルドの改善

3. **プラットフォーム拡張**
    - macOS サポート
    - その他のコンパイラ サポート (Clang など)

4. **app 単位のサブモジュール化**
    - `MYAPP_DIR` により、app 内の設定は自 app のルートを基準に記述されている
    - `$(MYAPP_DIR)/prod/include` や `$(MYAPP_DIR)/../utility/prod/include` のような app 起点記法を使用
    - 将来 `app/<name>` を独立した Git サブモジュールに分離する際、app 内の `makepart.mk` の記述を変更せずに済む
    - 内部では `realpath -m` で絶対パスに正規化されるため、`..` を含む cross-app 参照もクリーンなパスとしてコンパイラに渡される

## 実装のベスト プラクティスと重要なノート

このセクションでは、クロスプラットフォーム対応の実装で得られた重要な知見とベスト プラクティスを説明します。

### 環境設定スクリプト

Windows 環境では、`Start-VSCode-With-Env.ps1` で環境設定を行う必要があります。VS Code を起動せず環境変数のみを設定する場合は、`-EnvOnly` パラメーターを指定してドット ソースで実行します。

```powershell
. .\Start-VSCode-With-Env.ps1 -EnvOnly
```

### UTF-8 ソース コードへの対応

ソース コードが UTF-8 で記述されている場合、MSVC のコンパイル時に `/utf-8` オプションを追加する必要があります。

```makefile
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /I$(WORKSPACE_DIR)/app/example/prod/include
```

このオプションを指定しないと、以下の警告が発生します:

```text
warning C4819: The file contains a character that cannot be represented
in the current code page (932). Save the file in Unicode format to prevent data loss
```

### PDB ファイルの適切な配置

MSVC のデバッグ情報ファイル (PDB) は、以下のように配置します。

#### ライブラリの PDB

ライブラリ ファイル (.lib) と同じディレクトリに配置します。

```makefile
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /FS /Fd$(OUTPUT_DIR)/libexample.pdb /I...
```

生成結果:

```text
app/example/prod/lib/
+-- libexample.lib
+-- libexample.pdb
```

#### 実行ファイルの PDB

実行ファイルと同じディレクトリに配置します。コンパイル時の中間 PDB は obj ディレクトリに配置します。

```makefile
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /Fd$(OBJDIR)/add.pdb /I...
LDFLAGS := /DEBUG /PDB:$(OUTPUT_DIR)/add.pdb /LIBPATH:...
```

生成結果:

```text
app/example/prod/src/add/
+-- add.exe
+-- add.pdb     (リンク時の PDB、実行ファイルと同じ場所)
+-- obj/
    +-- add.obj
    +-- add.pdb (コンパイル時の PDB、obj ディレクトリ)
```

### /FS オプションの追加

複数のソース ファイルが同じ PDB ファイルに同時に書き込む場合、`/FS` オプションを追加して同期を保証する必要があります。

```makefile
CFLAGS := /W4 /Zi /TC /nologo /utf-8 /FS /Fd$(OUTPUT_DIR)/example.pdb /I...
```

このオプションを指定しないと、以下のエラーが発生することがあります:

```text
fatal error C1041: cannot open program database 'example.pdb';
if multiple CL.EXE write to the same .PDB file, please use /FS
```

### WORKSPACE_DIR のパス変換

makefile 内で Unix スタイルのパス (`/d/Users/...`) を取得した場合、MSVC はこのパスを認識できません。`cygpath -w` を使用して Windows スタイルのパス (`D:\Users\...`) に変換する必要があります。

```makefile
WORKSPACE_DIR := $(shell \
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

PDB ファイルを OUTPUT_DIR に出力する場合、コンパイル ルールに OUTPUT_DIR の依存関係を追加する必要があります。

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

対象プロジェクトのクロスプラットフォーム対応では、以下の方針を採用しました:

- **OS 判定による条件分岐**: コンパイラとツールを切り替え
- **MinGW 環境の活用**: 既存のシェル スクリプトとシェル コマンドを利用
- **makefw フレームワーク**: testfw から切り出し、クロスプラットフォーム対応
- **既存コードの保護**: Linux 環境では既存の動作を完全に維持

### メリット

- **再利用性**: makefw がクロスプラットフォーム対応され、他のプロジェクトでも利用できます。
- **保守性**: Linux と Windows で同じ makefile を使用できるため、保守性が向上します。
- **互換性**: MinGW 環境により、既存のシェル スクリプトとシェル コマンドを活用できます。
- **ネイティブ性**: MSVC を使用することで、Windows ネイティブなバイナリを生成できます。
- **安定性**: Linux の動作は完全に維持されます。

### 注意点

- **makefw の共有**: makefw は複数のプロジェクトで共有されるため、変更時には慎重に行います。
- **コンパイラ フラグ**: MSVC と GCC でコンパイラ フラグが異なるため、makepart.mk の条件分岐を慎重に設定
- **環境設定順序**: Windows では環境設定スクリプトを必ず正しい順序で実行
- **MinGW 必須**: doxyfw は MinGW の bash を前提としているため、Windows では MinGW 環境が必須
- **テスト ライブラリ**: テストを実行する場合、Windows 版の Google Test ライブラリが必要
- **文字コード**: ソース コードが UTF-8 の場合、MSVC では `/utf-8` オプションが必要
