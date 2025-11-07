# ワークスペースのディレクトリ
ifneq ($(OS),Windows_NT)
    # Linux
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
else
    # Windows
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
endif

# 準備処理 (Makefile テンプレートより前に include)
ifneq ($(OS),Windows_NT)
    # Linux
    include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk
endif

# ライブラリの指定
LIBSDIR += $(WORKSPACE_FOLDER)/prod/calc/lib
ifneq ($(OS),Windows_NT)
    # Linux
    LIBS += -lcalcbase -lcalc
else
    # Windows
    LIBS += calcbase.lib calc.lib
endif

# src の Makefile テンプレートを include
ifneq ($(OS),Windows_NT)
    # Linux
    include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
else
    # Windows
    include $(WORKSPACE_FOLDER)/prod/calc/src/makesrc-windows-poc.mk
endif
