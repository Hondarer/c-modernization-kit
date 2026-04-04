# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/console/console-util.c

# fflush 等の stdio モック
LIBS += mock_libc
