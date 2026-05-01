# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/calc/calc.c

# エントリーポイントの変更
# テスト対象のソースファイルにある main() は直接実行されず、
# テストコード内から __real_main() 経由で実行される
USE_WRAP_MAIN := 1

# ライブラリの指定
LIBS += mock_calc mock_libc com_util
