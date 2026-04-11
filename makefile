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

.PHONY: default clean test doxy
default clean test doxy :
	$(MAKE) -C app $@

.PHONY: docs
docs :
	@if [ -d framework/docsfw ] && [ -f framework/docsfw/bin/pub_markdown_core.sh ]; then \
		$(BASH) framework/docsfw/bin/pub_markdown_core.sh --workspaceFolder="$(CURDIR)" --details=both --docx=true; \
	else \
		echo "INFO: framework/docsfw directory not found, skipping."; \
	fi
