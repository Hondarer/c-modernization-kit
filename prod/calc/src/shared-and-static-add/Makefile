# Windows PoC 用 Makefile
# このディレクトリは shared-and-static-add コマンドをビルドします

# ライブラリの指定
# Windows: calc.lib calcbase.lib
# Linux: -lcalc -lcalcbase
ifeq ($(OS),Windows_NT)
    LIBS := calc.lib calcbase.lib
else
    LIBS := -lcalc -lcalcbase
endif

# Windows PoC テンプレートを include
include ../makesrc-windows-poc.mk
