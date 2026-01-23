# サブフォルダはコンパイルのみ
NO_LINK = 1

# テスト対象のソースファイル
# NOTE: 上位フォルダで TEST_SRCS を指定している場合、テスト対象ソースが重複しないように留意すること。
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/subfolder-sample/libsrc/libsubfolder-sample/subfolder_a/func_a.c
