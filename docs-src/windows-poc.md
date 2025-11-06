# Windows POC ビルドシステム

Windows Proof of Concept Build System

## 概要 / Overview

このプロジェクトの Windows 向け実験的なビルドシステムです。MSVC (Microsoft Visual C++) と MinGW 環境でのクロスプラットフォームビルドに対応しています。

This is an experimental build system for Windows. It supports cross-platform builds with MSVC (Microsoft Visual C++) and MinGW environments.

## 前提条件 / Prerequisites

### Windows 環境

1. **ポータブル版 Visual Studio Build Tools**
   - インストール済み
   - `set-vsbt-env-x64.bat` で環境変数を設定

2. **Git for Windows (MinGW)**
   - インストール済み
   - `Add-MinGW-Path.cmd` で環境変数を設定

3. **環境設定の実行順序（重要）**
   ```cmd
   call Add-MinGW-Path.cmd
   call set-vsbt-env-x64.bat
   ```
   この順序で実行することで、MSVC の `link.exe` が MinGW の `link` より優先されます。

**Important: Environment Setup Order**

```cmd
call Add-MinGW-Path.cmd
call set-vsbt-env-x64.bat
```

This order ensures MSVC's `link.exe` takes precedence over MinGW's `link`.

## ビルド方法 / Build Instructions

### 基本ビルド（静的ライブラリ）/ Basic Build (Static Library)

```cmd
build-poc.bat
```

このコマンドで以下がビルドされます：
- `prod/calc/lib/calcbase.lib` - 基本計算関数ライブラリ（静的）
- `prod/calc/lib/calc.lib` - 計算ハンドラーライブラリ（静的）
- `prod/calc/src/add/add.exe` - 加算コマンド

This builds:
- `prod/calc/lib/calcbase.lib` - Basic calculation library (static)
- `prod/calc/lib/calc.lib` - Calculation handler library (static)
- `prod/calc/src/add/add.exe` - Addition command

### 共有ライブラリビルド / Shared Library Build (DLL)

```cmd
set BUILD=shared
build-poc.bat
```

または / or:

```cmd
set BUILD=shared && build-poc.bat
```

このコマンドで以下がビルドされます：
- `prod/calc/lib/calcbase.lib` - 基本計算関数ライブラリ（静的）
- `prod/calc/lib/calc.dll` - 計算ハンドラーライブラリ（DLL、calcbase.lib を静的リンク）
- `prod/calc/lib/calc.lib` - calc.dll のインポートライブラリ
- `prod/calc/src/add/add.exe` - 加算コマンド（calc.lib をリンク）

**重要**: `BUILD=shared` の場合、calc.dll は calcbase.lib を内部に静的リンクするため、add.exe は calc.lib（インポートライブラリ）のみをリンクすれば動作します。

This builds:
- `prod/calc/lib/calcbase.lib` - Basic calculation library (static)
- `prod/calc/lib/calc.dll` - Calculation handler library (DLL with calcbase.lib statically linked)
- `prod/calc/lib/calc.lib` - Import library for calc.dll
- `prod/calc/src/add/add.exe` - Addition command (links calc.lib)

**Important**: When `BUILD=shared`, calc.dll statically links calcbase.lib internally, so add.exe only needs to link calc.lib (import library).

## ファイル構成 / File Structure

### POC Makefile ファイル

```
prod/calc/
├── libsrc/
│   ├── makelibsrc-windows-poc.mk       # 共通テンプレート（新規作成）
│   ├── calcbase/
│   │   ├── Makefile                    # Linux 版（testfw テンプレート使用）
│   │   └── Makefile.Windows-poc        # Windows POC 版（テンプレート利用、簡素化）
│   └── calc/
│       ├── Makefile                    # Linux 版（testfw テンプレート使用）
│       └── Makefile.Windows-poc        # Windows POC 版（テンプレート利用、簡素化）
└── src/
    └── add/
        ├── Makefile                    # Linux 版（testfw テンプレート使用）
        └── Makefile.Windows-poc        # Windows POC 版（変更なし）
```

**共通テンプレートの導入**: calcbase と calc の Makefile.Windows-poc はほぼ同じ内容だったため、`makelibsrc-windows-poc.mk` として共通化しました。各 Makefile は必要最小限の設定のみを記述し、テンプレートを include します。

### ビルドスクリプト

- `build-poc.bat` - 全体のビルドスクリプト（BUILD 変数対応）

## 主な機能 / Key Features

### 1. 共通テンプレートによる統合 / Common Template Integration

calcbase と calc の Makefile.Windows-poc はほぼ同じ内容だったため、`makelibsrc-windows-poc.mk` として共通化しました。

