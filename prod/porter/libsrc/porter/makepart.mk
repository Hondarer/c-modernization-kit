ifneq ($(OS),Windows_NT)
    LIBS += pthread z
endif
ifeq ($(OS),Windows_NT)
    LIBS += ws2_32 Cabinet
endif

ifeq ($(OS),Windows_NT)
    # DLL エクスポート定義
    CFLAGS   += /DPOTR_EXPORTS
    CXXFLAGS += /DPOTR_EXPORTS
endif

# 動的ライブラリとしてビルド
LIB_TYPE = shared
