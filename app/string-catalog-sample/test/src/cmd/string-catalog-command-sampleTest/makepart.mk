# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-command-sample/string-catalog-command-sample.c

# リンクを成立させるカタログの生成物 (sampleMessagesTest の対象)
ADD_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-command-sample/gen/sample_messages.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-command-sample/gen/sample_metrics.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-command-sample/gen/sample_trace.c

# エントリ ポイントの変更
# テスト対象のソース ファイルにある main() は直接実行されず、
# テスト コード内から __real_main() 経由で実行される
USE_WRAP_MAIN := 1

# 生成物と、手書きの分類値ヘッダーの探索パス
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-command-sample \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-command-sample/gen

LIBS += mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体で解決するため、DLL import を無効にする。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
