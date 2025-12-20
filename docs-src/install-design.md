# Install 機能設計書

## 概要

本ドキュメントは、makefw フレームワークおよび prod プロジェクトに `install` ターゲットを追加するための設計を説明します。

## 目的

ビルドした成果物（実行ファイル、ライブラリ、ヘッダーファイル）を、指定したディレクトリにインストールする機能を提供します。

## 現在の構造分析

### 成果物の生成場所

現在、ビルドシステムは以下の場所に成果物を生成しています：

- **ライブラリ**: `prod/calc/lib/`
  - 静的ライブラリ: `libcalcbase.a`
  - 動的ライブラリ: `libcalc.so` (Linux) / `calc.dll` (Windows)
- **実行ファイル**: `prod/calc/src/*/`
  - `prod/calc/src/add/add`
  - `prod/calc/src/calc/calc`
  - `prod/calc/src/shared-and-static-add/shared-and-static-add`
- **ヘッダーファイル**: `prod/calc/include/`
  - `libcalc.h`
  - `libcalcbase.h`
  - `libcalc_const.h`

### フレームワーク構成

makefw フレームワークは以下のファイルで構成されています：

- `prepare.mk` - 共通設定（コンパイラ、フラグ、定義など）
- `makesrc.mk` - 実行ファイルのビルドルール
- `makelibsrc.mk` - ライブラリファイルのビルドルール
- `_collect_srcs.mk` - ソースファイルの収集
- `_flags.mk` - コンパイラフラグの設定
- `_should_skip.mk` - ビルドスキップ判定

## 設計方針

### 基本方針

1. **フレームワーク層での抽象化**: 共通の install ロジックを `_install.mk` に集約
2. **宣言的な設定**: 各 Makefile で変数を設定するだけで install 対象を指定
3. **既存パターンとの一貫性**: `clean` や `test` ターゲットと同様の構造
4. **クロスプラットフォーム対応**: Linux/Windows の違いを吸収
5. **柔軟性**: PREFIX やインストール先ディレクトリをカスタマイズ可能

### インストール先ディレクトリ

標準的な Unix/Linux の慣習に従い、以下のディレクトリ構造を採用します：

#### Linux

```
$(PREFIX)/
├── bin/          # 実行ファイル
├── lib/          # ライブラリファイル (.a, .so)
└── include/      # ヘッダーファイル
```

デフォルトの `PREFIX`: `/usr/local`

#### Windows

```
$(PREFIX)/
├── bin/          # 実行ファイル (.exe), DLL (.dll)
├── lib/          # インポートライブラリ (.lib)
└── include/      # ヘッダーファイル
```

デフォルトの `PREFIX`: `C:/Program Files/$(PROJECT_NAME)`

## 実装設計

### 1. 新規ファイル: `makefw/makefiles/_install.mk`

install ターゲットの共通ロジックを定義する新規ファイルを作成します。

```makefile
# Install ターゲット用の共通設定
# Common settings for install target

# インストール先の基本設定
# Basic installation directory settings
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
LIBDIR ?= $(PREFIX)/lib
INCDIR_INSTALL ?= $(PREFIX)/include

# Windows の場合
# For Windows
ifeq ($(OS),Windows_NT)
    PREFIX ?= C:/Program Files/$(PROJECT_NAME)
    BINDIR ?= $(PREFIX)/bin
    LIBDIR ?= $(PREFIX)/lib
    INCDIR_INSTALL ?= $(PREFIX)/include
endif

# メインの install ターゲット
# Main install target
.PHONY: install
install: install-headers install-libs install-bins

# ヘッダーファイルのインストール
# Install header files
.PHONY: install-headers
install-headers:
ifneq ($(INSTALL_HEADERS),)
	@echo "Installing headers to $(INCDIR_INSTALL)/$(INSTALL_HEADERS_SUBDIR)"
	@mkdir -p $(INCDIR_INSTALL)/$(INSTALL_HEADERS_SUBDIR)
	@for header in $(INSTALL_HEADERS); do \
		echo "  Installing $$header"; \
		cp -p $$header $(INCDIR_INSTALL)/$(INSTALL_HEADERS_SUBDIR)/; \
	done
endif

# ライブラリのインストール
# Install library files
.PHONY: install-libs
install-libs:
ifneq ($(INSTALL_LIB),)
	@echo "Installing library to $(LIBDIR)"
	@mkdir -p $(LIBDIR)
	@echo "  Installing $(TARGETDIR)/$(TARGET)"
	@cp -p $(TARGETDIR)/$(TARGET) $(LIBDIR)/
ifeq ($(OS),Windows_NT)
    # Windows の場合、インポートライブラリもコピー
    # For Windows, also copy import library
	@if [ -f "$(TARGETDIR)/$(patsubst %.dll,%.lib,$(TARGET))" ]; then \
		echo "  Installing $(TARGETDIR)/$(patsubst %.dll,%.lib,$(TARGET))"; \
		cp -p $(TARGETDIR)/$(patsubst %.dll,%.lib,$(TARGET)) $(LIBDIR)/; \
	fi
endif
endif

# 実行ファイルのインストール
# Install binary files
.PHONY: install-bins
install-bins:
ifneq ($(INSTALL_BIN),)
	@echo "Installing binary to $(BINDIR)"
	@mkdir -p $(BINDIR)
	@echo "  Installing $(TARGETDIR)/$(TARGET)"
	@cp -p $(TARGETDIR)/$(TARGET) $(BINDIR)/
ifeq ($(OS),Windows_NT)
    # Windows の場合、PDB ファイルもコピー
    # For Windows, also copy PDB file
	@if [ -f "$(patsubst %.exe,%.pdb,$(TARGETDIR)/$(TARGET))" ]; then \
		echo "  Installing $(patsubst %.exe,%.pdb,$(TARGETDIR)/$(TARGET))"; \
		cp -p $(patsubst %.exe,%.pdb,$(TARGETDIR)/$(TARGET)) $(BINDIR)/; \
	fi
endif
endif
```

