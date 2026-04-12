# ライブラリの検索パス
LIBSDIR += $(MYAPP_FOLDER)/test/lib

ifdef PLATFORM_WINDOWS
    # 外部関数の static 定義
    CFLAGS   += /DCALC_STATIC
    CXXFLAGS += /DCALC_STATIC
endif
