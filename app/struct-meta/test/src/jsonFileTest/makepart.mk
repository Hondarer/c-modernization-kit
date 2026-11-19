# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/json/file.c

# ライブラリの指定
LIBS += struct_meta cjson_app mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体へ直接定義する。
    # 製品ライブラリのリンク方式は変更しない。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
