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

.DEFAULT_GOAL := default

.PHONY: default
default: build

.PHONY: build
build:
	dotnet build -c $(CONFIG)

.PHONY: test
test: build
	dotnet test -c $(CONFIG) --no-build --verbosity normal

.PHONY: clean
clean:
	rm -rf bin obj results

.PHONY: restore
restore:
	dotnet restore

.PHONY: rebuild
rebuild: clean build
