# サブディレクトリコンパイル拡張設計

## 目次

1. [概要](#概要)
2. [現状分析](#現状分析)
3. [アプローチ比較](#アプローチ比較)
4. [推奨アプローチ](#推奨アプローチ)
5. [実装ガイド](#実装ガイド)
6. [トラブルシューティング](#トラブルシューティング)
7. [今後の拡張](#今後の拡張)

## 概要

### 目的

`prod/subfolder-sample/libsrc/libsubfolder-sample/Makefile` にて、サブディレクトリ (subfolder_a, subfolder_b) のソースファイルをコンパイルできるように拡張する方法を設計し、実装する。

### 背景

makefw フレームワークは、デフォルトでカレントディレクトリの `.c` ファイルのみを収集してコンパイルする仕組みになっています。サブディレクトリに配置されたソースファイルは自動的に収集されないため、サブディレクトリを活用したモジュール構成を実現するには、適切な拡張が必要です。

### 対象プロジェクト

```
prod/subfolder-sample/libsrc/libsubfolder-sample/
├── Makefile (標準テンプレート)
├── makepart.mk (カスタマイズ用)
├── func.c (ルートレベルのソース)
├── subfolder_a/
│   └── func_a.c
└── subfolder_b/
    └── func_b.c
```

## 現状分析

### フレームワークの仕組み

#### ソース収集メカニズム

`makefw/makefiles/_collect_srcs.mk` の行103-104:

```makefile
SRCS_C := $(sort $(wildcard *.c) $(notdir $(filter %.c,$(CP_SRCS) $(LINK_SRCS))))
```

- `wildcard *.c`: カレントディレクトリの `.c` ファイルのみを検索
- サブディレクトリは検索対象外

#### オブジェクトファイルの配置

`makefw/makefiles/makelibsrc_c_cpp.mk` の行26-28:

```makefile
OBJS := $(filter-out $(OBJDIR)/%.inject.o, \
    $(sort $(addprefix $(OBJDIR)/, \
    $(notdir $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(patsubst %.cpp, %.o, $(SRCS_C) $(SRCS_CPP))))))))
```

- `$(notdir ...)`: ファイル名のみに変換
- すべてのオブジェクトファイルは `$(OBJDIR)/` (通常は `obj/`) に配置される
- サブディレクトリ構造は保持されない

### カスタマイズポイント

makepart.mk を使用した拡張が可能:

1. **ADD_SRCS**: フォルダ外のソースファイルを追加
   - シンボリックリンクまたはコピーして引き込む
   - Linux: inject/filter がなければシンボリックリンク
   - Windows: 常にコピー

2. **SRCS_C / SRCS_CPP**: 直接オーバーライド可能
   - フレームワークの想定外の使い方
   - 副作用のリスクあり

3. **LIBS / LIBSDIR**: 依存ライブラリの指定
   - 他のライブラリをリンクする際に使用
   - LIB_TYPE=shared の場合、静的ライブラリを埋め込み可能

### 現在の制約

1. サブディレクトリのソースは自動収集されない
2. オブジェクトファイルはフラットな構造でのみ配置可能
3. ファイル名の衝突リスク (同名ファイルが複数サブディレクトリにある場合)

## アプローチ比較

### 比較表

| アプローチ | 実装難易度 | 保守性 | スケーラビリティ | フレームワーク準拠 | 推奨度 |
|-----------|----------|--------|----------------|------------------|-------|
| 1. 再帰的ビルド | 中 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| 2. ADD_SRCS 手動追加 | 低 | ⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ |
| 3. ワイルドカード拡張 | 低 | ⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐ |
| 4. フラット化 | 低 | ⭐ | ⭐ | ⭐⭐⭐⭐⭐ | ⭐ |
| 5. フレームワーク拡張 | 高 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | - | ⭐ |

### アプローチ1: 再帰的ビルド (推奨)

#### 概要

サブディレクトリごとに Makefile を作成し、上位 Makefile から再帰的に make を呼び出す。

#### 実装方法

1. `subfolder_a/Makefile` と `subfolder_b/Makefile` を作成
2. 各サブディレクトリで静的ライブラリをビルド
3. 親ディレクトリの makepart.mk で LIBS を追加してリンク

#### 利点

- ✅ フレームワークの標準パターンに準拠 (prod/calc と同様の構造)
- ✅ モジュール性が高く、各サブディレクトリが独立してビルド可能
- ✅ 依存関係が明確で、並列ビルドに対応しやすい
- ✅ テンプレートの変更不要、既存の仕組みを活用
- ✅ サブディレクトリが増えても拡張しやすい
- ✅ オブジェクトファイルが各サブディレクトリに独立配置される

#### 欠点

- ❌ Makefile の数が増える (サブディレクトリごとに1つ)
- ❌ 小規模プロジェクトには過剰な構造
- ❌ ビルドプロセスが多段階になる

#### 適用ケース

- 中規模以上のプロジェクト
- サブディレクトリが今後増える可能性がある
- モジュール間の依存関係を明確にしたい
- 長期的な保守性を重視

### アプローチ2: ADD_SRCS による手動追加

#### 概要

makepart.mk で ADD_SRCS 変数にサブディレクトリのソースファイルを追加する。

#### 実装方法

```makefile
# makepart.mk
ADD_SRCS += subfolder_a/func_a.c
ADD_SRCS += subfolder_b/func_b.c
```

#### 利点

- ✅ 実装が簡単で、makepart.mk の数行で完結
- ✅ 既存のフレームワーク機能を活用
- ✅ 全ファイルが1つのライブラリにまとまる
- ✅ ビルドプロセスが単純

#### 欠点

- ❌ ファイルを手動で列挙する必要がある (メンテナンス負荷)
- ❌ サブディレクトリにファイルが追加されても自動検出されない
- ❌ Linux ではシンボリックリンク、Windows ではコピーが発生
- ❌ スケーラビリティに欠ける (ファイルが多いと管理困難)

#### 適用ケース

- ファイル数が非常に少ない (5個以下)
- 今後もファイルがあまり増えない見込み
- 簡潔な実装を優先したい

### アプローチ3: ワイルドカード拡張

#### 概要

makepart.mk で SRCS_C をオーバーライドし、サブディレクトリを含むワイルドカードを使用する。

#### 実装方法

```makefile
# makepart.mk
SRCS_C := $(sort $(wildcard *.c) $(wildcard subfolder_a/*.c) $(wildcard subfolder_b/*.c))
```

#### 利点

- ✅ 実装が簡単
- ✅ ファイルが自動検出される
- ✅ 全ファイルが1つのライブラリにまとまる

#### 欠点

- ❌ SRCS_C のオーバーライドがフレームワークの想定外の使い方
- ❌ オブジェクトファイルの配置が同一ディレクトリ (obj/) になり、名前衝突の可能性
- ❌ サブディレクトリが動的に増える場合に対応できない
- ❌ `_collect_srcs.mk` の処理と競合する可能性

#### 技術的注意点

`makelibsrc_c_cpp.mk` の `$(notdir ...)` により、サブディレクトリ構造が失われます:

```makefile
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(patsubst %.c, %.o, $(SRCS_C))))
```

例:
- `subfolder_a/func.c` → `obj/func.o`
- `subfolder_b/func.c` → `obj/func.o` (衝突!)

#### 適用ケース

- 一時的な実装
- サブディレクトリ数が固定かつ少数
- ファイル名の衝突がないことが保証される

### アプローチ4: フラット化

#### 概要

サブディレクトリのソースファイルをルートディレクトリに移動またはシンボリックリンクする。

#### 実装方法

1. 物理的にファイルを移動: `mv subfolder_a/func_a.c ./`
2. またはシンボリックリンク: `ln -s subfolder_a/func_a.c ./`

#### 利点

- ✅ フレームワークの変更が一切不要
- ✅ 既存の仕組みがそのまま動作

#### 欠点

- ❌ ディレクトリ構造が失われ、ファイルの論理的な整理ができない
- ❌ ファイル名の衝突リスクが高まる
- ❌ プロジェクトの可読性が低下
- ❌ サブディレクトリによる分類の意味がなくなる

#### 適用ケース

- サブディレクトリ構造が不要な場合
- 一時的な回避策として

### アプローチ5: フレームワーク拡張

#### 概要

`makefw/makefiles/_collect_srcs.mk` を拡張してサブディレクトリも検索する。

#### 実装方法

```makefile
# _collect_srcs.mk を変更
SRCS_C := $(sort $(shell find . -name "*.c") ...)
```

#### 利点

- ✅ すべてのプロジェクトで自動的にサブディレクトリをサポート
- ✅ フレームワークレベルの改善

#### 欠点

- ❌ フレームワーク本体の変更が必要 (サブモジュール)
- ❌ 既存プロジェクトへの影響が大きい
- ❌ オブジェクトファイルの配置戦略を再設計する必要がある
- ❌ 本プロジェクトだけの要件に対して変更範囲が大きすぎる

#### 適用ケース

- フレームワーク全体の改善が必要な場合
- 複数プロジェクトで同様の要件がある場合

## 推奨アプローチ

### 選定: アプローチ1 - 再帰的ビルド

**アプローチ1: 再帰的ビルド** を推奨します。

### 選定理由

1. **フレームワークの設計思想に合致**
   - makefw は階層的なビルドを想定
   - prod/calc プロジェクトも同様の構造を採用
   - 標準パターンに従うことで将来の保守が容易

2. **スケーラビリティ**
   - サブディレクトリが増えても容易に対応可能
   - 各サブディレクトリが独立してビルド可能
   - 並列ビルドに対応

3. **保守性**
   - 依存関係が明確
   - モジュール間の結合度が低い
   - 各モジュールの責任範囲が明確

4. **既存資産の活用**
   - 標準テンプレートをそのまま使用可能
   - フレームワークの変更不要
   - 既存のビルドロジックを再利用

### 小規模プロジェクト向けの代替案

ファイル数が非常に少なく (5-10ファイル程度)、かつ今後もあまり増えない見込みの場合は、**アプローチ2: ADD_SRCS による手動追加** も検討可能です。

ただし、長期的な保守性を考慮すると、最初から再帰的ビルドを採用する方が望ましいです。短期的な簡便性よりも、長期的な拡張性を優先することを推奨します。

## 実装ガイド

### 対象ファイル

実装では以下のファイルを作成・修正します:

| ファイル | 操作 | 説明 |
|---------|------|------|
| `prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_a/Makefile` | 新規作成 | 標準テンプレート |
| `prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_a/makepart.mk` | 新規作成 | OUTPUT_DIR設定 |
| `prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_b/Makefile` | 新規作成 | 標準テンプレート |
| `prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_b/makepart.mk` | 新規作成 | OUTPUT_DIR設定 |
| `prod/subfolder-sample/libsrc/libsubfolder-sample/makepart.mk` | 修正 | サブディレクトリビルド設定追加 |

### ステップ1: subfolder_a の Makefile 作成

**ファイル**: `prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_a/Makefile`

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

### ステップ2: subfolder_a の makepart.mk 作成

**ファイル**: `prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_a/makepart.mk`

```makefile
# 生成されるライブラリの出力先を親ライブラリと同じ場所に設定
OUTPUT_DIR := $(WORKSPACE_FOLDER)/prod/subfolder-sample/lib
```

**説明**:
- OUTPUT_DIR を親プロジェクトの lib ディレクトリに設定
- 生成される `libsubfolder_a.a` (静的ライブラリ) が親プロジェクトから参照しやすくなる

### ステップ3: subfolder_b の Makefile と makepart.mk 作成

subfolder_a と同じ内容で作成します。

**ファイル**: `prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_b/Makefile`
- subfolder_a/Makefile と同内容

**ファイル**: `prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_b/makepart.mk`
- subfolder_a/makepart.mk と同内容

### ステップ4: 親ディレクトリの makepart.mk 修正

**ファイル**: `prod/subfolder-sample/libsrc/libsubfolder-sample/makepart.mk`

```makefile
# サブディレクトリのライブラリを先にビルド
SUBDIRS := subfolder_a subfolder_b

# サブディレクトリビルド用のターゲット
.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

# メインターゲットの前にサブディレクトリをビルド
$(OUTPUT_DIR)/$(TARGET): $(SUBDIRS)

# リンク時にサブディレクトリのライブラリを含める
LIBS += subfolder_a subfolder_b
LIBSDIR += $(OUTPUT_DIR)

# 既存の設定
ifeq ($(OS),Windows_NT)
    # Windows
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DSUBFOLDER_SAMPLE_EXPORTS
    CXXFLAGS += /DSUBFOLDER_SAMPLE_EXPORTS
endif

# 生成されるライブラリを動的ライブラリ (shared) とする
# 未指定の場合 (デフォルト) は static
LIB_TYPE = shared
```

**説明**:
- **SUBDIRS**: サブディレクトリのリスト
- **.PHONY: $(SUBDIRS)**: サブディレクトリをビルドターゲットとして定義
- **$(SUBDIRS)**: 各サブディレクトリで `make` を実行
- **$(OUTPUT_DIR)/$(TARGET): $(SUBDIRS)**: メインライブラリビルド前にサブディレクトリをビルド
- **LIBS**: リンク時にサブディレクトリのライブラリを含める
- **LIBSDIR**: ライブラリの検索パスに OUTPUT_DIR を追加

### ビルドフロー

```
1. make を libsubfolder-sample/ で実行
   ↓
2. makepart.mk により、$(SUBDIRS) がビルドされる
   ├── make -C subfolder_a
   │   └── libsubfolder_a.a を生成 (prod/subfolder-sample/lib/)
   └── make -C subfolder_b
       └── libsubfolder_b.a を生成 (prod/subfolder-sample/lib/)
   ↓
3. 親ライブラリが func.c をコンパイル
   ↓
4. サブライブラリとリンク
   - libsubfolder_a.a を埋め込み
   - libsubfolder_b.a を埋め込み
   ↓
5. libsubfolder-sample.so (または .dll) を生成
```

### 生成される成果物

```
prod/subfolder-sample/lib/
├── libsubfolder_a.a (または subfolder_a.lib)
├── libsubfolder_b.a (または subfolder_b.lib)
└── libsubfolder-sample.so (または libsubfolder-sample.dll)
```

**Linux の場合**:
- `libsubfolder_a.a`: 静的ライブラリ
- `libsubfolder_b.a`: 静的ライブラリ
- `libsubfolder-sample.so`: 動的ライブラリ (サブライブラリを埋め込み)

**Windows の場合**:
- `subfolder_a.lib`: 静的ライブラリ
- `subfolder_b.lib`: 静的ライブラリ
- `libsubfolder-sample.dll`: 動的ライブラリ (サブライブラリを埋め込み)
- `libsubfolder-sample.lib`: インポートライブラリ
- `libsubfolder-sample.pdb`: デバッグシンボル

## トラブルシューティング

### 問題1: サブディレクトリのライブラリが見つからない

**症状**:
```
/usr/bin/ld: cannot find -lsubfolder_a
```

**原因**:
- LIBSDIR にライブラリの出力先が含まれていない
- サブディレクトリのビルドが失敗している

**解決方法**:
1. makepart.mk に `LIBSDIR += $(OUTPUT_DIR)` が記述されているか確認
2. サブディレクトリで単独ビルドを試す:
   ```bash
   cd subfolder_a
   make clean
   make
   ls -la ../../lib/libsubfolder_a.a
   ```

### 問題2: サブディレクトリがビルドされない

**症状**:
- サブディレクトリのライブラリが生成されない

**原因**:
- 依存関係の設定が不足
- SUBDIRS の定義が欠落

**解決方法**:
1. makepart.mk に以下が記述されているか確認:
   ```makefile
   SUBDIRS := subfolder_a subfolder_b
   .PHONY: $(SUBDIRS)
   $(SUBDIRS):
       $(MAKE) -C $@
   $(OUTPUT_DIR)/$(TARGET): $(SUBDIRS)
   ```

### 問題3: Windows でビルドエラー

**症状**:
```
LINK : fatal error LNK1181: cannot open input file 'subfolder_a.lib'
```

**原因**:
- Windows では静的ライブラリの拡張子が `.lib`
- パスの指定が Windows 形式でない

**解決方法**:
1. makefw のテンプレートが自動的に対応しているはずですが、LIBSDIR のパスを確認:
   ```makefile
   LIBSDIR += $(OUTPUT_DIR)
   ```
2. ライブラリが実際に生成されているか確認:
   ```cmd
   dir ..\..\lib\subfolder_a.lib
   ```

### 問題4: 並列ビルドで競合が発生

**症状**:
```
make[1]: *** [Makefile:10: ../../lib/libsubfolder_a.a] Error 1
```

**原因**:
- 複数のサブディレクトリが同時に同じディレクトリにライブラリを書き込もうとしている
- ディレクトリ作成の競合

**解決方法**:
1. makefw のテンプレートは `| $(OUTPUT_DIR)` で order-only 依存を使用しているため、通常は問題ありません
2. 問題が発生する場合は、順次ビルドを試す:
   ```bash
   make -j1
   ```

### 問題5: clean ターゲットがサブディレクトリを削除しない

**症状**:
- `make clean` してもサブディレクトリのオブジェクトファイルが残る

**原因**:
- 親 Makefile の clean ターゲットがサブディレクトリを処理していない

**解決方法**:
1. makepart.mk に clean ターゲットを追加:
   ```makefile
   .PHONY: clean
   clean:
       $(MAKE) -C subfolder_a clean
       $(MAKE) -C subfolder_b clean
   ```

2. または、prod/subfolder-sample/Makefile のパターンを使用:
   ```makefile
   .PHONY: clean
   clean: $(SUBDIRS)
   ```

## 検証手順

### Linux での検証

```bash
# 1. ビルドディレクトリに移動
cd /home/user/c-modernization-kit/prod/subfolder-sample/libsrc/libsubfolder-sample

# 2. クリーンビルド
make clean
make

# 3. 成果物の確認
ls -la ../../lib/
# 期待:
# - libsubfolder_a.a
# - libsubfolder_b.a
# - libsubfolder-sample.so

# 4. サブディレクトリのオブジェクトファイル確認
ls -la subfolder_a/obj/
ls -la subfolder_b/obj/
# 期待: 各ディレクトリに .o ファイルが生成されている

# 5. シンボル確認
nm ../../lib/libsubfolder-sample.so | grep func
# 期待: func, func_a, func_b のシンボルが含まれている

# 6. 依存ライブラリ確認
ldd ../../lib/libsubfolder-sample.so
# 期待: 標準ライブラリのみ (サブライブラリは埋め込まれている)
```

### Windows での検証

```cmd
REM 1. ビルドディレクトリに移動
cd C:\path\to\c-modernization-kit\prod\subfolder-sample\libsrc\libsubfolder-sample

REM 2. クリーンビルド
make clean
make

REM 3. 成果物の確認
dir ..\..\lib\
REM 期待:
REM - subfolder_a.lib
REM - subfolder_b.lib
REM - libsubfolder-sample.dll
REM - libsubfolder-sample.lib
REM - libsubfolder-sample.pdb

REM 4. サブディレクトリのオブジェクトファイル確認
dir subfolder_a\obj\
dir subfolder_b\obj\
REM 期待: 各ディレクトリに .obj ファイルが生成されている

REM 5. シンボル確認
dumpbin /EXPORTS ..\..\lib\libsubfolder-sample.dll
REM 期待: func, func_a, func_b のエクスポートが含まれている

REM 6. 依存DLL確認
dumpbin /DEPENDENTS ..\..\lib\libsubfolder-sample.dll
REM 期待: システムDLLのみ (サブライブラリは埋め込まれている)
```

## 今後の拡張

### 1. サブディレクトリの自動検出機能

現在は SUBDIRS を手動で列挙していますが、自動検出も可能です:

```makefile
# makepart.mk
# サブディレクトリの自動検出 (Makefile が存在するディレクトリ)
SUBDIRS := $(dir $(wildcard */Makefile))
SUBDIRS := $(SUBDIRS:/=)
```

**注意**: この方法はすべてのサブディレクトリを無条件でビルドするため、明示的な制御が必要な場合は手動列挙を推奨します。

### 2. テンプレートファイル生成スクリプト

サブディレクトリの Makefile と makepart.mk を自動生成するスクリプトを作成:

```bash
#!/bin/bash
# create-submodule.sh
SUBDIR=$1
mkdir -p "$SUBDIR"
cat > "$SUBDIR/Makefile" <<'EOF'
# Makefile テンプレート
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk
include $(WORKSPACE_FOLDER)/makefw/makefiles/makemain.mk
EOF

cat > "$SUBDIR/makepart.mk" <<EOF
OUTPUT_DIR := \$(WORKSPACE_FOLDER)/prod/subfolder-sample/lib
EOF
```

### 3. ドキュメント生成の統合

Doxygen でサブディレクトリのドキュメントも含める:

```
# Doxyfile.part.subfolder-sample
INPUT += prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_a \
         prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_b
```

### 4. テストフレームワークの統合

各サブディレクトリのテストを追加:

```
test/src/subfolder-sample/libsubfolder-sampleTest/
├── subfolder_aTest/
│   └── func_aTest.cc
└── subfolder_bTest/
    └── func_bTest.cc
```

### 5. 条件付きビルド

特定のサブディレクトリのみをビルドする機能:

```makefile
# makepart.mk
ifdef BUILD_SUBFOLDER_A_ONLY
    SUBDIRS := subfolder_a
else ifdef BUILD_SUBFOLDER_B_ONLY
    SUBDIRS := subfolder_b
else
    SUBDIRS := subfolder_a subfolder_b
endif
```

使用例:
```bash
make BUILD_SUBFOLDER_A_ONLY=1
```

## まとめ

### 推奨実装

1. **アプローチ1: 再帰的ビルド** を採用
2. 各サブディレクトリに Makefile と makepart.mk を作成
3. 親ディレクトリの makepart.mk でサブディレクトリをビルド・リンク

### 得られる効果

- ✅ モジュール性の向上
- ✅ スケーラビリティの確保
- ✅ 保守性の向上
- ✅ フレームワークの標準パターンに準拠

### 次のステップ

1. 本ドキュメントに基づいて実装
2. ビルドとテストの実行
3. 必要に応じて追加のサブディレクトリを作成
4. ドキュメント生成との統合を検討

## 参考資料

- makefw フレームワークのテンプレート:
  - `makefw/makefiles/prepare.mk`
  - `makefw/makefiles/makemain.mk`
  - `makefw/makefiles/makelibsrc_c_cpp.mk`
  - `makefw/makefiles/_collect_srcs.mk`

- 参考プロジェクト:
  - `prod/calc/libsrc/`: 階層的ビルドの実例
  - `prod/calc/libsrc/calc/makepart.mk`: 依存ライブラリの指定例

- Claude Code 設定:
  - `CLAUDE.md`: プロジェクト概要とガイドライン
