TEST_SRCS := $(MYAPP_DIR)/prod/src/cmd/struct-meta-sample/struct_meta_sample.c
INCDIR += $(MYAPP_DIR)/prod/src/cmd/struct-meta-sample
LIBS += struct_meta cjson mock_cplat mock_libc
USE_WRAP_MAIN := 1

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体へ直接定義する。
    # 製品ライブラリのリンク方式は変更しない。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
