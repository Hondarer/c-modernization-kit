SUBDIRS = \
	libsrc \
	src

.PHONY: all
all : $(SUBDIRS)

.PHONY: clean
clean : $(SUBDIRS)

.PHONY: test
test : $(SUBDIRS)
# gcovr (dnf install python3.11 python3.11-pip; pip3.11 install gcovr)
	@if command -v gcovr > /dev/null 2>&1; then \
		gcovr --exclude-unreachable-branches --cobertura-pretty --output coverage.xml; \
		#gcovr --exclude-unreachable-branches; \
	fi

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	@if [ -f $@/Makefile ]; then \
		echo make -C $@ $(MAKECMDGOALS); \
		make -C $@ $(MAKECMDGOALS); \
	else \
		echo "Skipping directory '$@' (no Makefile)"; \
	fi
