SUBDIRS = \
	util \
	calc \
	calc.net \
	subfolder-sample \
	override-sample \
	porter \
	doxygen-sample \
	tutorial

DOXY_CATEGORIES = \
	calc \
	calc.net \
	util \
	override-sample \
	doxygen-sample \
	porter

.DEFAULT_GOAL := default

.PHONY: submodule
submodule :
	@if [ ! -d ../framework/makefw ] || [ ! -f ../framework/makefw/.git ]; then \
		echo "ERROR: makefw submodule is not initialized."; \
		echo "Please run the following command to initialize submodules:"; \
		echo "  git submodule update --init --recursive"; \
		exit 1; \
	fi

.PHONY: default
default : submodule $(SUBDIRS)

.PHONY: clean
clean : submodule $(SUBDIRS)
	-find . -name "coverage.xml" -delete

.PHONY: test
test : submodule
    # 全プロダクトをビルド (デフォルトターゲット)
	$(MAKE) $(MFLAGS) $(SUBDIRS)
    # テスト実行
	@for dir in $(SUBDIRS); do \
		if [ -d $$dir ] && [ -f $$dir/makefile ]; then \
			echo $(MAKE) -C $$dir test; \
			$(MAKE) -C $$dir test || exit 1; \
		fi; \
	done
    # このフォルダ以下の coverage.xml をマージする
	python ../framework/testfw/bin/cobertura_merge.py . 1> /dev/null 2>&1

# NOTE: 単一カテゴリを指定して Doxygen を実行可能。
# 例: make doxy CATEGORY=porter
.PHONY: doxy
doxy : submodule
	@if [ -d ../framework/doxyfw ] && [ -f ../framework/doxyfw/makefile ]; then \
		set -e; \
		if [ -n "$(CATEGORY)" ]; then \
			echo $(MAKE) -C ../framework/doxyfw CATEGORY=$(CATEGORY); \
			$(MAKE) -C ../framework/doxyfw CATEGORY=$(CATEGORY); \
		elif [ -z "$(DOXY_CATEGORIES)" ]; then \
			echo $(MAKE) -C ../framework/doxyfw; \
			$(MAKE) -C ../framework/doxyfw; \
		else \
			for cat in $(DOXY_CATEGORIES); do \
				echo $(MAKE) -C ../framework/doxyfw CATEGORY=$$cat; \
				$(MAKE) -C ../framework/doxyfw CATEGORY=$$cat; \
			done; \
		fi; \
	else \
		echo "INFO: ../framework/doxyfw directory not found, skipping."; \
	fi

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		echo $(MAKE) -C $@ $(MAKECMDGOALS); \
		$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
	else \
		echo "INFO: $@/makefile not found, skipping."; \
	fi
