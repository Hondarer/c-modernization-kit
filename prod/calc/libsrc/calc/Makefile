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

# 生成されるライブラリを共有ライブラリ (shared, dynamic) とする
BUILD = shared

# ライブラリの追加
# .a は、.so に静的リンクされる
LIBSDIR += $(WORKSPACE_FOLDER)/prod/calc/lib
LIBS += -lcalcbase

# libsrc の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/testfw/makefiles/makelibsrc.mk
