# Windows PoC 用 Makefile
# このディレクトリは add コマンドをビルドします

# ライブラリの指定
# Windows: calcbase.lib
# Linux: -lcalcbase
ifeq ($(OS),Windows_NT)
    LIBS := calcbase.lib
else
    LIBS := -lcalcbase
endif

# Windows PoC テンプレートを include
include ../makesrc-windows-poc.mk
