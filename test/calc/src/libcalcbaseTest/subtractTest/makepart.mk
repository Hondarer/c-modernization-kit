# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/calc/libsrc/calcbase/subtract.c

# ライブラリの指定
# subtractTest では、add 関数のモックを使ってテストを行う。
LIBS += mock_calcbase
