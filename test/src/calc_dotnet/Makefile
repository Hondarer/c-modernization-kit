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
PROJECT_FILE := calc_dotnet_tests.csproj
NATIVE_LIB_DIR := $(WORKSPACE_FOLDER)/prod/calc/lib

ifneq ($(OS),Windows_NT)
    NATIVE_LIB := libcalc.so
    SET_LIB_PATH := LD_LIBRARY_PATH=$(NATIVE_LIB_DIR):$$LD_LIBRARY_PATH
else
    NATIVE_LIB := calc.dll
    SET_LIB_PATH :=
endif

.DEFAULT_GOAL := default

.PHONY: default
default: build

.PHONY: build
build:
	dotnet build $(PROJECT_FILE) -c $(CONFIGURATION)

.PHONY: test
test: build
	@if [ ! -f "$(NATIVE_LIB_DIR)/$(NATIVE_LIB)" ]; then \
		echo "Error: Native library not found at $(NATIVE_LIB_DIR)/$(NATIVE_LIB)"; \
		echo "Please build the native calc library first."; \
		exit 1; \
	fi
	$(SET_LIB_PATH) dotnet test $(PROJECT_FILE) -c $(CONFIGURATION) --no-build --verbosity normal

.PHONY: clean
clean:
	dotnet clean $(PROJECT_FILE) -c $(CONFIGURATION)
	rm -rf bin obj results

.PHONY: restore
restore:
	dotnet restore $(PROJECT_FILE)

.PHONY: rebuild
rebuild: clean build
