# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace-provider.c \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/file-provider.c \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/syslog-provider.c \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/etw-provider.c

ifeq ($(OS),Windows_NT)
    # TraceLogging API (EventRegister 等) は Advapi32.dll にあるためリンクが必要
    LDFLAGS += /DEFAULTLIB:Advapi32.lib
endif
