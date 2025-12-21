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
PROJECT_FILE := calc_dotnet_app.csproj
#OUTPUT_DIR := $(WORKSPACE_FOLDER)/prod/calc/bin
#OUTPUT_DIR := $(CURDIR)
NATIVE_LIB_DIR := $(WORKSPACE_FOLDER)/prod/calc/lib

ifneq ($(OS),Windows_NT)
    NATIVE_LIB := libcalc.so
    EXECUTABLE := calc_dotnet_app
else
    NATIVE_LIB := calc.dll
    EXECUTABLE := calc_dotnet_app.exe
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
#	dotnet build $(PROJECT_FILE) -c $(CONFIGURATION) -o $(OUTPUT_DIR)
	dotnet build $(PROJECT_FILE) -c $(CONFIGURATION)

.PHONY: run
run: build
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=$(NATIVE_LIB_DIR):$$LD_LIBRARY_PATH ./$(EXECUTABLE)

.PHONY: clean
clean:
	dotnet clean $(PROJECT_FILE) -c $(CONFIGURATION)
	rm -rf bin obj
#	rm -f $(OUTPUT_DIR)/$(EXECUTABLE) $(OUTPUT_DIR)/calc_dotnet.*

.PHONY: restore
restore:
	dotnet restore $(PROJECT_FILE)

.PHONY: rebuild
rebuild: clean build
