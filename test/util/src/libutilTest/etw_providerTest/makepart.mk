ifeq ($(OS),Windows_NT)
    # テスト対象のソースファイル
    TEST_SRCS := \
        $(WORKSPACE_FOLDER)/prod/util/libsrc/util/etw-provider.c
    # TraceLogging API (EventRegister 等) は Advapi32.dll にあるためリンクが必要
    LDFLAGS += /DEFAULTLIB:Advapi32.lib
endif
