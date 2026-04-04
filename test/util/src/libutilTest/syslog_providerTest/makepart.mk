# テスト対象のソースファイル
TEST_SRCS := \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/syslog-provider.c

CFLAGS   += -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util
CXXFLAGS += -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util
