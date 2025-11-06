# Windows PoC (Proof of Concept) ビルドシステム

## 概要

このプロジェクトの Windows 向け実験的なビルドシステムです。MSVC (Microsoft Visual C++) と MinGW 環境でのクロスプラットフォームビルドに対応しています。

## 前提条件

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

## ビルド方法

### ビルド実行

```cmd
build-poc.bat
```

このコマンドで以下がビルドされます（実験的実装として libcalcbase は静的固定、libcalc は動的固定）：

**ライブラリ / Libraries:**
- `prod/calc/lib/calcbase.lib` - 基本計算関数ライブラリ（静的ライブラリ固定）
- `prod/calc/lib/calc.dll` - 計算ハンドラーライブラリ（DLL 固定、calcbase.lib を内部に静的リンク）
- `prod/calc/lib/calc.lib` - calc.dll のインポートライブラリ

**コマンド / Commands:**
- `prod/calc/src/add/add.exe` - 加算コマンド（calcbase.lib のみをリンク）
- `prod/calc/src/calc/calc.exe` - 計算コマンド（calc.lib のみをリンク）
- `prod/calc/src/shared-and-static-add/shared-and-static-add.exe` - 加算コマンド（calc.lib と calcbase.lib の両方をリンク）

**重要**: libcalc は動的ライブラリ（DLL）として固定実装されており、calcbase.lib を内部に静的リンクします。shared-and-static-add.exe は、DLL と静的ライブラリの両方をリンクする実験的な実装例です。

## ファイル構成

### PoC Makefile ファイル

```
prod/calc/
├── libsrc/
│   ├── makelibsrc-windows-poc.mk       # ライブラリビルド用共通テンプレート
│   ├── calcbase/
│   │   ├── Makefile                    # Linux 版（testfw テンプレート使用）
│   │   └── Makefile.Windows-poc        # Windows PoC 版（静的ライブラリ固定）
│   └── calc/
│       ├── Makefile                    # Linux 版（testfw テンプレート使用）
│       └── Makefile.Windows-poc        # Windows PoC 版（動的ライブラリ固定）
└── src/
    ├── add/
    │   ├── Makefile                    # Linux 版（testfw テンプレート使用）
    │   └── Makefile.Windows-poc        # Windows PoC 版
    ├── calc/
    │   ├── Makefile                    # Linux 版（testfw テンプレート使用）
    │   └── Makefile.Windows-poc        # Windows PoC 版
    └── shared-and-static-add/
        ├── Makefile                    # Linux 版（testfw テンプレート使用）
        └── Makefile.Windows-poc        # Windows PoC 版（DLL + 静的ライブラリリンク）
```

**共通テンプレートの導入**: calcbase と calc の Makefile.Windows-poc はほぼ同じ内容だったため、`makelibsrc-windows-poc.mk` として共通化しました。各 Makefile は必要最小限の設定のみを記述し、テンプレートを include します。

### ビルドスクリプト

- `build-poc.bat` - 全体のビルドスクリプト（libcalcbase, libcalc, add, calc, shared-and-static-add を順次ビルド）

## 主な機能

### 1. 共通テンプレートによる統合

calcbase と calc の Makefile.Windows-poc はほぼ同じ内容だったため、`makelibsrc-windows-poc.mk` として共通化しました。

**各 Makefile で設定する変数:**
- `TARGET_BASE`: ライブラリ名のベース（例: calcbase, calc）- 必須
- `BUILD`: ビルドモード（static/shared）- 未設定の場合は static、calc では shared 固定
- `LIBS`: 依存ライブラリ（例: calcbase）- 必要な場合のみ
- `LIBSDIR`: ライブラリ検索パス - LIBS を使用する場合のみ
- `EXTRA_CFLAGS`: 追加のコンパイラフラグ（例: /DCALC_EXPORTS）- 必要な場合のみ

**calcbase の例 / calcbase example (静的ライブラリ固定 / static library only):**
```makefile
TARGET_BASE := calcbase
# BUILD 未設定 → デフォルトで static
# BUILD not set → defaults to static
include ../makelibsrc-windows-poc.mk
```

**calc の例 / calc example (動的ライブラリ固定 / dynamic library only):**
```makefile
TARGET_BASE := calc
BUILD := shared  # 動的ライブラリ（DLL）固定 / Fixed to dynamic library (DLL)
EXTRA_CFLAGS := /DCALC_EXPORTS
LIBSDIR = $(WORKSPACE_FOLDER)/prod/calc/lib
LIBS := calcbase
include ../makelibsrc-windows-poc.mk
```

### 2. 静的ライブラリの自動組み込み

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

### 3. クロスプラットフォーム対応

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

### 4. ライブラリ構成

本 PoC では、実験的実装として以下の構成で固定されています：

| ライブラリ | BUILD 設定 | Windows | Linux | 説明 |
|-----------|----------|---------|-------|------|
| libcalcbase | 未設定 (static) | `.lib` | `.a` | 静的ライブラリ固定 |
| libcalc | `shared` 固定 | `.dll` + `.lib` | `.so` | 動的ライブラリ固定 + インポートライブラリ |

**共通テンプレートの BUILD 変数サポート:**

共通テンプレート `makelibsrc-windows-poc.mk` 自体は BUILD 変数による static/shared の切り替えをサポートしています。各ライブラリの Makefile にて static/shared が固定的に設定されることに対応しています。

## Windows PoC の設計思想

### ライブラリ構成の固定化

