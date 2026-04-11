SUBDIRS = \
	util \
	calc \
	calc.net \
	subfolder-sample \
	override-sample \
	porter \
	doxygen-sample \
	tutorial

.DEFAULT_GOAL := default

.PHONY: default
default : $(SUBDIRS)

.PHONY: clean
clean : $(SUBDIRS)
	-find . -name "coverage.xml" -delete

.PHONY: test
test :
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

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		echo $(MAKE) -C $@ $(MAKECMDGOALS); \
		$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
	else \
		echo "INFO: $@/makefile not found, skipping."; \
	fi
