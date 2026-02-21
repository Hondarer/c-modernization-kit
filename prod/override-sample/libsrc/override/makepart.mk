# ライブラリの指定
LIBS += base

ifeq ($(OS),Windows_NT)
    # Windows
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DBASE_EXT_EXPORTS
    CXXFLAGS += /DBASE_EXT_EXPORTS
endif
