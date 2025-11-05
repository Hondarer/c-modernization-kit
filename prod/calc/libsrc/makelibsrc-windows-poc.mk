# Windows PoC: ライブラリビルド用共通テンプレート
# Common template for library builds in Windows PoC

# このファイルを使用する Makefile で設定する変数:
# Variables to be set in Makefiles that use this template:
#   TARGET_BASE - ライブラリ名のベース (例: calcbase, calc)
#   LIBS        - 依存ライブラリのリスト (オプション、例: calcbase)
#   LIBSDIR     - ライブラリ検索パス (オプション、LIBS を使用する場合)

# ワークスペースのディレクトリ検索
# Search for workspace directory
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

# ディレクトリ設定
# Directory settings
OBJDIR := obj
TARGETDIR := $(WORKSPACE_FOLDER)/prod/calc/lib

# BUILD 設定 (static または shared)
# BUILD setting (static or shared)
ifeq ($(BUILD),)
    BUILD := static
endif

# OS 判定
# OS detection
ifeq ($(OS),Windows_NT)
    # Windows 環境 (MSVC)
    # Windows environment (MSVC)
    CC := cl
    AR := lib
    LD := link
    OBJEXT := .obj

    # ビルドモードによる拡張子とフラグの設定
    # Extension and flags based on build mode
    ifeq ($(BUILD),shared)
        LIBEXT := .dll
        IMPLIBEXT := .lib
        CFLAGS := /W4 /Zi /TC /nologo /utf-8 /FS /MD /LD /Fd$(TARGETDIR)/$(TARGET_BASE).pdb /I$(WORKSPACE_FOLDER)/prod/calc/include $(EXTRA_CFLAGS)
        LDFLAGS := /DLL /DEBUG /PDB:$(TARGETDIR)/$(TARGET_BASE).pdb /IMPLIB:$(TARGETDIR)/$(TARGET_BASE)$(IMPLIBEXT) /ILK:$(OBJDIR)/$(TARGET_BASE).ilk
    else
        LIBEXT := .lib
        CFLAGS := /W4 /Zi /TC /nologo /utf-8 /FS /MD /Fd$(TARGETDIR)/$(TARGET_BASE).pdb /I$(WORKSPACE_FOLDER)/prod/calc/include $(EXTRA_CFLAGS)
    endif
else
    # Linux 環境 (GCC)
    # Linux environment (GCC)
    CC := gcc
    AR := ar
    LD := gcc
    OBJEXT := .o

    ifeq ($(BUILD),shared)
        LIBEXT := .so
        CFLAGS := -Wall -Wextra -g -std=c99 -fPIC -I$(WORKSPACE_FOLDER)/prod/calc/include
        LDFLAGS := -shared
    else
        LIBEXT := .a
        CFLAGS := -Wall -Wextra -g -std=c99 -I$(WORKSPACE_FOLDER)/prod/calc/include
    endif
endif

# ソースファイル
# Source files
SRCS_C := $(wildcard *.c)

# オブジェクトファイル
# Object files
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(patsubst %.c, %$(OBJEXT), $(SRCS_C))))

# ターゲット名
# Target name
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET_BASE)$(LIBEXT)
else
    ifeq ($(BUILD),shared)
        TARGET := lib$(TARGET_BASE)$(LIBEXT)
    else
        TARGET := lib$(TARGET_BASE)$(LIBEXT)
    endif
endif

