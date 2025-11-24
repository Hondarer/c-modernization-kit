# サブディレクトリの定義
SUBDIRS  = prod testfw test
TEST_SUBDIRS = testfw test
DOCS_SUBDIRS = doxyfw

# サブディレクトリで make を実行するマクロ
# $(1): サブディレクトリリスト
# $(2): make ターゲット (空の場合はデフォルトターゲット)
define make_in_subdirs
	@for dir in $(1); do \
		if [ -d $$dir ] && [ -f $$dir/Makefile ]; then \
			$(MAKE) -C $$dir $(2) || exit 1; \
		fi; \
	done
endef

.DEFAULT_GOAL := default

.PHONY: default
default : submodule
	$(call make_in_subdirs,$(SUBDIRS))

.PHONY: submodule
submodule :
	$(info INFO: submodule sync disabled.)
#	git submodule sync
#	git submodule update --init --recursive

.PHONY: clean
clean : submodule
	$(call make_in_subdirs,$(SUBDIRS),clean)

.PHONY: test
test : submodule
	$(call make_in_subdirs,$(TEST_SUBDIRS),test)

.PHONY: docs
docs : submodule
	$(call make_in_subdirs,$(DOCS_SUBDIRS),docs)
	$(BASH) docsfw/bin/pub_markdown_core.sh --workspaceFolder="$(CURDIR)" --details=both --docx=true
