# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (Makefile テンプレートより前に include)
include $(WORKSPACE_FOLDER)/makefw/makefiles/prepare.mk

# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/src/add/add.c

# リンクオプションの追加
# -Wl,--wrap=main により、エントリポイントを __wrap_main() に、元々のエントリポイントを __real_main() に変更
LDFLAGS += -Wl,--wrap=main

# ライブラリの追加
# gtest_wrapmain にて、__wrap_main() 経由でのテスト実施
# テスト対象のソースファイルにある main() は実行されない
LIBS += gtest_wrapmain mock_libc mock_calcbase test_com

# src の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makesrc.mk
