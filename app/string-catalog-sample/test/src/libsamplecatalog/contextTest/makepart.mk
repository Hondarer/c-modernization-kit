# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/samplecatalog/samplecatalog_context.c

# 公開ヘッダーの宣言をエクスポート装飾なしへ倒す。
# テストはライブラリをリンクせず対象のソースを直接取り込むため、
# Windows では dllimport 宣言と定義が衝突する。
CFLAGS   += -DSAMPLECATALOG_STATIC=1
CXXFLAGS += -DSAMPLECATALOG_STATIC=1

# 依存ライブラリ
LIBS += cplat
