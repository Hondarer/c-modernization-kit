SUBDIRS = \
	prod \
	test

# サブディレクトリで make を実行するマクロ
# $(1): サブディレクトリリスト
# $(2): make ターゲット (空の場合はデフォルトターゲット)
define make_in_subdirs
	@for dir in $(1); do \
		if [ -d $$dir ] && [ -f $$dir/Makefile ]; then \
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
	@if [ -d prod ] && [ -f prod/Makefile ]; then \
		$(MAKE) -C prod || exit 1; \
	else \
		echo "INFO: prod directory not found, skipping."; \
	fi
    # test ディレクトリで make test
	@if [ -d test ] && [ -f test/Makefile ]; then \
		$(MAKE) -C test test || exit 1; \
	else \
		echo "INFO: test directory not found, skipping."; \
	fi

.PHONY: doxy
doxy : submodule
	@if [ -d doxyfw ] && [ -f doxyfw/Makefile ]; then \
		set -e; \
		$(MAKE) -C doxyfw CATEGORY=calc; \
		$(MAKE) -C doxyfw CATEGORY=calc.net; \
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
