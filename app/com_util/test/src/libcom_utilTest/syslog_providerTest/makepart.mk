# テスト対象のソースファイル
TEST_SRCS := \
    $(WORKSPACE_FOLDER)/app/com_util/prod/libsrc/com_util/trace/backends/syslog/trace_syslog.c

CFLAGS   += \
    -I$(WORKSPACE_FOLDER)/app/com_util/prod/libsrc/com_util/trace/backends/syslog
CXXFLAGS += \
    -I$(WORKSPACE_FOLDER)/app/com_util/prod/libsrc/com_util/trace/backends/syslog

# memset が mock_libc のモック対象であるためリンクが必要
LIBS += mock_libc
