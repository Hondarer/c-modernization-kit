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
ifneq ($(OS),Windows_NT)
    # Linux
    # gcovr (dnf install python3.11 python3.11-pip; pip3.11 install gcovr)
	@if command -v gcovr > /dev/null 2>&1; then \
		gcovr --exclude-unreachable-branches --cobertura-pretty --output coverage.xml; \
	fi
else
    # Windows
	python ../testfw/cmnd/cobertura_merge.py . 1> /dev/null 2>&1
endif

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/Makefile ]; then \
		echo $(MAKE) -C $@ $(MAKECMDGOALS); \
		$(MAKE) -C $@ $(MAKECMDGOALS) || exit 1; \
	else \
		echo "Skipping directory '$@' (no Makefile)"; \
	fi
