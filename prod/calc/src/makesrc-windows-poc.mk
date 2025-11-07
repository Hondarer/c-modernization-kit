# Windows PoC用のビルドテンプレート
# このファイルは prod/calc/src 配下の各サブディレクトリの Makefile から include される

# ディレクトリ設定
OBJDIR := obj
TARGETDIR := .
TARGET_NAME := $(notdir $(CURDIR))

# ビルド設定
ifneq ($(OS),Windows_NT)
    # Linux
    CC := gcc
    LD := gcc
    OBJEXT := .o
    EXEEXT :=
    CFLAGS := -Wall -Wextra -g -std=c99 -I$(WORKSPACE_FOLDER)/prod/calc/include
    LDFLAGS := -L$(WORKSPACE_FOLDER)/prod/calc/lib
    # LIBS 変数は各 Makefile で Linux gcc 書式で定義される (例: LIBS = -lcalcbase -lcalc)
else
    # Windows
    CC := cl
    LD := link
    OBJEXT := .obj
    EXEEXT := .exe
    CFLAGS := /nologo /W4 /Zi /TC /utf-8 /MD /Fd$(OBJDIR)/$(TARGET_NAME).pdb /I$(WORKSPACE_FOLDER)/prod/calc/include
    LDFLAGS := /NOLOGO /DEBUG /PDB:$(TARGETDIR)/$(TARGET_NAME).pdb /INCREMENTAL /ILK:$(OBJDIR)/$(TARGET_NAME).ilk /LIBPATH:$(WORKSPACE_FOLDER)/prod/calc/lib
    # LIBS 変数は各 Makefile で Linux gcc 書式で定義される (例: LIBS = -lcalcbase -lcalc)
    # Linux gcc 書式を Windows 向け (例: LIBS = calcbase.lib calc.lib) に読み替える
    LIBS := $(patsubst -l%,%.lib,$(LIBS))
endif

# ソースファイル
SRCS_C := $(wildcard *.c)

# オブジェクトファイル
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(patsubst %.c, %$(OBJEXT), $(SRCS_C))))

# ターゲット名の取得（カレントディレクトリ名）
TARGET := $(TARGET_NAME)$(EXEEXT)

# デフォルトターゲット
.PHONY: all
all: $(TARGET)

# リンク
ifneq ($(OS),Windows_NT)
    # Linux
$(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
	$(LD) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
else
    # Windows
$(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
	$(LD) /OUT:$@ $(OBJS) $(LIBS) $(LDFLAGS)
endif

# コンパイル
ifneq ($(OS),Windows_NT)
    # Linux
$(OBJDIR)/%$(OBJEXT): %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<
else
    # Windows
$(OBJDIR)/%$(OBJEXT): %.c | $(OBJDIR)
	$(CC) $(CFLAGS) /c /Fo$@ $<
endif

# ディレクトリ作成
$(OBJDIR):
	mkdir -p $@

# クリーンアップ
.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGETDIR)/$(TARGET)
ifeq ($(OS),Windows_NT)
    # Windows
	rm -f $(TARGETDIR)/$(TARGET_NAME).pdb
endif

# ヘルプ
.PHONY: help
help:
	@echo "Windows PoC Makefile Template"
	@echo "Usage:"
	@echo "  make        - Build the target"
	@echo "  make clean  - Clean build artifacts"
	@echo "  make help   - Show this help message"
	@echo ""
	@echo "Environment:"
ifneq ($(OS),Windows_NT)
    # Linux
	@echo "  Platform: Linux"
	@echo "  Compiler: $(CC)"
else
    # Windows
	@echo "  Platform: Windows"
	@echo "  Compiler: $(CC)"
	@echo "  Linker: $(LD)"
endif
	@echo "  Target: $(TARGET)"
