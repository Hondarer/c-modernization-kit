# テスト対象のソース ファイル
# コンパイル層 (sample_filter_image.c / sample_filter_compile.c)、
# フィルター スロット層 (sample_filter_slot.c)、出力層 (sample_filter_output.c) と、
# それらが結び付くカタログ定義の生成物および名前解決テーブルをまとめて対象とする。
# string-catalog-filter-sample.c (コマンドの main) は対象外。
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_image.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_compile.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_slot.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_describe.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_output.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_worker_trace_key_names.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_worker_context.c \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/gen/sample_worker_trace.c

# モジュール私有ヘッダー (sample_filter.h / sample_filter_image.h / sample_filter_output.h) と、
# 生成物のモジュール私有ヘッダー (gen/sample_worker_trace.h) の探索パス
# テスト ディレクトリへ引き込んだソースからは、元ディレクトリを基準に解決できないため指定する
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/gen

# テスト対象が呼び出す標準ライブラリ関数は、testfw の include_override によって mock_libc へ差し替わる。
# フィルター層は cplat の同期プリミティブとトレーサーを実際に駆動して確認するため、
# cplat は mock ではなく実体をリンクする (sampleMessagesTest と同じ)。
LIBS += mock_libc cplat
