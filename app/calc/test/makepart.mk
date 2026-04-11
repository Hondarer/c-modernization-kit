# ライブラリの検索パス
LIBSDIR += $(WORKSPACE_FOLDER)/app/calc/test/lib

ifdef PLATFORM_WINDOWS
    # 外部関数の static 定義
    CFLAGS   += /DCALC_STATIC
    CXXFLAGS += /DCALC_STATIC
endif
