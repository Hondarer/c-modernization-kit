# Windows PoC用のビルドテンプレート
# このファイルは prod/calc/src 配下の各サブディレクトリの Makefile から include される

# Windows 環境の検出
ifeq ($(OS),Windows_NT)
    WINDOWS := 1
else
    WINDOWS := 0
endif

# ワークスペースのディレクトリ
# Windows と Linux で異なるパス検索方法を使用
ifeq ($(WINDOWS),1)
    # Windows の場合
    WORKSPACE_FOLDER := $(shell \
        for /f "delims=" %%i in ('cd') do @echo %%i & \
        for %%d in (.) do @if exist "%%~fd\.workspaceRoot" echo %%~fd \
    )
else
    # Linux の場合
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
endif

# Windows ビルド設定
ifeq ($(WINDOWS),1)
    # コンパイラとリンカー
    CC := cl
    LD := link

    # 実行可能ファイルの拡張子
    EXE_EXT := .exe
    OBJ_EXT := .obj
    LIB_PREFIX :=
    LIB_EXT := .lib

    # コンパイラフラグ
    CFLAGS := /nologo /W3 /EHsc /O2 /MD
    CFLAGS += /I"$(WORKSPACE_FOLDER)/prod/calc/include"

    # リンカフラグ
    LDFLAGS := /NOLOGO /SUBSYSTEM:CONSOLE
    LDFLAGS += /LIBPATH:"$(WORKSPACE_FOLDER)/prod/calc/lib"

    # ライブラリ
    # LIBS 変数は各 Makefile で定義される（例: LIBS = calcbase.lib calc.lib）

else
    # Linux ビルド設定（参考用）
    CC := gcc
    LD := gcc

    EXE_EXT :=
    OBJ_EXT := .o
    LIB_PREFIX := lib
    LIB_EXT := .a

    CFLAGS := -Wall -O2
    CFLAGS += -I$(WORKSPACE_FOLDER)/prod/calc/include

    LDFLAGS := -L$(WORKSPACE_FOLDER)/prod/calc/lib

    # LIBS 変数は各 Makefile で定義される（例: LIBS = -lcalcbase -lcalc）
endif

# ターゲット名の取得（カレントディレクトリ名）
ifeq ($(WINDOWS),1)
    TARGET_NAME := $(notdir $(shell cd))
else
    TARGET_NAME := $(notdir $(CURDIR))
endif

TARGET := $(TARGET_NAME)$(EXE_EXT)

# ソースファイルとオブジェクトファイル
SRCS := $(wildcard *.c)
ifeq ($(WINDOWS),1)
    OBJS := $(SRCS:.c=.obj)
else
    OBJS := $(SRCS:.c=.o)
endif

# デフォルトターゲット
.PHONY: all
all: $(TARGET)

# リンク
ifeq ($(WINDOWS),1)
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) /OUT:$@ $(OBJS) $(LIBS)
else
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
endif

# コンパイル
ifeq ($(WINDOWS),1)
%.obj: %.c
	$(CC) $(CFLAGS) /c $< /Fo$@
else
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
endif

# クリーンアップ
.PHONY: clean
clean:
ifeq ($(WINDOWS),1)
	-del /Q $(OBJS) $(TARGET) 2>NUL
else
	rm -f $(OBJS) $(TARGET)
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
ifeq ($(WINDOWS),1)
	@echo "  Platform: Windows"
	@echo "  Compiler: $(CC)"
	@echo "  Linker: $(LD)"
else
	@echo "  Platform: Linux"
	@echo "  Compiler: $(CC)"
endif
	@echo "  Target: $(TARGET)"
