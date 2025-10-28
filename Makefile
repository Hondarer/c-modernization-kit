# ターゲットなしの make 対応
.PHONY: default
default : submodule
	make -C testfw
	make -C test
	make -C doxyfw

.PHONY: submodule
submodule :
	git submodule sync
	git submodule update --init --recursive

.PHONY: all
all : submodule
	make -C testfw all
	make -C test all
	make -C doxyfw all

.PHONY: clean
clean : submodule
	make -C testfw clean
	make -C test clean
	make -C doxyfw clean

.PHONY: test
test : submodule
	make -C testfw test
	make -C test test

.PHONY: docs
docs : submodule
	make -C doxyfw docs
