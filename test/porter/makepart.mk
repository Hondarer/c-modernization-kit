# ライブラリの検索パス
LIBSDIR += $(WORKSPACE_FOLDER)/test/porter/lib

ifdef PLATFORM_WINDOWS
    # 外部関数の static 定義
    CFLAGS   += /DPOTR_STATIC
    CXXFLAGS += /DPOTR_STATIC
endif
