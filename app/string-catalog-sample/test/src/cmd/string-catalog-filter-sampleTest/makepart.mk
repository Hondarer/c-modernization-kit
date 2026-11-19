# テスト対象のソース ファイル
# コマンドの本体 (ワーカー、コマンドの処理、表示) を対象とする。
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c

# リンクを成立させる同じコマンドのほかのソースと生成物 (sampleFilterTest の対象)
ADD_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_file.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_share.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_warning.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_worker_context.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/gen/sample_worker_trace.c

# エントリ ポイントの変更
# テスト対象のソース ファイルにある main() は、テストのエントリ ポイントと衝突させない
USE_WRAP_MAIN := 1

# モジュール私有ヘッダーと、生成物のモジュール私有ヘッダー (gen/sample_worker_trace.h) の探索パス
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/gen

LIBS += mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体で解決するため、DLL import を無効にする。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
