# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/trace.c \
	$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/file/trace_file.c \
    $(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/syslog/trace_syslog.c \
    $(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/etw/trace_etw.c

CFLAGS   += -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace
CFLAGS   += \
    -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/file \
    -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/syslog \
    -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/etw
CXXFLAGS += -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace
CXXFLAGS += \
    -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/file \
    -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/syslog \
    -I$(WORKSPACE_FOLDER)/app/util/prod/libsrc/util/trace/backends/etw

LIBS += mock_libc