### 2. `makesrc.mk` の修正

ファイルの最後に以下を追加します：

```makefile
# Install サポート
# Install support
include $(WORKSPACE_FOLDER)/makefw/makefiles/_install.mk
```

### 3. `makelibsrc.mk` の修正

ファイルの最後に以下を追加します：

```makefile
# Install サポート
# Install support
include $(WORKSPACE_FOLDER)/makefw/makefiles/_install.mk
```

### 4. トップレベル Makefile の修正

`prod/Makefile` および `prod/calc/Makefile` に install ターゲットを追加します：

```makefile
.PHONY: install
install : $(SUBDIRS)
```

これにより、サブディレクトリの install ターゲットが再帰的に実行されます。

### 5. プロジェクト側の設定

各 Makefile で以下の変数を設定して、インストール対象を指定します：

#### 変数一覧

- `INSTALL_BIN` - 実行ファイルをインストールする場合に `yes` を設定
- `INSTALL_LIB` - ライブラリをインストールする場合に `yes` を設定
- `INSTALL_HEADERS` - インストールするヘッダーファイルのリスト
- `INSTALL_HEADERS_SUBDIR` - ヘッダーファイルのインストール先サブディレクトリ

#### 設定例

**`prod/calc/libsrc/calcbase/Makefile`**:

```makefile
# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (Makefile テンプレートより前に include)
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

# ライブラリをインストール対象に指定
INSTALL_LIB = yes

# ヘッダーファイルをインストール対象に指定
INSTALL_HEADERS = ../../include/libcalcbase.h
INSTALL_HEADERS_SUBDIR = calc

# libsrc の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makelibsrc.mk
```

**`prod/calc/libsrc/calc/Makefile`**:

```makefile
# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (Makefile テンプレートより前に include)
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

# ライブラリの追加
LIBS += calcbase

# ライブラリをインストール対象に指定
INSTALL_LIB = yes

# ヘッダーファイルをインストール対象に指定
INSTALL_HEADERS = ../../include/libcalc.h ../../include/libcalc_const.h
INSTALL_HEADERS_SUBDIR = calc

# libsrc の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makelibsrc.mk
```

**`prod/calc/src/add/Makefile`**:

```makefile
# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (Makefile テンプレートより前に include)
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

# ライブラリの指定
# libcalcbase.a のみ
LIBS += calcbase

# 実行ファイルをインストール対象に指定
INSTALL_BIN = yes

# src の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makesrc.mk
```

**`prod/calc/src/calc/Makefile`**:

```makefile
# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (Makefile テンプレートより前に include)
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

# ライブラリの指定
# libcalc.so のみ
LIBS += calc

# 実行ファイルをインストール対象に指定
INSTALL_BIN = yes

# src の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makesrc.mk
```

## 使用方法

### 基本的な使用方法

#### デフォルトの場所にインストール

```bash
cd /home/user/c-modernization-kit/prod
make install
```

Linux の場合: `/usr/local` 配下にインストールされます。

#### カスタムプレフィックスを指定してインストール

```bash
cd /home/user/c-modernization-kit/prod
make install PREFIX=/opt/myapp
```

指定した `PREFIX` 配下にインストールされます。

#### 個別のディレクトリを指定してインストール

```bash
cd /home/user/c-modernization-kit/prod
make install PREFIX=/usr BINDIR=/usr/bin LIBDIR=/usr/lib64
```

