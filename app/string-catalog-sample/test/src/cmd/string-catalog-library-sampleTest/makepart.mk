# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c

# エントリ ポイントの変更
# テスト対象のソース ファイルにある main() は直接実行されず、
# テスト コード内から __real_main() 経由で実行される
USE_WRAP_MAIN := 1

# samplecatalog が公開するカタログを利用する。
LIBS += samplecatalog mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体で解決するため、DLL import を無効にする。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
