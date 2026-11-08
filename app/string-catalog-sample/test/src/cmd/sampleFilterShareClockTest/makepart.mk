TEST_SRCS := $(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c
INCDIR += $(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample
LIBS += mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat の通常関数をテスト実行体で解決するため、DLL import を無効にする。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
