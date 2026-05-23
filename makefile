BASH ?= bash
WORKSPACE_DIR ?= $(CURDIR)
MAKEFW_HOME := $(strip $(MAKEFW_HOME))
ifeq ($(MAKEFW_HOME),)
    $(error MAKEFW_HOME is required. Export MAKEFW_HOME before running make)
endif
DOXYFW_HOME ?= $(WORKSPACE_DIR)/framework/doxyfw
TESTFW_HOME ?= $(WORKSPACE_DIR)/framework/testfw
DOCSFW_SCRIPT := $(CURDIR)/framework/docsfw/bin/pub_markdown_core.sh
DOCS_WARN_FILE := $(CURDIR)/docs.warn
EXTRACT_DOCS_WARNINGS := $(CURDIR)/framework/docsfw/bin/extract_docs_warnings.sh
TESTFW_BANNER = $(TESTFW_HOME)/bin/banner.sh

export WORKSPACE_DIR
export MAKEFW_HOME
export DOXYFW_HOME
export TESTFW_HOME

# Windows 環境チェック: SHELL が POSIX シェル (bash/sh) かどうかを確認
# bash が PATH に通っていれば GNU Make は SHELL を /bin/sh (スラッシュあり) にセットする。
# bash がなく cmd.exe へフォールバックしている場合は SHELL = "sh" (スラッシュなし) のままになる。
ifeq ($(OS),Windows_NT)
    ifeq ($(findstring /,$(SHELL)),)
        $(info )
        $(info ERROR: Build environment is not configured correctly.)
        $(info Please launch VS Code via Start-VSCode-With-Env to set up the environment.)
        $(info )
        $(error Aborted.)
    endif
endif

.DEFAULT_GOAL := default

.PHONY: default
default :
	$(MAKE) skills
	$(MAKE) -C $(TESTFW_HOME)
	$(MAKE) -C app

.PHONY: with-cov
with-cov :
	$(MAKE) skills
	$(MAKE) -C $(TESTFW_HOME)
	$(MAKE) -C app with-cov

.PHONY: test
test :
	$(MAKE) -C $(TESTFW_HOME)
	$(MAKE) -C app test

.PHONY: doxy
doxy :
	$(MAKE) -C app doxy

.PHONY: skills
skills :
	@printf 'INFO: Checking skills sync...\n'
	@"$(BASH)" "$(CURDIR)/bin/sync-skills.sh"
	@printf 'INFO: skills sync completed.\n'

.PHONY: clean
clean :
	$(MAKE) -C $(TESTFW_HOME) clean
	$(MAKE) -C app clean
	$(MAKE) cleandocs

.PHONY: cleandocs
cleandocs :
	@if [ -d pages ]; then \
		find pages/ -mindepth 1 -maxdepth 1 ! -name 'doxygen' -exec rm -rf {} +; \
	fi
	rm -f "$(DOCS_WARN_FILE)"

.PHONY: docs
docs :
	$(MAKE) skills
	@if [ -d framework/docsfw ] && [ -f "$(DOCSFW_SCRIPT)" ]; then \
		DOCS_LOGFILE=$$(mktemp); \
		DOCS_PIPE="$$DOCS_LOGFILE.pipe"; \
		rm -f "$(DOCS_WARN_FILE)"; \
		mkfifo "$$DOCS_PIPE"; \
		tee "$$DOCS_LOGFILE" < "$$DOCS_PIPE" & \
		TEE_PID=$$!; \
		if "$(BASH)" "$(DOCSFW_SCRIPT)" --workspaceFolder="$(CURDIR)" > "$$DOCS_PIPE" 2>&1; then \
			DOCS_EXIT=0; \
		else \
			DOCS_EXIT=$$?; \
		fi; \
		wait $$TEE_PID; \
		rm -f "$$DOCS_PIPE"; \
		if [ -x "$(EXTRACT_DOCS_WARNINGS)" ]; then \
			"$(EXTRACT_DOCS_WARNINGS)" "$$DOCS_LOGFILE" "$(DOCS_WARN_FILE)"; \
		fi; \
		if [ -s "$(DOCS_WARN_FILE)" ]; then \
			printf '\n'; \
			"$(BASH)" "$(TESTFW_BANNER)" "DOCS ISSUES" "\e[33m"; \
			printf '\n'; \
			printf '\033[33m===== %s =====\033[0m\n' "$(DOCS_WARN_FILE)"; \
			while IFS= read -r line || [ -n "$$line" ]; do \
				clean_line=$$(printf '%s' "$$line" | tr -d '\r'); \
				printf '\033[33m%s\033[0m\n' "$$clean_line"; \
			done < "$(DOCS_WARN_FILE)"; \
		fi; \
		rm -f "$$DOCS_LOGFILE"; \
		exit $$DOCS_EXIT; \
	else \
		echo "INFO: framework/docsfw directory not found, skipping."; \
	fi
