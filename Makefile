# サブディレクトリの定義
ALL_SUBDIRS  = prod testfw test doxyfw
TEST_SUBDIRS = test
DOCS_SUBDIRS = doxyfw

# サブディレクトリで make を実行するマクロ
# $(1): サブディレクトリリスト
# $(2): make ターゲット (空の場合はデフォルトターゲット)
define make_in_subdirs
	@for dir in $(1); do \
		[ -d $$dir ] && [ -f $$dir/Makefile ] && make -C $$dir $(2) || true; \
	done
endef

# ターゲットなしの make 対応
.PHONY: default
default : submodule
	$(call make_in_subdirs,$(ALL_SUBDIRS))

.PHONY: submodule
submodule :
	git submodule sync
	git submodule update --init --recursive

.PHONY: all
all : submodule
	$(call make_in_subdirs,$(ALL_SUBDIRS),all)

.PHONY: clean
clean : submodule
	$(call make_in_subdirs,$(ALL_SUBDIRS),clean)

.PHONY: test
test : submodule
	$(call make_in_subdirs,$(TEST_SUBDIRS),test)

.PHONY: docs
docs : submodule
	$(call make_in_subdirs,$(DOCS_SUBDIRS),docs)
