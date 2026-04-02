# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace-provider.c \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/file-provider.c \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/syslog-provider.c \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/etw-provider.c

LIBS += mock_libc
