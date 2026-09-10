# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/string_catalog/string_catalog_render.c

# モジュール私有ヘッダー format_engine.h の探索パス
# テスト ディレクトリへ引き込んだソースからは、元ディレクトリを基準に解決できないため指定する
INCDIR += \
	$(MYAPP_DIR)/prod/libsrc/string_catalog

# テスト対象が使用する標準ライブラリ関数のモック
# 既定では実関数へ委譲する
LIBS += mock_libc
