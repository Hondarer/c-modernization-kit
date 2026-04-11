# ライブラリの検索パス
LIBSDIR += $(WORKSPACE_FOLDER)/app/porter/test/lib

ifdef PLATFORM_WINDOWS
    # 外部関数の static 定義
    CFLAGS   += /DPOTR_STATIC
    CXXFLAGS += /DPOTR_STATIC
endif
