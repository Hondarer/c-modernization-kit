# 利用者が用意するカタログと、ライブラリの書式展開を実際に結合して確認する統合テスト
# 個々のソースは単体テストがカバレッジを充足するため、TEST_SRCS は宣言しない
ADD_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample/gen/sample_messages.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample/gen/sample_metrics.c

# 生成物と、手書きの分類値ヘッダーの探索パス
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample/gen

LIBS += cplat