### Windows での使用方法

```cmd
cd C:\Users\user\c-modernization-kit\prod
make install PREFIX="C:/MyApp"
```

## インストール結果の例

### Linux の場合

デフォルト設定 (`PREFIX=/usr/local`) でインストールした場合：

```
/usr/local/
├── bin/
│   ├── add
│   ├── calc
│   └── shared-and-static-add
├── lib/
│   ├── libcalcbase.a
│   └── libcalc.so
└── include/
    └── calc/
        ├── libcalcbase.h
        ├── libcalc.h
        └── libcalc_const.h
```

### Windows の場合

デフォルト設定 (`PREFIX=C:/Program Files/c-modernization-kit`) でインストールした場合：

```
C:/Program Files/c-modernization-kit/
├── bin/
│   ├── add.exe
│   ├── add.pdb
│   ├── calc.exe
│   ├── calc.pdb
│   ├── shared-and-static-add.exe
│   ├── shared-and-static-add.pdb
│   └── calc.dll
├── lib/
│   ├── calcbase.lib
│   └── calc.lib
└── include/
    └── calc/
        ├── libcalcbase.h
        ├── libcalc.h
        └── libcalc_const.h
```

## メリット

1. **フレームワーク層での抽象化**: 各プロジェクトで共通の install ロジックを再利用可能
2. **柔軟な設定**: PREFIX や各ディレクトリを簡単にカスタマイズ可能
3. **クロスプラットフォーム対応**: Linux/Windows の違いを吸収
4. **既存構造との整合性**: 現在の `clean`, `test` ターゲットと同様の構造
5. **段階的な導入**: 必要な Makefile から順次 `INSTALL_*` 変数を設定可能
6. **宣言的な設定**: 各 Makefile で変数を設定するだけで、複雑なインストールロジックを記述する必要がない

## 実装手順

以下の順序で実装を進めます：

1. `makefw/makefiles/_install.mk` を新規作成
2. `makefw/makefiles/makesrc.mk` に `_install.mk` の include を追加
3. `makefw/makefiles/makelibsrc.mk` に `_install.mk` の include を追加
4. `prod/Makefile` に install ターゲットを追加
5. `prod/calc/Makefile` に install ターゲットを追加
6. 各ライブラリ・実行ファイルの Makefile に `INSTALL_*` 変数を設定
   - `prod/calc/libsrc/calcbase/Makefile`
   - `prod/calc/libsrc/calc/Makefile`
   - `prod/calc/src/add/Makefile`
   - `prod/calc/src/calc/Makefile`
   - `prod/calc/src/shared-and-static-add/Makefile`

## 追加検討事項

将来的に以下の機能を追加することも検討できます：

### 1. uninstall ターゲット

インストールしたファイルを削除する機能。実装には、インストール時にファイルリストを記録する仕組みが必要です。

```makefile
.PHONY: uninstall
uninstall:
	@echo "Uninstalling files from $(PREFIX)"
	-rm -f $(BINDIR)/$(TARGET)
	-rm -f $(LIBDIR)/$(TARGET)
	# etc...
```

### 2. DESTDIR サポート

パッケージング（RPM, DEB など）時に使用する `DESTDIR` 変数のサポート。

```makefile
DESTDIR ?=
install:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp -p $(TARGET) $(DESTDIR)$(BINDIR)/
```

### 3. 権限管理

システムディレクトリへのインストール時に `sudo` が必要な場合の処理。

```makefile
install:
	@if [ ! -w $(BINDIR) ]; then \
		echo "Warning: $(BINDIR) is not writable. You may need to run 'sudo make install'"; \
	fi
```

### 4. インストールログ

インストールしたファイルのリストを記録し、後で uninstall に使用。

```makefile
INSTALL_LOG ?= $(PREFIX)/.install_manifest.txt

install:
	@echo "$(BINDIR)/$(TARGET)" >> $(INSTALL_LOG)
```

### 5. ファイルの上書き確認

既存ファイルが存在する場合の上書き確認機能。

```makefile
install-bins:
	@if [ -f "$(BINDIR)/$(TARGET)" ]; then \
		read -p "$(BINDIR)/$(TARGET) already exists. Overwrite? [y/N] " answer; \
		if [ "$$answer" != "y" ]; then exit 1; fi; \
	fi
	cp -p $(TARGET) $(BINDIR)/
```

## まとめ

この設計により、makefw フレームワークの設計思想を維持しつつ、標準的な Unix/Linux の `make install` の慣習に従った実装が可能になります。クロスプラットフォーム対応、柔軟な設定、既存構造との整合性を保ちながら、段階的に導入できる点が特徴です。
