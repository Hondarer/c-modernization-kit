# ライブラリの検索パス
LIBSDIR += $(MYAPP_DIR)/test/lib

ifdef PLATFORM_WINDOWS
    # 外部関数の static 定義
    CFLAGS   += /DPOTR_STATIC
    CXXFLAGS += /DPOTR_STATIC
endif
