# テスト対象のソースファイル
TEST_SRCS := \
    $(MYAPP_DIR)/prod/libsrc/com_util/trace/backends/etw/trace_etw.c

# TEST_SRCS の相対インクルード解決
INCDIR += $(MYAPP_DIR)/prod/libsrc/com_util/trace/backends/etw
