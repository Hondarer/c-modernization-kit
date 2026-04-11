# テスト対象のソースファイル
TEST_SRCS := \
    $(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/etw/trace_etw.c

CFLAGS   += \
    -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/etw
CXXFLAGS += \
    -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/etw
