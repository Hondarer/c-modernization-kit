# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/com_util/fs/path_format.c \
	$(MYAPP_DIR)/prod/libsrc/com_util/fs/file_io.c

# ライブラリの指定
LIBS += mock_libc
