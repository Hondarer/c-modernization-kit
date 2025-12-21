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
PROJECT_FILE := CalcDotNetLib.Tests.csproj

.DEFAULT_GOAL := default

.PHONY: default
default: build

.PHONY: build
build:
	dotnet build $(PROJECT_FILE) -c $(CONFIGURATION)

.PHONY: test
test: build
	dotnet test $(PROJECT_FILE) -c $(CONFIGURATION) --no-build --verbosity normal

.PHONY: clean
clean:
	dotnet clean $(PROJECT_FILE) -c $(CONFIGURATION)
	rm -rf bin obj results

.PHONY: restore
restore:
	dotnet restore $(PROJECT_FILE)

.PHONY: rebuild
rebuild: clean build