# ライブラリファイルの解決（BUILD=shared かつ LIBS が定義されている場合のみ）
# Resolve library files (only when BUILD=shared and LIBS is defined)
ifeq ($(BUILD),shared)
    ifneq ($(LIBS),)
        # 静的ライブラリファイルの検索
        # Search for static library files
        ifeq ($(OS),Windows_NT)
            # Windows: .lib を検索
            # Windows: search for .lib
            STATIC_LIBS := $(foreach lib,$(LIBS),$(wildcard $(LIBSDIR)/$(lib).lib))
        else
            # Linux: .a を検索
            # Linux: search for .a
            STATIC_LIBS := $(foreach lib,$(LIBS),$(wildcard $(LIBSDIR)/lib$(lib).a))
        endif

        # 見つからないライブラリは動的リンク用フラグとして保持
        # Libraries not found are kept as dynamic link flags
        ifeq ($(OS),Windows_NT)
            FOUND_LIBS := $(notdir $(basename $(STATIC_LIBS)))
            NOT_FOUND_LIBS := $(filter-out $(FOUND_LIBS),$(LIBS))
            DYNAMIC_LIBS := $(addsuffix .lib,$(NOT_FOUND_LIBS))
        else
            FOUND_LIBS := $(patsubst lib%.a,%,$(notdir $(STATIC_LIBS)))
            NOT_FOUND_LIBS := $(filter-out $(FOUND_LIBS),$(LIBS))
            DYNAMIC_LIBS := $(addprefix -l,$(NOT_FOUND_LIBS))
        endif
    endif
endif

# ビルドルール
# Build rules
.PHONY: all
all: $(TARGETDIR)/$(TARGET)

ifeq ($(OS),Windows_NT)
    # Windows (MSVC) のビルドルール
    # Windows (MSVC) build rules
    ifeq ($(BUILD),shared)
        # 共有ライブラリ（DLL）のビルド
        # Build shared library (DLL)
$(TARGETDIR)/$(TARGET): $(OBJS) $(STATIC_LIBS) | $(TARGETDIR)
	$(LD) /OUT:$@ $(OBJS) $(STATIC_LIBS) $(DYNAMIC_LIBS) $(LDFLAGS)
	@if [ -f "$(TARGETDIR)/$(TARGET_BASE).exp" ]; then mv "$(TARGETDIR)/$(TARGET_BASE).exp" "$(OBJDIR)/"; fi
    else
        # 静的ライブラリのビルド
        # Build static library
$(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
	$(AR) /OUT:$@ $(OBJS)
    endif

$(OBJDIR)/%$(OBJEXT): %.c | $(OBJDIR) $(TARGETDIR)
	$(CC) $(CFLAGS) /c /Fo$@ $<
else
    # Linux (GCC) のビルドルール
    # Linux (GCC) build rules
    ifeq ($(BUILD),shared)
        # 共有ライブラリ（.so）のビルド
        # Build shared library (.so)
$(TARGETDIR)/$(TARGET): $(OBJS) $(STATIC_LIBS) | $(TARGETDIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(STATIC_LIBS) $(DYNAMIC_LIBS)
    else
        # 静的ライブラリのビルド
        # Build static library
$(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
	$(AR) rvs $@ $(OBJS)
    endif

$(OBJDIR)/%$(OBJEXT): %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<
endif

# ディレクトリ作成
# Create directories
$(TARGETDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

# クリーン
# Clean
.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGETDIR)/$(TARGET)
ifeq ($(OS),Windows_NT)
	rm -f $(TARGETDIR)/$(TARGET_BASE).pdb
    ifeq ($(BUILD),shared)
	rm -f $(TARGETDIR)/$(TARGET_BASE)$(IMPLIBEXT)
    endif
endif

# デバッグ用: 変数の表示
# Debug: Display variables
.PHONY: debug
debug:
	@echo "TARGET_BASE = $(TARGET_BASE)"
	@echo "BUILD = $(BUILD)"
	@echo "OS = $(OS)"
	@echo "LIBS = $(LIBS)"
	@echo "LIBSDIR = $(LIBSDIR)"
	@echo "STATIC_LIBS = $(STATIC_LIBS)"
	@echo "DYNAMIC_LIBS = $(DYNAMIC_LIBS)"
	@echo "TARGET = $(TARGET)"
	@echo "OBJS = $(OBJS)"
