# ライブラリの指定
LIBS += base

ifdef PLATFORM_WINDOWS
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DBASE_EXT_EXPORTS
    CXXFLAGS += /DBASE_EXT_EXPORTS
endif
