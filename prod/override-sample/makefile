SUBDIRS = \
	libsrc \
	src

.DEFAULT_GOAL := default

.PHONY: default
default : $(SUBDIRS)

.PHONY: clean
clean : $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		echo $(MAKE) -C $@ $(MAKECMDGOALS); \
		$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
	else \
		echo "Skipping directory '$@' (no makefile)"; \
	fi
