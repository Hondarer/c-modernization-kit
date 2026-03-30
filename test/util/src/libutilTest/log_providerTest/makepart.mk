# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/log-provider.c

ifeq ($(OS),Windows_NT)
    # Windows: ETW プロバイダも必要
    TEST_SRCS += $(WORKSPACE_FOLDER)/prod/util/libsrc/util/etw-provider.c
    # TraceLogging API (EventRegister 等) は Advapi32.dll にあるためリンクが必要
    LDFLAGS += /DEFAULTLIB:Advapi32.lib
else
    # Linux: syslog プロバイダも必要
    TEST_SRCS += $(WORKSPACE_FOLDER)/prod/util/libsrc/util/syslog-provider.c
endif
