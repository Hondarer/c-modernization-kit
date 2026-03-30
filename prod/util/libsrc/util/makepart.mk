ifeq ($(OS),Windows_NT)
    # Windows
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DFMTIO_UTIL_EXPORTS /DTRACE_ETW_UTIL_EXPORTS /DTRACE_UTIL_EXPORTS /DTRACE_FILE_UTIL_EXPORTS /DCONSOLE_UTIL_EXPORTS
    CXXFLAGS += /DFMTIO_UTIL_EXPORTS /DTRACE_ETW_UTIL_EXPORTS /DTRACE_UTIL_EXPORTS /DTRACE_FILE_UTIL_EXPORTS /DCONSOLE_UTIL_EXPORTS
    # TraceLogging API (EventRegister 等) は Advapi32.dll にあるためリンクが必要
    LDFLAGS  += /DEFAULTLIB:Advapi32.lib
endif

# 生成されるライブラリを動的ライブラリ (shared) とする
# 未指定の場合 (デフォルト) は static
LIB_TYPE = shared