Since calcbase and calc Makefile.Windows-poc had nearly identical content, they were unified into `makelibsrc-windows-poc.mk`.

**各 Makefile で設定する変数:**
- `TARGET_BASE`: ライブラリ名のベース（例: calcbase, calc）
- `BUILD`: ビルドモード（static/shared、calcbase は設定不要）
- `LIBS`: 依存ライブラリ（例: calcbase、必要な場合のみ）
- `LIBSDIR`: ライブラリ検索パス（LIBS を使用する場合）

**Variables set in each Makefile:**
- `TARGET_BASE`: Base library name (e.g., calcbase, calc)
- `BUILD`: Build mode (static/shared, calcbase doesn't need to set)
- `LIBS`: Dependent libraries (e.g., calcbase, only if needed)
- `LIBSDIR`: Library search paths (when using LIBS)

**calcbase の例 / calcbase example:**
```makefile
TARGET_BASE := calcbase
include ../makelibsrc-windows-poc.mk
```

**calc の例 / calc example:**
```makefile
TARGET_BASE := calc
BUILD := shared
LIBSDIR = $(WORKSPACE_FOLDER)/prod/calc/lib
LIBS := calcbase
include ../makelibsrc-windows-poc.mk
```

### 2. 静的ライブラリの自動組み込み / Automatic Static Library Embedding

`BUILD=shared` の場合、`calc.dll` は `calcbase.lib` を自動的に静的リンクします。

When `BUILD=shared`, `calc.dll` automatically statically links `calcbase.lib`.

**calc/Makefile.Windows-poc の設定例:**
```makefile
TARGET_BASE := calc
BUILD := shared
LIBSDIR = $(WORKSPACE_FOLDER)/prod/calc/lib
LIBS := calcbase
```

**動作:**
- `LIBS` に指定された `calcbase` を `LIBSDIR` から検索
  - Windows: `calcbase.lib` を検索
  - Linux: `libcalcbase.a` を検索
- 見つかった場合は DLL/.so に静的リンク
- 見つからない場合は動的リンクフラグとして保持

**Behavior:**
- Search for `calcbase` specified in `LIBS` within `LIBSDIR`
  - Windows: Search for `calcbase.lib`
  - Linux: Search for `libcalcbase.a`
- If found, it's statically linked into the DLL/.so
- If not found, it's kept as a dynamic link flag

### 3. クロスプラットフォーム対応 / Cross-Platform Support

共通テンプレート `makelibsrc-windows-poc.mk` が Windows (MSVC) と Linux (GCC) の両方に対応しています。

The common template `makelibsrc-windows-poc.mk` supports both Windows (MSVC) and Linux (GCC).

**テンプレート内の OS 判定:**
```makefile
ifeq ($(OS),Windows_NT)
    # Windows (MSVC)
    CC := cl
    LD := link
    # ...
else
    # Linux (GCC)
    CC := gcc
    LD := gcc
    # ...
endif
```

各 Makefile はテンプレートを include するだけで、OS 固有の処理は不要です。

Each Makefile only needs to include the template; no OS-specific code is required.

### 4. BUILD 変数による切り替え / BUILD Variable Switching

| BUILD 値 | Windows | Linux | 説明 |
|----------|---------|-------|------|
| `static` (デフォルト) | `.lib` | `.a` | 静的ライブラリ |
| `shared` | `.dll` + `.lib` | `.so` | 共有ライブラリ + インポートライブラリ |

| BUILD value | Windows | Linux | Description |
|-------------|---------|-------|-------------|
| `static` (default) | `.lib` | `.a` | Static library |
| `shared` | `.dll` + `.lib` | `.so` | Shared library + import library |

## 今回の改修内容 / Changes in This Update

### 変更したファイル / Modified Files

1. **新規作成**: `prod/calc/libsrc/makelibsrc-windows-poc.mk`（共通テンプレート）
2. **新規作成**: `prod/calc/libsrc/calcbase/Makefile.Windows-poc`（簡素化、テンプレート利用）
3. **更新**: `prod/calc/libsrc/calc/Makefile.Windows-poc`（簡素化、テンプレート利用、静的リンク機能追加）
4. **更新**: `build-poc.bat`（calcbase ビルドを追加、BUILD 変数対応）

**変更していないファイル / Unchanged Files**

- `prod/calc/src/add/Makefile.Windows-poc` - add コマンドは変更なし
- `testfw/` 以下のすべてのファイル - testfw は変更なし（Windows POC は独立）

### 改修の範囲 / Scope of Changes

**今回の改修はライブラリ側（calc）の機能変更のみです。**

This update only modifies the library side (calc) functionality.

- **ライブラリ側（calc）**: `BUILD=shared` 時に calcbase.lib を自動的に静的リンク
- **アプリ側（add）**: 変更なし。calc.lib をリンクするだけ

**Library side (calc)**: Automatically statically links calcbase.lib when `BUILD=shared`
**Application side (add)**: No changes. Just links calc.lib

これにより、Linux 版（testfw/makefiles/makelibsrc.mk）と同様の動作を Windows POC でも実現しました。

This achieves the same behavior as the Linux version (testfw/makefiles/makelibsrc.mk) in Windows POC.

## testfw テンプレートとの違い / Differences from testfw Templates

### testfw (Linux 版) の特徴

- `testfw/makefiles/makelibsrc.mk` を使用
- 高度な機能（inject, filter, テストフレームワーク統合）
- `-l` オプションの自動解決（`.a` ファイルを検索）
- 複数の検索パス対応（`LIBSDIR`, `-L`, システムパス）

**testfw (Linux version) features:**
- Uses `testfw/makefiles/makelibsrc.mk`
- Advanced features (inject, filter, test framework integration)
- Automatic `-l` option resolution (searches for `.a` files)
- Multiple search path support (`LIBSDIR`, `-L`, system paths)

### Windows POC の特徴

- スタンドアロン Makefile（testfw に依存しない）
- シンプルな構成（実験的な実装）
- ライブラリ名を直接指定（`LIBS := calcbase`）
- Windows/Linux 両対応

**Windows POC features:**
- Standalone Makefiles (no testfw dependency)
- Simple structure (experimental implementation)
- Direct library name specification (`LIBS := calcbase`)
- Windows/Linux cross-platform support

## Linux 版との対応関係 / Correspondence with Linux Version

| 項目 | Linux (testfw) | Windows POC |
|------|----------------|-------------|
| calcbase | `BUILD` なし（static） | `BUILD` なし（static） |
| calc | `BUILD=shared` | `BUILD=shared` 対応（静的リンク機能追加） |
| add | testfw テンプレート | 変更なし（calc.lib をリンク） |
| 依存関係 | `-lcalcbase` (自動解決) | `LIBS := calcbase` (直接指定) |
| 検索パス | `LIBSDIR`, `-L`, システム | `LIBSDIR` のみ |

| Item | Linux (testfw) | Windows POC |
|------|----------------|-------------|
| calcbase | No `BUILD` (static) | No `BUILD` (static) |
| calc | `BUILD=shared` | `BUILD=shared` supported (static linking added) |
| add | testfw template | No changes (links calc.lib) |
| Dependencies | `-lcalcbase` (auto-resolved) | `LIBS := calcbase` (direct) |
| Search paths | `LIBSDIR`, `-L`, system | `LIBSDIR` only |

## デバッグ / Debugging

共通テンプレートには `debug` ターゲットがあり、設定された変数を表示できます：

The common template has a `debug` target to display configured variables:

```cmd
cd prod\calc\libsrc\calc
make -f Makefile.Windows-poc debug BUILD=shared
```

出力例 / Example output:
```
TARGET_BASE = calc
BUILD = shared
OS = Windows_NT
LIBS = calcbase
LIBSDIR = D:/Users/tetsuo/Local/repos/doxygen-sample/prod/calc/lib
STATIC_LIBS = D:/Users/tetsuo/Local/repos/doxygen-sample/prod/calc/lib/calcbase.lib
DYNAMIC_LIBS =
TARGET = calc.dll
OBJS = obj/calcHandler.obj
```

## 制限事項 / Limitations

1. **testfw テンプレートの機能は未対応**
   - inject, filter, モック機能は Windows POC には含まれていません

2. **検索パスは LIBSDIR のみ**
   - Linux 版の `-L` オプションやシステムパスの自動検索は未実装

3. **実験的な実装**
   - 本番環境での使用は推奨されません
   - 将来的に testfw との統合が必要

**Limitations:**

1. **testfw template features not supported**
   - inject, filter, mock features are not included in Windows POC

2. **Search path is LIBSDIR only**
   - Linux version's `-L` option and system path auto-search not implemented

3. **Experimental implementation**
   - Not recommended for production use
   - Future integration with testfw needed

## 今後の課題 / Future Work

1. testfw テンプレートとの統合
2. `-L` オプションのサポート
3. より高度な依存関係解決
4. Windows 専用の最適化

1. Integration with testfw templates
2. Support for `-L` option
3. More advanced dependency resolution
4. Windows-specific optimizations
