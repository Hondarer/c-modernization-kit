# テスト対象のソース ファイル
# 利用者が用意するカタログ定義であり、ライブラリには含まれない
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample/gen/sample_messages.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample/gen/sample_metrics.c

# 生成物と、手書きの分類値ヘッダーの探索パス
# テスト ディレクトリへ引き込んだソースからは、元ディレクトリを基準に解決できないため指定する
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample/gen

# 注入したカタログを確認するためにライブラリをリンクする
LIBS += cplat