本 PoC では、実験的実装として以下の構成で固定しています：
- **libcalcbase**: 静的ライブラリのみ（BUILD 変数未設定、デフォルトで static）
- **libcalc**: 動的ライブラリのみ（BUILD := shared を Makefile 内で固定）

この構成により、以下のメリットがあります：
1. calc.dll が calcbase.lib を内部に静的リンクする
2. 依存関係が単純化され、配布時に calc.dll のみを配置すれば動作
3. Linux 版（testfw/makefiles/makelibsrc.mk）と同様の動作を実現

### 実装ファイル

**ライブラリ:**
1. `prod/calc/libsrc/makelibsrc-windows-poc.mk` - 共通テンプレート（BUILD 変数による static/shared 切り替えをサポート）
2. `prod/calc/libsrc/calcbase/Makefile.Windows-poc` - calcbase ビルド設定（BUILD 未設定 → static）
3. `prod/calc/libsrc/calc/Makefile.Windows-poc` - calc ビルド設定（BUILD := shared 固定）

**コマンド:**
- `prod/calc/src/add/Makefile.Windows-poc` - add コマンド（calc.lib のみリンク）
- `prod/calc/src/calc/Makefile.Windows-poc` - calc コマンド（calc.lib のみリンク）
- `prod/calc/src/shared-and-static-add/Makefile.Windows-poc` - shared-and-static-add コマンド（calc.lib + calcbase.lib をリンク）

**ビルドスクリプト:**
- `build-poc.bat` - 全体のビルドスクリプト（libcalcbase, libcalc, add, calc, shared-and-static-add を順次ビルド）

**注**: testfw 以下のファイルは PoC としては変更なし。Windows PoC は testfw から独立した実装です。

## testfw テンプレートとの違い

### testfw (Linux 版) の特徴

- `testfw/makefiles/makelibsrc.mk` を使用
- 高度な機能（inject, filter, テストフレームワーク統合）
- `-l` オプションの自動解決（`.a` ファイルを検索）
- 複数の検索パス対応（`LIBSDIR`, `-L`, システムパス）

### Windows PoC の特徴

- スタンドアロン Makefile（testfw に依存しない）
- シンプルな構成（実験的な実装）
- ライブラリ名を直接指定（`LIBS := calcbase`）
- Windows/Linux 両対応

## Linux 版との対応関係

| 項目 | Linux (testfw) | Windows PoC |
|------|----------------|-------------|
| calcbase | `BUILD` なし（static 固定） | `BUILD` なし（static 固定） |
| calc | `BUILD=shared`（calc.mk で固定） | `BUILD := shared`（Makefile で固定） |
| add | testfw テンプレート（calc.lib のみリンク） | calc.lib のみリンク |
| calc コマンド | testfw テンプレート（calc.lib のみリンク） | calc.lib のみリンク |
| shared-and-static-add | testfw テンプレート（calc.lib + calcbase.lib） | calc.lib + calcbase.lib |
| 依存関係（ライブラリ） | `-lcalcbase` (自動解決) | `LIBS := calcbase` (直接指定) |
| 検索パス | `LIBSDIR`, `-L`, システム | `LIBSDIR` のみ |

| Item | Linux (testfw) | Windows PoC |
|------|----------------|-------------|
| calcbase | No `BUILD` (static only) | No `BUILD` (static only) |
| calc | `BUILD=shared` (fixed in calc.mk) | `BUILD := shared` (fixed in Makefile) |
| add | testfw template (links calc.lib only) | Links calc.lib only |
| calc command | testfw template (links calc.lib only) | Links calc.lib only |
| shared-and-static-add | testfw template (calc.lib + calcbase.lib) | calc.lib + calcbase.lib |
| Dependencies (library) | `-lcalcbase` (auto-resolved) | `LIBS := calcbase` (direct) |
| Search paths | `LIBSDIR`, `-L`, system | `LIBSDIR` only |

## デバッグ

共通テンプレートには `debug` ターゲットがあり、設定された変数を表示できます：

The common template has a `debug` target to display configured variables:

**calcbase のデバッグ:**
```cmd
cd prod\calc\libsrc\calcbase
make -f Makefile.Windows-poc debug
```

出力例 / Example output:
```
TARGET_BASE = calcbase
BUILD = static
OS = Windows_NT
LIBS =
LIBSDIR =
STATIC_LIBS =
DYNAMIC_LIBS =
TARGET = calcbase.lib
OBJS = obj/add.obj
```

**calc のデバッグ:**
```cmd
cd prod\calc\libsrc\calc
make -f Makefile.Windows-poc debug
```

出力例（BUILD は Makefile 内で shared 固定）/ Example output (BUILD fixed to shared in Makefile):
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

## 制限事項

1. **ライブラリ構成は固定**
   - libcalcbase は静的ライブラリ固定（BUILD 変数で変更不可）
   - libcalc は動的ライブラリ（DLL）固定（BUILD 変数で変更不可）

2. **testfw テンプレートの機能は未対応**
   - inject, filter, モック機能は Windows PoC には含まれていません

3. **検索パスは LIBSDIR のみ**
   - Linux 版に実装がある `-L` オプションやシステムパスの自動検索は未実装

4. **実験的な実装**
   - 本番環境での使用は推奨されません
   - 将来的に testfw との統合が必要

## 今後の課題 / Future Work

1. testfw テンプレートとの統合
2. `-L` オプションのサポート
3. より高度な依存関係解決
4. Windows 専用の最適化
