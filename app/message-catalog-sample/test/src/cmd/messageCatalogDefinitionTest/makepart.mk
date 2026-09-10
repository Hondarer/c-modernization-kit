# テスト対象のソース ファイル
# 利用者が用意するカタログ定義であり、ライブラリには含まれない
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/message-catalog-sample/message_catalog_definition.c

# モジュール私有ヘッダー message_catalog_definition.h の探索パス
# テスト ディレクトリへ引き込んだソースからは、元ディレクトリを基準に解決できないため指定する
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/message-catalog-sample

# 注入したカタログを確認するためにライブラリをリンクする
LIBS += message_catalog
