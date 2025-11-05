# ワークスペースのディレクトリ
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

# 準備処理 (Makefile テンプレートより前に include)
include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk

# ライブラリの追加
LIBSDIR += $(WORKSPACE_FOLDER)/prod/calc/lib
LIBS += -lcalc -lcalcbase

# src の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
