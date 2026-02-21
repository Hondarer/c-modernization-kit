# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/file-util.c

# ライブラリの指定
LIBS += mock_libc
