# テスト対象のソースファイル
TEST_SRCS := \
    $(WORKSPACE_FOLDER)/app/com_util/prod/libsrc/com_util/trace/backends/etw/trace_etw.c \
    $(WORKSPACE_FOLDER)/app/com_util/prod/libsrc/com_util/trace/backends/etw/trace_etw_session.c

CFLAGS   += \
    -I$(WORKSPACE_FOLDER)/app/com_util/prod/libsrc/com_util/trace/backends/etw
CXXFLAGS += \
    -I$(WORKSPACE_FOLDER)/app/com_util/prod/libsrc/com_util/trace/backends/etw
