# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample_windows.c

# テスト対象ソースのローカル ヘッダーを参照する
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/service-sample

# ライブラリの指定
# 共通処理 (service-sample.c) は組み込まず、テスト側で代替を定義する。
LIBS += mock_cplat mock_libc
