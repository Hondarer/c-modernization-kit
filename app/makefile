# 依存関係に留意して順番を定義
SUBDIRS = \
	com_util \
	calc \
	calc.net \
	subfolder-sample \
	override-sample \
	porter \
	doxygen-sample \
	tutorial

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
	@warn_files=$$(find . -type f -name '*.warn' -size +0 | sort); \
	if [ -n "$$warn_files" ]; then \
		echo ""; \
		bash ../framework/testfw/bin/banner.sh WARNING "\e[33m"; \
		echo ""; \
		printf '\033[33m'; \
		for warn_file in $$warn_files; do \
			printf '===== %s =====\n' "$$warn_file"; \
			cat "$$warn_file"; \
			printf '\n\n'; \
		done; \
		printf '\033[0m'; \
	fi

.PHONY: clean
clean : submodule $(SUBDIRS)
	-find . -name "coverage.xml" -delete

.PHONY: test
test : submodule
    # 全プロダクトをビルド (default ターゲット)
	$(MAKE) $(MFLAGS) default
    # テスト実行
	@for dir in $(SUBDIRS); do \
		if [ -d $$dir ] && [ -f $$dir/makefile ]; then \
			echo $(MAKE) -C $$dir test; \
			$(MAKE) -C $$dir test || exit 1; \
		fi; \
	done
    # このフォルダ以下の coverage.xml をマージする
	python ../framework/testfw/bin/cobertura_merge.py . 1> /dev/null 2>&1

.PHONY: doxy
doxy : submodule $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		echo $(MAKE) -C $@ $(filter-out $(SUBDIRS),$(MAKECMDGOALS)); \
		$(MAKE) -C $@ $(filter-out $(SUBDIRS),$(MAKECMDGOALS)) || exit 1; \
	else \
		echo "INFO: $@/makefile not found, skipping."; \
	fi
