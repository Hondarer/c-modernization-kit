# ワークスペースのディレクトリ
find-up = \
    $(if $(wildcard $(1)/$(2)),$(1),\
        $(if $(filter $(1),$(patsubst %/,%,$(dir $(1)))),,\
            $(call find-up,$(patsubst %/,%,$(dir $(1))),$(2))\
        )\
    )
WORKSPACE_FOLDER := $(strip $(call find-up,$(CURDIR),.workspaceRoot))

# Configuration
CONFIGURATION ?= Release
PROJECT_FILE := calc_dotnet.csproj
OUTPUT_DIR := $(WORKSPACE_FOLDER)/prod/calc/bin
NATIVE_LIB_DIR := $(WORKSPACE_FOLDER)/prod/calc/lib

# Detect OS
ifneq ($(OS),Windows_NT)
    DETECTED_OS := $(shell uname -s)
    NATIVE_LIB := libcalc.so
    EXECUTABLE := calc_dotnet
else
    DETECTED_OS := Windows
    NATIVE_LIB := calc.dll
    EXECUTABLE := calc_dotnet.exe
endif

.DEFAULT_GOAL := default

.PHONY: default
default: build

.PHONY: build
build:
	@if [ ! -f "$(NATIVE_LIB_DIR)/$(NATIVE_LIB)" ]; then \
		echo "Error: Native library not found at $(NATIVE_LIB_DIR)/$(NATIVE_LIB)"; \
		echo "Please build the native calc library first."; \
		exit 1; \
	fi
	dotnet build $(PROJECT_FILE) -c $(CONFIGURATION) -o $(OUTPUT_DIR)

.PHONY: run
run: build
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=$(NATIVE_LIB_DIR):$$LD_LIBRARY_PATH ./$(EXECUTABLE)

.PHONY: clean
clean:
	dotnet clean $(PROJECT_FILE) -c $(CONFIGURATION)
	rm -rf bin obj
	rm -f $(OUTPUT_DIR)/$(EXECUTABLE) $(OUTPUT_DIR)/calc_dotnet.*

.PHONY: restore
restore:
	dotnet restore $(PROJECT_FILE)

.PHONY: rebuild
rebuild: clean build
