# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/com_util/trace/trace.c \
	$(MYAPP_DIR)/prod/libsrc/com_util/trace/backends/file/trace_file.c \
    $(MYAPP_DIR)/prod/libsrc/com_util/trace/backends/syslog/trace_syslog.c \
    $(MYAPP_DIR)/prod/libsrc/com_util/trace/backends/etw/trace_etw.c

# TEST_SRCS の相対インクルード解決
INCDIR += \
    $(MYAPP_DIR)/prod/libsrc/com_util/trace \
    $(MYAPP_DIR)/prod/libsrc/com_util/trace/backends/file \
    $(MYAPP_DIR)/prod/libsrc/com_util/trace/backends/syslog \
    $(MYAPP_DIR)/prod/libsrc/com_util/trace/backends/etw

# ライブラリの指定
LIBS += mock_libc
