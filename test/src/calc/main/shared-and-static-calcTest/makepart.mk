# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/src/shared-and-static-calc/shared-and-static-calc.c

# エントリーポイントの変更
# テスト対象のソースファイルにある main() は直接実行されず、
# テストコード内から __real_main() 経由で実行される
USE_WRAP_MAIN := 1

# ライブラリの指定
LIBS += mock_calc mock_calcbase mock_libc
