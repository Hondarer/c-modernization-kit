SUBDIRS = \
	app

DOXY_CATEGORIES = \
	calc \
	calc.net \
	util \
	override-sample \
	doxygen-sample \
	porter

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
	@if [ ! -d framework/makefw ] || [ ! -f framework/makefw/.git ]; then \
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
    # app ディレクトリで make test (ビルド + テスト実行)
	@if [ -d app ] && [ -f app/makefile ]; then \
		$(MAKE) -C app test || exit 1; \
	else \
		echo "INFO: app directory not found, skipping."; \
	fi

# NOTE: 単一カテゴリを指定して Doxygen を実行可能。
# 例: make doxy CATEGORY=porter
.PHONY: doxy
doxy : submodule
	@if [ -d framework/doxyfw ] && [ -f framework/doxyfw/makefile ]; then \
		set -e; \
		if [ -n "$(CATEGORY)" ]; then \
			echo $(MAKE) -C framework/doxyfw CATEGORY=$(CATEGORY); \
			$(MAKE) -C framework/doxyfw CATEGORY=$(CATEGORY); \
		elif [ -z "$(DOXY_CATEGORIES)" ]; then \
			echo $(MAKE) -C framework/doxyfw; \
			$(MAKE) -C framework/doxyfw; \
		else \
			for cat in $(DOXY_CATEGORIES); do \
				echo $(MAKE) -C framework/doxyfw CATEGORY=$$cat; \
				$(MAKE) -C framework/doxyfw CATEGORY=$$cat; \
			done; \
		fi; \
	else \
		echo "INFO: framework/doxyfw directory not found, skipping."; \
	fi

.PHONY: docs
docs : submodule
	@if [ -d framework/docsfw ] && [ -f framework/docsfw/bin/pub_markdown_core.sh ]; then \
		$(BASH) framework/docsfw/bin/pub_markdown_core.sh --workspaceFolder="$(CURDIR)" --details=both --docx=true; \
	else \
		echo "INFO: framework/docsfw directory not found, skipping."; \
	fi
