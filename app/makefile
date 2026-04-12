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

TESTFW_BANNER = ../framework/testfw/bin/banner.sh

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
		printf '\n'; \
		bash "$(TESTFW_BANNER)" WARNING "\e[33m"; \
		printf '\n'; \
		first_warn=1; \
		for warn_file in $$warn_files; do \
			if [ $$first_warn -eq 0 ]; then \
				printf '\n'; \
			fi; \
			first_warn=0; \
			printf '\033[33m===== %s =====\033[0m\n' "$$warn_file"; \
			while IFS= read -r line || [ -n "$$line" ]; do \
				printf '\033[33m%s\033[0m\n' "$$line"; \
			done < "$$warn_file"; \
		done; \
	fi

.PHONY: clean
clean : submodule $(SUBDIRS)
	-find . -name "coverage.xml" -delete

.PHONY: test
test : submodule
    # 全プロダクトをビルド (default ターゲット)
	$(MAKE) $(MFLAGS)
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
		if [ -n "$(filter-out $(SUBDIRS),$(MAKECMDGOALS))" ]; then \
			echo $(MAKE) -C $@ $(filter-out $(SUBDIRS),$(MAKECMDGOALS)); \
			$(MAKE) -C $@ $(filter-out $(SUBDIRS),$(MAKECMDGOALS)) || exit 1; \
		else \
			echo $(MAKE) -C $@; \
			$(MAKE) -C $@ || exit 1; \
		fi; \
	else \
		:; # echo "Skipping directory '$@' (no makefile)"; \
	fi
