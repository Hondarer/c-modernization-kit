# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# 準備処理 (Makefile テンプレートより前に include)
ifneq ($(OS),Windows_NT)
    # Linux
    include $(WORKSPACE_FOLDER)/testfw/makefiles/prepare.mk
endif

# ライブラリの指定
LIBSDIR += $(WORKSPACE_FOLDER)/prod/calc/lib
ifneq ($(OS),Windows_NT)
    # Linux
    LIBS += -lcalc
else
    # Windows
    LIBS += calc.lib
endif

# src の Makefile テンプレートを include
ifneq ($(OS),Windows_NT)
    # Linux
    include $(WORKSPACE_FOLDER)/testfw/makefiles/makesrc.mk
else
    # Windows
    include $(WORKSPACE_FOLDER)/prod/calc/src/makesrc-windows-poc.mk
endif
