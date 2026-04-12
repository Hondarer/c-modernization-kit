# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/com_util/fs/path_format.c

# ライブラリの指定
LIBS += mock_libc
