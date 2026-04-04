# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/console/console.c

# fflush 等の stdio モック
LIBS += mock_libc

# console_internal.h の解決パス
CFLAGS   += -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/console
CXXFLAGS += -I$(WORKSPACE_FOLDER)/prod/util/libsrc/util/console
