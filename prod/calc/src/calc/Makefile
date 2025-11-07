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

# ライブラリの指定
LIBS += -lcalc

# src の Makefile テンプレートを include
ifneq ($(OS),Windows_NT)
    # Linux
    include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
else
    # Windows
    include $(WORKSPACE_FOLDER)/prod/calc/src/makesrc-windows-poc.mk
endif
