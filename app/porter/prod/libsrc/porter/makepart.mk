# ライブラリの指定
LIBS += util
ifdef PLATFORM_LINUX
    LIBS += pthread z crypto
endif
ifdef PLATFORM_WINDOWS
    LIBS += ws2_32 Cabinet bcrypt
endif

ifdef PLATFORM_WINDOWS
    # DLL エクスポート定義
    CFLAGS   += /DPOTR_EXPORTS
    CXXFLAGS += /DPOTR_EXPORTS
endif

# 生成されるライブラリを動的ライブラリ (shared) とする
# 未指定の場合 (デフォルト) は static
LIB_TYPE = shared
