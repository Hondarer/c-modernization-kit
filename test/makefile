SUBDIRS = \
	libsrc \
	src

.DEFAULT_GOAL := default

.PHONY: default
default : $(SUBDIRS)

.PHONY: clean
clean : $(SUBDIRS)
	-rm -f coverage.xml

.PHONY: test
test : $(SUBDIRS)
    # このフォルダ以下の coverage.xml をマージする
	python ../testfw/cmnd/cobertura_merge.py . 1> /dev/null 2>&1

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		echo $(MAKE) -C $@ $(MAKECMDGOALS); \
		$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
	else \
		echo "Skipping directory '$@' (no makefile)"; \
	fi
