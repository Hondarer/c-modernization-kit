# テスト関連ライブラリは、すべて静的リンクとする
ifeq ($(OS),Windows_NT)
    # Windows
    CFLAGS   += /DFILE_UTIL_STATIC
    CXXFLAGS += /DFILE_UTIL_STATIC
endif
