# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c

# リンクを成立させる配列の出力 (structMetaGenEmitArrayTest の対象)
ADD_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit_array.c

INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/struct-meta-gen

LIBS += struct_meta mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体へ直接定義する。
    # 製品ライブラリのリンク方式は変更しない。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
