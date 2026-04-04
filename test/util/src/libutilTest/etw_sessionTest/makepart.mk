# テスト対象のソースファイル
TEST_SRCS := \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/backends/etw/trace_etw.c \
    $(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/backends/etw/trace_etw_session.c

CFLAGS   += \
    -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/backends/etw
CXXFLAGS += \
    -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/trace/backends/etw
