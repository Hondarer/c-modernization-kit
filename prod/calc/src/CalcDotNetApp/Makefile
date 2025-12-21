# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# Configuration
CONFIG ?= RelWithDebInfo
OUTPUT_DIR := $(CURDIR)/bin
NATIVE_LIB_DIR := $(WORKSPACE_FOLDER)/prod/calc/lib

ifneq ($(OS),Windows_NT)
    EXECUTABLE := CalcDotNetApp
else
    EXECUTABLE := CalcDotNetApp.exe
endif

.DEFAULT_GOAL := default

.PHONY: default
default: build

.PHONY: build
build:
	dotnet build -c $(CONFIG) -o $(OUTPUT_DIR)

.PHONY: run
run: build
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=$(NATIVE_LIB_DIR):$$LD_LIBRARY_PATH ./$(EXECUTABLE)

.PHONY: clean
clean:
	rm -f $(OUTPUT_DIR)/$(EXECUTABLE)
	rm -rf bin obj

.PHONY: restore
restore:
	dotnet restore

.PHONY: rebuild
rebuild: clean build
