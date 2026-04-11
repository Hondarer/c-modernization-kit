.DEFAULT_GOAL := default

.PHONY: default
default :
	@if [ -d prod ] && [ -f prod/makefile ]; then $(MAKE) -C prod || exit 1; fi
	@if [ -d test ] && [ -f test/makefile ]; then $(MAKE) -C test || exit 1; fi

.PHONY: test
test :
	@if [ -d prod ] && [ -f prod/makefile ]; then $(MAKE) -C prod || exit 1; fi
	@if [ -d test ] && [ -f test/makefile ]; then $(MAKE) -C test test || exit 1; fi

.PHONY: clean
clean :
	@if [ -d prod ] && [ -f prod/makefile ]; then $(MAKE) -C prod clean || exit 1; fi
	@if [ -d test ] && [ -f test/makefile ]; then $(MAKE) -C test clean || exit 1; fi
