ifeq ($(OS),Windows_NT)
    # TraceLogging API (EventRegister 等) は Advapi32.dll にあるためリンクが必要
    LDFLAGS += /DEFAULTLIB:Advapi32.lib
endif
