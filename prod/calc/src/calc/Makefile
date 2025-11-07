# Windows PoC 用 Makefile
# このディレクトリは calc コマンドをビルドします

# ライブラリの指定
# Windows: calc.lib
# Linux: -lcalc
ifeq ($(OS),Windows_NT)
    LIBS := calc.lib
else
    LIBS := -lcalc
endif

# Windows PoC テンプレートを include
include ../makesrc-windows-poc.mk
