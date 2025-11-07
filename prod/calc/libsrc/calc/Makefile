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

# 生成されるライブラリを共有ライブラリ (shared, dynamic) とする
BUILD = shared

# ライブラリの追加
LIBSDIR += $(WORKSPACE_FOLDER)/prod/calc/lib
LIBS += -lcalcbase

# libsrc の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/testfw/makefiles/makelibsrc.mk
