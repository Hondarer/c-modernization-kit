# テスト関連ライブラリは、すべて静的リンクとする
ifeq ($(OS),Windows_NT)
    # Windows
    CFLAGS   += /DCALC_STATIC
    CXXFLAGS += /DCALC_STATIC
endif

LIBSDIR += $(WORKSPACE_FOLDER)/test/calc/lib
