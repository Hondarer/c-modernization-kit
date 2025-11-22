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

# ライブラリの指定
# libcalcbase.a と libcalc.so の両方
LIBS += calcbase calc

# src の Makefile テンプレートを include
include $(WORKSPACE_FOLDER)/makefw/makefiles/makesrc.mk
