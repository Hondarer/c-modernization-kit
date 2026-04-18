BASH ?= bash
DOCSFW_SCRIPT := $(CURDIR)/framework/docsfw/bin/pub_markdown_core.sh
DOCS_WARN_FILE := $(CURDIR)/docs.warn
EXTRACT_DOCS_WARNINGS := $(CURDIR)/framework/docsfw/bin/extract_docs_warnings.sh
TESTFW_BANNER = framework/testfw/bin/banner.sh

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

.PHONY: default test doxy
default test doxy :
	@if [ -n "$(MAKECMDGOALS)" ]; then \
		$(MAKE) -C app $@; \
	else \
		$(MAKE) -C app; \
	fi

.PHONY: clean
clean :
    # Windows PowerShell + recursive GNU Make では、子 make から戻った直後に
    # カーソル列だけが 0 に戻らず、続く "Leaving directory" やプロンプトが崩れることがある。
    # そのため、最後の clean コマンド後に CR を流して親 make の次行出力開始位置を補正する。
	@$(MAKE) -C app clean; printf '\r'
	@printf '%s\n' 'rm -f "$(DOCS_WARN_FILE)"'
	@rm -f "$(DOCS_WARN_FILE)"; printf '\r'

.PHONY: docs
docs :
	@if [ -d framework/docsfw ] && [ -f "$(DOCSFW_SCRIPT)" ]; then \
		DOCS_LOGFILE=$$(mktemp); \
		DOCS_PIPE="$$DOCS_LOGFILE.pipe"; \
		rm -f "$(DOCS_WARN_FILE)"; \
		mkfifo "$$DOCS_PIPE"; \
		tee "$$DOCS_LOGFILE" < "$$DOCS_PIPE" & \
		TEE_PID=$$!; \
		if "$(BASH)" "$(DOCSFW_SCRIPT)" --workspaceFolder="$(CURDIR)" --details=both --docx=true > "$$DOCS_PIPE" 2>&1; then \
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
