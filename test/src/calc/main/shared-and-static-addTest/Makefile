# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (Makefile テンプレートより前に include)
include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk

# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/src/shared-and-static-add/shared-and-static-add.c

# リンクオプションの追加
# -Wl,--wrap=main により、エントリポイントを __wrap_main() に、元々のエントリポイントを __real_main() に変更
LDCOMFLAGS += -Wl,--wrap=main

# ライブラリの追加
# -lgtest_wrapmain にて、__wrap_main() 経由でのテスト実施
# テスト対象のソースファイルにある main() は実行されない
LIBS += -lgtest_wrapmain -lmock_libc -lmock_calc -lmock_calcbase -ltest_com

# src の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
