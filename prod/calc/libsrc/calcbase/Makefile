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

# 生成されるライブラリを静的ライブラリ (static) とする
# 未指定の場合 (デフォルト) は shared
BUILD = static

# libsrc の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makelibsrc.mk
