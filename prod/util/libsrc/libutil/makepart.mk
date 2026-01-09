ifeq ($(OS),Windows_NT)
    # Windows
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DFILE_UTIL_EXPORTS
    CXXFLAGS += /DFILE_UTIL_EXPORTS
endif

# 生成されるライブラリを動的ライブラリ (shared) とする
# 未指定の場合 (デフォルト) は static
LIB_TYPE = shared
