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
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/tutorial/libsrc/samplestatic/samplestatic.c

# ライブラリの追加
LIBS += mock_libc

# src の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makesrc.mk
