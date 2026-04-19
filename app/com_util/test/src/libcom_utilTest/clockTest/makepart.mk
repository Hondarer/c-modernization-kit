# テスト対象のソースファイル
TEST_SRCS := \
    $(MYAPP_DIR)/prod/libsrc/com_util/clock/clock.c

# clock の override を無効化し、本物の実装を直接検証する
DEFINES += COM_UTIL_CLOCK_NO_OVERRIDE

# ライブラリの指定
LIBS += mock_libc
