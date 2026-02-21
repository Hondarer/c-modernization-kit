ifneq ($(OS),Windows_NT)
    # Linux: dlopen/dlsym/dlclose を使用するため -ldl が必要
    LDFLAGS += -ldl
else
    # Windows
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DBASE_EXPORTS
    CXXFLAGS += /DBASE_EXPORTS
endif
