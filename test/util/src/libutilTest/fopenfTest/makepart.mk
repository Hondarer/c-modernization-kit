# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/fs/path_format.c

# ライブラリの指定
LIBS += mock_libc
