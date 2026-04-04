# テスト対象のソースファイル
TEST_SRCS := \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/backends/syslog/trace_syslog.c

CFLAGS   += \
    -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/backends/syslog
CXXFLAGS += \
    -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/backends/syslog
