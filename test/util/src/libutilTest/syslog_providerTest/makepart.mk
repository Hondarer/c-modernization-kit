# テスト対象のソースファイル
TEST_SRCS := \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/syslog-provider.c

CFLAGS   += -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace
CXXFLAGS += -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace
