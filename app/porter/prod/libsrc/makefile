SUBDIRS = \
	porter

.DEFAULT_GOAL := default

.PHONY: default
default : $(SUBDIRS)

.PHONY: clean
clean : $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/makefile ]; then \
		if [ -n "$(MAKECMDGOALS)" ]; then \
			echo $(MAKE) -C $@ $(MAKECMDGOALS); \
			$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
		else \
			echo $(MAKE) -C $@; \
			$(MAKE) -C $@ || exit 1; \
		fi; \
	else \
		:; # echo "Skipping directory '$@' (no makefile)"; \
	fi
