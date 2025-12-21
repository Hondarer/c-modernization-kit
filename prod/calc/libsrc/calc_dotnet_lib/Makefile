# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# Configuration
CONFIGURATION ?= RelWithDebInfo
PROJECT_FILE := calc_dotnet_lib.csproj
OUTPUT_DIR := $(WORKSPACE_FOLDER)/prod/calc/lib

.DEFAULT_GOAL := default

.PHONY: default
default: build

.PHONY: build
build:
	dotnet build $(PROJECT_FILE) -c $(CONFIGURATION) -o $(OUTPUT_DIR)

.PHONY: clean
clean:
	dotnet clean $(PROJECT_FILE) -c $(CONFIGURATION)
	rm -rf bin obj
	rm -f $(OUTPUT_DIR)/calc_dotnet_lib.*

.PHONY: restore
restore:
	dotnet restore $(PROJECT_FILE)

.PHONY: rebuild
rebuild: clean build
