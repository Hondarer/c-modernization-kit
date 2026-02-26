SUBDIRS = \
	prod \
	test

DOXY_CATEGORIES = \
	calc \
	calc.net \
	util \
	override-sample \
	doxygen-sample

# Windows 環境チェック: SHELL が POSIX シェル (bash/sh) かどうかを確認
# bash が PATH に通っていれば GNU Make は SHELL を /bin/sh (スラッシュあり) にセットする。
# bash がなく cmd.exe へフォールバックしている場合は SHELL = "sh" (スラッシュなし) のままになる。
ifeq ($(OS),Windows_NT)
    ifeq ($(findstring /,$(SHELL)),)
        $(info )
        $(info ERROR: Build environment is not configured correctly.)
        $(info Please launch VS Code via Start-VSCode-With-Env.ps1 to set up the environment.)
        $(info )
        $(error Aborted.)
    endif
endif

# サブディレクトリで make を実行するマクロ
# $(1): サブディレクトリリスト
# $(2): make ターゲット (空の場合はデフォルトターゲット)
define make_in_subdirs
	@for dir in $(1); do \
		if [ -d $$dir ] && [ -f $$dir/makefile ]; then \
			$(MAKE) -C $$dir $(2) || exit 1; \
		else \
			echo "INFO: $$dir directory not found, skipping."; \
		fi; \
	done
endef

.DEFAULT_GOAL := default

.PHONY: default
default : submodule
	$(call make_in_subdirs,$(SUBDIRS))

.PHONY: submodule
submodule :
	@if [ ! -d makefw ] || [ ! -f makefw/.git ]; then \
		echo "ERROR: makefw submodule is not initialized."; \
		echo "Please run the following command to initialize submodules:"; \
		echo "  git submodule update --init --recursive"; \
		exit 1; \
	fi

.PHONY: clean
clean : submodule
	$(call make_in_subdirs,$(SUBDIRS),clean)

.PHONY: test
test : submodule
    # prod ディレクトリで make (引数なし、デフォルトターゲット)
	@if [ -d prod ] && [ -f prod/makefile ]; then \
		$(MAKE) -C prod || exit 1; \
	else \
		echo "INFO: prod directory not found, skipping."; \
	fi
    # test ディレクトリで make test
	@if [ -d test ] && [ -f test/makefile ]; then \
		$(MAKE) -C test test || exit 1; \
	else \
		echo "INFO: test directory not found, skipping."; \
	fi

.PHONY: doxy
doxy : submodule
	@if [ -d doxyfw ] && [ -f doxyfw/makefile ]; then \
		set -e; \
		if [ -z "$(DOXY_CATEGORIES)" ]; then \
			echo $(MAKE) -C doxyfw; \
			$(MAKE) -C doxyfw; \
		else \
			for cat in $(DOXY_CATEGORIES); do \
				echo $(MAKE) -C doxyfw CATEGORY=$$cat; \
				$(MAKE) -C doxyfw CATEGORY=$$cat; \
			done; \
		fi; \
	else \
		echo "INFO: doxyfw directory not found, skipping."; \
	fi

.PHONY: docs
docs : submodule
	@if [ -d docsfw ] && [ -f docsfw/bin/pub_markdown_core.sh ]; then \
		$(BASH) docsfw/bin/pub_markdown_core.sh --workspaceFolder="$(CURDIR)" --details=both --docx=true; \
	else \
		echo "INFO: docsfw directory not found, skipping."; \
	fi
