# テスト対象のソース ファイル
# フィルター本体 (コンパイル、イメージ、スロット、説明文、ソース領域) は cplat の公開 API として実体をリンクして確認する。
# ここでは条件式リストのファイル (sample_filter_file.c)、共有配布 (sample_filter_share*.c)、警告の文 (sample_filter_warning.c) と、生成物のフィルター接続 (gen/sample_worker_trace.c) を対象とする。
# string-catalog-filter-sample.c (コマンドの main) は対象外。
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_file.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_share.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_warning.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_worker_context.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/gen/sample_worker_trace.c

# モジュール私有ヘッダー (sample_filter_share.h など) と、
# 生成物のモジュール私有ヘッダー (gen/sample_worker_trace.h) の探索パス
# テスト ディレクトリへ引き込んだソースからは、元ディレクトリを基準に解決できないため指定する
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/gen

# テスト対象が呼び出す標準ライブラリ関数は、testfw の include_override によって mock_libc へ差し替わる。
# フィルター層は cplat の同期プリミティブとトレーサーを実際に駆動して確認するため、
# cplat は mock ではなく実体をリンクする (sampleMessagesTest と同じ)。
LIBS += mock_libc cplat
