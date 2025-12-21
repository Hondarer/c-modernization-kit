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

.DEFAULT_GOAL := default

.PHONY: default
default: build

.PHONY: build
build:
	dotnet build -c $(CONFIG) -o $(OUTPUT_DIR)

.PHONY: clean
clean:
	rm -f $(OUTPUT_DIR)/CalcDotNetLib.*
	rm -rf bin obj

.PHONY: restore
restore:
	dotnet restore

.PHONY: rebuild
rebuild: clean build
