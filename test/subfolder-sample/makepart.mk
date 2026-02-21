# テスト関連ライブラリは、すべて静的リンクとする
ifeq ($(OS),Windows_NT)
    # Windows
    CFLAGS   += /DSUBFOLDER_SAMPLE_STATIC
    CXXFLAGS += /DSUBFOLDER_SAMPLE_STATIC
endif
