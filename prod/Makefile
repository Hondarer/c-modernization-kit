SUBDIRS = \
	calc

.PHONY: all
all : $(SUBDIRS)

.PHONY: clean
clean : $(SUBDIRS)

.PHONY: test
test : $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/Makefile ]; then \
		echo $(MAKE) -C $@ $(MAKECMDGOALS); \
		$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
	else \
		echo "Skipping directory '$@' (no Makefile)"; \
	fi
