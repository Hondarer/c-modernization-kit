# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/trace-provider.c \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/file-provider.c \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/syslog-provider.c \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/etw-provider.c

CFLAGS   += -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace
CXXFLAGS += -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace

LIBS += mock_libc
