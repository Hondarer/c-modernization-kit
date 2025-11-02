SUBDIRS = $(wildcard */)

.PHONY: all
all : $(SUBDIRS)

.PHONY: clean
clean : $(SUBDIRS)

.PHONY: test
test : $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS) : 
	@if [ -f $@/Makefile ]; then \
		echo make -C $@ $(MAKECMDGOALS); \
		make -C $@ $(MAKECMDGOALS); \
	else \
		echo "Skipping directory '$@' (no Makefile)"; \
	fi
