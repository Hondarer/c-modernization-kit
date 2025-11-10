# Windows PoC用のビルドテンプレート
# このファイルは prod/calc/src 配下の各サブディレクトリの Makefile から include される

# ディレクトリ設定
OBJDIR := obj
TARGETDIR := .
TARGET_NAME := $(notdir $(CURDIR))

# ビルド設定
CC := cl
LD := link
OBJEXT := .obj
EXEEXT := .exe
CFLAGS := /nologo /W4 /Zi /TC /utf-8 /MD /Fd$(OBJDIR)/$(TARGET_NAME).pdb /I$(WORKSPACE_FOLDER)/prod/calc/include
LDFLAGS := /NOLOGO /DEBUG /PDB:$(TARGETDIR)/$(TARGET_NAME).pdb /INCREMENTAL /ILK:$(OBJDIR)/$(TARGET_NAME).ilk /LIBPATH:$(WORKSPACE_FOLDER)/prod/calc/lib
# LIBS 変数は各 Makefile で Linux gcc 書式で定義される (例: LIBS = -lcalcbase -lcalc)
# Linux gcc 書式を Windows 向け (例: LIBS = calcbase.lib calc.lib) に読み替える
LIBS := $(patsubst -l%,%.lib,$(LIBS))

# ソースファイル
SRCS_C := $(wildcard *.c)

# オブジェクトファイル
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(patsubst %.c, %$(OBJEXT), $(SRCS_C))))

# ターゲット名の取得 (カレントディレクトリ名)
TARGET := $(TARGET_NAME)$(EXEEXT)

# デフォルトターゲット
.PHONY: all
all: $(TARGET)

# リンク
$(TARGETDIR)/$(TARGET): $(OBJS) | $(TARGETDIR)
	$(LD) /OUT:$@ $(OBJS) $(LIBS) $(LDFLAGS)

# コンパイル
$(OBJDIR)/%$(OBJEXT): %.c | $(OBJDIR)
	$(CC) $(CFLAGS) /c /Fo$@ $<

# ディレクトリ作成
$(OBJDIR):
	mkdir -p $@

# クリーンアップ
.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGETDIR)/$(TARGET)
	rm -f $(TARGETDIR)/$(TARGET_NAME).pdb
