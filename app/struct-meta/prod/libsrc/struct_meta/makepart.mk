# ライブラリの指定
LIBS += cplat cjson

ifdef PLATFORM_WINDOWS
    # DLL エクスポート定義
    CFLAGS   += /DSTRUCT_META_EXPORTS
    CXXFLAGS += /DSTRUCT_META_EXPORTS
endif

# 生成されるライブラリを動的ライブラリ (shared) とする
# 未指定の場合 (既定値) は static
LIB_TYPE = shared
