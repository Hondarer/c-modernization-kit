# 依存する静的ライブラリの検索パス
LIBSDIR += $(WORKSPACE_FOLDER)/prod/simplecomm/lib

# 依存ライブラリ
LIBS += simplecommcore
ifneq ($(OS),Windows_NT)
    LIBS += pthread
endif
ifeq ($(OS),Windows_NT)
    LIBS += ws2_32
endif

ifeq ($(OS),Windows_NT)
    # DLL エクスポート定義
    CFLAGS   += /DCOMM_EXPORTS
    CXXFLAGS += /DCOMM_EXPORTS
endif

# 動的ライブラリとしてビルド
LIB_TYPE = shared
