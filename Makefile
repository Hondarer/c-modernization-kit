# サブディレクトリの定義
MAKE_SUBDIRS  = prod testfw test
TEST_SUBDIRS = testfw test

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
	$(call make_in_subdirs,$(MAKE_SUBDIRS))

.PHONY: submodule
submodule :
	$(info INFO: submodule sync disabled.)
#	git submodule sync
#	git submodule update --init --recursive

.PHONY: clean
clean : submodule
	$(call make_in_subdirs,$(MAKE_SUBDIRS),clean)

.PHONY: test
test : submodule
	$(call make_in_subdirs,$(TEST_SUBDIRS),test)

.PHONY: doxy
doxy : submodule
	@if [ -d doxyfw ] && [ -f doxyfw/Makefile ]; then \
		$(MAKE) -C doxyfw  CATEGORY=calc; \
		$(MAKE) -C doxyfw  CATEGORY=calc.net; \
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
