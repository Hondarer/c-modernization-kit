ifeq ($(OS),Windows_NT)
    # テスト対象のソースファイル
    TEST_SRCS := \
    	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/etw-provider.c \
    	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/etw-session.c

    # TraceLogging + ETW Consumer API は Advapi32.dll にあるためリンクが必要
    LDFLAGS += /DEFAULTLIB:Advapi32.lib
endif
