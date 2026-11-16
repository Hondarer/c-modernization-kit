TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/print/print.c

ADD_SRCS += \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/access/access.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/bytes.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/index.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/integer.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/validate.c

LIBS += mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体へ直接定義する。
    # 製品ライブラリのリンク方式は変更しない。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
