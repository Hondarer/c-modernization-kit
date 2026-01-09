# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/libutil/file-util.c

# ライブラリの指定
LIBS += mock_libc
