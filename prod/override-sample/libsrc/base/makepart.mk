ifneq ($(OS),Windows_NT)
    # Linux: dlopen/dlsym/dlclose を使用するため -ldl が必要
    #        pthread_once を使用するため -lpthread が必要
    LDFLAGS += -ldl -lpthread
    LIBS += funcman
else
    # Windows
    # DLL エクスポート定義
    # DLL export definition
    CFLAGS   += /DBASE_EXPORTS
    CXXFLAGS += /DBASE_EXPORTS
    LIBS += funcman
endif
