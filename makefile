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
ROOT_RUNNER = $(MAKEFW_HOME)/bin/run_ordered_subdir_target.sh
FRAMEWORK_MAKE_DIRS = $(TESTFW_HOME)

include $(MAKEFW_HOME)/makefiles/_parallel.mk

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
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	framework_jobs="$$jobs"; \
	if [ -z "$$framework_jobs" ]; then framework_jobs=1; fi; \
	MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$framework_jobs" default "$(FRAMEWORK_MAKE_DIRS)"
	$(MAKE) -C app

.PHONY: with-cov
with-cov :
	$(MAKE) skills
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	framework_jobs="$$jobs"; \
	if [ -z "$$framework_jobs" ]; then framework_jobs=1; fi; \
	MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$framework_jobs" default "$(FRAMEWORK_MAKE_DIRS)"
	$(MAKE) -C app with-cov

.PHONY: test
test :
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	framework_jobs="$$jobs"; \
	if [ -z "$$framework_jobs" ]; then framework_jobs=1; fi; \
	MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$framework_jobs" test "$(FRAMEWORK_MAKE_DIRS)"
	$(MAKE) -C app test

.PHONY: doxy
doxy :
	$(MAKE) -C app doxy

.PHONY: skills
skills :
	@printf 'INFO: Checking skills sync...\n'
	@"$(BASH)" "$(CURDIR)/bin/sync-skills.sh"
	@printf 'INFO: skills sync completed.\n'

.PHONY: check-nbsp
check-nbsp :
	python3 "$(CURDIR)/bin/check-nbsp.py"

.PHONY: clean
clean :
	@$(call _MAKEFW_RESOLVE_PARALLEL_SHELL) \
	framework_jobs="$$jobs"; \
	if [ -z "$$framework_jobs" ]; then framework_jobs=1; fi; \
	MAKEFW_SUBDIR_MAKE="$(MAKE)" "$(SHELL)" \
		"$(ROOT_RUNNER)" \
		--app-deps --silent-missing --echo-command --progress \
		"$$framework_jobs" clean "$(FRAMEWORK_MAKE_DIRS)"
	$(MAKE) -C app clean
	$(MAKE) cleandocs

.PHONY: cleandocs
cleandocs :
	@if [ -d pages ]; then \
		find pages/ -mindepth 1 -maxdepth 1 ! -name 'doxygen' -print | while IFS= read -r path; do \
			printf 'rm -rf "%s"\n' "$$path"; \
			rm -rf "$$path"; \
		done; \
	fi
	rm -f "$(DOCS_WARN_FILE)"

.PHONY: docs
docs :
	$(MAKE) skills
	@if [ -d framework/docsfw ] && [ -f "$(DOCSFW_SCRIPT)" ]; then \
		DOCS_PID=""; \
		TEE_PID=""; \
		DOCS_LOGFILE=$$(mktemp); \
		DOCS_PIPE="$$DOCS_LOGFILE.pipe"; \
		docs_kill_pid() { \
			_pid="$$1"; \
			[ -n "$$_pid" ] || return 0; \
			kill "$$_pid" 2>/dev/null || true; \
			case "$$(uname -s 2>/dev/null)" in \
				MINGW*|MSYS*|CYGWIN*) \
					if command -v taskkill.exe >/dev/null 2>&1; then \
						MSYS2_ARG_CONV_EXCL='*' taskkill.exe /PID "$$_pid" /T /F >/dev/null 2>&1 || true; \
					fi; \
					;; \
			esac; \
		}; \
		docs_cleanup() { \
			rm -f "$$DOCS_PIPE"; \
			if [ "$${DOCS_KEEP_LOG:-0}" != "1" ]; then \
				rm -f "$$DOCS_LOGFILE"; \
			fi; \
		}; \
		docs_on_signal() { \
			_exit_code="$$1"; \
			trap - INT TERM HUP EXIT; \
			docs_kill_pid "$$DOCS_PID"; \
			docs_kill_pid "$$TEE_PID"; \
			wait "$$DOCS_PID" 2>/dev/null || true; \
			wait "$$TEE_PID" 2>/dev/null || true; \
			docs_cleanup; \
			exit "$$_exit_code"; \
		}; \
		trap 'docs_on_signal 130' INT; \
		trap 'docs_on_signal 143' TERM; \
		trap 'docs_on_signal 129' HUP; \
		trap 'docs_cleanup' EXIT; \
		rm -f "$(DOCS_WARN_FILE)"; \
		mkfifo "$$DOCS_PIPE"; \
		tee "$$DOCS_LOGFILE" < "$$DOCS_PIPE" & \
		TEE_PID=$$!; \
		"$(BASH)" "$(DOCSFW_SCRIPT)" --workspaceFolder="$(CURDIR)" > "$$DOCS_PIPE" 2>&1 & \
		DOCS_PID=$$!; \
		if wait "$$DOCS_PID"; then \
			DOCS_EXIT=0; \
		else \
			DOCS_EXIT=$$?; \
		fi; \
		wait $$TEE_PID; \
		DOCS_KEEP_LOG=1; \
		rm -f "$$DOCS_PIPE"; \
		if [ -x "$(EXTRACT_DOCS_WARNINGS)" ]; then \
			"$(EXTRACT_DOCS_WARNINGS)" "$$DOCS_LOGFILE" "$(DOCS_WARN_FILE)"; \
		fi; \
		if [ -s "$(DOCS_WARN_FILE)" ]; then \
			printf '\n'; \
			"$(BASH)" "$(TESTFW_BANNER)" WARNING "\e[33m"; \
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
